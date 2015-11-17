// Button.h
// Created 14/11/2015
// Sam Stephenson
// A very simple interrupt driven button library
// for handling the state of, and debouncing 
// a single button. 

#ifndef BUTTON_H_
#define BUTTON_H_


#include <stdlib.h>
#include <stdbool.h>
#include <avr/interrupt.h>

#include "RTC.h"

#define DEBOUNCE_POLL_RATE_HZ 200 // During debounce poll button at x hertz
#define DEBOUNCE_NUM 5 // number of consecutive reads for debouncing to be considered a successs

#define BUTTON_PIN (1<<2) //PORTD

typedef enum states {UN_INIT, BUTTON_PUSHED, BUTTON_DOWN, BUTTON_RELEASED, BUTTON_UP} button_state_t;

void button_init();

void button_int_disable();

void button_int_enable();

// Edge detection functions
bool button_pushed();
bool button_released();

// Check for an unregistered button edge event, regardless of current state
bool button_push_occured(); 
bool button_release_occured();

bool button_up();
bool button_down();




#endif //BUTTON_H_
