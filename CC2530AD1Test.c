#include "ioCC2530.h"
#include "initUART_Timer.h"
#include "stdio.h"
#include "string.h"
#include "LCD.h"


INT16 AvgTemp; 

/****************************************************************
温度传感器初始化函数						
****************************************************************/
void initTempSensor(void){
   DISABLE_ALL_INTERRUPTS();            //关闭所有中断
   InitClock();                         //设置系统主时钟为32M
   *((BYTE __xdata*) 0x624B) = 0x01;    //开启温度传感器,TR0的地址为0x624B
   *((BYTE __xdata*) 0x61BD) = 0x01;    //将温度传感器与ADC连接起来,ATEST的地址为0x61BD
}
/****************************************************************
读取温度传感器AD值函数						
****************************************************************/
INT8 getTemperature(void){
  UINT8   i;
  UINT16  AdcValue;
  UINT16  value;

  AdcValue = 0;
  for( i = 0; i < 4; i++ )
  {
    ADC_SINGLE_CONVERSION(ADC_REF_1_25_V | ADC_14_BIT | ADC_TEMP_SENS);    // 使用1.25V内部电压，12位分辨率，AD源为：温度传感器
    ADC_SAMPLE_SINGLE();                                                   //开启单通道ADC
    while(!ADC_SAMPLE_READY());                 //等待AD转换完成
    value =  ADCL >> 2;                         //ADCL寄存器低2位无效
    value |= (((UINT16)ADCH) << 6);    
    AdcValue += value;                          //AdcValue被赋值为4次AD值之和
  }
  value = AdcValue >> 2;                        //累加除以4，得到平均值
  return ADC14_TO_CELSIUS(value);               //根据AD值，计算出实际的温度
}
/****************************************************************
主函数	
****************************************************************/
void main(void)
{	
	char i;
        char TempValue[30]={0}; 
        
	InitUART0();                                    //初始化串口
	initTempSensor();                               //初始化ADC

        LCD_Init();//初始化LCD        
        LCD_CLS();
        LCD_welcome();
        
        while(1)
        {
          AvgTemp = 0;
          for(i = 0 ; i < 64 ; i++)
          {
            AvgTemp += getTemperature();              
            AvgTemp >>= 1;                              //每次累加后除2.
          }
             memset(TempValue, 0, 30);
             sprintf(TempValue, "ADTestRead = %dC", (INT8)AvgTemp);
             UartTX_Send_String(TempValue,strlen(TempValue));
             UartTX_Send_String("\n",1);
             LCD_P8x16Str(0, 4, (unsigned char*)TempValue);
             Delay(50000); 
        }
}

