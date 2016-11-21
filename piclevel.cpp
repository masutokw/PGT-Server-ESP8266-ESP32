#include <ESP8266WiFi.h>
#include "piclevel.h"
//PICMSG msg = {  {0, 's', 'd'},0 };
#define ASGN msg.id = msg.idd= id;msg.command
#define SEND_COMMAND  Serial.write((char *) &msg, msg.len)
picmsg  msg = {CMD_FCODE, 0xFE, 0xFE, 0xFF, 0, 0};

int h, v;

/*int readcounters (char id)
{
    PICRSP rsp;

    int  count, countr;
    ASGN = COUNTERS_GET_CMD;
    msg.len = 4;
    // Clean_Buffer(com_handler);
    SEND_COMMAND;
    //delay(7);
    countr = 0;
    count = 9;
    yield();
    //countr = Serial.readBytes((char *)&rsp,count);
    countr = Serial.readBytes((char *)&rsp,count);
    if ((countr == 9) && (rsp.control == RESPONSE_OK))
    {
        return rsp.counter;
    }

    else
    {
        // printf("com error %d %d \n\r",count,id);
        //  Clean_Buffer(com_handler);
        return -1;
    }
    //  Clean_Buffer(com_handler);
};
*/
void pollcounters(char id)
{
    PICRSP rsp;

    int  count, countr;
    ASGN = COUNTERS_GET_CMD;
    msg.len = 4;
    SEND_COMMAND;

}
int readcounters (char id)
{
    PICRSP rsp;

    int  count, countr;
    ASGN = COUNTERS_GET_CMD;
    msg.len = 4;
    countr = 0;
    count = 9;
    if (Serial.available())
    {

        countr = Serial.readBytes((char *)&rsp,count);
        if ((countr == 9) && (rsp.control == RESPONSE_OK))
        {
            return rsp.counter;
        }

        else
        {
            // printf("com error %d %d \n\r",count,id);
            //  Clean_Buffer(com_handler);
            return -1;
        }
        //  Clean_Buffer(com_handler);
    }
}
void
motor_set_period (char id, int base, char post)
{
    //  picmsg  msg;
    msg.code = (CMD_FCODE);
    ASGN = M1_SPEED_SET_CMD;
    //msg1.id = msg1.idd= id;msg1.command=M1_SPEED_SET_CMD;
    msg.data.dint = base;
    msg.data.buff[2] = post;
    msg.len = 7;
    SEND_COMMAND;



}

void
aux_set_period (char id, int base)
{
    ASGN = M2_SPEED_SET_CMD;
    msg.data.dint = base;
    msg.len = 6;
    SEND_COMMAND;


}
void
set_motor_target (char id, int base)
{
    ASGN = M1_SET_CMD_TARGET;
    msg.data.dint = base;

    msg.len = 8;
    SEND_COMMAND;

}
void
set_aux_target (char id, int base)
{
    ASGN = M2_SET_CMD_TARGET;
    msg.data.dint = base;
    msg.len = 8;
    sendcommand();

}
void save_counters(char id)
{
    ASGN = COUNTERS_SAVE_CMD;
    msg.len = 4;
    SEND_COMMAND;


}
void
setmotorbackslash(char id, int back)
{
    ASGN = M1_SET_BACKSLASH_CMD;
    msg.data.dint = back;
    msg.len = 6;
    SEND_COMMAND;

}
char*
setauxbackslash(char id, int back)
{
    ASGN = M2_SET_BACKSLASH_CMD;
    msg.data.dint = back;
    msg.len = 6;
    SEND_COMMAND;

}
void
changePWM(char id, unsigned char index , char value)
{
    ASGN = M1_CHANGE_PMW_CMD;
    msg.data.buff[0] = index;
    msg.data.buff[1] = value;
    sendcommand();
    msg.len = 6;

}

void
changespeedtable(char id, int value, unsigned char index)
{
    ASGN = SPEED_TABLE_CHANGE_CMD;
    msg.data.dint = value;
    msg.data.buff[2] = index;

    msg.len = 7;
    sendcommand();

}
void saveconf(char id)

{
    ASGN = SPEED_TABLE_SAVE_CMD;
    msg.len = 4;
    sendcommand();

}
void
set_motor_max_counter(char id, unsigned int max)
{
    ASGN = M1_MAX_COUNT_SAVE_CMD;
    msg.data.dint = max;
    msg.len = 8;
    SEND_COMMAND;

}

void
set_aux_max_counter(char id, unsigned int max)
{
    ASGN = M1_MAX_COUNT_SAVE_CMD;
    msg.data.dint = max;
    msg.len = 8;
    SEND_COMMAND;

}
void
set_motor_counter(char id, int current)
{
    ASGN = M1_COUNTER_SET_CMD;
    msg.data.dint = current;
    msg.len = 8;
    SEND_COMMAND;


}
void set_aux_counter(char id, unsigned int current)
{
    ASGN = M2_COUNTER_SET_CMD;
    msg.data.dint = current;
    msg.len = 8;
    SEND_COMMAND;

}
void
set_start_up(char id)
{
    ASGN = M1_STARTUP_SET_CMD;
    msg.len = 4;
    SEND_COMMAND;

}
void
unset_start_up(char id)
{
    ASGN = M1_STARTUP_UNSET_CMD;
    msg.len = 4;
    SEND_COMMAND;

}
void
set_baud_rate(char id, char baud)
{
    ASGN = BAUDRATE_SET;
    msg.data.buff[0] = baud;
    msg.len = 5;
    SEND_COMMAND;

}
void
set_prescaler(char id, char prescaler)
{
    ASGN = PRESCALER_SET;
    msg.data.buff[0] =  prescaler;
    msg.len = 5;
    SEND_COMMAND;

}

int sendcommand(void)
{
    Serial.write((char *) &msg, msg.len);
    Serial.printf(" bytes %d\n\r", msg.len);
    return msg.len;

}




