#ifndef MOTOR_H_INCLUDED
#define MOTOR_H_INCLUDED
#include <ESP8266WiFi.h>
#include <math.h>
#define TIM_TICK 4e-7
#define SEC_TO_RAD (M_PI/(3600.0*180.0))
typedef struct
{
    double pos_angle,target,delta,resolution,
           targetspeed,speed,current_speed,
           acceleration,maxrange,min_range,
           timertick;
    int counter,maxcounter,range,slewing;
    char id;
} motor_t;

int sign(double t);

void init_motor(motor_t* mt,char ref,int maxcounter,double spd,double tick);
double getposition(motor_t* mt);
void  setspeed(motor_t* mt,double tspeed);
void  setmaxcounter(motor_t* M,int value);
void  readcounter(motor_t* mt);
void  setposition(motor_t* mt,double pos);
void  settarget(motor_t* mt,int pos);
void  go_to(motor_t* mt,double position,double speed);
void  speed_up_down(motor_t* mt);
void  setcounter(motor_t* mt,int count);
void  loadconf(motor_t* mt,char* name);
void  savemotorcounter(motor_t* mt);
#endif




