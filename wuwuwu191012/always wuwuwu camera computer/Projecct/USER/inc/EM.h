#ifndef _EM_H_
#define _EM_H_

#include "headfile.h"

extern float EM_Value_1 ;
extern float EM_Value_2 ;
extern float EM_Value_3 ;
extern float EM_Value_4 ;
//extern float EM_Value_5 ;
//extern float EM_Value_6 ;

extern int EM_Ring_State ;
extern int EM_Ramp_State ;
extern int Peak_Cnt;
extern int EM_Road;
extern float Lef_EM_Sum;
extern float Rig_EM_Sum;
extern float EM_Ramp_4_Speed;



void EM_Init(void);
void EM_main(void);
void EM_Get(void);
void EM_offset_fig(void);
void EM_offset_filter(void);
void EM_Ring_Rec(void);
void EM_Ramp_Rec(void);
void EM_Speed_Control(void);
void EM_Curve_Rec(void);
void Block_By_Pass(void);


int limit_em(int n, int lower, int higher);
float abs_f_em(float n);
#endif 