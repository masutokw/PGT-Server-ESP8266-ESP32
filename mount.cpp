#include "mount.h"
#include "misc.h"
char sel_flag;

mount_t* create_mount(void)

{
    int maxcounter=AZ_RED;
    int maxcounteralt =ALT_RED;
    mount_t *m;
    m = (mount_t*)malloc(sizeof(mount_t));
    //if (m) return NULL;
    m->azmotor = (motor_t*)malloc(sizeof(motor_t));
    m->altmotor = (motor_t*)malloc(sizeof(motor_t));
    m->track = 0;
    m->rate[3] = RATE_SLEW;
    m->rate[2] = RATE_FIND;
    m->rate[1] = RATE_CENTER;
    m->rate[0] = RATE_GUIDE;
    m->srate = 0;
    m->maxspeed = (m->rate[3] * SID_RATE * SEC_TO_RAD);
    m->longitude = LOCAL_LONGITUDE;
    m->lat = LOCAL_LATITUDE;
    m->time_zone = TIME_ZONE;
    m->prescaler=0.4;
    init_motor( m->azmotor, AZ_ID, maxcounter,SID_RATE * SEC_TO_RAD,m->prescaler);
    init_motor( m->altmotor,  ALT_ID, maxcounteralt,0,m->prescaler);
    
    return m;
}

int  destroy_mount(mount_t* m)
{
    free(m->azmotor);
    free(m->altmotor);
    free(m);
}

void thread_motor(mount_t* m)
{
    speed_up_down(((mount_t*)m)->altmotor);
    speed_up_down(((mount_t*)m)->azmotor);
}

void thread_counter(mount_t* mt1)
{
    double  s;
    double delta;
    double sgndelta;
    double speed;
    int aval=Serial.available();
    if (aval>=9)
    {
        while(Serial.available()>9) Serial.read();
        if (sel_flag)
        {
            readcounter(mt1->altmotor);
            //goto -------------------------------------------------------------------------
            if ( mt1->altmotor->slewing)
            {
                sgndelta = (sign(delta = mt1->altmotor->delta));
                if (fabs(delta) > (M_PI)) sgndelta = -sgndelta;

                if ( fabs(delta / (SEC_TO_RAD)) >= 1.0)
                {
                    speed = fmin(mt1->maxspeed, fabs(delta)) * sgndelta;
                    mt1->altmotor->targetspeed = -speed;
                }
                else
                {
                    mt1->altmotor->targetspeed = 0.0;
                    mt1->altmotor->slewing = 0;
                }
            }
        }
        else
        {
            readcounter(mt1->azmotor);
            sgndelta = sign (delta =mt1->azmotor->delta= mt1->azmotor->pos_angle - calc_Ra(mt1->azmotor->target, mt1->longitude));
            if ( mt1->azmotor->slewing)
            {
                //  sgndelta = sign (delta =mt1->azmotor->delta= mt1->azmotor->pos_angle - calc_Ra(mt1->azmotor->target, mt1->longitude));
                if (fabs(delta) > (M_PI)) sgndelta = -sgndelta;
                if ( fabs(delta / (SEC_TO_RAD)) > ARC_SEC_LMT)
                {
                    speed = fmin(mt1->maxspeed, fabs(delta)) * sgndelta;
                    mt1->azmotor->targetspeed = -(speed) + (SID_RATE * SEC_TO_RAD);
                }
                else
                {
                    mt1->azmotor->targetspeed = SID_RATE * SEC_TO_RAD;
                    mt1->azmotor->slewing = 0;
                }
            }
        }

        while(Serial.available())            Serial.read();
    };
    if (sel_flag) pollcounters(254) ;
    else pollcounters(253);
    sel_flag=!sel_flag;
}

int set_mount_target( mount_t *mt, double ra, double dec)
{
    // mt->current_dec = dec;
    // mt->current_ra = ra;

}
int sync_eq_old(mount_t *mt)
{
    mt->altmotor->slewing= mt->azmotor->slewing=FALSE;
    setposition(mt->altmotor,mt->altmotor->target);
    setposition( mt->azmotor,calc_Ra(mt->azmotor->target,mt->longitude));
    mt->altmotor->slewing= mt->azmotor->slewing=FALSE;
}

int sync_eq(mount_t *mt)
{
    mt->altmotor->slewing= mt->azmotor->slewing=FALSE;

    eq_to_enc(&(mt->azmotor->target),&(mt->altmotor->target),
              mt->ra_target,mt->dec_target,get_pierside(mt));

    setposition(mt->altmotor,mt->altmotor->target);
    setposition( mt->azmotor,calc_Ra(mt->azmotor->target,mt->longitude));
    mt->altmotor->slewing= mt->azmotor->slewing=FALSE;
}

int mount_stop(mount_t *mt, char direction)
{
    mt->altmotor->slewing= mt->azmotor->slewing=FALSE;
    switch (direction)
    {
    case'n':
    case's':
        mt->altmotor->targetspeed = 0.0;
        break;
    case 'w':
    case 'e':
        mt->azmotor->targetspeed = SID_RATE * SEC_TO_RAD;
        break;
    default:
        mt->altmotor->targetspeed = 0.0;
        mt->azmotor->targetspeed = SID_RATE * SEC_TO_RAD;
        break;
    };
}
void mount_move(mount_t *mt,char dir)
{
    mt->altmotor->slewing= mt->azmotor->slewing=FALSE;
    int srate=mt->srate;
    int invert =(get_pierside(mt))? -1:1;
    int  sid = (srate==0)? 1:-1;
    switch (dir)
    {
    case 'n':
        mt->altmotor->targetspeed=SID_RATE*mt->rate[srate]*SEC_TO_RAD*invert;
        break;
    case 's':
        mt->altmotor->targetspeed=-SID_RATE*mt->rate[srate]*SEC_TO_RAD*invert;
        break;
    case 'w':
        mt->azmotor->targetspeed= SID_RATE*(mt->rate[srate]+sid)*SEC_TO_RAD;
        break;
    case 'e':
        mt->azmotor->targetspeed=-SID_RATE*(mt->rate[srate]-sid)*SEC_TO_RAD;
        break;
    };
}

void select_rate(mount_t *mt,char dir)
{
    switch (dir)
    {
    case 'C':
        mt->srate=1;
        break;
    case 'G':
        mt->srate=0;
        break;
    case 'M':
        mt->srate=2;
        break;
    case 'S':
        mt->srate=3;
        break;


    };

}

int mount_slew(mount_t *mt)
{
    eq_to_enc(&(mt->azmotor->target),&(mt->altmotor->target),
              mt->ra_target,mt->dec_target,get_pierside(mt));
    mt->azmotor->slewing=mt->altmotor->slewing=true;
}

int get_pierside(mount_t *mt)
{

    return (((mt->altmotor->counter) > (mt->altmotor->maxcounter / 4 ))&&((mt->altmotor->counter) < (3 * mt->altmotor->maxcounter / 4 )));
}

void mount_lxra_str(char *message,mount_t *mt)
{

    double ang=calc_Ra(mt->azmotor->pos_angle,mt->longitude);
    if (get_pierside(mt))
    {
        if (ang<M_PI) ang+=M_PI;
        else ang-=M_PI;
    }
    int x = ang*RAD_TO_DEG*3600.0/15.0;
    int gra=x/3600;
    int temp=(x %3600);
    int min=temp/60;
    int sec=temp%60;
    sprintf(message,"%02d:%02d:%02d#",gra,min,sec);
};

void mount_lxde_str(char* message,mount_t *mt)

{
    double ang =mt->altmotor->pos_angle;
    if (ang>1.5*M_PI) ang=ang-(M_PI*2.0)
                              ;
    else if (ang>M_PI/2.0) ang=M_PI-ang;

    int x = ang*RAD_TO_DEG*3600.0;
    char c='+';
    if (x<0)
    {
        x=-x;
        c= '-';
    }
    int gra=x/3600;
    int temp=(x %3600);
    int min=temp/60;
    int sec=temp%60;
    sprintf(message,"%c%02d%c%02d:%02d#",c,gra,225,min,sec);


};

int readconfig(mount_t *mt)
{
    int maxcounter,maxcounteralt ;
    File f = SPIFFS.open("/mount.config", "r");
    if (!f) return -1;
    String s=f.readStringUntil('\n');
    maxcounter=s.toInt();
    s=f.readStringUntil('\n');
    maxcounteralt=s.toInt();
    for (int n=0; n<4; n++)
    {
        s=f.readStringUntil('\n');
        mt->rate[n] = s.toFloat();
    }
    mt->srate = 0;
    mt->maxspeed = (mt->rate[3] * SID_RATE * SEC_TO_RAD);
     s=f.readStringUntil('\n');
    mt->prescaler=s.toFloat();
    if ((mt->prescaler <0.3)|| (mt->prescaler >2.0)) mt->prescaler=0.4;
    s=f.readStringUntil('\n');
    mt->longitude = s.toFloat();
    s=f.readStringUntil('\n');
    mt->lat = s.toFloat();
     s=f.readStringUntil('\n');
    mt->time_zone = s.toFloat();
    init_motor( mt->azmotor, AZ_ID, maxcounter,SID_RATE * SEC_TO_RAD,mt->prescaler);
    init_motor( mt->altmotor,  ALT_ID, maxcounteralt,0,mt->prescaler);
    return 0;


}
