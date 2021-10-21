#ifndef FOCUS_H_INCLUDED
#define FOCUS_H_INCLUDED
#include "misc.h"
#include "motor.h"
void setfocuserspeed(motor_t* mt,int speed);
void gotofocuser(motor_t* mt,int pos,int speed);
void stopfocuser(motor_t* mt);
#endif
