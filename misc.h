#ifndef MISC_H_INCLUDED
#define MISC_H_INCLUDED
#include <ESP8266WiFi.h>
#include <time.h>
#include <sys/time.h>
#include <machine/time.h>
#include <sys/reent.h>
#include <user_interface.h>
#include "sntp.h"
#include <math.h>
#include <stdio.h>
//#include <Arduino.h>
#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE  1
#endif
#define RAD_TO_DEG 180.0/M_PI
#define K_SID 1.00273790935
#define SEC_TO_RAD (M_PI/(3600.0*180.0))
typedef struct timespec wallclock_t;
void sdt_init(double longitude,int tz);
double ln_range_degrees (double angle);
double sidereal_timeGMT_alt(double longitudez);
double sidereal_timeGMT (double longitude,int t);
void lxprintde1(char* message,double ang);
void lxprintde(char* message,double ang);
void lxprintra1(char *message,double ang);
void lxprintra(char *message,double ang);
double calc_Ra(double lha,double longitude);
double calc_lha(double ra,double longitude);
void config_NTP(int zone,int dls );
int sign(double t);

#endif
