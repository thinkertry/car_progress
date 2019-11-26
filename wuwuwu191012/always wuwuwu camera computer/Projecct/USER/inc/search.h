#ifndef _SEARCH_H_
#define _SEARCH_H_

#include "headfile.h"

#define MIDMAP 300

#define Car_Speed_Str     1   //直道速度
#define Car_Speed_Cur1    0.8   //弯道速度1
#define Car_Speed_Cur2    0.9   //弯道速度2
#define Car_Speed_Cur3    0.5   //弯道速度3

#define FIG_AREA_NEAR   50//350
#define FIG_AREA_FAR   28//550   //逆透视变换后的图像计算区域
#define FIG_AREA_FAR2   20//600
#define FIG_AREA_FAR3   15//700

#define FIG_AREA_NEAR1   100//350
#define FIG_AREA_FAR1   200//550   //逆透视变换后的图像计算区域

#define ROAD_HALF_WIDTH  150  //赛道半宽

#define Fir_row      2        //以第7行作为最小有效行
#define Last_row     (LCDH-2)    //以该行为最大有效行
#define Fir_col      2         //以第7列作为最小有效列
#define Last_col     (LCDW-3)    //以该列为最大有效列

//图像DFS搜索算法起始搜索区域
#define seed_down	55
#define seed_up		40
#define seed_left	30
#define seed_right	50

#define CAMERA_W 80
#define CAMERA_H 60

#define LCDH    60  //OLED显示的行数
#define LCDW    80  //OLED显示的列数

//图像DFS搜索算法起始搜索区域
#define seed_down	55
#define seed_up		40
#define seed_left	30
#define seed_right	50

#define LEFT        0
#define RIGHT       79
#define TOP         0
#define BOTTOM      59

void Pic_undistort(void);//图像去畸变

void pixel_undistort(int x,int y,int LR);//像素去畸变
extern void Allwhite_find(void);//查找全白行
extern void Pic_find_circle(void);//左右黑线寻找环状黑线
extern void Pic_find_leap(void);//寻找突变点
void Pic_Block_Rec(void);//障碍物识别
void Cam_Break_Rec(void);//断路识别

void Pic_seedfill(void); 
char Final_line(void);
void Pic_seedfill_grow(uint8 flag[CAMERA_H][CAMERA_W],int i,int j);   //DFS搜索算法递归函数

void Pic_noi_elim(void);//图像噪点去除
extern void Road_rec(void);
extern void Pic_offset_fig(void);//offset计算
extern void Pic_offset_filter(void);//offset滤波处理
extern void Pic_main(void);
extern void Pic_Fix_Line(void);//补线程序
extern void Pic_thresh_get(void);
void Pic_DrawMid(void);//图像绘制中心线

void Cam_End_Detect(void); //End记录从上至下最后一行边界行


extern void LR_Slope_fig(void);
extern float Slope(int F1x,int F1y,int F2x,int F2y);
extern float L_R_Slope(char LR);

extern float offset_map[60];

extern int Lef[LCDH]; //道路左分离点的纵坐标
extern int Rig[LCDH]; //道路右分离点的纵坐标
extern int Mid[LCDH]; //道路中心点的纵坐标

extern int Lef_leap[4];
extern int Rig_leap[4];//左右变线跳变点坐标
extern int Fix_line;
extern int Road;
extern int Road1_flag;
extern int Road2_flag;
extern int Allwhitestart;
extern int Allwhiteend;
extern int Allwhitestart2;
extern int Allwhiteend2;

extern float Lef_slope;
extern float Rig_slope;
extern float LR_slope;
extern int Lef_circle;
extern int Rig_circle;
extern int Lef_break_point;
extern int Rig_break_point;
extern int Cam_Block_State;
extern int Middle;
extern int end;

//倒跑
extern float err;
extern int angle;
extern float offset_map[60];
extern int Lef_edge,Rig_edge;
extern int Road6_flag;
extern int ramp_cnt ;
extern int iii;
extern int block_flag;

#endif
