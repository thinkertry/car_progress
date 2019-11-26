/*********************************************************************************************************************
 * COPYRIGHT NOTICE
 * Copyright (c) 2018,逐飞科技
 * All rights reserved.
 * 技术讨论QQ群：179029047
 *
 * 以下所有内容版权均属逐飞科技所有，未经允许不得用于商业用途，
 * 欢迎各位使用并传播本程序，修改内容时必须保留逐飞科技的版权声明。
 *
 * @file       		总钻风
 * @company	   		成都逐飞科技有限公司
 * @author     		逐飞科技(QQ3184284598)
 * @version    		查看common.h内VERSION宏定义
 * @Software 		IAR 7.8 or MDK 5.24a
 * @Target core		LPC54606J512BD100
 * @Taobao   		https://seekfree.taobao.com/
 * @date       		2018-05-24
 * @note		
					接线定义：
					------------------------------------ 
						模块管脚            单片机管脚
						SDA(51的RX)         A9
						SCL(51的TX)         A8
						场中断(VSY)         A0
						行中断(HREF)		未使用，因此不接
						像素中断(PCLK)      A1        
						数据口(D0-D7)		B24-B31
					------------------------------------ 
	
					默认分辨率是            188*120
					默认FPS                 50帧
 ********************************************************************************************************************/


#include "common.h"
#include "LPC546XX_sct.h"
#include "LPC546XX_dma.h"
#include "LPC546XX_gpio.h"
#include "LPC546XX_uart.h"
#include "LPC546XX_systick.h"
#include "LPC546XX_pint.h"
#include "SEEKFREE_MT9V032.h"




                  
__ramfunc void Get_Use_Image(void);

uint8   Image_Use[CAMERA_H][CAMERA_W];
uint8   image[MT9V032_H][MT9V032_W];      //图像数组
uint16  Pixle[CAMERA_H][CAMERA_W]; 		//change to uint8
uint16 FINAL[59];
uint8   receive[3];
uint8   receive_num = 0;
uint8   uart_receive_flag = 1;

uint8 threshold;
        
//需要配置到摄像头的数据
int16 MT9V032_CFG[CONFIG_FINISH][2]=
{
    {AUTO_EXP,          0},   //自动曝光设置      范围1-63 0为关闭 如果自动曝光开启  EXP_TIME命令设置的数据将会变为最大曝光时间，也就是自动曝光时间的上限
                              //一般情况是不需要开启这个功能，因为比赛场地光线一般都比较均匀，如果遇到光线非常不均匀的情况可以尝试设置该值，增加图像稳定性
    {EXP_TIME,          160}, //曝光时间      //600    摄像头收到后会自动计算出最大曝光时间，如果设置过大则设置为计算出来的最大曝光值
    {FPS,               100},  //图像帧率      //50    摄像头收到后会自动计算出最大FPS，如果过大则设置为计算出来的最大FPS
    {SET_COL,           MT9V032_W}, //图像列数量        范围1-752     K60采集不允许超过188
    {SET_ROW,           MT9V032_H}, //图像行数量        范围1-480
    {LR_OFFSET,         0},   //图像左右偏移量    正值 右偏移   负值 左偏移  列为188 376 752时无法设置偏移    摄像头收偏移数据后会自动计算最大偏移，如果超出则设置计算出来的最大偏移
    {UD_OFFSET,         0},   //图像上下偏移量    正值 上偏移   负值 下偏移  行为120 240 480时无法设置偏移    摄像头收偏移数据后会自动计算最大偏移，如果超出则设置计算出来的最大偏移
    {GAIN,              25},  //图像增益     //32     范围16-64     增益可以在曝光时间固定的情况下改变图像亮暗程度

    
    {INIT,              0}    //摄像头开始初始化
};

//从摄像头内部获取到的配置数据
int16 GET_CFG[CONFIG_FINISH-1][2]=
{
    {AUTO_EXP,          0},   //自动曝光设置      
    {EXP_TIME,          0},   //曝光时间          
    {FPS,               0},   //图像帧率          
    {SET_COL,           0},   //图像列数量        
    {SET_ROW,           0},   //图像行数量        
    {LR_OFFSET,         0},   //图像左右偏移量    
    {UD_OFFSET,         0},   //图像上下偏移量    
    {GAIN,              0},   //图像增益          
};


//-------------------------------------------------------------------------------------------------------------------
//  @brief      MT9V032摄像头串口中断函数
//  @param      NULL
//  @return     void					
//  @since      v1.0
//  Sample usage:	
//  @note       该函数在ISR文件 串口3中断程序被调用
//-------------------------------------------------------------------------------------------------------------------

/*改了*/
void mt9v032_cof_uart_interrupt(void)
{
    uart_query(MT9V032_COF_UART,&receive[receive_num]);
    receive_num++;
    
    if(1==receive_num && 0XA5!=receive[0])  receive_num = 0;
    if(3 == receive_num)
    {
        receive_num = 0;
        uart_receive_flag = 1;
    }
        
}
/* */

//-------------------------------------------------------------------------------------------------------------------
//  @brief      获取摄像头内部配置信息
//  @param      NULL
//  @return     void
//  @since      v1.0
//  Sample usage:				调用该函数前请先初始化uart3
//-------------------------------------------------------------------------------------------------------------------
void get_config(void)
{
	uint16 temp, i;
    uint8  send_buffer[4];
    
    for(i=0; i<CONFIG_FINISH-1; i++)
    {
        send_buffer[0] = 0xA5;
        send_buffer[1] = GET_STATUS;
        temp = GET_CFG[i][0];
        send_buffer[2] = temp>>8;
        send_buffer[3] = (uint8)temp;
        
        uart_putbuff(MT9V032_COF_UART,send_buffer,4);
        
        //等待接受回传数据
        while(!uart_receive_flag);
        uart_receive_flag = 0;
        
        GET_CFG[i][1] = receive[1]<<8 | receive[2];
        
    }
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      获取摄像头固件版本
//  @param      NULL
//  @return     void
//  @since      v1.0
//  Sample usage:				调用该函数前请先初始化uart3
//-------------------------------------------------------------------------------------------------------------------
uint16 get_version(void)
{
    uint16 temp;
    uint8  send_buffer[4];
    send_buffer[0] = 0xA5;
    send_buffer[1] = GET_STATUS;
    temp = GET_VERSION;
    send_buffer[2] = temp>>8;
    send_buffer[3] = (uint8)temp;
    
    uart_putbuff(MT9V032_COF_UART,send_buffer,4);
        
    //等待接受回传数据
    while(!uart_receive_flag);
    uart_receive_flag = 0;
    
    return ((uint16)(receive[1]<<8) | receive[2]);
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      单独设置摄像头曝光时间
//  @param      light   设置曝光时间越大图像越亮，摄像头收到后会根据分辨率及FPS计算最大曝光时间如果设置的数据过大，那么摄像头将会设置这个最大值
//  @return     uint16  当前曝光值，用于确认是否正确写入
//  @since      v1.0
//  Sample usage:				
//-------------------------------------------------------------------------------------------------------------------
uint16 set_exposure_time(uint16 light)
{
	uint16 temp;
    uint8  send_buffer[4];

    send_buffer[0] = 0xA5;
    send_buffer[1] = SET_EXP_TIME;
    temp = light;
    send_buffer[2] = temp>>8;
    send_buffer[3] = (uint8)temp;
    
    uart_putbuff(MT9V032_COF_UART,send_buffer,4);
    
    //等待接受回传数据
    while(!uart_receive_flag);
    uart_receive_flag = 0;
    
    temp = receive[1]<<8 | receive[2];
    return temp;

}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      对摄像头内部寄存器进行写操作
//  @param      addr    摄像头内部寄存器地址
//  @param      data    需要写入的数据
//  @return     uint16  寄存器当前数据，用于确认是否写入成功
//  @since      v1.0
//  Sample usage:				
//-------------------------------------------------------------------------------------------------------------------
uint16 set_mt9v032_reg(uint8 addr, uint16 data)
{
	uint16 temp;
    uint8  send_buffer[4];
    
    send_buffer[0] = 0xA5;
    send_buffer[1] = SET_ADDR;
    temp = addr;
    send_buffer[2] = temp>>8;
    send_buffer[3] = (uint8)temp;
    
    uart_putbuff(MT9V032_COF_UART,send_buffer,4);
    systick_delay_ms(10);
    
    send_buffer[0] = 0xA5;
    send_buffer[1] = SET_DATA;
    temp = data;
    send_buffer[2] = temp>>8;
    send_buffer[3] = (uint8)temp;
    
    uart_putbuff(MT9V032_COF_UART,send_buffer,4);
    
    //等待接受回传数据
    while(!uart_receive_flag);
    uart_receive_flag = 0;
    
    temp = receive[1]<<8 | receive[2];
    return temp;

}


//-------------------------------------------------------------------------------------------------------------------
//  @brief      MT9V032摄像头初始化
//  @param      NULL
//  @return     void					
//  @since      v1.0
//  Sample usage:		
//-------------------------------------------------------------------------------------------------------------------
void camera_init(void)
{
    //设置参数    具体请参看使用手册
    uint16 temp, i;
    uint8  send_buffer[4];

    
    uart_init (MT9V032_COF_UART, 9600,MT9V032_COF_UART_TX,MT9V032_COF_UART_RX);	//初始换串口 配置摄像头    
    uart_rx_irq(MT9V032_COF_UART,1);
    EnableInterrupts;
    
    //等待摄像头上电初始化成功
    systick_delay_ms(1000);
    uart_receive_flag = 0;
    
    //开始配置摄像头并重新初始化
    for(i=0; i<CONFIG_FINISH; i++)
    {
        send_buffer[0] = 0xA5;
        send_buffer[1] = MT9V032_CFG[i][0];
        temp = MT9V032_CFG[i][1];
        send_buffer[2] = temp>>8;
        send_buffer[3] = (uint8)temp;
        
        uart_putbuff(MT9V032_COF_UART,send_buffer,4);
        systick_delay_ms(2);
    }
    //等待摄像头初始化成功
    while(!uart_receive_flag);
    uart_receive_flag = 0;
    while((0xff != receive[1]) || (0xff != receive[2]));
    //以上部分对摄像头配置的数据全部都会保存在摄像头上51单片机的eeprom中
    //利用set_exposure_time函数单独配置的曝光数据不存储在eeprom中
    
    //获取配置便于查看配置是否正确
    get_config();
    
    
    //摄像头采集初始化
    DisableInterrupts;
    //初始化SCT模块用于接收PCLK信号并触发DMA
    sct_camera_dma(MT9V032_PCLK_SCT, MT9V032_PCLK, RISING);
    dam_init(MT9V032_DMA_CH, (void *)&MT9V032_DATAPORT, (void *)image[0], MT9V032_W);
    pint_init(MT9V032_VSYNC_PINT,MT9V032_VSYNC_PIN,FALLING);
    
    
    set_irq_priority(MT9V032_VSYNC_IRQN,0); //设置场中断为最高优先级
    set_irq_priority(DMA0_IRQn,1);          //设置DMA中断为第二优先级
    
    enable_irq(MT9V032_VSYNC_IRQN);
    enable_irq(DMA0_IRQn);
    sct_start();

	EnableInterrupts;
}


uint16  mt9v032_now_row = 0;        //当前采集行数
uint8   mt9v032_finish_flag = 0;    //一场图像采集完成标志位
//-------------------------------------------------------------------------------------------------------------------
//  @brief      MT9V032摄像头场中断
//  @param      NULL
//  @return     void			
//  @since      v1.0
//  Sample usage:				在isr.c里面先创建对应的中断函数，然后调用该函数(之后别忘记清除中断标志位)
//-------------------------------------------------------------------------------------------------------------------
void mt9v032_vsync(void)
{
    PINT_IST_FLAG_CLEAR(MT9V032_VSYNC_PINT);    //清除标志位
    DMA_DISABLE(MT9V032_DMA_CH);
    mt9v032_now_row = 0;
    if(SCT_DMA0REQUEST_DRQ0_MASK & SCT0->DMA0REQUEST){sct_restart();}
    dma_reload(MT9V032_DMA_CH, (void *)&MT9V032_DATAPORT, (void *)image[0], MT9V032_W);
    DMA_ENABLE(MT9V032_DMA_CH);
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      MT9V032摄像头DMA完成中断
//  @param      NULL
//  @return     void			
//  @since      v1.0
//  Sample usage:				在isr.c里面先创建对应的中断函数，然后调用该函数(之后别忘记清除中断标志位)
//-------------------------------------------------------------------------------------------------------------------
void mt9v032_dma(void)
{
	CLEAR_DMA_FLAG(MT9V032_DMA_CH);
    mt9v032_now_row++;

    if((MT9V032_H)>mt9v032_now_row)
    {
        dma_reload(MT9V032_DMA_CH, (void *)&MT9V032_DATAPORT, (void *)image[mt9v032_now_row], MT9V032_W);
        DMA_ENABLE(MT9V032_DMA_CH);
    }
    else    mt9v032_finish_flag = 1;   
    //一副图像从采集开始到采集结束耗时3.8MS左右(50FPS、188*120分辨率)
}
 

//-------------------------------------------------------------------------------------------------------------------
//  @brief      总钻风摄像头图像发送至上位机查看图像
//  @param      uartn			使用的串口号
//  @return     void			
//  @since      v1.0
//  Sample usage:				
//-------------------------------------------------------------------------------------------------------------------
void seekfree_sendimg_032(UARTN_enum uartn)
{
	uart_putchar(uartn,0x00);uart_putchar(uartn,0xff);uart_putchar(uartn,0x01);uart_putchar(uartn,0x01);//发送命令
    uart_putbuff(uartn, (uint8_t *)image, MT9V032_H*MT9V032_W);  //发送图像
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      总钻风摄像头图像二值化
//  @param      void
//  @return     void			
//  @since      v1.0
//  Sample usage:				
//-------------------------------------------------------------------------------------------------------------------

/*************************************************************** 
* 
* 函数名称：uint8_t GetOSTU(uint8_t tmImage[IMAGEH][IMAGEW]) 
* 功能说明：求阈值大小 
* 参数说明： 
* 函数返回：阈值大小 
* 修改时间：2018年3月27日 
* 备 注： 
参考：https://blog.csdn.net/zyzhangyue/article/details/45841255
      https://www.cnblogs.com/moon1992/p/5092726.html
      https://www.cnblogs.com/zhonghuasong/p/7250540.html     
Ostu方法又名最大类间差方法，通过统计整个图像的直方图特性来实现全局阈值T的自动选取，其算法步骤为：
1) 先计算图像的直方图，即将图像所有的像素点按照0~255共256个bin，统计落在每个bin的像素点数量
2) 归一化直方图，也即将每个bin中像素点数量除以总的像素点
3) i表示分类的阈值，也即一个灰度级，从0开始迭代
4) 通过归一化的直方图，统计0~i 灰度级的像素(假设像素值在此范围的像素叫做前景像素) 所占整幅图像的比例w0，并统计前景像素的平均灰度u0；统计i~255灰度级的像素(假设像素值在此范围的像素叫做背景像素) 所占整幅图像的比例w1，并统计背景像素的平均灰度u1；
5) 计算前景像素和背景像素的方差 g = w0*w1*(u0-u1) (u0-u1)
6) i++；转到4)，直到i为256时结束迭代
7）将最大g相应的i值作为图像的全局阈值
缺陷:OSTU算法在处理光照不均匀的图像的时候，效果会明显不好，因为利用的是全局像素信息。
***************************************************************/ 
uint8_t GetOSTU(uint8_t tmImage[CAMERA_H][CAMERA_W]) 
{ 
  // tmImage 图像每个点的灰度值
  int16_t i,j; 
  uint32_t Amount = 0; 
  uint32_t PixelBack = 0; 
  uint32_t PixelIntegralBack = 0; 
  uint32_t PixelIntegral = 0; 
  int32_t PixelIntegralFore = 0; 
  int32_t PixelFore = 0; 
  double OmegaBack, OmegaFore, MicroBack, MicroFore, SigmaB, Sigma; // 类间方差; 
  int16_t MinValue, MaxValue;  //最小灰度值 最大灰度值
  uint8_t Threshold = 0;
  
  
  uint16 HistoGram[256];              

  // 初始化灰度直方图
  for (j = 0; j < 256; ++j)  HistoGram[j] = 0; //初始化灰度直方图 
  
  for (j = 0; j < CAMERA_H; ++j) 
  { 
    for (i = 0; i < CAMERA_W; ++i) 
    { 
      ++HistoGram[tmImage[j][i]]; //统计灰度级0-255中每个bin在整幅图像中的像素个数
    } 
  } 
  // 计算阈值
  for (MinValue = 0; MinValue < 256 && HistoGram[MinValue] == 0; MinValue++) ;        //获取最小灰度的值
  for (MaxValue = 255; MaxValue > MinValue && HistoGram[MaxValue] == 0; MaxValue--) ; //获取最大灰度的值
  /*mk*/  
  //if (MaxValue == MinValue)     return MaxValue;         // 图像中只有一个颜色    
  //if (MinValue + 1 == MaxValue)  return MinValue;        // 图像中只有二个颜色
    if (MaxValue == MinValue || MaxValue == MinValue + 1) return MinValue; //图像中只有一个或两个灰度
  
  for (j = MinValue; j <= MaxValue; ++j)    Amount += HistoGram[j];        //  像素总数
    
  PixelIntegral = 0; //灰度和
  for (j = MinValue; j <= MaxValue; ++j)
  {
    PixelIntegral += HistoGram[j] * j;//灰度和 = ∑灰度值 * 像素点数
  }
  SigmaB = -1;
  for (j = MinValue; j < MaxValue; ++j) // 是否可以  for (j = MinValue; j < MaxValue - 1; ++j)
  {
    //假设取阈值为 j
    PixelBack = PixelBack + HistoGram[j];    //前景像素点数
    PixelFore = Amount - PixelBack;         //背景像素点数
    OmegaBack = (double)PixelBack / Amount;//前景像素百分比
    OmegaFore = (double)PixelFore / Amount;//背景像素百分比 //1-?
    PixelIntegralBack += HistoGram[j] * j;  //前景灰度值
    PixelIntegralFore = PixelIntegral - PixelIntegralBack;//背景灰度值
    MicroBack = (double)PixelIntegralBack / PixelBack;   //前景灰度百分比 **这哪是前景灰度百分比 这是平均前景灰度**
    MicroFore = (double)PixelIntegralFore / PixelFore;   //背景灰度百分比 **这也是 这是平均背景灰度** //1-?
    Sigma = OmegaBack * OmegaFore * (MicroBack - MicroFore) * (MicroBack - MicroFore);//计算类间方差 
    if (Sigma > SigmaB)                    //遍历最大的类间方差g //找出最大类间方差以及对应的阈值
    {
      SigmaB = Sigma;
      Threshold = j;
    }
  }
  return Threshold;                        //返回最佳阈值;
} 

void Get_01_Value(void)
{
  int i = 0,j = 0;
  for(i = 0; i < LCDH; i++)
  {
    for(j = 0; j < LCDW; j++)
    {                                
      
      if(Image_Use[i][j] > 110) //大津法阈值   数值越大，显示的内容越多，较浅的图像也能显示出来    
        Pixle[i][j] =1;        
      else                                        
        Pixle[i][j] =0;
    }    
  }
}

void swap(uint8_t *p,int a,int b){
	uint8_t temp;
	temp=p[a];
	p[a]=p[b];
	p[b]=temp;
}


void sobel()//Sobel边沿检测
{
        
	int tempx=0,tempy=0,i=0,j=0;
        double tempsqrt=0; 
        threshold=GetOSTU(Image_Use);
	for(i=1;i <LCDH-1; i++)
            for(j=1;j<LCDW-1;j++)
            {
                
                if(Image_Use[i][j]<threshold + threshold_offset)
                {
                  Pixle[i][j] = 0;
                if(j == 40)
                  FINAL[i] = 0;
                  continue;
                }
		tempx=-Image_Use[i-1][j-1]-2*Image_Use[i][j-1]-Image_Use[i+1][j-1]+Image_Use[i-1][j+1]+2*Image_Use[i][j+1]+Image_Use[i+1][j+1];				
		tempy= Image_Use[i+1][j-1]+2*Image_Use[i+1][j]+Image_Use[i+1][j+1]-Image_Use[i-1][j-1]-2*Image_Use[i-1][j]-Image_Use[i-1][j+1];
                tempsqrt=sqrt(tempx*tempx+tempy*tempy);
                if(i<Sobel_Near_Far_Line && tempsqrt > Sobel_Threshold_Far && Image_Use[i][j]<200)
                {   Pixle[i][j] = 0;
                if(j == 40)
                    FINAL[i] = 0;
                }
                else if(i>=Sobel_Near_Far_Line && tempsqrt > Sobel_Threshold_Near && Image_Use[i][j]<200)
                {   Pixle[i][j] = 0;
                if(j == 40)
                    FINAL[i] = 0;
                }
                else
                {   Pixle[i][j] = 1;
                if(j == 40)
                    FINAL[i] = 1;
                }
            }
	
}
/*************************************************************************
*  函数名称：void Pic_particular()
*  功能说明：图像特殊处理
*  参数说明：无
*  函数返回：无
*  修改时间：2019.7.7
*  备    注：
*************************************************************************/

int Lef_edge=0,Rig_edge=0;
void Pic_particular(void)
{
  int i;
  Lef_edge=0;
  Rig_edge=0;
  for(i=Last_row;i>Fir_row-1;i--)
  {
    if(New_Lef[i]==-MIDMAP)
      Lef_edge+=1;
    if(New_Rig[i]==MIDMAP)
      Rig_edge+=1;
  }
  for(i=Last_row;i>Fir_row-1;i--)//平均值法中心线绘制
  {
      Mid[i]=(int)((New_Lef[i]+New_Rig[i])/20)+40;
  }
}

/*************************************************************************
*  函数名称：void Pic_main()
*  功能说明：图像处理主函数
*  参数说明：无
*  函数返回：无
*  修改时间：2019.5.31
*  备    注：
*************************************************************************/

int circle_set = 350;
int circle_set2 = -350;
int circle_flag = 0;

void Camera_main(void)//摄像头处理主函数
{  
  
      //static char Road3_cnt = 0 ; 
      Get_Use_Image(); //图像预处理
      /*
     if(Road == 0) //直
     {
       Turn_Angle = 0;
       circle_flag = 0;
     }
     if(Road == 1) //左or右
     {
       if( (circle_set - Turn_Angle) <1)//Cam_Block_Cnt == 100)
    {
      Road = 0;
    }
     }
     if(Road == 2) //左or右
     {
       if( (circle_set + Turn_Angle) <1)//Cam_Block_Cnt == 100)
    {
      Road = 0;
    }
       
     }
     if(Road == 6 && Road6_flag != 2)//||(Cam_Block_State ==1))
      { 
         //sobel边缘检测
        Get_01_Value();
      
      }
      else
      {
        */
       sobel();
       //if(Road ==1 && Road1_flag == 4)//||Road ==2)
       //{ 
          //Pic_seedfill();///种子搜索算法
       //}
       Pic_noi_elim();//图像简单去噪点
     // }
      Pic_DrawMid();//寻找左右边线  
      Cam_End_Detect();
      Pic_undistort();//图像去畸变
      Pic_particular();
      LR_Slope_fig();//左右边线斜率计算
      Allwhite_find();//查找全白行
      Pic_find_circle();//寻找环状黑线及拐点
      Pic_find_leap();//寻找突变点
      Pic_Block_Rec();
      Cam_Break_Rec();
      Road_rec();//利用左右边线斜率识别赛道
      Pic_Fix_Line();//补线处理
      Pic_offset_fig();//offset计算
      Pic_offset_filter();//offset滤波 
      
      //if(Road == 0)
      //{
       
        //}
     }

__ramfunc void Get_Use_Image(void)
{
	int i = 0,j = 0;
	int row = 0;
	for(i = 20; i  < 80; i+=1)  //120行，每2行采集一行，15  74
	{
		for(j = 15;j < 174; j+=2)  //188，取94列
		{        
			Image_Use[row][(j-15)>>1] = image[i][j];   
                 
		}     
                row+=1;
	}  
}