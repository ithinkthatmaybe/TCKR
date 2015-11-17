
#include "RTC.h"


unsigned long g_ulTicks = 0;

#define CLKDIV 256
#define RTC_PRESCALE 0x04

// Task scheduling arrays
void (*task_buffer[RTC_TASK_STACK_SIZE])();
int task_enabled[RTC_TASK_STACK_SIZE] = {0};
unsigned long task_interval[RTC_TASK_STACK_SIZE] = {0};
unsigned long task_t_last[RTC_TASK_STACK_SIZE] = {0};
	
unsigned int stacked_tasks = 0;

ISR(TIMER0_COMPA_vect)
{
	cli();
	g_ulTicks++;	
	
	unsigned int i;
	for (i = 0; i < stacked_tasks; i++)
	{
		if (g_ulTicks >= (task_t_last[i]+task_interval[i]))
		{
			if (task_enabled[i])
			{				
				task_t_last[i] = g_ulTicks;
				(*task_buffer[i])();				
			}
		}
	}
	/*
	static int barrier = 0;
	//Only allow one instance of the ISR into the task scheduling block
	if (barrier == 0)
	{
		barrier = 1;
		//sei(); // Enable interrupt preemption
		unsigned int i;
		for (i = 0; i< stacked_tasks; i++)
		{
			if (task_enabled[i] && (g_ulTicks >= (task_t_last[i]+task_interval[i])))
			{				
				task_t_last[i] = g_ulTicks;
				(*task_buffer[i])();
			}
		}
		barrier = 0;
	}
	*/
	sei();
}

RTC_task_handle_t RTC_push_task(void (*task)(void), unsigned long inter_task_period)
{
	task_buffer[stacked_tasks] = task;
	task_interval[stacked_tasks] = inter_task_period;
	cli();
	task_t_last[stacked_tasks] = g_ulTicks;
	sei();
	RTC_task_handle_t task_num = stacked_tasks;
	stacked_tasks++;
	return task_num;
}

RTC_task_handle_t RTC_push_task_enabled(void (*task)(void), unsigned long inter_task_period)
{
	RTC_task_handle_t task_handle = RTC_push_task(task, inter_task_period);
	RTC_start_task(task_handle);
	return task_handle;
}


void RTC_pop_task(void)
{
	stacked_tasks--;
}

void RTC_pop_tasks(int num_tasks)
{
	int i;
	for (i = 0; i < num_tasks; i++)
		RTC_pop_task();
}

void RTC_start_task(RTC_task_handle_t task_num)
{
	cli();
	task_t_last[task_num] = g_ulTicks;
	task_enabled[task_num] = 1;
	sei();
}

void RTC_stop_task(RTC_task_handle_t task_num)
{
	task_enabled[task_num] = 0;
}

void RTC_toggle_task(RTC_task_handle_t task_num)
{
	task_enabled[task_num] ^= 1;
}

void RTC_start_tasks(RTC_task_handle_t task_nums[], int num_tasks)
{
	int i;
	for(i = 0; i < num_tasks; i++)
		RTC_start_task(task_nums[i]);
}

void RTC_stop_tasks(RTC_task_handle_t task_nums[], int num_tasks)
{
	int i;
	for (i = 0; i < num_tasks; i++)
		RTC_stop_task(task_nums[i]);
}

void RTC_set_intertask_period(unsigned int interval, RTC_task_handle_t task_num)
{
	task_interval[task_num] = interval;
}



void RTC_init()
{
	TCCR0A = (0x03<<6) | 0x02; 	// Set OC0A on match | CTC mode
	TCCR0B = RTC_PRESCALE;
	OCR0A = F_CPU/CLKDIV/RTC_TICK_RATE_HZ;	
	//OCR0A = 1;
	TIMSK0 = 0x02;			// Enable compare match Interrupt
}

void RTC_halt()
{
	TCCR0B &= ~0x07;
}

void RTC_start()
{
	TCCR0B |= RTC_PRESCALE;
}

unsigned long RTC_get_ms()
{
	cli();
	unsigned long result = g_ulTicks/RTC_TICK_RATE_HZ*1000;
	sei();
	return result;
}

unsigned long RTC_get_us()
{
	cli();
	unsigned long result = g_ulTicks*(1000000/RTC_TICK_RATE_HZ);
	sei();
	return result;
}

unsigned long RTC_get_raw()
{
	cli();
	unsigned long result = g_ulTicks;
	sei();
	return result;
}

//void RTC_set_ms();
//void RTC_set_us();
void RTC_set_raw(long val)
{
	cli();
	g_ulTicks = val;
	sei();
}