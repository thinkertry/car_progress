#include "headfile.h"
#include "EM.h"

float EM_Value_1 = 0;
float EM_Value_2 = 0;
float EM_Value_3 = 0;
float EM_Value_4 = 0;
//float EM_Value_5 = 0;
//float EM_Value_6 = 0;

float EM_offset_buff[4]={0};

int EM_Peak_Time_Cnt = 0;
int EM_Ramp_Time_Cnt = 0;
int EM_Ramp_State7_Cnt = 0;
int Peak_Cnt = 0;
int Curve_Cnt=0;
float EM_Value_23_Sum = 0;
float Old_EM_Value_23_Sum = 0;
float Peak_Point = 0;
float EM_Ring_Min = 6;
float EM_Ring_Max = 0;
float EM_2_Max = 0;
float EM_3_Max = 0;
float EM_Road1_Cnt = 0;
float EM_Road2_Cnt = 0;
float EM_Ramp_Distance_2 = 0;
float EM_Ramp_4_Speed = 0;
int EM_Ring_State = 0;
int  EM_Ramp_State = 0;
int  EM_Road = 0;
int Judge_Flag = 0;
float Lef_EM_Sum = 0;
float Rig_EM_Sum = 0;



float L2_LEFT_TABLE[17]      = {    0 , 0.034 , 0.092 , 0.162 ,  0.27 , 0.428 , 0.594 ,   0.8 , 1.082 , 1.397 , 1.806 , 2.244 , 2.63 ,   2.89 , 2.95 ,  3.05 , 3.1 };
float L2_EM_OFFSET_TABLE[17] = { -400 ,  -200 ,  -190 ,  -180 ,  -170 ,  -160 ,  -150 ,  -140 ,  -130 ,  -120 ,  -110 ,  -100 ,  -90 ,    -80 ,  -70 ,  -60  , -50 };

float L3_RIGHT_TABLE[16]     = {  3.1 ,  3.05 , 2.968 , 2.822 , 2.551 , 2.075 , 1.618 , 1.235 , 0.942 , 0.704 , 0.508 , 0.356 , 0.232 , 0.141 , 0.078 ,    0 };
float L3_EM_OFFSET_TABLE[16] = {   60 ,    70 ,    80 ,    90 ,   100 ,   110 ,   120 ,   130 ,   140 ,   150 ,   160 ,   170 ,   180 ,   190 ,   200 ,  400 };

extern uint8 Running;



int limit_em(int n, int lower, int higher){
  if(n<lower){
    return lower;
  }
  else if(n>higher){
    return higher;
  }
  else{
    return n;
  }
}

float abs_f_em(float n){
  if(n>=0) return n;
  else return -n;
}


/*************************************************************************
*  函数名称：void EM_init(void)
*  功能说明：电磁初始化
*  参数说明：无
*  函数返回：无
*  修改时间：2019.5.13
*  备    注：adc_init(ADCN_enum adcn, ADCCH_enum ch, ADCRES_enum resolution)
              可选ADC模块、通道、分辨率可见zf_adc.h文件

*************************************************************************/
void EM_Init(void)
{
  adc_init(ADC_1,ADC1_CH3_B17,ADC_12BIT);
  adc_init(ADC_2,ADC2_CH3_B18,ADC_12BIT);
  adc_init(ADC_1,ADC1_CH4_B19,ADC_12BIT);
  adc_init(ADC_2,ADC2_CH4_B20,ADC_12BIT);
//  adc_init(ADC_1,ADC1_CH4_B19,ADC_12BIT);
//  adc_init(ADC_1,ADC1_CH5_B21,ADC_12BIT);
}
/*************************************************************************
*  函数名称：void EM_get(void)
*  功能说明：均值滤波获取电感反馈量
*  参数说明：无
*  函数返回：无
*  修改时间：2019.5.13
*  备    注：adc_mean_filter为AD转换均值滤波函数
             参数第三项为均值滤波次数
*************************************************************************/
int stop_cnt;
void EM_Get(void)
{
  EM_Value_1=(int)adc_mean_filter(ADC_1,ADC1_CH3_B17,5);
  EM_Value_2=(int)adc_mean_filter(ADC_2,ADC2_CH3_B18,5);
  EM_Value_3=(int)adc_mean_filter(ADC_1,ADC1_CH4_B19,5);
  EM_Value_4=(int)adc_mean_filter(ADC_2,ADC2_CH4_B20,5);
//  EM_Value_5=(int)adc_mean_filter(ADC_1,ADC1_CH4_B19,10);
//  EM_Value_6=(int)adc_mean_filter(ADC_1,ADC1_CH5_B21,10);
  EM_Value_1=(float)(EM_Value_1*3.3/4096);
  EM_Value_2=(float)(EM_Value_2*3.3/4096);
  EM_Value_3=(float)(EM_Value_3*3.3/4096);
  EM_Value_4=(float)(EM_Value_4*3.3/4096);
  if(EM_Value_1 < 0.1 && EM_Value_2 < 0.1 && EM_Value_3 < 0.1 && EM_Value_4 < 0.1 && Road != 5 && Road != 3)
  {
    stop_cnt++;
   
  }
  else
  {
    stop_cnt = 0;
  }
  if(stop_cnt > 5)
  {
    Road = 8;
  }
//  EM_Value_5=(float)(EM_Value_5*3.3/4096);
//  EM_Value_6=(float)(EM_Value_6*3.3/4096);
}

/*************************************************************************
*  函数名称：void EM_offset_fig(void)
*  功能说明：电磁中线偏差
*  参数说明：无
*  函数返回：无
*  修改时间：2019.5.13
*  备    注：

*************************************************************************/
void EM_offset_fig(void){
        float AD[2]={0};
        float EAD;
        AD[0]=EM_Value_2;
        AD[1]=EM_Value_3;
//            if(abs(AD[0]-AD[1])<0.01)
//          {
//            EM_offset=0.1;
//            return;
//          }
		
        EAD=AD[1]-AD[0];
		//remove EAD restriction
	/*if(EM_Road==4)
	{
        if(EAD>0.9)
          EAD=0.9;
        else if(EAD<-0.9)
          EAD=-0.9;
	}
	else if(EM_Road==0)
	{
        if(EAD>1.5)
          EAD=1.5;
        else if(EAD<-1.5)
          EAD=-1.5;
	}
*/	
          EM_offset=EAD*100;

          return ;
        
}

void EM_offset_filter(void)
{
  EM_offset_buff[3]=EM_offset_buff[2];
  EM_offset_buff[2]=EM_offset_buff[1];
  EM_offset_buff[1]=EM_offset_buff[0];
  EM_offset_buff[0]=EM_offset;
  EM_offset=EM_offset_buff[0]*0.5+EM_offset_buff[1]*0.2+EM_offset_buff[2]*0.2+EM_offset_buff[3]*0.1;
  //EM_offset *= 40;
  
}

void EM_Speed_Control(void)
{
  if(EM_Road == 3)
  {
    if(EM_Ramp_State == 3)//上坡坡道
    {
//      if(EM_Ramp_Distance_2>0.45)
//      {
//        if(Car_Angle > 18)
//        {
//          Car_Speed = 1.8;
//        }
//        else
//        {
//          Car_Speed = 2.1;
//        }
//      }
      //Car_Speed -= 0.2;
      //Car_Speed = 1.7;
    }
    else if(EM_Ramp_State == 4)//上坡结束
    {
      //Car_Speed = 0.8;
    }
    else if(EM_Ramp_State == 5)//平台
    {
      //Car_Speed = 1;//EM_Ramp_4_Speed ;
    }
    else if(EM_Ramp_State == 6)//下坡开始
    {
      //Car_Speed = 1;
    }
    else if(EM_Ramp_State == 7)//下坡坡道
    {
      //Car_Speed -= 0.2;
     //Car_Speed =1.5;
    }
    else if(EM_Ramp_State == 8)//下坡结束→角度震荡结束
    {
      //Car_Speed = 1.7;
    }
    return;
  }
  else if((EM_Road == 4  ||EM_Road == 1 ||EM_Road == 2) && Running == 1 ){
      //Car_Speed = Speed_Curve;
      return;
    }
}

void EM_Curve_Rec(void)//弯道识别
{
  if(EM_Road == 0 )//从直道到弯道的判断依据为：连续采集到三次1,4电感之差大于0.6
  {
    if(abs_f_em(EM_Value_1 - EM_Value_4)>0.3)
    {
      Curve_Cnt++;
      if(Curve_Cnt == 30)
      {
        EM_Road = 4;//EM_Road=4表示弯道
        Curve_Cnt = 0;
      }
    }
    else
    {
      Curve_Cnt = 0;
    }
  }
 
  
  else if(EM_Road == 4)//从弯道到直道的判断依据为：连续采集到30次采集到1,4电感之差小于0.6，则认为进入直道
  {
    if(abs_f_em(EM_Value_1 - EM_Value_4)<0.1)
    {
      Curve_Cnt++;
      if(Curve_Cnt == 3)
      {
        EM_Road = 0;
        Curve_Cnt = 0;
      }
    }
    else
    {
      Curve_Cnt = 0;
    }
  }
}

void EM_Ramp_Rec(void){
//  EM_Ramp_Time_Cnt++;
//  if(EM_Ramp_Time_Cnt>400 )
//  {
//    EM_Ramp_State = 0;
//    if(EM_Road == 3)EM_Road = 0;
//    EM_Ramp_Time_Cnt = 0;
//  }
//  if( EM_Value_2 + EM_Value_3 < 0.4 && EM_Ramp_State == 0 && (EM_Road==0 || EM_Road==4) )//&& (EM_Road==0 || EM_Road==4)
//  {
//    EM_Ramp_State = 1;
//    EM_Ramp_Time_Cnt = 0;
//  }
//  else if(EM_Ramp_State == 1)//角速度大于200，则进入坡状态机
//  {
//    if(Car_W>200 )
//    {
//      EM_Ramp_State = 2; 
//      EM_Road = 3;//坡
//      EM_Ramp_Time_Cnt = 0;
//    }    
//  }
//  else if(EM_Ramp_State == 2)//上坡起始状态1，当角速度绝对值小于10时，则认为在上坡坡道状态
//  {
//    if( abs_f(Car_W) < 10 && Car_Angle>10 )
//    {
//      EM_Ramp_State = 3;
//      EM_Ramp_Time_Cnt = 0;
//      EM_Ramp_Distance_2 = 0;
//    }
//  }
//  else if(EM_Ramp_State == 3)//上坡坡道状态2，当角速度小于-80，则认为上坡结束状态
//  {
//    EM_Ramp_Distance_2 += 0.00125*(Real_Speed_L+Real_Speed_R);
//    if( Car_W < -100 )
//    {
//      EM_Ramp_State = 4;
//      EM_Ramp_Time_Cnt = 0;
//    }
//  }
//  else if(EM_Ramp_State == 4)//上坡结束状态3，当角速度绝对值小于10，则认为平台状态
//  {
//    if( abs_f(Car_W) < 10 )
//    {
//      EM_Ramp_State = 5;
//      EM_Ramp_Time_Cnt = 0;
//      EM_Ramp_4_Speed = (Real_Speed_L+Real_Speed_R)*0.5 + 0.2;
//    }
//  }
//  else if(EM_Ramp_State == 5)//平台状态4，当角速度小于-150，则认为下坡开始状态
//  {
//    if(Car_W< -150 )
//    {
//      EM_Ramp_State = 6;
//      EM_Ramp_Time_Cnt = 0;
//    }
//  }
//  else if(EM_Ramp_State == 6 )//下坡开始状态5，当角速度大于150时，则认为下坡坡道状态
//  {
//    if(abs_f(Car_W)<10)
//    {
//      EM_Ramp_State = 7;
//    }
//  } 
//   else if(EM_Ramp_State == 7 )//下坡坡道状态6，当角速度大于150时，则认为下坡结束状态
//  {
//    if(Car_W>150)
//    {
//      EM_Ramp_State = 8;
//      EM_Ramp_State7_Cnt = 0;
//    }
//  } 
//  else if(EM_Ramp_State == 8)
//  {
//    EM_Ramp_State7_Cnt++;
//    EM_Ramp_Time_Cnt = 0;
//    if(abs_f(Car_W)>100)
//    {
//      EM_Ramp_State7_Cnt = 0;
//    }
//    if(EM_Ramp_State7_Cnt > 100)
//    {
//      EM_Ramp_State = 0;
//      EM_Road = 0;
//    }
//  }
}

void EM_Ring_Rec(void){
  EM_Peak_Time_Cnt++;
  EM_Peak_Time_Cnt = limit_em(EM_Peak_Time_Cnt,0,1000);

  EM_Value_23_Sum = EM_Value_2 + EM_Value_3;
  if(EM_Value_23_Sum>5.2 && EM_Ring_State == 0 && (EM_Road==0 || EM_Road==4) )//2+3之和大于3.8，开始进行圆环状态机
  {
     EM_Ring_State = 1;
     EM_Ring_Max = EM_Value_23_Sum;
     EM_Peak_Time_Cnt = 0;
     Lef_EM_Sum = 0;
     Rig_EM_Sum = 0;
     if(EM_Value_4>EM_Value_1  )
    {
      EM_Road2_Cnt += 1000 ; 
    }
    else if(EM_Value_1>EM_Value_4  )
    {
      EM_Road1_Cnt += 1000 ;
    }
  }
  else if(EM_Ring_State == 1)//状态1下，若2+3大于峰值-0.2，则保持状态1，否则跳到状态2
  {

    if(EM_Value_4>EM_Value_1 && EM_Value_4>1.2 && abs_f_em(EM_offset)<100)
    {
      EM_Road2_Cnt += 200 - abs_f_em(EM_offset); 
    }
    else if(EM_Value_1>EM_Value_4 && EM_Value_1>1.2 && abs_f_em(EM_offset)<100)
    {
      EM_Road1_Cnt += 200 - abs_f_em(EM_offset);
    }
    if( EM_Value_23_Sum>EM_Ring_Max-0.4 )
    {
      if(EM_Value_23_Sum>EM_Ring_Max)
      {
        EM_Ring_Max = EM_Value_23_Sum;
        EM_2_Max = EM_Value_2;
        EM_3_Max = EM_Value_3;
      }
    }
    else
    {

      EM_Ring_State = 2;
      EM_Ring_Max = 0;
      EM_Ring_Min = EM_Value_23_Sum;
    }
  }
  else if(EM_Ring_State == 2) //状态2下，若2+3小于最小值+0.2，则保持状态2，否则跳到状态3
  {

    if(EM_Value_4>EM_Value_1 && EM_Value_4>1.2 && abs_f_em(EM_offset)<100)
    {
      EM_Road2_Cnt += 200 - abs_f_em(EM_offset); 
    }
    else if(EM_Value_1>EM_Value_4 && EM_Value_1>1.2 && abs_f_em(EM_offset)<100)
    {
      EM_Road1_Cnt += 200 - abs_f_em(EM_offset);
    }
    if( EM_Value_23_Sum<EM_Ring_Min+0.1)
    {
      if(EM_Value_23_Sum<EM_Ring_Min)
      {
        EM_Ring_Min = EM_Value_23_Sum;
      }
    }
    else
    {
      if(EM_Road1_Cnt > EM_Road2_Cnt)
      {
        EM_Road = 1;
        EM_Road1_Cnt = 0;
        EM_Road2_Cnt = 0;
      }
      else 
      {
        EM_Road = 2;
        EM_Road1_Cnt = 0;
        EM_Road2_Cnt = 0;
      }

      EM_Ring_State = 3;
      EM_Peak_Time_Cnt = 0;
      EM_Ring_Min = 6;
    }
  }
  else if(EM_Ring_State == 3)//状态3下，若2+3小于固定值3，则表示已经进入了圆环
  {

    if((EM_Value_23_Sum < 3.5 && EM_Road == 1 && EM_Value_1>1 && EM_Value_4<0.2)||(EM_Value_23_Sum < 3.5 && EM_Road == 2 && EM_Value_4>1 && EM_Value_1<0.2))
    {
      EM_Ring_State = 4;
    }
  }
  else if(EM_Ring_State == 4)//状态4下，若2+3大于固定值3.5，则表示又一次进入了圆环岔道处，且处于刚出环状态5
  {
    if(EM_Value_23_Sum >5)
    {
      EM_Ring_Max = EM_Value_23_Sum;
      if(EM_Road == 1 && EM_Value_1>EM_Value_4 && EM_Value_1>0.5)
      {
        EM_Ring_State = 5;
      }
      else if(EM_Road == 2 && EM_Value_1<EM_Value_4 && EM_Value_4>0.5)
      {
        EM_Ring_State = 5;
      }
    }
  }
  else if(EM_Ring_State == 5)//状态5下，若2+3小于第一个峰值减0.2，则认为已经下坡，进入状态6
  {
    if( EM_Value_23_Sum>EM_Ring_Max-0.2 )
    {
      if(EM_Value_23_Sum>EM_Ring_Max)
      {
        EM_Ring_Max = EM_Value_23_Sum;
      }
    }
    else
    {
      EM_Ring_State = 6;
      EM_Ring_Max = 0;
      EM_Ring_Min = EM_Value_23_Sum;
    }
  }
  else if(EM_Ring_State == 6)//状态6下，若2+3大于波谷+0.3，则认为爬第二个坡，进入状态7
  {
    if( EM_Value_23_Sum<EM_Ring_Min+0.3)
    {
      if(EM_Value_23_Sum<EM_Ring_Min)
      {
        EM_Ring_Min = EM_Value_23_Sum;
      }
    }
    else
    {
      EM_Ring_State = 7;
      EM_Ring_Min = 6;
    }
  }
  else if(EM_Ring_State == 7)
  {
    if( EM_Value_23_Sum < 3.2 && EM_Value_1 < 1 && EM_Value_4 < 1)
    {
      EM_Ring_State = 0;
      EM_Road = 0;
    }
  }

}


/*************************************************************************
*  函数名称：void EM_main(void)
*  功能说明：电磁主函数
*  参数说明：无
*  函数返回：无
*  修改时间：2019.5.13
*  备    注：
*************************************************************************/
void EM_main(void)
{
  EM_Get();
  //EM_Ring_Rec();
  //EM_Ramp_Rec();
  //EM_Curve_Rec();
  //EM_Speed_Control();
  //EM_offset_fig();
  //Block_By_Pass();
  //EM_offset_filter();
}




