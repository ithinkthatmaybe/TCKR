// Button.c
// Created 14/11/2015
// Sam Stephenson
// A very simple interrupt driven button library
// for handling the state of, and debouncing
// a single button.


#include "button.h"

#include <util/delay.h> // temporary

button_state_t state = UN_INIT;
button_state_t pushed = UN_INIT; // used by pushed/released_previously functions
button_state_t released = UN_INIT; 

void debounce(void);
RTC_task_handle_t debounce_task_handle;


void button_none(void)
{
	return;
}

void (*press_callback)(void) = &button_none;
void (*release_callback)(void) = &button_none;

// Using Int0 isn't ideal, as this is the second highest priority interrupt
// But the execution time and frequency of this ISR is actually very low
// because most of the work is offloaded to the RTC.
ISR(INT0_vect) //encoder button push
{
	button_int_disable(); // disable button interrupts until debouncing complete
	RTC_start_task(debounce_task_handle);
}


// Debounce by repeatedly polling the pin for a short period
// Finish debouncing when the same value is repeatedly read 5 times
void debounce(void)
{
	static int counter = DEBOUNCE_NUM;
	static int last_val = 0;
	
	int read_val = PIND & BUTTON_PIN;
	
	if (read_val == last_val)
		counter--;
	else
		counter = 5;
	last_val = read_val;
		
	
	if (counter == 0)
	{
		if (read_val == 0x00)
		{
			state = BUTTON_PUSHED;
			pushed = BUTTON_PUSHED;
			#ifdef USE_CALLBACKS
			press_callback();
			#endif
		}
		else
		{
			state = BUTTON_RELEASED;
			released = BUTTON_RELEASED;
			#ifdef USE_CALLBACKS
			release_callback();
			#endif
		}
		RTC_stop_task(debounce_task_handle);
		button_int_enable();
	}
}


void button_init()
{	
	EICRA = 0x01;
	EIMSK = 0x01;
	
	DDRD &= ~(BUTTON_PIN); //Set as input w. pullup
	PORTD |= BUTTON_PIN;
	debounce_task_handle = RTC_push_task(debounce, RTC_HZ_TO_TICKS(DEBOUNCE_POLL_RATE_HZ));
}

void button_int_disable()
{
	EIMSK = 0x00;
}

void button_int_enable()
{
	EIMSK = 0x01;
}

// Edge detection functions
bool button_pushed()
{
	if (state == BUTTON_PUSHED)
	{
		state = BUTTON_DOWN;
		return true;
	}
	else
		return false;
}

bool button_push_occured()
{
	if (pushed == BUTTON_PUSHED)
	{
		pushed = UN_INIT;
		return true;
	}
	else
		return false;
}

bool button_released()
{
	if (state == BUTTON_RELEASED)
	{
		state = BUTTON_UP;
		return true;
	}
	else
		return false;
}

bool button_release_occured()
{
	if (released == BUTTON_RELEASED)
	{
		released = BUTTON_UP;
		return true;
	}
	else
		return false;
}

bool button_up()
{
	return state == button_released() || state == BUTTON_UP;
}


bool button_down()
{
	return (state == BUTTON_PUSHED || state == BUTTON_DOWN);
}


void button_register_press_callback(void func(void))
{
	press_callback = func;
}

void button_register_release_callback(void func(void))
{
	release_callback = func;
}