
/* #line 1 "command.rl" */
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
    int day,month,year,dayofyear;
    int hour,min,sec;
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
        //telescope->longitude=(-date/3600.0);
		if (date <648000)
        telescope->longitude=-date/3600.0; else
		telescope->longitude=360.0-(date/3600.0);
        break;
    case 'L' :
        //timer0SetOverflowCount((long) (30.518 *date));
        break;
    case 'S':
        break;

    }


}

void set_date( int day,int month,int year)
{   mount.month=month-1;
    mount.day=day;
    mount.year=100+year;
     setclock (mount.year,mount.month,mount.day,mount.hour,mount.min,mount.sec,telescope->time_zone);
	//sprintf(tmessage,"%cUpdating Planetary Data# %d %d %d  #",'1', mount.month,mount.day,mount.year);APPEND;
	sprintf(tmessage,"%cUpdating Planetary Data#     #",'1');APPEND;
}
void set_time( int hour,int min,int sec)
{  //sprintf(tmessage,"year %d  month %d day %d Hour %d Min %d sec %d GMT %d" ,mount.year,mount.month, mount.day,hour,min,sec,telescope->time_zone);APPEND;
   //setclock (mount.year,mount.month, mount.day,hour,min,sec,telescope->time_zone);
    mount.min=min;
    mount.hour=hour;
    mount.sec=sec;
  // setclock (mount.year,mount.month,mount.day,hour,min,sec,telescope->time_zone);
   sprintf(tmessage,"1");APPEND;

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
	int pulse=0;
    
/* #line 2 "command.cpp" */
static const char _command_actions[] = {
	0, 1, 0, 1, 1, 1, 2, 1, 
	3, 1, 4, 1, 5, 1, 6, 1, 
	7, 1, 8, 1, 9, 1, 10, 1, 
	11, 1, 12, 1, 13, 1, 14, 1, 
	15, 1, 17, 1, 18, 1, 19, 1, 
	20, 1, 21, 1, 22, 1, 25, 1, 
	26, 1, 27, 1, 29, 1, 30, 2, 
	1, 23, 2, 2, 24, 2, 28, 16
	
};

static const unsigned char _command_key_offsets[] = {
	0, 0, 6, 7, 9, 10, 23, 24, 
	25, 26, 27, 28, 29, 30, 31, 32, 
	33, 39, 43, 45, 47, 49, 51, 52, 
	53, 54, 59, 60, 64, 67, 68, 78, 
	83, 85, 87, 88, 90, 92, 93, 95, 
	97, 98, 103, 105, 107, 111, 113, 114, 
	116, 121, 123, 125, 126, 128, 130, 131, 
	133, 135, 136, 141, 143, 145, 147, 149, 
	151, 154, 156, 157, 159, 161, 163, 165, 
	168, 175, 182, 184, 186, 196, 198, 200, 
	201, 209, 212, 213
};

static const char _command_trans_keys[] = {
	67, 71, 77, 81, 82, 83, 77, 35, 
	82, 35, 65, 67, 68, 71, 76, 77, 
	82, 83, 90, 100, 103, 114, 116, 35, 
	35, 35, 35, 35, 35, 35, 35, 35, 
	35, 71, 83, 101, 110, 115, 119, 101, 
	110, 115, 119, 48, 57, 48, 57, 48, 
	57, 48, 57, 35, 35, 35, 35, 101, 
	110, 115, 119, 35, 67, 71, 77, 83, 
	35, 48, 52, 35, 67, 71, 76, 83, 
	97, 100, 103, 114, 116, 122, 32, 9, 
	13, 48, 57, 48, 57, 48, 57, 47, 
	48, 57, 48, 57, 47, 48, 57, 48, 
	57, 35, 32, 43, 45, 9, 13, 43, 
	45, 48, 57, 35, 46, 48, 57, 48, 
	57, 35, 35, 46, 32, 9, 13, 48, 
	57, 48, 57, 48, 57, 58, 48, 57, 
	48, 57, 58, 48, 57, 48, 57, 35, 
	32, 9, 13, 48, 50, 48, 50, 48, 
	57, 47, 58, 48, 53, 48, 57, 46, 
	47, 58, 48, 57, 35, 48, 53, 48, 
	57, 35, 46, 48, 57, 35, 48, 57, 
	32, 43, 45, 9, 13, 48, 57, 32, 
	43, 45, 9, 13, 48, 57, 48, 57, 
	48, 57, 33, 47, 48, 57, 58, 64, 
	91, 96, 123, 126, 48, 53, 48, 57, 
	35, 33, 47, 58, 64, 91, 96, 123, 
	126, 6, 35, 58, 58, 58, 48, 53, 
	0
};

static const char _command_single_lengths[] = {
	0, 6, 1, 2, 1, 13, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 
	6, 4, 0, 0, 0, 0, 1, 1, 
	1, 5, 1, 4, 1, 1, 10, 1, 
	0, 0, 1, 0, 0, 1, 0, 0, 
	1, 3, 2, 0, 2, 0, 1, 2, 
	1, 0, 0, 1, 0, 0, 1, 0, 
	0, 1, 1, 0, 0, 2, 0, 0, 
	3, 0, 1, 0, 0, 2, 0, 1, 
	3, 3, 0, 0, 0, 0, 0, 1, 
	0, 3, 1, 1
};

static const char _command_range_lengths[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 1, 1, 1, 1, 0, 0, 
	0, 0, 0, 0, 1, 0, 0, 2, 
	1, 1, 0, 1, 1, 0, 1, 1, 
	0, 1, 0, 1, 1, 1, 0, 0, 
	2, 1, 1, 0, 1, 1, 0, 1, 
	1, 0, 2, 1, 1, 0, 1, 1, 
	0, 1, 0, 1, 1, 0, 1, 1, 
	2, 2, 1, 1, 5, 1, 1, 0, 
	4, 0, 0, 1
};

static const short _command_index_offsets[] = {
	0, 0, 7, 9, 12, 14, 28, 30, 
	32, 34, 36, 38, 40, 42, 44, 46, 
	48, 55, 60, 62, 64, 66, 68, 70, 
	72, 74, 80, 82, 87, 90, 92, 103, 
	107, 109, 111, 113, 115, 117, 119, 121, 
	123, 125, 130, 133, 135, 139, 141, 143, 
	146, 150, 152, 154, 156, 158, 160, 162, 
	164, 166, 168, 172, 174, 176, 179, 181, 
	183, 187, 189, 191, 193, 195, 198, 200, 
	203, 209, 215, 217, 219, 225, 227, 229, 
	231, 236, 240, 242
};

static const char _command_indicies[] = {
	0, 2, 3, 4, 5, 6, 1, 7, 
	1, 8, 9, 1, 8, 1, 10, 11, 
	10, 12, 13, 14, 15, 13, 10, 16, 
	17, 18, 19, 1, 20, 1, 21, 1, 
	22, 1, 23, 1, 24, 1, 25, 1, 
	26, 1, 27, 1, 28, 1, 29, 1, 
	30, 31, 32, 32, 32, 32, 1, 33, 
	33, 33, 33, 1, 34, 1, 35, 1, 
	36, 1, 37, 1, 38, 1, 39, 1, 
	40, 1, 41, 42, 42, 42, 42, 1, 
	41, 1, 43, 43, 43, 43, 1, 44, 
	45, 1, 44, 1, 46, 47, 48, 49, 
	50, 50, 50, 49, 50, 50, 1, 51, 
	51, 52, 1, 52, 1, 53, 1, 54, 
	1, 55, 1, 56, 1, 57, 1, 58, 
	1, 59, 1, 60, 1, 61, 62, 63, 
	61, 1, 62, 63, 1, 64, 1, 65, 
	66, 67, 1, 68, 1, 65, 1, 65, 
	66, 1, 69, 69, 70, 1, 70, 1, 
	71, 1, 72, 1, 73, 1, 74, 1, 
	75, 1, 76, 1, 77, 1, 78, 1, 
	79, 79, 80, 1, 80, 1, 81, 1, 
	82, 82, 1, 83, 1, 84, 1, 85, 
	86, 86, 1, 87, 1, 88, 1, 89, 
	1, 90, 1, 88, 91, 1, 92, 1, 
	88, 93, 1, 94, 95, 96, 94, 97, 
	1, 95, 95, 96, 95, 97, 1, 97, 
	1, 98, 1, 99, 100, 99, 99, 99, 
	1, 101, 1, 102, 1, 103, 86, 99, 
	99, 99, 99, 1, 104, 105, 106, 1, 
	106, 1, 106, 89, 1, 0
};

static const char _command_trans_targs[] = {
	2, 0, 5, 16, 25, 27, 30, 3, 
	82, 4, 6, 7, 8, 9, 10, 11, 
	12, 13, 14, 15, 82, 82, 82, 82, 
	82, 82, 82, 82, 82, 82, 17, 23, 
	24, 18, 19, 20, 21, 22, 82, 82, 
	82, 82, 26, 28, 82, 29, 31, 41, 
	48, 58, 72, 32, 33, 34, 35, 36, 
	37, 38, 39, 40, 82, 42, 43, 43, 
	44, 82, 45, 47, 46, 49, 50, 51, 
	52, 53, 54, 55, 56, 57, 82, 59, 
	60, 61, 62, 63, 64, 65, 67, 66, 
	82, 68, 69, 70, 71, 66, 73, 74, 
	74, 75, 76, 77, 80, 78, 79, 83, 
	82, 82, 1
};

static const char _command_trans_actions[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 
	37, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 23, 29, 51, 41, 
	31, 21, 27, 33, 25, 35, 0, 0, 
	45, 45, 7, 7, 7, 7, 13, 15, 
	11, 17, 45, 45, 19, 0, 0, 0, 
	0, 45, 45, 0, 1, 1, 0, 3, 
	3, 0, 5, 5, 47, 0, 0, 9, 
	1, 61, 0, 1, 0, 0, 1, 1, 
	0, 3, 3, 0, 5, 5, 53, 0, 
	1, 1, 0, 3, 55, 0, 0, 39, 
	43, 5, 58, 0, 0, 0, 0, 0, 
	9, 1, 1, 0, 1, 3, 55, 43, 
	49, 0, 0
};

static const int command_start = 81;
static const int command_first_final = 81;
static const int command_error = 0;

static const int command_en_main = 81;


/* #line 117 "command.rl" */





    
/* #line 175 "command.cpp" */
	{
	cs = command_start;
	}

/* #line 178 "command.cpp" */
	{
	int _klen;
	unsigned int _trans;
	const char *_acts;
	unsigned int _nacts;
	const char *_keys;

	if ( p == pe )
		goto _test_eof;
	if ( cs == 0 )
		goto _out;
_resume:
	_keys = _command_trans_keys + _command_key_offsets[cs];
	_trans = _command_index_offsets[cs];

	_klen = _command_single_lengths[cs];
	if ( _klen > 0 ) {
		const char *_lower = _keys;
		const char *_mid;
		const char *_upper = _keys + _klen - 1;
		while (1) {
			if ( _upper < _lower )
				break;

			_mid = _lower + ((_upper-_lower) >> 1);
			if ( (*p) < *_mid )
				_upper = _mid - 1;
			else if ( (*p) > *_mid )
				_lower = _mid + 1;
			else {
				_trans += (_mid - _keys);
				goto _match;
			}
		}
		_keys += _klen;
		_trans += _klen;
	}

	_klen = _command_range_lengths[cs];
	if ( _klen > 0 ) {
		const char *_lower = _keys;
		const char *_mid;
		const char *_upper = _keys + (_klen<<1) - 2;
		while (1) {
			if ( _upper < _lower )
				break;

			_mid = _lower + (((_upper-_lower) >> 1) & ~1);
			if ( (*p) < _mid[0] )
				_upper = _mid - 2;
			else if ( (*p) > _mid[1] )
				_lower = _mid + 2;
			else {
				_trans += ((_mid - _keys)>>1);
				goto _match;
			}
		}
		_trans += _klen;
	}

_match:
	_trans = _command_indicies[_trans];
	cs = _command_trans_targs[_trans];

	if ( _command_trans_actions[_trans] == 0 )
		goto _again;

	_acts = _command_actions + _command_trans_actions[_trans];
	_nacts = (unsigned int) *_acts++;
	while ( _nacts-- > 0 )
	{
		switch ( *_acts++ )
		{
	case 0:
/* #line 124 "command.rl" */
	{ADD_DIGIT(deg,(*p)); }
	break;
	case 1:
/* #line 125 "command.rl" */
	{ADD_DIGIT(min,(*p)); }
	break;
	case 2:
/* #line 126 "command.rl" */
	{ADD_DIGIT(sec,(*p)); }
	break;
	case 3:
/* #line 127 "command.rl" */
	{ADD_DIGIT(pulse,(*p));}
	break;
	case 4:
/* #line 128 "command.rl" */
	{ neg=-1;}
	break;
	case 5:
/* #line 129 "command.rl" */
	{mount_move(telescope,stcmd);}
	break;
	case 6:
/* #line 130 "command.rl" */
	{pulse_guide(telescope,stcmd,pulse);}
	break;
	case 7:
/* #line 131 "command.rl" */
	{mount_slew(telescope); sprintf(tmessage,"0");APPEND;}
	break;
	case 8:
/* #line 132 "command.rl" */
	{mount_stop(telescope,stcmd);}
	break;
	case 9:
/* #line 133 "command.rl" */
	{select_rate(telescope,stcmd); }
	break;
	case 10:
/* #line 134 "command.rl" */
	{mount_lxra_str(tmessage,telescope);APPEND;}
	break;
	case 11:
/* #line 135 "command.rl" */
	{mount_lxde_str(tmessage,telescope);APPEND;}
	break;
	case 12:
/* #line 136 "command.rl" */
	{ ;}
	break;
	case 13:
/* #line 137 "command.rl" */
	{;}
	break;
	case 14:
/* #line 138 "command.rl" */
	{lxprintdate1(tmessage);APPEND;}
	break;
	case 15:
/* #line 139 "command.rl" */
	{ lxprintsite();}
	break;
	case 16:
/* #line 140 "command.rl" */
	{;}
	break;
	case 17:
/* #line 143 "command.rl" */
	{lxprintlong1(tmessage,telescope->longitude);APPEND;}
	break;
	case 18:
/* #line 144 "command.rl" */
	{lxprintlat1(tmessage,telescope->lat);APPEND;}
	break;
	case 19:
/* #line 146 "command.rl" */
	{sync_eq(telescope);sprintf(tmessage,SYNC_MESSAGE);
                    telescope->altmotor->slewing= telescope->azmotor->slewing=FALSE;
                    APPEND;}
	break;
	case 20:
/* #line 149 "command.rl" */
	{deg+=((*p)-'0')*6;}
	break;
	case 21:
/* #line 150 "command.rl" */
	{ lxprinttime1(tmessage);APPEND;}
	break;
	case 22:
/* #line 151 "command.rl" */
	{
            set_cmd_exe(stcmd,(neg*(deg )));
            sprintf(tmessage,"1");
            APPEND;
            deg=sec=min=0;
        }
	break;
	case 23:
/* #line 157 "command.rl" */
	{deg=deg*3600+min*60;}
	break;
	case 24:
/* #line 158 "command.rl" */
	{deg+=sec;}
	break;
	case 25:
/* #line 159 "command.rl" */
	{stcmd=(*p);}
	break;
	case 26:
/* #line 160 "command.rl" */
	{set_date(min,deg,sec);}
	break;
	case 27:
/* #line 161 "command.rl" */
	{sprintf(tmessage,"P"); APPEND;}
	break;
	case 28:
/* #line 162 "command.rl" */
	{ telescope->time_zone=deg;}
	break;
	case 29:
/* #line 163 "command.rl" */
	{lxprintGMT_offset(tmessage,telescope->time_zone );APPEND}
	break;
	case 30:
/* #line 164 "command.rl" */
	{set_time(deg,min,sec);}
	break;
/* #line 351 "command.cpp" */
		}
	}

_again:
	if ( cs == 0 )
		goto _out;
	if ( ++p != pe )
		goto _resume;
	_test_eof: {}
	_out: {}
	}

/* #line 198 "command.rl" */


//---------------------------------------------------------------------------------------------------------------------
    if ( cs < command_first_final )
        //	fprintf( stderr, "LX command:  error\n" );

        return  neg;
};



