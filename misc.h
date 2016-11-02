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
typedef struct timespec wallclock_t;

double ln_range_degrees (double angle);
double sidereal_timeGMT (double longitude);
void lxprintde1(char* message,double ang);
void lxprintra1(char *message,double ang);
double calc_Ra(double lha,double longitude);
double calc_lha(double ra,double longitude);
void config_NTP(int zone,int dls );
