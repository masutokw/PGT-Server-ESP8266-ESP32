#include "pad.h"
#include "mount.h"
#define SEC_TO_RAD (M_PI/(3600.0*180.0))
#define SID_RATE 15.04106711786691
extern mount_t *telescope;
volatile int state;
volatile int event=3;
volatile long lastDebounceTime = 0;
const int debounceDelay = 50;
int bpin= 2;
void pad_Init(void){
    pinMode(bpin, INPUT);
     attachInterrupt(bpin, onChange, CHANGE);
}
void onChange() {
  // Get the pin reading.
  int reading = digitalRead(bpin);

  // Ignore dupe readings.
  if(reading == state) return;

  boolean debounce = false;

  // Check to see if the change is within a debounce delay threshold.
  if((millis() - lastDebounceTime) <= debounceDelay) {
    debounce = true;
  }
  // This update to the last debounce check is necessary regardless of debounce state.
  lastDebounceTime = millis();
  // Ignore reads within a debounce delay threshold.
  if(debounce) return;
  // All is good, persist the reading as the state.
  state = reading;
  event =state;
  // Work with the value now.
//  Serial.println("button: " + String(reading));
}
void doEvent(void){
 switch (event)
    {
       case  0:telescope->altmotor->targetspeed=SID_RATE*100*SEC_TO_RAD;event=3;
        break;
        case 1:telescope->altmotor->targetspeed=0;event=3;
        break;
    }}
