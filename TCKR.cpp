/*
 * Timelapse.cpp
 *
 * Created: 11/9/2015 7:47:22 PM
 *  Author: Sam
 *
 * Firmware for a time-lapse motion controller.
 */ 



/*
* TODOs
*	-Build a menu system
*	-EEPROM parameter storage
*/

#ifndef F_CPU
#define F_CPU 16000000UL
#endif


#include <stdlib.h>
#include<util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "Arduino.h"

#include "button.h"
#include "encoder.h"
#include "U8glib.h"
#include "RTC.h"


//==============================================================================

// 
#define MENU_NUM_ITEMS 5
#define NUM_PARAMS 4
#define START_INDEX NUM_PARAMS

#define NUM_SHOOT_TASKS 4 //Number of foreground tasks scheduled for shooting

#define MENU_PARAM_INDENT 6*13
#define MENU_LINE_SPACING 12

#define TIMELAPSE_FINAL_FPS 33



#define INT_MAX 1000L
#define INT_MIN (-1000L)


// HW defines
#define PIN 0
#define DDR 1
#define PORT 2

#define SHOOT_PULSE_WIDTH_MS 200

#define CAMERA_PORT PORTB // "open drain, ie don't stick 5v on the camera please :)"
#define CAMERA_DDR DDRB
#define CAMERA_SHOOT_PIN (1<<2)
#define CAMERA_FOCUS_PIN (1<<3)

#define ROT_PORT PORTC
#define ROT_DDR DDRC
#define ROT_STEP_PIN (1<<3) // arduino nano pin 8
#define ROT_DIR_PIN (1<<2)

#define LIN_PORT PORTC
#define LIN_DDR DDRC
#define LIN_STEP_PIN (1<<1)
#define LIN_DIR_PIN (1<<0)

#define ENABLE_PORT PORTB
#define ENABLE_DDR DDRB
#define ENABLE_PIN (1<<4)

#define LIN_AXIS_NUM 0
#define ROT_AXIS_NUM 1


typedef enum {STATE_IDLE, STATE_PARAM_EDIT, STATE_SHOOTING} main_state_t;
	
//==============================================================================

// constants 
const int steps_per_unit[2] = {10, 222};
	
// Background for menu
const char page_background[5][23] = 	{
	{"  Shot time:      Min\0"},
	{"  Clip time:      Sec\0"},
	{"  Rail dist:      mm \0"},
	{"  RotA dist:      deg\0"},
	{"                     \0"}
};


// Globals
U8GLIB_SH1106_128X64 u8g(U8G_I2C_OPT_DEV_0|U8G_I2C_OPT_FAST);	



//==============================================================================


// Initialise GPIO requierd to operate camera and stepper motor controllers
void init_shooting_peripherals(void)
{
	// Configure shooting stuff
	CAMERA_PORT &= ~(CAMERA_FOCUS_PIN | CAMERA_SHOOT_PIN); // Set output low
	CAMERA_DDR &= ~(CAMERA_FOCUS_PIN | CAMERA_SHOOT_PIN); // Initialise as high z
		
	LIN_DDR |= (LIN_DIR_PIN | LIN_STEP_PIN);
	ROT_DDR |= (ROT_DIR_PIN | ROT_STEP_PIN);
	
	DDRB |= (1<<5);		
}


void flash(void)
{
	PORTB ^= (1<<5);
}


// Send half a pulse to the linear axis stepper driver
void pulse_lin(void)
{
	LIN_PORT ^= LIN_STEP_PIN;
}


// Send half a pulse to the rotation axis stepper driver
void pulse_rot(void)
{
	ROT_PORT ^= ROT_STEP_PIN;
}


// Second half of camera trigger routine
RTC_task_handle_t shoot_finish_task;
void shoot_finish(void)
{
	CAMERA_DDR &= ~(CAMERA_SHOOT_PIN | CAMERA_FOCUS_PIN);
	RTC_stop_task(shoot_finish_task);
}


// Activate the camera shutter, a pulse width of ~200ms is required, so use 
// the RTC
void shoot(void)
{
	DDRB |= (1<<2)|(1<<3);
	//CAMERA_DDR |= CAMERA_SHOOT_PIN | CAMERA_FOCUS_PIN; // Change camera data direction (open drain)
	RTC_start_task(shoot_finish_task);
}


// Utility function, limits val between min and max
int int_limit_val(int val, int min, int max)
{
	if (val < min)
		return min;
	else if (val > max)
		return max;
	else
		return val;	
}

void update_shoot_intervals(RTC_task_handle_t shoot_tasks[], int shot_time_m, int clip_time_s, int rail_dist, int rotA_arc)
{	
	// set step direction
	if (rotA_arc < 0)
		ROT_PORT |= ROT_DIR_PIN;
	else
		ROT_PORT &= ~ROT_DIR_PIN;
				
	if (rail_dist < 0)
		LIN_PORT |= LIN_DIR_PIN;
	else
		LIN_PORT &= ~LIN_DIR_PIN;		
				
	// Update pulse intervals for current parameters
	unsigned long shot_time_ticks = shot_time_m*60*RTC_TICK_RATE_HZ;
	RTC_set_intertask_period(shot_time_ticks/(clip_time_s*TIMELAPSE_FINAL_FPS),
	shoot_tasks[1]);
	RTC_set_intertask_period(shot_time_ticks/(steps_per_unit[LIN_AXIS_NUM]*abs(rail_dist)),
	shoot_tasks[2]);
	RTC_set_intertask_period(shot_time_ticks/(steps_per_unit[ROT_AXIS_NUM]*abs(rotA_arc)),
	shoot_tasks[3]);
}




char buff[50];	
/*
void display_update(main_state_t state, int menu_index, int* parameter_table[])
{
	u8g.firstPage();
	do {
		u8g.drawStr(0, (menu_index+1)*12, ">");
		
		int line_num;
		for(line_num = 0; line_num < MENU_NUM_ITEMS; line_num++)
			u8g.drawStr(0, (line_num+1)*MENU_LINE_SPACING, page_background[line_num]);	

		int i;
		for (i=0; i < NUM_PARAMS;i++)
		{
			sprintf(buff, "%d", *parameter_table[i]);
			u8g.drawStr(MENU_PARAM_INDENT, (i+1)*MENU_LINE_SPACING, buff);
		}				
		
				
		if (STATE_PARAM_EDIT == state)
			u8g.drawStr(MENU_PARAM_INDENT, (menu_index+1)*MENU_LINE_SPACING+1, "_____");
		
				
		if (STATE_SHOOTING == state)
		{
			sprintf(buff, "> Stop %lu", RTC_get_raw());
			u8g.drawStr(0, (NUM_PARAMS+1)*MENU_LINE_SPACING, buff);
		}
		else
			u8g.drawStr(0, (NUM_PARAMS+1)*MENU_LINE_SPACING, "  Start ");
							
	} while(u8g.nextPage());
}
*/


void bootstrap_test(void)
{
	RTC_init();
	//RTC_task_handle_t flash_task_handle = RTC_push_task(flash, RTC_TICK_RATE_HZ/5);
	button_init();
	encoder_init();
	U8GLIB_SH1106_128X64 u8g(U8G_I2C_OPT_DEV_0|U8G_I2C_OPT_FAST);	// Init display
    u8g.setColorIndex(1);         // B/W
    u8g.setFont(u8g_font_courR08);
	
	char buff[10];
	
	sei();
	while(1)
	{	
		//if (button_pushed())
		//	RTC_toggle_task(flash_task_handle);

		u8g.firstPage();
		do {

			if (button_down())
				u8g.drawStr(0, 12, "DOWN");
			else if (button_up())
				u8g.drawStr(0, 12, "UP");
			else
				u8g.drawStr(0, 12, "ERROR");
			sprintf(buff, "%d", encoder_get_val());
			u8g.drawStr(0, 24, buff);
		} while( u8g.nextPage() );
	}
	
}

//==============================================================================

int main(void)
{
	//bootstrap_test();	
	cli();	
	
	// Parameters
	int shot_time_m = 5;
	int clip_time_s = 10;
	int rail_dist = 180;
	int rotA_arc = 30;

	int* parameter_table[4] = {&shot_time_m, &clip_time_s, &rail_dist, &rotA_arc};
	int max_param_vals[4] = {INT_MAX, INT_MAX, 180, INT_MAX};
	int min_param_vals[4] = {1, 1, -180, INT_MIN};
	int inc_table[4] = {1, 1, 5, 5};

	// input variables
	int encoder_diff;
	int button_pushed;
		
	int menu_index = 4;
		
	unsigned long shot_finish_time;
	
	main_state_t state = STATE_IDLE;
	main_state_t next_state = STATE_IDLE;
	
	// Initialisation	
	RTC_init(); 
	encoder_init();
	button_init();
	init_shooting_peripherals();
	
	// Init display
	u8g.setColorIndex(1);         // B/W
	u8g.setFont(u8g_font_courR08);

	// Register foreground tasks (Not running)
	
	shoot_finish_task = RTC_push_task(shoot_finish, RTC_MS_TO_TICKS(SHOOT_PULSE_WIDTH_MS));
	RTC_task_handle_t shoot_tasks[NUM_SHOOT_TASKS];	
	shoot_tasks[0] = RTC_push_task(flash, 200);
	shoot_tasks[1] = RTC_push_task(shoot, 200);
	shoot_tasks[2] = RTC_push_task(pulse_lin, 200);
	shoot_tasks[3] = RTC_push_task(pulse_rot, 200);
	
	sei();
	while(1)
	{	
		//display_update(state, menu_index, parameter_table);		
		
		// Check input
		button_pushed = button_push_occured();
		encoder_diff = encoder_get_diff();			
			
		if (STATE_IDLE == state)
		{
			if (button_pushed)
			{
				if (menu_index == START_INDEX)
				{
					shot_finish_time = (shot_time_m*60*RTC_TICK_RATE_HZ) + RTC_get_raw();
					update_shoot_intervals(shoot_tasks, shot_time_m, clip_time_s, rail_dist, rotA_arc);
					RTC_start_tasks(shoot_tasks, NUM_SHOOT_TASKS);
					next_state = STATE_SHOOTING;
				}
				else
					next_state = STATE_PARAM_EDIT;
			}
			else
				menu_index = int_limit_val(menu_index + encoder_diff, 0, MENU_NUM_ITEMS-1);
		}		
		else if (STATE_PARAM_EDIT == state)
		{
			if (button_pushed)
				next_state = STATE_IDLE;
			else
			{
				int new_val = *parameter_table[menu_index] + encoder_diff*inc_table[menu_index];
				*parameter_table[menu_index] = int_limit_val(new_val,
					min_param_vals[menu_index], max_param_vals[menu_index]);
			}		
		}				
		else if (STATE_SHOOTING == state)
		{
			if (button_pushed || (RTC_get_raw() > shot_finish_time))
			{
				RTC_stop_tasks(shoot_tasks, NUM_SHOOT_TASKS);
				next_state = STATE_IDLE;	
			}
		}
		state = next_state;
		
		// Update display
		u8g.firstPage();
		do {
			u8g.drawStr(0, (menu_index+1)*12, ">");
			
			int line_num;
			for(line_num = 0; line_num < MENU_NUM_ITEMS; line_num++)
			u8g.drawStr(0, (line_num+1)*MENU_LINE_SPACING, page_background[line_num]);


			int i;
			for (i=0; i < NUM_PARAMS;i++)
			{
				sprintf(buff, "%d", *parameter_table[i]);
				u8g.drawStr(MENU_PARAM_INDENT, (i+1)*MENU_LINE_SPACING, buff);
			}
			
			
			if (STATE_PARAM_EDIT == state)
				u8g.drawStr(MENU_PARAM_INDENT, (menu_index+1)*MENU_LINE_SPACING+1, "_____");
			
			
			if (STATE_SHOOTING == state)
			{
				sprintf(buff, "> Stop %lu", RTC_get_raw());
				u8g.drawStr(0, (NUM_PARAMS+1)*MENU_LINE_SPACING, buff);
			}
			else
				u8g.drawStr(0, (NUM_PARAMS+1)*MENU_LINE_SPACING, "  Start ");
			
		} while(u8g.nextPage());
	}
}
		
		/*		
		else 
		{
			if (menu_index < 4) // Editing a parameter
			{
				*parameter_table[menu_index] += encoder_diff*inc_table[menu_index];
				if (min_table[menu_index] > *parameter_table[menu_index])
					*parameter_table[menu_index] = minov_table[menu_index];
				if (max_table[menu_index] < *parameter_table[menu_index])
					*parameter_table[menu_index] = maxov_table[menu_index];
			}
			else if (shooting == false)
			{
				shooting = true;
				shot_start_time = RTC_get_raw();
				// Set direction pins
				if (rotA_arc < 0)
					ROT_PORT |= ROT_DIR_PIN;
				else
					ROT_PORT &= ~ROT_DIR_PIN;
				
				if (rail_dist < 0)
					LIN_PORT |= LIN_DIR_PIN;
				else
					LIN_PORT &= ~LIN_DIR_PIN;
				
				// Update pulse intervals for current parameters
				unsigned long shot_time_ticks = shot_time_m*60*RTC_TICK_RATE_HZ;
				RTC_set_intertask_period(shot_time_ticks/(clip_time_s*TIMELAPSE_FINAL_FPS), 
					shoot_tasks[1]);
				RTC_set_intertask_period(shot_time_ticks/(steps_per_unit[LIN_AXIS_NUM]*abs(rail_dist)), 
					shoot_tasks[2]);
				RTC_set_intertask_period(shot_time_ticks/(steps_per_unit[ROT_AXIS_NUM]*abs(rotA_arc)), 
					shoot_tasks[3]);	
				RTC_start_tasks(shoot_tasks, 4);
			}
			
		}
			
		bool shot_finished = (RTC_get_raw()-shot_start_time) > (shot_time_m*60*RTC_TICK_RATE_HZ);
		if ((!selected && shooting) || shot_finished)
		{
			selected = false;
			shooting = false;			
			RTC_stop_tasks(shoot_tasks, 4);		
		}
		
		
	}
}
*/