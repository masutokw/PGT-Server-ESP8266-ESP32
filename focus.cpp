#include "focus.h"
#include "piclevel.h"
int  focuspeed=20;
int  focuspeed_low=100;
int focusmax=50000;
#define AZ_ID 0xFD
void setfocuserspeed(motor_t* mt,int speed)
{
  aux_set_period(mt->id, speed);
}
void gotofocuser(motor_t* mt,int pos,int speed) {
  int focusdest;
  int count, fspeed;
  if (pos < 1) pos = 0;
  focusdest = pos;
  set_aux_target(mt->id, pos);

  setfocuserspeed(mt,speed * sign(pos - mt->auxcounter));

}
void stopfocuser(motor_t* mt) {
  setfocuserspeed(mt,0);
}
