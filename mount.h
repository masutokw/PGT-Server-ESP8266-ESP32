#include <ESP8266WiFi.h>
#include "piclevel.h"
#include "motor.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#define SID_RATE 15.041
typedef struct
{
  motor_t *altmotor,*azmotor;
  double dec_target,ra_target;
  double alt_target,az_target;
  double current_dec,current_ra;
  double lat,longitude;
  double rate[4];
  double maxspeed;
  int srate;
  int track;
} mount_t;
void readconfig(void);
mount_t* create_mount(void);
int destroy_mount(mount_t* m);
int set_mount_target( mount_t *mt,double ra,double dec);
int sync_ra_dec(mount_t *mt);
int stop_mount(mount_t *mt,char direction);
void thread_motor(mount_t* m);
void thread_counter(mount_t* mt);
