/*
 * shot_control.cpp
 *
 * Created: 12/5/2015 9:45:00 PM
 *  Author: Sam
 */ 

#define NUM_SHOOT_TASKS 3

#include "shot_control.h"


int shot_time_m = 5;
int clip_time_s = 10;
int rail_dist = 180;
int rotA_arc = 30;

	
unsigned long shot_finish_time;


RTC_task_handle_t shooting_tasks[NUM_SHOOT_TASKS];
RTC_task_handle_t shoot_shutter_deactivate_task;
RTC_task_handle_t shoot_end_task;


void shot_control_init(void)
{
	// Configure shooting stuff
	CAMERA_PORT &= ~(CAMERA_FOCUS_PIN | CAMERA_SHOOT_PIN); // Set output low
	CAMERA_DDR &= ~(CAMERA_FOCUS_PIN | CAMERA_SHOOT_PIN); // Initialise as high z
		
	LIN_DDR |= (LIN_DIR_PIN | LIN_STEP_PIN);
	ROT_DDR |= (ROT_DIR_PIN | ROT_STEP_PIN);
		
	DDRB |= (1<<5);
	

	shooting_tasks[0] = RTC_push_task(shot_control_shutter_activate, 1);
	shooting_tasks[1] = RTC_push_task(shot_control_lin_move, 1);
	shooting_tasks[2] = RTC_push_task(shot_control_rot_move, 1);	
	shoot_shutter_deactivate_task = RTC_push_task(shot_control_shutter_deactivate, 1);	
	shoot_end_task = RTC_push_task(shot_control_shot_end, 1);
}

void shot_control_shutter_activate(void)
{
	DDRB |= (1<<2)|(1<<3);
	//CAMERA_DDR |= CAMERA_SHOOT_PIN | CAMERA_FOCUS_PIN; // Change camera data direction (open drain)
	RTC_start_task(shoot_shutter_deactivate_task);
}
void shot_control_shutter_deactivate(void)
{
	CAMERA_DDR &= ~(CAMERA_SHOOT_PIN | CAMERA_FOCUS_PIN);
	RTC_stop_task(shoot_shutter_deactivate_task);	
}

void shot_control_rot_move(void)
{
	ROT_PORT ^= ROT_STEP_PIN;
}

void shot_control_lin_move(void)
{
	LIN_PORT ^= LIN_STEP_PIN;
}



void shot_control_shot_begin()
{
	shot_finish_time = (shot_time_m*60*RTC_TICK_RATE_HZ) + RTC_get_raw();
	
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
	shooting_tasks[1]);
	RTC_set_intertask_period(shot_time_ticks/(steps_per_unit[LIN_AXIS_NUM]*abs(rail_dist)),
	shooting_tasks[2]);
	RTC_set_intertask_period(shot_time_ticks/(steps_per_unit[ROT_AXIS_NUM]*abs(rotA_arc)),
	shooting_tasks[3]);
	
	
	RTC_start_tasks(shooting_tasks, NUM_SHOOT_TASKS);
}

void shot_control_shot_end(void)
{
	RTC_stop_tasks(shooting_tasks, NUM_SHOOT_TASKS);
}
