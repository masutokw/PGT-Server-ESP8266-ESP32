#include "mount.h"
#include "misc.h"
#define SID_RATE 15.04106711786691
#define ARC_SEC_LMT 3.5
char sel_flag;

mount_t* create_mount(void)

{


    int maxcounter=AZ_RED;
    int maxcounteralt = ALT_RED;
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
    init_motor( m->azmotor, AZ_ID, maxcounter);
    init_motor( m->altmotor,  ALT_ID, maxcounteralt);
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

    if (Serial.available()>=9)
    {
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
                    mt1->altmotor->targetspeed = -speed
                                                 ;
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

        while(Serial.available())
            Serial.read();

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
int sync_eq(mount_t *mt)
{
    mt->altmotor->slewing= mt->azmotor->slewing=FALSE;
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
    int invert =1 ;
    if (get_pierside(mt)) invert=-1;;
    switch (dir)
    {
    case 'n':

        mt->altmotor->targetspeed=SID_RATE*mt->rate[srate]*SEC_TO_RAD*invert;

        break;
    case 's':

        mt->altmotor->targetspeed=-SID_RATE*mt->rate[srate]*SEC_TO_RAD*invert;
        break;
    case 'w':

        mt->azmotor->targetspeed= SID_RATE*mt->rate[srate]*SEC_TO_RAD;
        break;
    case 'e':

        mt->azmotor->targetspeed=-SID_RATE*mt->rate[srate]*SEC_TO_RAD;
        break;
    };
};
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
    mt->azmotor->slewing=mt->altmotor->slewing=true;
};

int get_pierside(mount_t *mt){

    return (((mt->altmotor->counter) > (mt->altmotor->maxcounter / 4 ))&&((mt->altmotor->counter) < (3 * mt->altmotor->maxcounter / 4 )));
}
void readconfig(mount_t *mt)
{
    //maxcounteraz
    //rateg ratec rateF rateS accel
    //maxcounteralt
    //rateg ratec rateF rateS accel
    //

    //File f = SPIFFS.open("/conf.txt", "r");

    //if (!f) return -1;




}
