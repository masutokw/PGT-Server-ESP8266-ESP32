/*
 * Parses LX200 protocol.
 */
#include <string.h>
#include <stdio.h>
#include "mount.h"
#include "misc.h"
#include <math.h>
#define MOVE printf("move ");
#define GOTO printf("GOTO ");
//#define RATE printf ("rateset ");

#define ADD_DIGIT(var,digit) var=var*10+digit-'0';
#define APPEND strcat(response,tmessage);//printf("%s\r\n", buffcom);
char response [200];
char tmessage[50];
const int month_days[] = {31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365};
struct _telescope_
{
    long dec_target,ra_target;
    long alt_target,az_target;
    long current_dec,current_ra;
    long lat,longitude;
    char day,month,year,dayofyear;
}
mount;
extern mount_t *mount_test;
void lxprintde(long x)

{
    char c='+';
    if (x<0)
    {
        x=-x;
        c= '-';
    }

    long gra=x/3600;
    long temp=(x %3600);
    int min=temp/60;
    int sec=temp%60;
    sprintf(tmessage,"%c%02d%c%02d'%02d#",c,gra,223,min,sec);
    APPEND
};

void lxprintra(long x)
{
    long gra=x/3600;
    long temp=(x %3600);
    int min=temp/60;
    int sec=temp%60;
    sprintf(tmessage,"%02d:%02d:%02d#",gra,min,sec);
    APPEND
};
void lxprintdate(void)
{
    printf ("%02d/%02d/%02d#",mount.month,mount.day,mount.year);
}
void lxprintsite(void)
{
    printf("Site Name#");
};
void move(char fc)
{ mount_test->altmotor->slewing= mount_test->azmotor->slewing=FALSE;
int srate=mount_test->srate;
    switch (fc)
    {
    case 'n':

        mount_test->altmotor->targetspeed=SID_RATE*mount_test->rate[srate]*SEC_TO_RAD;

        break;
    case 's':

        mount_test->altmotor->targetspeed=-SID_RATE*mount_test->rate[srate]*SEC_TO_RAD;
        break;
    case 'w':

        mount_test->azmotor->targetspeed= SID_RATE*mount_test->rate[srate]*SEC_TO_RAD;
        break;
    case 'e':

        mount_test->azmotor->targetspeed=-SID_RATE*mount_test->rate[srate]*SEC_TO_RAD;
        break;
    };
};


void ltime(void)
{
    long pj =(long)1;// (timer0GetOverflowCount ()  /30.518);
    lxprintra(pj);
    //;rprintf("1");
}

void set_cmd_exe(char cmd,long date)
{
    switch (cmd)
    {
    case 'r':
        mount.ra_target=date;
        mount_test->azmotor->target=mount_test->ra_target=date*SEC_TO_RAD*15.0;
        break;
    case 'd':
        mount.dec_target=(date) ;

        mount_test->dec_target=date*SEC_TO_RAD;
        if  (mount_test->dec_target<0.0)
        mount_test->altmotor->target=2*M_PI+mount_test->dec_target;else
        mount_test->altmotor->target=mount_test->dec_target;

        break;
    case 'a':
        mount.alt_target=date;
        break;
    case 'z':
        mount.az_target=date ;
        break;
    case 't':
        mount.lat=date ;
        mount_test->lat=date;
        break;
    case 'g':
        mount.longitude=date ;
          mount_test->longitude;
        break;
    case 'L' :
        //timer0SetOverflowCount((long) (30.518 *date));
        break;
    case 'S':
        break;

    }

    //sprintf(tmessage,"1");APPEND
}
void set_date( int day,int month,int year)
{

    mount.month=month;
    mount.day=day;
    mount.year=year;
    mount.dayofyear=day+month_days[month-1];
    if  ((month>2)&&(year%4==0)) mount.dayofyear++;

}
void sync_all(void)
{int temp;
   // mount_test->track=FALSE;
   mount_test->altmotor->slewing= mount_test->azmotor->slewing=FALSE;
    mount.current_dec=mount.dec_target;
    mount_test->altmotor->position= mount_test->altmotor->target;
    temp=( (mount_test->altmotor->position)/(mount_test->altmotor->resolution));
    setposition(mount_test->altmotor,temp);
    //printf(" contador %d    %d \r\n",temp,mount.dec_target);
    mount.current_ra=mount.ra_target;
    setposition( mount_test->azmotor,
    (mount_test->azmotor->position=calc_Ra(mount.ra_target*15.0*SEC_TO_RAD,mount_test->longitude))
    /mount_test->azmotor->resolution);
    sprintf(tmessage,"sync#");
    APPEND

};
void stop(char fc)
{ //mount_test->track=FALSE;
  mount_test->altmotor->slewing= mount_test->azmotor->slewing=FALSE;
    switch (fc)
    {
    case 'n':
        mount_test->altmotor->targetspeed=0.0;
        break;
    case 's':
        mount_test->altmotor->targetspeed=0.0;
        break;
    case 'w':
        mount_test->azmotor->targetspeed= 15.04*SEC_TO_RAD;
        break;
    case 'e':
        mount_test->azmotor->targetspeed=15.04*SEC_TO_RAD;
        break;
    default:
        mount_test->altmotor->targetspeed=0.0;
        mount_test->azmotor->targetspeed=15.04*SEC_TO_RAD;
        break;
    };
}

void rate(char fc)
 {
    switch (fc)
    {
        case 'C':mount_test->srate=1;
        break;
        case 'G':mount_test->srate=0;
        break;
         case 'M':mount_test->srate=2;
        break;
        case 'S':mount_test->srate=3;
        break;


    };

}

//----------------------------------------------------------------------------------------
long command( char *str )
{
    char *p = str, *pe = str + strlen( str );
    int cs;
    char stcmd;
    long deg=0;
    int min=0;
    int sec=0;

    // deg=min=sec=0;
    int neg = 1;
    tmessage[0]=0;
    response[0]=0;

    %%{
        machine command;
        write data;
    }%%




    %%{
#Acciones
        action getgrads {ADD_DIGIT(deg,fc); }
        action getmin {ADD_DIGIT(min,fc); }
        action getsec {ADD_DIGIT(sec,fc); }


        action neg { neg=-1;}
        action dir {move(stcmd);}
        action Goto { sprintf(tmessage,"0");APPEND;
         //mount_test->track=TRUE;
         mount_test->altmotor->slewing= mount_test->azmotor->slewing=TRUE;
         }
        action stop {stop(stcmd);}

        action rate {rate(stcmd); }

        #action return_ra { lxprintra(mount.current_ra);}
        #action return_dec {lxprintde( mount.current_dec);}
        action return_ra { lxprintra1(tmessage,calc_Ra(mount_test->azmotor->position,mount_test->longitude));; APPEND;}
        action return_dec { lxprintde1(tmessage,mount_test->altmotor->position); APPEND;}
        action return_ra_target { lxprintra(mount.ra_target);}
        action return_dec_target {lxprintde( mount.dec_target);}
        action return_date {lxprintdate();}
        action return_site { lxprintsite();}
        action ok {;} # {sprintf(tmessage,"1");}
        action return_longitude {sprintf(tmessage,"-004ß12#");}
        action return_lat {sprintf(tmessage,"+36ß53#");}


        action return_sid_time { ;}
        action sync {sync_all();}

        #action settargetra { set_target_ra( deg,min, sec);}
        #action settargetde { set_target_de( deg,min, sec,neg);}
        action rafrac {deg+=(fc-'0')*6;}
        action return_local_time { ltime();}

        action set_cmd_exec {
            set_cmd_exe(stcmd,(neg*(deg )));
            sprintf(tmessage,"1");APPEND;
            deg=sec=min=0;
        }
        action addmin {deg=deg*3600+min*60;}
        action addsec {deg+=sec;}
        action storecmd {stcmd=fc;}
        action setdate {set_date(sec,min,deg);}

#definicion sintaxis LX terminos auxiliares
        sexmin =  ([0-5][0-9])$getmin@addmin ;
        sex= ([0-5][0-9] )$getsec@addsec;
        deg =(([\+] | [\-]@neg) |(digit @getgrads))(digit @getgrads){2} any  sexmin ([\:]  sex)? ;
        RA = ([0-2] digit) $getgrads   ':' sexmin ('.'digit@rafrac | ':' sex) ;
        date = digit{2}$getmin "/" digit{2}$getsec "/" digit{2}$getgrads ;
#Definicion sintaxis comandos
        Poll= 'G'( 'R'%return_ra | [DZA]%return_dec |'r'%return_ra_target | 'd'%return_dec_target | 'L'%return_local_time |'S'%return_local_time|'C'%return_date|'M'%return_site|'t'%return_longitude|'g'%return_lat);
        Move = 'M' ([nswe]@storecmd %dir | 'S'%Goto);
        Rate = 'R' [CGMS]@storecmd %rate;
        Set='S'(((([dazgt]@storecmd (''|space) deg ) | ([rLS]@storecmd (''|space) RA))%set_cmd_exec)|'C 'date%setdate|'w 3'%ok);
        Sync = "CM"%sync;
        Stop ='Q' (''|[nsew])@storecmd %stop;

        main := (''|'#') (':' (Set | Move | Stop|Rate | Sync | Poll) '#')*  ;
        #1main :=  (':' (Set | Move | Stop|Rate | Sync | Poll) '#')*  ;

# Initialize and execute.
        write init;
        write exec;
    }%%

//---------------------------------------------------------------------------------------------------------------------
    if ( cs < command_first_final )
        //	fprintf( stderr, "LX command:  error\n" );

        return  neg;
};



