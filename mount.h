#ifndef MOUNT_H_INCLUDED
#define MOUNT_H_INCLUDED
#include <ESP8266WiFi.h>
#define COUNTERS_POLL_TICKER 100
#include <FS.h>
#include "piclevel.h"
#include "motor.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#define SID_RATE 15.04106711786691
#define ARC_SEC_LMT 1.0
#define AZ_RED 8000*6*180
#define ALT_RED  8000*6*180
#define RATE_GUIDE 0.5
#define RATE_CENTER 8
#define RATE_FIND 50
#define RATE_SLEW 290
#define LOCAL_LONGITUDE -4.2
#define LOCAL_LATITUDE 36.72
#define ALT_ID 0XFD
#define AZ_ID 0xFE
#define TIME_ZONE 1

typedef struct
{
    motor_t *altmotor,*azmotor;
    double dec_target,ra_target;//radians
    double alt_target,az_target;
    double lat,longitude;
    double rate[4][2];
    double prescaler;
    double maxspeed[2];
    int srate;
    int track;
    int time_zone;
} mount_t;

mount_t* create_mount(void);
int readconfig(mount_t *mt);
int destroy_mount(mount_t* m);
int set_mount_target( mount_t *mt,double ra,double dec);
int sync_eq(mount_t *mt);
void mount_move(mount_t *mt,char direction);
int mount_stop(mount_t *mt,char direction);
int mount_slew(mount_t *mt);
void select_rate(mount_t *mt,char rate);
void thread_motor(mount_t* m);
void thread_counter(mount_t* mt);
int get_pierside(mount_t *mt);
void eq_to_enc(double *ra,double *dec,double a,double  b,int pier);
void mount_lxde_str(char* message,mount_t *mt);
void mount_lxra_str(char *message,mount_t *mt);
void mount_park(mount_t *mt);
#endif
