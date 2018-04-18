#include "piclevel.h"
#include <math.h>
#include "motor.h"
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

int sign(double t)
{
    if (t<0.0) return -1;
    else if (t==0.0 )return 0;
    else return 1;

}

void init_motor(motor_t* mt,char ref,int maxcounter,double spd,double tick)
{
    mt->speed=0;
    mt->targetspeed=spd;
    mt->pos_angle=0;
    mt->timertick= tick*1e-6;
    mt->maxcounter=maxcounter;//8000*6*180;//
    mt->resolution=(2.0* M_PI) /mt->maxcounter;
    mt->acceleration=6e-4;
    mt->id=ref;
    mt->slewing=0;
    set_motor_max_counter(ref, maxcounter);
}

void setspeed(motor_t* mt ,double tspeed)
{
    int base,postscaler;//ispeed
    // mt->current_speed=tspeed;
    base=0;    //timer2 preload
    postscaler=0;  //PIC timer2 iterations
    if (tspeed!=0.0)
    {
        base=fabs((mt->resolution)/( tspeed*(mt->timertick)));
        postscaler=base / 65535;
        if (postscaler > 0)
        {
            base=base / (postscaler+1);
        }   ;
        postscaler++;
    }
    else base=65535;
    motor_set_period (mt->id,65535-base,sign(tspeed)*postscaler);
}




void setposition(motor_t* mt,double pos)
{
    mt->pos_angle=pos;
    mt->counter=trunc(mt->pos_angle/mt->resolution);
    set_motor_counter(mt->id, mt->counter);
}

void go_to(motor_t* mt,double position,double speed)
{
    mt->slewing=true;
}


void readcounter(motor_t* mt)
{
    int n=readcounters(mt->id);
    if (n!=-1)
    {
        mt->pos_angle= mt->resolution*(mt->counter=n);
        mt->delta= mt->pos_angle-mt->target;
    }
}

void setmaxcounter(motor_t* M,int value)
{
    set_motor_max_counter(M->id,value);
    M->maxcounter=value;
}
void settarget(motor_t* mt,int pos)
{
    set_motor_target(mt->id,pos);
}

void speed_up_down(motor_t* mt)
{
    if  (mt->speed!=mt->targetspeed)
    {
        if  (fabs(mt->targetspeed-mt->speed)<fabs(mt->acceleration))
        {
            mt->speed= mt->targetspeed;
        }
        if (mt->speed< mt->targetspeed) mt->speed=mt->speed+mt->acceleration;
        else if (mt->speed> mt->targetspeed)   mt->speed=mt->speed-mt->acceleration;
        setspeed(mt,mt->speed);
    }
}

void  setcounter(motor_t* mt,int count)
{
    set_motor_counter(mt->id,count);
}

void  loadconf(motor_t* mt,char* name) {}


void  savemotorcounter(motor_t* mt)
{
  save_counters (mt->id);
}

