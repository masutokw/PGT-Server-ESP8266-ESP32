#ifndef PICLEVEL_H_INCLUDED
#define PICLEVEL_H_INCLUDED
#define M1_SPEED_SET_CMD 0
#define COUNTERS_GET_CMD 1
#define M2_SPEED_SET_CMD  27
#define M2_SET_CMD_TARGET 3
#define M1_SET_CMD_TARGET 4
#define COUNTERS_SAVE_CMD 5
#define M2_SET_BACKSLASH_CMD 6
#define M1_SET_BACKSLASH_CMD 7
#define M1_CHANGE_PMW_CMD 8
#define SPEED_TABLE_CHANGE_CMD 9
#define SPEED_TABLE_SAVE_CMD 10
#define M1_BACKSLASH_MODE  12
#define M2_BACKSLASH_MODE 13
#define LED_HALF_CMD 11
#define M1_MAX_COUNT_SAVE_CMD 14
#define M2_MAX_COUNT_SAVE_CMD 15
#define M1_COUNTER_SET_CMD 16
#define M2_COUNTER_SET_CMD 17
#define M1_STARTUP_UNSET_CMD 18
#define M1_STARTUP_SET_CMD 19
#define BAUDRATE_SET 20
#define PRESCALER_SET 21
#define PWM_FREQ_SET 24
#define PWM_CAL_SET 5
#define M1_GET_SPEED_CMD 6
#define CMD_FCODE 239 //0x33
#define RESPONSE_OK 44

typedef struct
{
    char code, id, idd;
    char command;
    union
    {
        char buff[4];
        int dint;
    } data;
    char len;
} picmsg;
//picmsg PICMSG;

struct picresponse
{
    int counter, counteraux;
    char control;
};
typedef struct picresponse PICRSP;

void pollcounters(char id);
int  readcounters (char id);
void motor_set_period (char id ,  int base  , char post );
void aux_set_period (char id, int base);
void set_motor_target (char id, int base);
void set_aux_target (char id, int base);
void save_counters(char id);
void setmotorbackslash(char id, int back);
void restoreaux(char id);
void changePWM(char id, unsigned char index , char value);
void changespeedtable(char id, int value, unsigned char index);
void savetable(char id);
void set_motor_max_counter(char id, unsigned int max);
void set_aux_max_counter(char id, unsigned int max);
void set_motor_counter(char id, int current);
void set_aux_counter(char id, unsigned int current);
void set_start_up(char id);
void unset_start_up(char id);
void set_baud_rate(char id, char baud);
void set_prescaler(char id, char prescaler);
int sendcommand(void);

#endif
