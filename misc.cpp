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
void lxprintlat1(char *message,double ang)
{

  int x = ang  * 3600.0;
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
  sprintf(message, "%c%02d%c%02d#", c, gra, 225, min);
}

void lxprintlong1(char *message,double ang)
{

  int x = ang * 3600.0;
  char c = '-';
  if (x < 0)
  {
    x = -x;
    c = '+';
  }
  int gra = x / 3600;
  int temp = (x % 3600);
  int min = temp / 60;
  int sec = temp % 60;
  sprintf(message, "%c%03d%c%02d#", c, gra, 225,min);
  }
  void lxprinttime1(char *message)
{
    time_t now;
    struct tm *mtime;
    now = time(nullptr);
    mtime=localtime(&now);
    sprintf(message,"%02d:%02d:%02d#",mtime->tm_hour,mtime->tm_min,mtime->tm_sec);

}
void lxprintdate1(char *message)
{
    time_t now;
    struct tm *mtime;
    now = time(nullptr);
    mtime=localtime(&now);
    if ((mtime->tm_year)<100) mtime->tm_year=100;
    sprintf(message,"%02d/%02d/%02d#",mtime->tm_mon+1,mtime->tm_mday,mtime->tm_year-100);
}
void lxprintGMT_offset(char *message,double offset )
{int gmt=offset;
    char c='+';
    if (offset>0) c='-';

    sprintf(message,"%c%02d.0#",c,gmt);
}
void setclock (int year,int month,int day,int hour,int min,int sec,int gmt)
{
    time_t lxtime;
    struct tm  lxtimeinfo;
     struct timeval tv;

    lxtimeinfo.tm_year = year;
    lxtimeinfo.tm_mon = month;
    lxtimeinfo.tm_mday = day;
    lxtimeinfo.tm_hour = hour;
    lxtimeinfo.tm_min = min;
    lxtimeinfo.tm_sec = sec;
    lxtime= mktime (&lxtimeinfo );
    tv.tv_sec = lxtime;
    tv.tv_usec = 0;
    settimeofday(&tv, nullptr);

}

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
   setenv ("TZ", tx, 1);
   tzset (); 
#else
    configTime(zone * 3600, dls * 3600 , "pool.ntp.org");
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

//Preccesion from libnova
void ln_get_equ_prec (double mean_ra,double mean_dec, double JD,double  *position_ra,double *position_dec)
{
   double t, t2, t3, A, B, C, zeta, eta, theta, ra, dec;

  /* change original ra and dec to radians */
  mean_ra = mean_ra /(RAD_TO_DEG);
  mean_dec = mean_dec/(RAD_TO_DEG);

  /* calc t, zeta, eta and theta for J2000.0 Equ 20.3 */
  t = (JD - JD2000) / 36525.0;
  t *= 1.0 / 3600.0;
  t2 = t * t;
  t3 = t2 *t;
  zeta = 2306.2181 * t + 0.30188 * t2 + 0.017998 * t3;
  eta = 2306.2181 * t + 1.09468 * t2 + 0.041833 * t3;
  theta = 2004.3109 * t - 0.42665 * t2 - 0.041833 * t3;
  zeta =  (zeta)/(RAD_TO_DEG);
  eta =  (eta)/(RAD_TO_DEG);;
  theta =  (theta)/(RAD_TO_DEG);;

  /* calc A,B,C equ 20.4 */
  A = cos (mean_dec) * sin (mean_ra + zeta);
  B = cos (theta) * cos (mean_dec) * cos (mean_ra + zeta) - sin (theta) * sin (mean_dec);
  C = sin (theta) * cos (mean_dec) * cos (mean_ra + zeta) + cos (theta) * sin (mean_dec);

  ra = atan2 (A,B) + eta;

  /* check for object near celestial pole */
  if (mean_dec > (0.4 * M_PI) || mean_dec < (-0.4 * M_PI)) {
    /* close to pole */
    dec = acos (sqrt(A * A + B * B));
    if (mean_dec < 0.)
      dec *= -1; /* 0 <= acos() <= PI */
  } else {
    /* not close to pole */
    dec = asin (C);
  }

  /* change to degrees */
  *position_ra = ln_range_degrees ((RAD_TO_DEG)* (ra));
  *position_dec =(RAD_TO_DEG) * (dec);
}
