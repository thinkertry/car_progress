#ifndef _SEARCH_H_
#define _SEARCH_H_

#include "headfile.h"

#define MIDMAP 300

#define Car_Speed_Str     1   //ֱ���ٶ�
#define Car_Speed_Cur1    0.8   //����ٶ�1
#define Car_Speed_Cur2    0.9   //����ٶ�2
#define Car_Speed_Cur3    0.5   //����ٶ�3

#define FIG_AREA_NEAR   50//350
#define FIG_AREA_FAR   28//550   //��͸�ӱ任���ͼ���������
#define FIG_AREA_FAR2   20//600
#define FIG_AREA_FAR3   15//700

#define FIG_AREA_NEAR1   100//350
#define FIG_AREA_FAR1   200//550   //��͸�ӱ任���ͼ���������

#define ROAD_HALF_WIDTH  150  //�������

#define Fir_row      2        //�Ե�7����Ϊ��С��Ч��
#define Last_row     (LCDH-2)    //�Ը���Ϊ�����Ч��
#define Fir_col      2         //�Ե�7����Ϊ��С��Ч��
#define Last_col     (LCDW-3)    //�Ը���Ϊ�����Ч��

//ͼ��DFS�����㷨��ʼ��������
#define seed_down	55
#define seed_up		40
#define seed_left	30
#define seed_right	50

#define CAMERA_W 80
#define CAMERA_H 60

#define LCDH    60  //OLED��ʾ������
#define LCDW    80  //OLED��ʾ������

//ͼ��DFS�����㷨��ʼ��������
#define seed_down	55
#define seed_up		40
#define seed_left	30
#define seed_right	50

#define LEFT        0
#define RIGHT       79
#define TOP         0
#define BOTTOM      59

void Pic_undistort(void);//ͼ��ȥ����

void pixel_undistort(int x,int y,int LR);//����ȥ����
extern void Allwhite_find(void);//����ȫ����
extern void Pic_find_circle(void);//���Һ���Ѱ�һ�״����
extern void Pic_find_leap(void);//Ѱ��ͻ���
void Pic_Block_Rec(void);//�ϰ���ʶ��
void Cam_Break_Rec(void);//��·ʶ��

void Pic_seedfill(void); 
char Final_line(void);
void Pic_seedfill_grow(uint8 flag[CAMERA_H][CAMERA_W],int i,int j);   //DFS�����㷨�ݹ麯��

void Pic_noi_elim(void);//ͼ�����ȥ��
extern void Road_rec(void);
extern void Pic_offset_fig(void);//offset����
extern void Pic_offset_filter(void);//offset�˲�����
extern void Pic_main(void);
extern void Pic_Fix_Line(void);//���߳���
extern void Pic_thresh_get(void);
void Pic_DrawMid(void);//ͼ�����������

void Cam_End_Detect(void); //End��¼�����������һ�б߽���


extern void LR_Slope_fig(void);
extern float Slope(int F1x,int F1y,int F2x,int F2y);
extern float L_R_Slope(char LR);

extern float offset_map[60];

extern int Lef[LCDH]; //��·�������������
extern int Rig[LCDH]; //��·�ҷ�����������
extern int Mid[LCDH]; //��·���ĵ��������

extern int Lef_leap[4];
extern int Rig_leap[4];//���ұ������������
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

//����
extern float err;
extern int angle;
extern float offset_map[60];
extern int Lef_edge,Rig_edge;
extern int Road6_flag;
extern int ramp_cnt ;
extern int iii;
extern int block_flag;

#endif
