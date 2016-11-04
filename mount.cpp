#include "mount.h"
//#include <unistd.h>
#include "misc.h"
#define SID_RATE 15.041
char sel_flag;
extern void* sserver(void *);
mount_t* create_mount(void)

{
    //FILE  *fp;

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
void motor_speed_thread_create(void* mt)
{

    //  pthread_create(&th, NULL, thread_motor, mt);
    // if (th)  printf("Motor control thread running ID %d OK \r\n",(int)th);
    // else perror("Opps! thread create failed");
    // return th;

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
            sgndelta = sign (delta = mt1->azmotor->position - calc_Ra(mt1->azmotor->target, mt1->longitude));
            if ( mt1->azmotor->slewing)
            {
                sgndelta = sign (delta = mt1->azmotor->position - calc_Ra(mt1->azmotor->target, mt1->longitude));
                if (fabs(delta) > (M_PI)) sgndelta = -sgndelta;
                if ( fabs(delta / (SEC_TO_RAD)) >= 1.0)
                {
                    speed = fmin(mt1->maxspeed, fabs(delta)) * sgndelta;
                    mt1->azmotor->targetspeed = -(speed) + (SID_RATE * SEC_TO_RAD)
                                                ;
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
    mt->current_dec = dec;
    mt->current_ra = ra;

}
int sync_ra_dec(mount_t *mt)
{
    mt->current_dec = mt->dec_target;
    setposition( mt->altmotor, (mt->altmotor->position = mt->dec_target * SEC_TO_RAD) / mt->altmotor->resolution);
    mt->current_ra = mt->ra_target;
    setposition( mt->azmotor, (mt->azmotor->position = calc_Ra(mt->ra_target * 15.0 * SEC_TO_RAD, mt->longitude)) / mt->azmotor->resolution);

}
int stop_mount(mount_t *mt, char direction)
{
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
