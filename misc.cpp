#include "misc.h"
//#include <TZ.h>
double sdt;
long sdt_millis;
void sdt_init(double longitude, int tz)
{
  sdt_millis = millis();
  sdt = sidereal_timeGMT(longitude, tz);
}

double ln_range_degrees (double angle)
{
  double temp;

  if (angle >= 0.0 && angle < 360.0)
    return angle;

  temp = (int)(angle / 360);
  if (angle < 0.0)
    temp --;
  temp *= 360;
  return angle - temp;
}

//input deg ,output hour
double sidereal_timeGMT (double longitude, int tz)
{
  struct timeval now;
  double JD;
  double sidereal, tmp;
  double T;
  gettimeofday(&now, NULL);
  // time_t jnow = time(nullptr);
  JD = (now.tv_sec + now.tv_usec / 1000000.0) / 86400.0 + 2440587.5;
  //JD = (jnow+(millis()%1000)/1000.0)/86400.0+2440587.5;
  T = (JD - 2451545.0) / 36525.0;
  /* calc mean angle */
  sidereal = 280.46061837 + (360.98564736629 * (JD - 2451545.0)) + (0.000387933 * T * T) - (T * T * T / 38710000.0);
  /* add a convenient multiple of 360 degrees */
  sidereal = ln_range_degrees (sidereal + longitude);
  /* change to hours */
  sidereal /= 15.0 ;

  return sidereal;
}

//input rad,deg output rad
double calc_Ra( double lha, double longitude)

{
  double temp;
  double sid = sidereal_timeGMT_alt(longitude) * 15.0;
  temp = (lha * 180.0) / M_PI;
  /*  if  (sid>=temp) return ((sid-lha) *M_PI)/180.0                             ;
    else return  ((sid-lha+360.0) *(M_PI))/180.0;*/
  if  (sid >= temp) return sid * (M_PI / 180.0) - lha;
  else return  sid * (M_PI / 180.0) - lha + 2.0 * M_PI;


}
double calc_lha(double ra, double longitude)
{
  double sid = sidereal_timeGMT(longitude, 1) * 15.0;

  return (sid - (ra * 180.0 / M_PI)) * (M_PI / 180.0);
}


void lxprintde1(char* message, double ang)

{
  if (ang > M_PI) ang = ang - (M_PI * 2.0);

  int x = ang * RAD_TO_DEG * 3600.0;
  char c = '+';
  if (x < 0)
  {
    x = -x;
    c = '-';
  }
  int gra = x / 3600;
  int temp = (x % 3600);
  int min = temp / 60;
  int sec = temp % 60;
  sprintf(message, "%c%02d%c%02d:%02d#", c, gra, 225, min, sec);

};
void lxprintde(char* message, double ang)

{
  if (ang > M_PI) ang = ang - (M_PI * 2.0);

  int x = ang * RAD_TO_DEG * 3600.0;
  char c = '+';
  if (x < 0)
  {
    x = -x;
    c = '-';
  }
  int gra = x / 3600;
  int temp = (x % 3600);
  int min = temp / 60;
  int sec = temp % 60;
  sprintf(message, "%c%02d %02d:%02d", c, gra, min, sec);

};
void lxprintra1(char *message, double ang)
{
  int x = ang * RAD_TO_DEG * 3600.0 / 15.0;
  int gra = x / 3600;
  int temp = (x % 3600);
  int min = temp / 60;
  int sec = temp % 60;
  sprintf(message, "%02d:%02d:%02d#", gra, min, sec);
  //APPEND
};
void lxprintra(char *message, double ang)
{
  int x = ang * RAD_TO_DEG * 3600.0 / 15.0;
  int gra = x / 3600;
  int temp = (x % 3600);
  int min = temp / 60;
  int sec = temp % 60;
  sprintf(message, "%02d:%02d:%02d", gra, min, sec);
  //APPEND
};
void config_NTP(int zone, int dls)
{ 
#ifdef esp8266
  char tx[10];
  int x = zone;
  char c = '-';

 // configTime(TZ_Europe_Madrid, "pool.ntp.org");
  if (x < 0)
  {
    x = -x;
    c = '+';
  }
  sprintf(tx, "GMT%c%x", c, x);

   configTime(tx, "pool.ntp.org");
#else

    configTime(zone * 3600, dls * 3600 , " 193.145.15.15", "0.es.pool.ntp.org", "cuco.rediris.es");
#endif
}

void enc_to_eq(double x, double y, double *a, double  *b, char *pier)
{
  *a = x;
  *pier = true;
  if (y <= M_PI / 2.0) *b = y;
  else if (y < (M_PI * 3 / 2))
  {
    {
      *b = M_PI - y ;
      if (x < M_PI) *a += M_PI ;
      else *a -= M_PI;
      *pier = false;
    }

  }
  else  *b = y - M_PI * 2;

}


void eq_to_enc(double *ra, double *dec, double a, double  b, int pier)
{

  if (!pier)
  {
    *ra = a;
    if (b < 0.0)  *dec = b + 2 * M_PI;
    else *dec = b;
  }
  else
  {
    *dec = M_PI - b;
    if (a >= M_PI) *ra = a - M_PI;
    else *ra = M_PI + a;
  }
}
//output hours
double sidereal_timeGMT_alt(double longitude)
{ double temp = (millis() - sdt_millis) / (1000.0 * 3600.0);
  temp = sdt + temp * K_SID;
  if (temp >= 24.0) return temp - 24.0;
  return temp;
}
int sign(double t)
{
  if (t < 0.0) return -1;
  else if (t == 0.0 )return 0;
  else return 1;

}
