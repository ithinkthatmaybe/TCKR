// Created 9/11/2015 
// Sam Stephenson

// Currently this just looks at the previous state after 
// entering the 11 state (see below) which occasionally falsely inc/decrements. 
// This could be improved by following the state transitions below.

//  State
//Hex |	encoder Channels
//0x03	1-1	 ___
//	     |		|
//0x01	0-1		|
//	     |		|Increment
//0x00	0-0		|
//	     |		|
//0x02	1-0		|
//		 |		|
//0x03 	1-1	 <--
//	     |		|
//0x02	1-0		|
//		 |		|Decrement
//0x00	0-0		|
//	     |		|
//0x00	0-1		|
//	 	 |		|
//0x00	1-1 ____|


#ifndef ENCODER_H
#define ENCODER_H


// Encoder module can be otherwise used in the background or 
// from a shared ISR using the encoder update function
// Comment out to do so
#define USE_INTTERUPT

#define DEBOUNCE_PERIOD_US 200

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>
#include <util/delay.h>

#include "RTC.h"


void encoder_init(void);
void encoder_update(char state);

// Return the number of increments / decrements read by the encoder
int encoder_get_diff();

int encoder_get_val();
void encoder_set_val(int val);


#endif //ENCODER_H