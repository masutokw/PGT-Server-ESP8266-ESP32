
/* #line 1 "C:\\Documents and Settings\\Administrador\\Mis documentos\\Arduino\\esp8266pgt\\command.rl" */
/*
 * Parses LX200 protocol.
 */
#include <string.h>
#include <stdio.h>
#include "mount.h"
#include "misc.h"
#include <math.h>
#define ADD_DIGIT(var,digit) var=var*10+digit-'0';
#define APPEND strcat(response,tmessage);
#define SYNC_MESSAGE "sync#"
//#define SYNC_MESSAGE "Coordinates     matched.        #"
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

    // deg=min=sec=0;
    int neg = 1;
    tmessage[0]=0;
    response[0]=0;

    
/* #line 2 "command.cpp" */
static const char _command_actions[] = {
	0, 1, 0, 1, 1, 1, 2, 1, 
	3, 1, 4, 1, 5, 1, 6, 1, 
	7, 1, 8, 1, 9, 1, 10, 1, 
	11, 1, 12, 1, 13, 1, 14, 1, 
	15, 1, 16, 1, 17, 1, 18, 1, 
	19, 1, 20, 1, 23, 1, 24, 2, 
	1, 21, 2, 2, 22
};

static const char _command_key_offsets[] = {
	0, 0, 6, 7, 9, 10, 22, 23, 
	24, 25, 26, 27, 28, 29, 30, 31, 
	36, 37, 38, 43, 44, 48, 51, 52, 
	62, 63, 65, 67, 68, 70, 72, 73, 
	75, 77, 78, 83, 85, 87, 88, 90, 
	92, 94, 96, 97, 99, 101, 108, 112, 
	114, 116, 116, 118, 120, 122, 123, 124, 
	125, 127
};

static const char _command_trans_keys[] = {
	67, 71, 77, 81, 82, 83, 77, 35, 
	82, 35, 65, 67, 68, 76, 77, 82, 
	83, 90, 100, 103, 114, 116, 35, 35, 
	35, 35, 35, 35, 35, 35, 35, 83, 
	101, 110, 115, 119, 35, 35, 35, 101, 
	110, 115, 119, 35, 67, 71, 77, 83, 
	35, 48, 52, 35, 67, 76, 83, 97, 
	100, 103, 114, 116, 119, 122, 32, 48, 
	57, 48, 57, 47, 48, 57, 48, 57, 
	47, 48, 57, 48, 57, 35, 32, 9, 
	13, 48, 50, 48, 50, 48, 57, 58, 
	48, 53, 48, 57, 46, 58, 48, 57, 
	35, 48, 53, 48, 57, 32, 43, 45, 
	9, 13, 48, 57, 43, 45, 48, 57, 
	48, 57, 48, 57, 48, 53, 48, 57, 
	35, 58, 32, 51, 35, 35, 58, 58, 
	0
};

static const char _command_single_lengths[] = {
	0, 6, 1, 2, 1, 12, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 5, 
	1, 1, 5, 1, 4, 1, 1, 10, 
	1, 0, 0, 1, 0, 0, 1, 0, 
	0, 1, 1, 0, 0, 1, 0, 0, 
	2, 0, 1, 0, 0, 3, 2, 0, 
	0, 0, 0, 0, 2, 1, 1, 1, 
	2, 1
};

static const char _command_range_lengths[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 1, 0, 0, 
	0, 1, 1, 0, 1, 1, 0, 1, 
	1, 0, 2, 1, 1, 0, 1, 1, 
	0, 1, 0, 1, 1, 2, 1, 1, 
	1, 0, 1, 1, 0, 0, 0, 0, 
	0, 0
};

static const unsigned char _command_index_offsets[] = {
	0, 0, 7, 9, 12, 14, 27, 29, 
	31, 33, 35, 37, 39, 41, 43, 45, 
	51, 53, 55, 61, 63, 68, 71, 73, 
	84, 86, 88, 90, 92, 94, 96, 98, 
	100, 102, 104, 108, 110, 112, 114, 116, 
	118, 121, 123, 125, 127, 129, 135, 139, 
	141, 143, 144, 146, 148, 151, 153, 155, 
	157, 160
};

static const char _command_indicies[] = {
	0, 2, 3, 4, 5, 6, 1, 7, 
	1, 8, 9, 1, 8, 1, 10, 11, 
	10, 12, 13, 14, 12, 10, 15, 16, 
	17, 18, 1, 19, 1, 20, 1, 21, 
	1, 22, 1, 23, 1, 24, 1, 25, 
	1, 26, 1, 27, 1, 28, 29, 29, 
	29, 29, 1, 30, 1, 31, 1, 32, 
	33, 33, 33, 33, 1, 32, 1, 34, 
	34, 34, 34, 1, 35, 36, 1, 35, 
	1, 37, 38, 38, 39, 39, 39, 38, 
	39, 40, 39, 1, 41, 1, 42, 1, 
	43, 1, 44, 1, 45, 1, 46, 1, 
	47, 1, 48, 1, 49, 1, 50, 1, 
	51, 51, 52, 1, 52, 1, 53, 1, 
	54, 1, 55, 1, 56, 1, 57, 58, 
	1, 59, 1, 60, 1, 61, 1, 62, 
	1, 63, 64, 65, 63, 66, 1, 64, 
	65, 66, 1, 67, 1, 68, 1, 69, 
	70, 1, 71, 1, 60, 58, 1, 72, 
	1, 73, 1, 74, 1, 75, 76, 1, 
	76, 1, 0
};

static const char _command_trans_targs[] = {
	2, 0, 5, 15, 18, 20, 23, 3, 
	57, 4, 6, 7, 8, 9, 10, 11, 
	12, 13, 14, 57, 57, 57, 57, 57, 
	57, 57, 57, 57, 16, 17, 57, 57, 
	57, 19, 21, 57, 22, 24, 34, 45, 
	53, 25, 26, 27, 28, 29, 30, 31, 
	32, 33, 57, 35, 36, 37, 38, 39, 
	40, 41, 43, 42, 57, 44, 42, 46, 
	47, 47, 47, 48, 49, 50, 51, 52, 
	54, 55, 57, 57, 1
};

static const char _command_trans_actions[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 
	35, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 19, 25, 39, 27, 17, 
	23, 33, 21, 31, 0, 43, 11, 9, 
	13, 43, 43, 15, 0, 0, 43, 43, 
	0, 0, 3, 3, 0, 5, 5, 0, 
	1, 1, 45, 0, 1, 1, 0, 3, 
	47, 0, 0, 37, 41, 5, 50, 0, 
	0, 7, 1, 1, 1, 0, 3, 47, 
	0, 0, 29, 0, 0
};

static const int command_start = 56;
static const int command_first_final = 56;
static const int command_error = 0;

static const int command_en_main = 56;


/* #line 107 "C:\\Documents and Settings\\Administrador\\Mis documentos\\Arduino\\esp8266pgt\\command.rl" */





    
/* #line 132 "command.cpp" */
	{
	cs = command_start;
	}

/* #line 135 "command.cpp" */
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
/* #line 114 "C:\\Documents and Settings\\Administrador\\Mis documentos\\Arduino\\esp8266pgt\\command.rl" */
	{ADD_DIGIT(deg,(*p)); }
	break;
	case 1:
/* #line 115 "C:\\Documents and Settings\\Administrador\\Mis documentos\\Arduino\\esp8266pgt\\command.rl" */
	{ADD_DIGIT(min,(*p)); }
	break;
	case 2:
/* #line 116 "C:\\Documents and Settings\\Administrador\\Mis documentos\\Arduino\\esp8266pgt\\command.rl" */
	{ADD_DIGIT(sec,(*p)); }
	break;
	case 3:
/* #line 117 "C:\\Documents and Settings\\Administrador\\Mis documentos\\Arduino\\esp8266pgt\\command.rl" */
	{ neg=-1;}
	break;
	case 4:
/* #line 118 "C:\\Documents and Settings\\Administrador\\Mis documentos\\Arduino\\esp8266pgt\\command.rl" */
	{mount_move(telescope,stcmd);}
	break;
	case 5:
/* #line 119 "C:\\Documents and Settings\\Administrador\\Mis documentos\\Arduino\\esp8266pgt\\command.rl" */
	{mount_slew(telescope); sprintf(tmessage,"0");APPEND;}
	break;
	case 6:
/* #line 120 "C:\\Documents and Settings\\Administrador\\Mis documentos\\Arduino\\esp8266pgt\\command.rl" */
	{mount_stop(telescope,stcmd);}
	break;
	case 7:
/* #line 121 "C:\\Documents and Settings\\Administrador\\Mis documentos\\Arduino\\esp8266pgt\\command.rl" */
	{select_rate(telescope,stcmd); }
	break;
	case 8:
/* #line 122 "C:\\Documents and Settings\\Administrador\\Mis documentos\\Arduino\\esp8266pgt\\command.rl" */
	{mount_lxra_str(tmessage,telescope);APPEND;}
	break;
	case 9:
/* #line 123 "C:\\Documents and Settings\\Administrador\\Mis documentos\\Arduino\\esp8266pgt\\command.rl" */
	{mount_lxde_str(tmessage,telescope);APPEND;}
	break;
	case 10:
/* #line 124 "C:\\Documents and Settings\\Administrador\\Mis documentos\\Arduino\\esp8266pgt\\command.rl" */
	{ ;}
	break;
	case 11:
/* #line 125 "C:\\Documents and Settings\\Administrador\\Mis documentos\\Arduino\\esp8266pgt\\command.rl" */
	{;}
	break;
	case 12:
/* #line 126 "C:\\Documents and Settings\\Administrador\\Mis documentos\\Arduino\\esp8266pgt\\command.rl" */
	{lxprintdate();}
	break;
	case 13:
/* #line 127 "C:\\Documents and Settings\\Administrador\\Mis documentos\\Arduino\\esp8266pgt\\command.rl" */
	{ lxprintsite();}
	break;
	case 14:
/* #line 128 "C:\\Documents and Settings\\Administrador\\Mis documentos\\Arduino\\esp8266pgt\\command.rl" */
	{;}
	break;
	case 15:
/* #line 129 "C:\\Documents and Settings\\Administrador\\Mis documentos\\Arduino\\esp8266pgt\\command.rl" */
	{sprintf(tmessage,"+004*12#");}
	break;
	case 16:
/* #line 130 "C:\\Documents and Settings\\Administrador\\Mis documentos\\Arduino\\esp8266pgt\\command.rl" */
	{sprintf(tmessage,"+36*43#");}
	break;
	case 17:
/* #line 132 "C:\\Documents and Settings\\Administrador\\Mis documentos\\Arduino\\esp8266pgt\\command.rl" */
	{sync_eq(telescope);sprintf(tmessage,SYNC_MESSAGE);
                    telescope->altmotor->slewing= telescope->azmotor->slewing=FALSE;
                    APPEND;}
	break;
	case 18:
/* #line 135 "C:\\Documents and Settings\\Administrador\\Mis documentos\\Arduino\\esp8266pgt\\command.rl" */
	{deg+=((*p)-'0')*6;}
	break;
	case 19:
/* #line 136 "C:\\Documents and Settings\\Administrador\\Mis documentos\\Arduino\\esp8266pgt\\command.rl" */
	{ ltime();}
	break;
	case 20:
/* #line 137 "C:\\Documents and Settings\\Administrador\\Mis documentos\\Arduino\\esp8266pgt\\command.rl" */
	{
            set_cmd_exe(stcmd,(neg*(deg )));
            sprintf(tmessage,"1");
            APPEND;
            deg=sec=min=0;
        }
	break;
	case 21:
/* #line 143 "C:\\Documents and Settings\\Administrador\\Mis documentos\\Arduino\\esp8266pgt\\command.rl" */
	{deg=deg*3600+min*60;}
	break;
	case 22:
/* #line 144 "C:\\Documents and Settings\\Administrador\\Mis documentos\\Arduino\\esp8266pgt\\command.rl" */
	{deg+=sec;}
	break;
	case 23:
/* #line 145 "C:\\Documents and Settings\\Administrador\\Mis documentos\\Arduino\\esp8266pgt\\command.rl" */
	{stcmd=(*p);}
	break;
	case 24:
/* #line 146 "C:\\Documents and Settings\\Administrador\\Mis documentos\\Arduino\\esp8266pgt\\command.rl" */
	{set_date(sec,min,deg);}
	break;
/* #line 290 "command.cpp" */
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

/* #line 166 "C:\\Documents and Settings\\Administrador\\Mis documentos\\Arduino\\esp8266pgt\\command.rl" */


//---------------------------------------------------------------------------------------------------------------------
    if ( cs < command_first_final )
        //	fprintf( stderr, "LX command:  error\n" );

        return  neg;
};



