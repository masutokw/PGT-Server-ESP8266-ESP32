#include "piclevel.h"
#include <math.h>
#include "motor.h"
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#define  PIC_COUNTER_TOP   0xFFFF

extern int aux_count;

void init_motor(motor_t* mt,char ref,int maxcounter,double spd,double tick,double accel,int back)
{
    mt->speed=0;
    mt->targetspeed=spd;
    mt->pos_angle=0;
    mt->timertick= tick*1e-6;
    mt->maxcounter=maxcounter;//8000*6*180;//
    mt->resolution=(2.0* M_PI) /mt->maxcounter;
    if (accel > 0.0)mt->acceleration = accel * SEC_TO_RAD ;
    else mt->acceleration = 20e-4;
    //mt->acceleration=6e-4;
    mt->id=ref;
    mt->slewing=0;
    set_motor_max_counter(ref, maxcounter);
    mt->auxcounter=0;
    mt->backslash=back;
    setbackslash(mt,back,1);
    mt->locked=0;
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
        postscaler=base /  PIC_COUNTER_TOP;
        if (postscaler > 0)
        {
            base=base / (postscaler+1);
        }   ;
        postscaler++;
    }
    else base= PIC_COUNTER_TOP-mt->locked;
    motor_set_period (mt->id, PIC_COUNTER_TOP-base,sign(tspeed)*postscaler);
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
        n%=mt->maxcounter;
        mt->auxcounter=aux_count;
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
void setbackslash(motor_t* mt,int back,char act)
{ 
setmotorbackslash(mt->id,abs(back));
set_motor_back_slash_mode(mt->id,(back>0)? 1:0);
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
