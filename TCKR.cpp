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
#include <assert.h>
#include <limits.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>


//#include "Arduino.h"
#include "RTC.h"
#include "button.h"
#include "encoder.h"
#include "shot_control.h"
#include "3menu.h"
#include "TLmenu.h"

//==============================================================================

void flash_init(void)
{
	DDRB |= (1<<5);
}

void flash(void)
{
	PORTB ^= (1<<5);
}

int main(void)
{
	cli();	
	
	// Initialisation	
	RTC_init();
	flash_init();
	//RTC_push_task_enabled(&flash, RTC_HZ_TO_TICKS(5)); 	
	button_init();
	encoder_init();
	shot_control_init();	
	TLmenu_init();	
	
	button_register_press_callback(&menu_action_register_select);
	encoder_register_decrement_callback(&menu_action_register_decrement);
	encoder_register_increment_callback(&menu_action_register_increment);
	
	sei();

	while(1)
	{	
		menu_update();
	}
}
		
