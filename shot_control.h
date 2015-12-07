/*
 * shot_control.h
 *
 * Created: 12/5/2015 9:45:16 PM
 *  Author: Sam
 */ 


#ifndef SHOT_CONTROL_H_
#define SHOT_CONTROL_H_



//typedef enum {STATE_IDLE, STATE_PARAM_EDIT, STATE_SHOOTING} main_state_t;


// HW defines
#define PIN 0
#define DDR 1
#define PORT 2


#define TIMELAPSE_FINAL_FPS 33	//Number of pictures to take for a second of video
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


#include <stdlib.h>
#include "RTC.h"

// constants
const int steps_per_unit[2] = {10, 222};


extern int shot_time_m;
extern int clip_time_s;
extern int rail_dist;
extern int rotA_arc;



void shot_control_init(void);  // register all shooting tasks

void shot_control_shutter_activate(void);
void shot_control_shutter_deactivate(void);
void shot_control_rot_move(void);
void shot_control_lin_move(void);

void shot_control_shot_begin(void);
void shot_control_shot_end(void); 




#endif /* SHOT_CONTROL_H_ */