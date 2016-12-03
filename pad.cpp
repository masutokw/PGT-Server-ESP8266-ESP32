#include "pad.h"
#include "mount.h"
extern mount_t *telescope;
volatile int state[4]= {1,1,1,1};
volatile int event[4]= {3,3,3,3};
volatile long lastDebounceTime[4] = {0,0,0,0};
const int debounceDelay = 50;
int bpin[4]= {2,0,4,5};


void onChange(int b)
{
    int reading = digitalRead(bpin[b]);
    if(reading == state[b]) return;
    boolean debounce = false;
    if((millis() - lastDebounceTime[b]) <= debounceDelay)
    {
        debounce = true;
    }
    lastDebounceTime[b] = millis();
    if(debounce) return;
    state[b] = reading;
    event[b] =state[b];

}
void onChange_North(void)
{
    onChange(0);
}
void onChange_South(void)
{
    onChange(1);
}
void onChange_West(void)
{
    onChange(2);
}
void onChange_East(void)
{
    onChange(3);
}
void doEvent(void)
{
    int n;

    switch (event[0])
    {
    case  0:
       if (digitalRead(16)) mount_move(telescope,'n');
       else telescope->srate=(telescope->srate+1)%4;
        event[0]=3;
        break;
    case 1:
        if (digitalRead(16)) mount_stop(telescope,'n');
        event[0]=3;
        break;
    }

    switch (event[1])
    {
    case  0:
        mount_move(telescope,'s');
        event[1]=3;
        break;
    case 1:
        mount_stop(telescope,'s');
        event[1]=3;
        break;
    }
    switch (event[2])
    {
    case  0:
        mount_move(telescope,'w');
        event[2]=3;
        break;
    case 1:
        mount_stop(telescope,'w');
        event[2]=3;
        break;
    }

    switch (event[3])
    {
    case  0:
        mount_move(telescope,'e');
        event[3]=3;
        break;
    case 1:
        mount_stop(telescope,'e');
        event[3]=3;
        break;
    }

}

void pad_Init(void)
{
    pinMode(bpin[0], INPUT_PULLUP);
    pinMode(bpin[1], INPUT_PULLUP);
    pinMode(bpin[2], INPUT_PULLUP);
    pinMode(bpin[3], INPUT_PULLUP);
    pinMode(16,INPUT_PULLUP);
    attachInterrupt(bpin[0], onChange_North, CHANGE);
    attachInterrupt(bpin[1], onChange_South, CHANGE);
    attachInterrupt(bpin[2], onChange_West, CHANGE);
    attachInterrupt(bpin[3], onChange_East, CHANGE);

}

