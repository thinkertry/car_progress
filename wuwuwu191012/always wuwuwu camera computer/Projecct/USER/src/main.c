/*********************************************************************************************************************
 * COPYRIGHT NOTICE
 * Copyright (c) 2018,��ɿƼ�
 * All rights reserved.
 * ��������QQȺ��179029047
 *
 * �����������ݰ�Ȩ������ɿƼ����У�δ��������������ҵ��;��
 * ��ӭ��λʹ�ò������������޸�����ʱ���뱣����ɿƼ��İ�Ȩ������
 *
 * @file       		main
 * @company	   		�ɶ���ɿƼ����޹�˾
 * @author     		��ɿƼ�(QQ3184284598)
 * @version    		�鿴LPC546XX_config.h�ļ��ڰ汾�궨��
 * @Software 		IAR 7.8 or MDK 5.24a
 * @Target core		LPC54606J512BD100
 * @Taobao   		https://seekfree.taobao.com/
 * @date       		2018-05-24
 ********************************************************************************************************************/

/*
�ӿڶ��壺

����ͷ             ��Ƭ���ӿ�
D0-D7���ݿ�        B24 B25 B26 B27 B28 B29 B30 B31
SDA                A9   F5
SCL                A8
���ź�             A0
�����ź�           A1

�˷�
ͨ��1              A10
ͨ��2              A15
ͨ��3              A16
ͨ��4              A31
ͨ��5              B0
ͨ��6              A23

IIC                 
SDA                A13 F1
SCL                A14

���PWM
PWM1               A18   T1
PWM2               A20
PWM3               B4   T2
PWM4               B5

������         
LSB1               A2   T0
DIR                A3
LSB2               A4  T3
DIR                A5


���   
PWM                A6 T4

OLED/TFT
SCK                B19
MOSI               B21
MISO               B20      Һ��û���������  ����SPI��ռ���������
CS                 B9 F4

DC                 B10
RES                B8
BL                 B7

UART                
RX                 A24
TX                 A25  

����   
B1 B2 B3 B23���⽻��  


���뿪��		
B17  B22���⽻��  

NRF
B15 B14 A29 A22 A21 A17	���⽻��  

*/


//���µĹ��̻��߹����ƶ���λ�����ִ�����²���
//��һ�� �ر��������д򿪵��ļ�
//�ڶ��� project  clean  �ȴ��·�����������

#include "headfile.h"

int threshold_offset = 5;

float offset_map[60]={(float)31/77,(float)31/77,(float)31/9	,(float)31/9	 ,(float)31/10	,(float)31/11	,(float)31/12	 ,(float)31/13	,(float)31/14,(float)31/15,(float)31/16		 	
,(float)31/17,(float)31/17,(float)31/19,(float)31/19,(float)31/21	,(float)31/21,(float)31/23,(float)31/24	,(float)31/25	 ,(float)31/25	,(float)31/27,(float)31/28,(float)31/29	 	 	
,(float)31/30,(float)31/31,(float)31/31	,(float)31/33,(float)31/34,(float)31/34,(float)31/36,(float)31/36	,(float)31/36	,(float)31/38	,(float)31/40,(float)31/40,(float)31/42,(float)31/42	  	
,(float)31/42	,(float)31/44	 ,(float)31/46	 ,(float)31/46	 ,(float)31/48	  ,(float)31/48	,(float)31/48	,(float)31/50	,(float)31/51,(float)31/52,(float)31/53,(float)31/54,(float)31/54,(float)31/56,(float)31/56,(float)31/58,(float)31/58,(float)31/58,(float)31/60,(float)31/60,(float)31/61,(float)31/77};


int main(void)
{   
    get_clk();
    
    uart_init(USART_0,1500000,UART0_TX_A25,UART0_RX_A24);
    uart_rx_irq(USART_0,1);       		// �򿪴���0�����ж�
    //oled_init();
    //��ʾģʽ����Ϊ2  ����ģʽ
    //lcd_init();     //��ʼ��TFT��Ļ
    
    camera_init();  //��ʼ������ͷ
    EnableInterrupts;
    

    while(1)
    {
        //printf("��ɿƼ� printf����");
        //oled_dis_bmp(MT9V032_H,MT9V032_W, image[0],130);
        //lcd_displayimage032(image[0],MT9V032_W,MT9V032_H);
        //Get_Use_Image();//ȡ��Чͼ��
        //Get_01_Value();//��ֵ��
        //sobel();
        //Pic_noi_elim();
        //Pic_undistort();
        Camera_main();
        Send_Data();
    }
}


