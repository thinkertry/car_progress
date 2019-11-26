/*********************************************************************************************************************
 * COPYRIGHT NOTICE
 * Copyright (c) 2018,逐飞科技
 * All rights reserved.
 * 技术讨论QQ群：179029047
 *
 * 以下所有内容版权均属逐飞科技所有，未经允许不得用于商业用途，
 * 欢迎各位使用并传播本程序，修改内容时必须保留逐飞科技的版权声明。
 *
 * @file       		main
 * @company	   		成都逐飞科技有限公司
 * @author     		逐飞科技(QQ3184284598)
 * @version    		查看LPC546XX_config.h文件内版本宏定义
 * @Software 		IAR 7.8 or MDK 5.24a
 * @Target core		LPC54606J512BD100
 * @Taobao   		https://seekfree.taobao.com/
 * @date       		2018-05-24
 ********************************************************************************************************************/

/*
接口定义：

摄像头             单片机接口
D0-D7数据口        B24 B25 B26 B27 B28 B29 B30 B31
SDA                A9   F5
SCL                A8
场信号             A0
像素信号           A1

运放
通道1              A10
通道2              A15
通道3              A16
通道4              A31
通道5              B0
通道6              A23

IIC                 
SDA                A13 F1
SCL                A14

电机PWM
PWM1               A18   T1
PWM2               A20
PWM3               B4   T2
PWM4               B5

编码器         
LSB1               A2   T0
DIR                A3
LSB2               A4  T3
DIR                A5


舵机   
PWM                A6 T4

OLED/TFT
SCK                B19
MOSI               B21
MISO               B20      液晶没有这个引脚  但是SPI会占用这个引脚
CS                 B9 F4

DC                 B10
RES                B8
BL                 B7

UART                
RX                 A24
TX                 A25  

按键   
B1 B2 B3 B23任意交换  


拨码开关		
B17  B22任意交换  

NRF
B15 B14 A29 A22 A21 A17	任意交换  

*/


//打开新的工程或者工程移动了位置务必执行以下操作
//第一步 关闭上面所有打开的文件
//第二步 project  clean  等待下方进度条走完

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
    uart_rx_irq(USART_0,1);       		// 打开串口0接收中断
    //oled_init();
    //显示模式设置为2  横屏模式
    //lcd_init();     //初始化TFT屏幕
    
    camera_init();  //初始化摄像头
    EnableInterrupts;
    

    while(1)
    {
        //printf("逐飞科技 printf测试");
        //oled_dis_bmp(MT9V032_H,MT9V032_W, image[0],130);
        //lcd_displayimage032(image[0],MT9V032_W,MT9V032_H);
        //Get_Use_Image();//取有效图像
        //Get_01_Value();//二值化
        //sobel();
        //Pic_noi_elim();
        //Pic_undistort();
        Camera_main();
        Send_Data();
    }
}


