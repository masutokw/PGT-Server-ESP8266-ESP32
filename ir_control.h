#include "conf.h"
#ifdef IR_CONTROL
#ifndef IR_CONTROL_H_INCLUDED
#define IR_CONTROL_H_INCLUDED

#include "misc.h"
#define REMOTE_T 0

#if REMOTE_T ==1
#define EAST 0x16
#define WEST  0x14
#define NORTH 0x11
#define SOUTH 0x19
#define OK  0x15
#define FOCUS_F 0x0A
#define FOCUS_B 0x06
#define GUIDE 1
#define CENTER 2
#define FIND 4
#define SLEW 5
#endif

#if REMOTE_T ==2
#define EAST 0x8
#define WEST  90
#define NORTH 24
#define SOUTH 82
#define OK  28
#define FOCUS_F 67
#define FOCUS_B 9
#define GUIDE 69
#define CENTER 70
#define FIND 71
#define SLEW 68
#endif

#if REMOTE_T ==3
#define EAST 17
#define WEST 15
#define NORTH 19
#define SOUTH 16
#define OK  28
#define FOCUS_F 26
#define FOCUS_B 30
#define GUIDE 14
#define CENTER 29
#define FIND 25
#define SLEW 22
#endif

#if REMOTE_T ==4
#define EAST 91
#define WEST 88
#define NORTH 94
#define SOUTH 86
#define OK  90
#define FOCUS_F 18
#define FOCUS_B 2
#define GUIDE 13
#define CENTER 75
#define FIND 74
#define SLEW 79
#define N_EAST 83
#define N_WEST 11
#define S_EAST 87
#define S_WEST 78
#define TRACK 6
#define TRACK_OFF 15
#define B_1 30
#define B_2 68
#define B_3 17
#define B_4 95
#define B_5 76
#define B_6 77
#define B_7 73
#define B_8 64
#define B_9 72
#define B_0 69
#define GO_TO 31
#define CLEAR 92
#define FLIP_W 25
#define FLIP_E 89
#endif
#if REMOTE_T ==5
#define EAST 10
#define WEST 8
#define NORTH 9
#define SOUTH 1
#define OK  11
#define FOCUS_F 13
#define FOCUS_B 17
#define GUIDE 6
#define CENTER 7
#define FIND 5
#define SLEW 4
#define N_EAST 80
#define N_WEST 68
#define S_EAST 76
#define S_WEST 72
#define TRACK 89
#define TRACK_OFF 30
#define B_1 14
#define B_2 15
#define B_3 12
#define B_4 18
#define B_5 19
#define B_6 16
#define B_7 22
#define B_8 23
#define B_9 20
#define B_0 26
#define GO_TO 28
#define CLEAR 29
#define FLIP_W 25
#define FLIP_E 89
#endif

#if REMOTE_T == 0
#define EAST 0
#define WEST 1
#define NORTH 2
#define SOUTH 3
#define OK  4
#define FOCUS_F 5
#define FOCUS_B 6
#define GUIDE 7
#define CENTER 8
#define FIND 9
#define SLEW 10
#define N_EAST 11
#define N_WEST 12
#define S_EAST 13
#define S_WEST 14
#define TRACK 15
#define TRACK_OFF 16
#define B_1 17
#define B_2 18
#define B_3 19
#define B_4 20
#define B_5 21
#define B_6 22
#define B_7 23
#define B_8 24
#define B_9 25
#define B_0 26
#define GO_TO 27
#define CLEAR 28
#define FLIP_W 29
#define FLIP_E 30
#endif

const char names[17][6] = {"A UMI", "A TAU", "B ORI", "A AUR", "A ORI", "A CMA", "A GEM", "A CMI",
                           "B GEM", "A LEO", "A VIR", "A BOO", "A SCO", "A LYR", "A AQL", "A CYG", "A PSA"
                          };

const double star_mat[17][2] =
{
  {37.960,  89.264}
  ,  {68.980,  16.509}
  ,  {78.634,  -8.202}
  ,  {79.172,  45.998}
  ,  {88.793,   7.407}
  , {101.287, -16.716}
  , {113.650,  31.888}
  , {114.825,   5.225}
  , {116.329,  28.026}
  , {152.093,  11.967}
  , {201.298, -11.161}
  , {213.915,  19.183}
  , {247.352, -26.432}
  , {279.234,  38.784}
  , {297.695,   8.868}
  , {310.358,  45.280}
  , {344.413, -29.622}
};
const double Messier[110][2] ={
{83.6250,22.0167},
{323.3750,0.0000},
{205.5500,28.3833},
{245.9000,-26.5333},
{229.6500,2.0833},
{265.0250,-32.2167},
{268.4750,-34.8167},
{270.9500,-24.3833},
{259.8000,-18.5167},
{254.2750,-4.1000},
{282.7750,-6.2667},
{251.8000,-1.9500},
{250.4250,36.4667},
{264.4000,-3.2500},
{322.5000,12.1667},
{274.7000,-13.7833},
{275.2000,-16.1833},
{274.9750,-17.1333},
{255.6500,-26.2667},
{270.6500,-23.0333},
{271.1500,-22.5000},
{279.1000,-23.9000},
{269.2000,-19.0167},
{274.2250,-18.4833},
{277.9000,-19.2500},
{281.3000,-9.4000},
{299.9000,22.7167},
{276.1250,-24.8667},
{305.9750,38.5333},
{325.1000,-23.1833},
{10.6750,41.2667},
{10.6750,40.8667},
{23.4750,30.6500},
{40.5000,42.7833},
{92.2250,24.3333},
{84.0250,34.1333},
{88.1000,32.5500},
{82.1750,35.8333},
{323.0500,48.4333},
{185.6000,58.0833},
{101.7500,-20.0167},
{83.8500,-5.4500},
{83.9000,-5.2667},
{130.0250,19.9833},
{56.7500,24.1167},
{115.4500,-14.8167},
{114.1500,-14.5000},
{123.4500,-5.8000},
{187.4500,8.0000},
{105.8000,-8.3333},
{202.4750,47.2000},
{351.0500,61.5833},
{198.2250,18.1667},
{283.7750,-30.4833},
{295.0000,-30.9667},
{289.1500,30.1833},
{283.4000,33.0333},
{189.4250,11.8167},
{190.5000,11.6500},
{190.9250,11.5500},
{185.4750,4.4667},
{255.3000,-30.1167},
{198.9500,42.0333},
{194.1750,21.6833},
{169.7250,13.0833},
{170.0500,12.9833},
{132.6000,11.8167},
{189.8750,-26.7500},
{277.8500,-32.3500},
{280.8000,-32.3000},
{298.4500,18.7833},
{313.3750,-12.5333},
{314.7250,-12.6333},
{24.1750,15.7833},
{301.5250,-21.9167},
{25.6000,51.5667},
{40.6750,0.0000},
{86.6750,0.0000},
{81.1250,-24.5500},
{244.2500,-22.9833},
{148.9000,69.0667},
{148.9500,69.6833},
{204.2500,-29.8667},
{186.2750,12.8833},
{186.3500,18.1833},
{186.5500,12.9500},
{187.7000,12.4000},
{188.0000,14.4167},
{188.9250,12.5500},
{189.2000,13.1667},
{188.7500,14.5000},
{259.2750,43.1333},
{116.1500,-23.8667},
{192.7250,41.1167},
{161.0000,11.7000},
{161.7000,11.8167},
{168.7000,55.0167},
{183.4500,14.9000},
{184.7000,14.4167},
{185.7250,15.8167},
{210.8000,54.3500},
{226.6250,55.7667},
{23.3000,60.7000},
{190.0000,-11.6167},
{161.9500,12.5833},
{184.7500,47.3000},
{248.1250,-13.0500},
{167.8750,55.6667},
{179.4000,53.3833},
{10.1250,41.6833},
};
void ir_init(void);
void ir_read(void);

uint32 get_IR_lcode(uint32_t bcode);

#endif
#endif
