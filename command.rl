/*
 * Parses LX200 protocol.
 */

#define ADD_DIGIT(var,digit) var=var*10+digit-'0';
#define APPEND strcat(response,tmessage);
#define SYNC_MESSAGE "sync#"
//#define SYNC_MESSAGE "Coordinates     matched.        #"

#include <string.h>
#include <stdio.h>
#include "mount.h"
#include "misc.h"
#include <math.h>

char response [200];
char tmessage[50];
const int month_days[] = {31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365};
struct _telescope_
{
    char day,month,year,dayofyear;
}
mount;
extern mount_t *telescope;
void lxprintdate(void)
{
    printf ("%02d/%02d/%02d#",mount.month,mount.day,mount.year);
}
void lxprintsite(void)
{
    printf("Site Name#");
};

void ltime(void)
{
    long pj =(long)1;
}

void set_cmd_exe(char cmd,long date)
{
    switch (cmd)
    {
    case 'r':
        telescope->azmotor->target=telescope->ra_target=date*SEC_TO_RAD*15.0;
        break;
    case 'd':

        telescope->dec_target=date*SEC_TO_RAD;
        if  (telescope->dec_target<0.0)
            telescope->altmotor->target=2*M_PI+telescope->dec_target;
        else
            telescope->altmotor->target=telescope->dec_target;
        break;
    case 'a':
        telescope->alt_target=date*SEC_TO_RAD;

        break;
    case 'z':
        telescope->az_target=date*SEC_TO_RAD;
        break;
    case 't':
        telescope->lat=date/3600.0;
        break;
    case 'g':
        telescope->longitude=(-date/3600.0);
        break;
    case 'L' :
        //timer0SetOverflowCount((long) (30.518 *date));
        break;
    case 'S':
        break;

    }


}
void set_date( int day,int month,int year)
{

    mount.month=month;
    mount.day=day;
    mount.year=year;
    mount.dayofyear=day+month_days[month-1];
    if  ((month>2)&&(year%4==0)) mount.dayofyear++;

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
        action dir {mount_move(telescope,stcmd);}
        action Goto {mount_slew(telescope); sprintf(tmessage,"0");APPEND;}
        action stop {mount_stop(telescope,stcmd);}
        action rate {select_rate(telescope,stcmd); }
        action return_ra  {mount_lxra_str(tmessage,telescope);APPEND;}
        action return_dec {mount_lxde_str(tmessage,telescope);APPEND;}
        action return_ra_target { ;}
        action return_dec_target {;}
        action return_date {lxprintdate();}
        action return_site { lxprintsite();}
        action ok {;} # {sprintf(tmessage,"1");}
        action return_longitude {sprintf(tmessage,"+004*12#");}
        action return_lat {sprintf(tmessage,"+36*43#");}
        action return_sid_time { ;}
        action sync {sync_eq(telescope);sprintf(tmessage,SYNC_MESSAGE);
                    telescope->altmotor->slewing= telescope->azmotor->slewing=FALSE;
                    APPEND;}
        action rafrac {deg+=(fc-'0')*6;}
        action return_local_time { ltime();}
        action set_cmd_exec {
            set_cmd_exe(stcmd,(neg*(deg )));
            sprintf(tmessage,"1");
            APPEND;
            deg=sec=min=0;
        }
        action addmin {deg=deg*3600+min*60;}
        action addsec {deg+=sec;}
        action storecmd {stcmd=fc;}
        action setdate {set_date(sec,min,deg);}

#definicion sintaxis LX terminos auxiliares
        sexmin =  ([0-5][0-9])$getmin@addmin ;
        sex= ([0-5][0-9] )$getsec@addsec;
        deg =(([\+] | [\-]@neg) |(digit @getgrads))(digit @getgrads){2} any sexmin ([\:]  sex)? ;
        RA = ([0-2] digit) $getgrads   ':' sexmin ('.'digit@rafrac | ':' sex) ;
        date = digit{2}$getmin "/" digit{2}$getsec "/" digit{2}$getgrads ;
#Definicion sintaxis comandos
        Poll= 'G'( 'R'%return_ra | [DZA]%return_dec |'r'%return_ra_target | 'd'%return_dec_target | 'L'%return_local_time |'S'%return_local_time|'C'%return_date|'M'%return_site|'t'%return_longitude|'g'%return_lat);
        Move = 'M' ([nswe]@storecmd %dir | 'S'%Goto);
        Rate = 'R' [CGMS]@storecmd (''|[0-4]) %rate;
        Set='S'(((([dazgt]@storecmd (''|space) deg ) | ([rLS]@storecmd (''|space) RA))%set_cmd_exec)|'C 'date%setdate|'w 3'%ok);
        Sync = "CM"(''|'R')%sync;
        Stop ='Q' (''|[nsew])@storecmd %stop;
        main := (''|'#') (':' (Set | Move | Stop|Rate | Sync | Poll) '#')*  ;

# Initialize and execute.
        write init;
        write exec;
    }%%

//---------------------------------------------------------------------------------------------------------------------
    if ( cs < command_first_final )
        //	fprintf( stderr, "LX command:  error\n" );

        return  neg;
};



