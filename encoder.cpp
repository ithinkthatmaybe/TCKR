// Created 9/11/15
// Sam Stephenson
// Look after a rotary encoder

#include "encoder.h"

int g_val = 0;

#ifdef USE_INTTERUPT
ISR(PCINT2_vect)
{
	/*	
	static unsigned long t_last = 0;	
	unsigned long t = RTC_get_raw();

	if (t > (t_last + RTC_TICK_RATE_HZ/500))
	{
		t_last = t;
		char state = (PIND>>3);
		encoder_update(state);
	}
	*/
	
	char state = (PIND>>3);
	encoder_update(state);
	_delay_ms(2);	
}
#endif


void encoder_init(void)
{
	g_val = 0;	
	DDRD &= ~((1<<3)|(1<<4));
	PORTD |= (1<<3)|(1<<4);

#ifdef USE_INTTERUPT	
	PCICR |= (1<<2);
	PCMSK2 = (1<<3)|(1<<4);
#endif
}

// 
void encoder_update(char state)
{
	static int state_last;
		
	if (state == 0x03)
	{
		if (state_last == 0x01)
			g_val++;
		if (state_last == 0x02)
			g_val--;
	}		
	state_last = state;
}

// Return the number of increments / decrements read by the encoder
int encoder_get_diff()
{
	static int last_read = 0;	
	int result =  g_val - last_read;
	last_read = g_val;
	return result;
}

int encoder_get_val()
{
	return g_val;
}

void encoder_set_val(int new_val)
{
	g_val = new_val;
}
