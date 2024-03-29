#include "Data_Send.h"
#include "search.h"

uint16 Pic_new[600];
int cnt;
uint8 ch=0;
float temp1=0;
float Variable[20];


void Send_Data(void){
    Pic_send_new();
    Variable_update();
    Send_Begin();
    Send_Variable();
    Send_Img();
}





/*************************************************************************
*  函数名称：void Send_Begin(void)
*  功能说明：
*  参数说明：无
*  函数返回：无
*  修改时间：2019.3.``````````````````````````````````````````
*  备    注：

*************************************************************************/
void Send_Begin(void)
{
  My_Put_Char(0x55);
  My_Put_Char(0xaa);
  My_Put_Char(0x11);
}

/*************************************************************************
*  函数名称：void Send_Img(void)
*  功能说明：
*  参数说明：无
*  函数返回：无
*  修改时间：2019.3.6
*  备    注：

*************************************************************************/


void Send_Img(void)
{

    uint16 i=0,num;
    My_Put_Char(0x55);
    My_Put_Char(0xaa);
    My_Put_Char(0xff);
    My_Put_Char(0xa2);
    My_Put_Char(0x01); //小车状态
    
    num = cnt + 2 + 180;//4+4*2
    //num = cnt+2+180;
    //num=cont+2+180+36;  
    //统计将要传输的数据量 2是因为要传输关键字即0xf0和0xf2   (0xf2 所有数据结束，0xf0图像数据结束，180=60*3)
    //180是边线的位 36是变量的位 如果不传输就不要加上！
    
    My_Put_Char(BYTE0(num)); 
    My_Put_Char(BYTE1(num));
    for(i=0;i<cnt;i++)
    {
      My_Put_Char(Pic_new[i]);
    }
//    for(i=0;i< 60;i++)
//    {
//            My_Put_Char(Lef[i]);
//            My_Put_Char(Rig[i]);
//            My_Put_Char(0xff);
//    }
    My_Put_Char(0xf0);  //代表图像数据发完了
    /******************星号围起来的可以不传输*******************/

    ////////////////////////传输左右边线和计算得到的中线///////////
    for(i=0;i<60;i++){
      My_Put_Char(Lef[i]);//New_Lef[i]/10+40); 
    }
    for(i=0;i<60;i++){
      My_Put_Char(Mid[i]); 
    }
    for(i=0;i<60;i++){
      My_Put_Char(Rig[i]);//New_Rig[i]/10+40); 
    }
    

    /*****************************************************/
    My_Put_Char(0xf2); //代表整个数据都发完了

}
 
void Pic_send_new(void)
{
  int i;
  int j;
  cnt=0;
  for(i=0;i<60;i++)
  {
    if(Pixle[i][0]==1)
    {      
      Pic_new[cnt]=0;
      cnt++;
    }
    for(j=1;j<80;j++)
    {
      if(Pixle[i][j]!=Pixle[i][j-1])
      {
        Pic_new[cnt]=j;
        cnt++;
      }
    }
    Pic_new[cnt]=0xff;
    cnt++;
  }
}
/*************************************************************************
*  函数名称：void My_Put_Char(void)
*  功能说明：
*  参数说明：无
*  函数返回：无
*  修改时间：2019.3.6
*  备    注：

*************************************************************************/
                
void My_Put_Char(char tmp)
{
    uart_putchar(USART_0,tmp);//根据实际的串口号来修改
}


void Send_Variable(void){
  
  My_Put_Char(0x55);
  My_Put_Char(0xaa);
  My_Put_Char(0xff);
  My_Put_Char(0x01);
  My_Put_Char(4);
  for(int i=0;i<4;i++){
            temp1=Variable[i];
            ch=BYTE0(temp1);
            My_Put_Char(ch);
            ch=BYTE1(temp1);
            My_Put_Char(ch);
            ch=BYTE2(temp1);
            My_Put_Char(ch);
            ch=BYTE3(temp1);
            My_Put_Char(ch);
  }
  My_Put_Char(0x01);
  
}



/*************************************************************************
*  函数名称：void Variable_update(void)
*  功能说明：
*  参数说明：无
*  函数返回：无
*  修改时间：2019.3.10
*  备    注：

*************************************************************************/

void Variable_update(void)
{
  Variable[0]= cnt;//Cam_offset;//ToF_distance;
  Variable[1]= cnt;//Cam_Turn_Control;//;//stat_slope;//gyroy_1;//Cam_Turn_Control;//Car_W;
  Variable[2]= cnt;//speedTarget1; //Allwhiteend;
  Variable[3]= threshold;//CarSpeed1;//whitecnt;
  /*
  Variable[4]= EM_Turn_Control;//speedTarget2;//Cam_Turn_Control;
  Variable[5]= Lef_circle;//Cam_offset;
  Variable[6]= Rig_circle;
  Variable[7]= Road1_flag;
  Variable[8]= Road2_flag;
  Variable[9]= Lef_slope;
  Variable[10]=Rig_slope;
*/
  //Variable[11]=Road;/*Rig_circle;*///Road6_flag ;//CarSpeed2;
/*
  Variable[12]=whitecnt;//Road1_turnout;//limit_pos(EM_Value_1/1.5-EM_Value_2/3.5);//speedTarget1;//map_line[MIN(50,AllWhileStartLine)];
  Variable[13]=EM_Value_2;//speedTarget2;//lib_active_diff_get();//map_line[MIN(50,AllWhileEndLine)];_
  Variable[14]=EM_Value_3;//Cam_Block_State;//MotorOut1;//CarSpeed1;//(MotorOut1+MotorOut2)/2;
  Variable[15]=ToF_distance;//EM_Value_4;//ToF_distance;//MotorOut2;//CarSpeed2;
  */

}