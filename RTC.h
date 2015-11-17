// 10/11/15
// Sam Stephenson
//
// A real time clock with un-prioritised foreground task scheduling
//
// Currently tasks can only be pushed and popped from a stack, so active tasks at runtime should be 
// controlled with the start/stop task functions. 
//
// Tasks are not enabled by default
//
// After starting a task, the task will not execute for the set intertask period.
// 
// Also tasks are scheduled for a single execution by scheduling normally and stopping the task
// within the task.

/*
* TODOS: Make a nice task struct to get rid of parallel arrays
*/


#ifndef RTC_H
#define RTC_H

#include <avr/io.h> // Temporary 
#include <avr/interrupt.h>
#include <util/atomic.h>

typedef unsigned int RTC_task_handle_t;

#ifndef RTC_TASK_STACK_SIZE
#define RTC_TASK_STACK_SIZE 10
#endif

#ifndef RTC_TICK_RATE_HZ
#define RTC_TICK_RATE_HZ 2000UL
#endif

#define RTC_HZ_TO_TICKS(HZ) (RTC_TICK_RATE_HZ/HZ)
#define RTC_MS_TO_TICKS(MS) (RTC_TICK_RATE_HZ/1000*MS)
#define RTC_US_TO_TICKS(US) (RTC_TICK_RATE_HZ*US/1E6)

void RTC_init();

unsigned long RTC_get_ms();
unsigned long RTC_get_us();

unsigned long RTC_get_raw();
void RTC_set_raw(long val);


RTC_task_handle_t RTC_push_task(void task(void), unsigned long inter_task_period);	// Add to a stack of tasks to be executed periodically
RTC_task_handle_t RTC_push_task_enabled(void task(void), unsigned long inter_task_period);
void RTC_pop_task(void);	// pop top task off stack
void RTC_pop_tasks(int num_tasks);


void RTC_start_task(RTC_task_handle_t task_num);
void RTC_stop_task(RTC_task_handle_t task_num);
void RTC_toggle_task(RTC_task_handle_t task_num); 

void RTC_start_tasks(RTC_task_handle_t task_nums[], int num_tasks);
void RTC_stop_tasks(RTC_task_handle_t task_nums[], int num_tasks);

void RTC_set_intertask_period(unsigned int interval, RTC_task_handle_t task_num);


#endif //RTC_H