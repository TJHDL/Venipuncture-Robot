#include "exti.h"
#include "delay.h"
#include "usart.h"
#include "key.h"
#include "driver.h"

//////////////////////////////////////////////////////////////////////////////////	 
//              按键KEY_UP触发中断后的中断服务子程序						  
//////////////////////////////////////////////////////////////////////////////////

int motor_start = 0;
int motor_stop = 0;

//外部中断服务程序
void EXTIX_Init(void)
{
   	EXTI_InitTypeDef EXTI_InitStructure;
 	NVIC_InitTypeDef NVIC_InitStructure;
    //使能复用功能时钟
  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
    
    //初始化配置KEY0
    //GPIOE.4 = KEY0
    //外部通道EXIT4
	//下降沿触发
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOE,GPIO_PinSource4);
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  	EXTI_InitStructure.EXTI_Line=EXTI_Line4;
  	EXTI_Init(&EXTI_InitStructure);
    //使能按键KEY_UP所在的外部中断通道
    NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;
    //抢占优先级2，子优先级1
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
    //使能外部中断通道
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure); 
    
    //初始化配置KEY1
    //GPIOE.3 = KEY1
    //外部通道EXIT3
	//下降沿触发
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOE,GPIO_PinSource3);
  	EXTI_InitStructure.EXTI_Line=EXTI_Line3;
  	EXTI_Init(&EXTI_InitStructure);
    //使能按键KEY_1所在的外部中断通道
    NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;
    //抢占优先级2，子优先级2
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
    //使能外部中断通道
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure); 

    //初始化配置KEY_UP
    //GPIOA.0 = KEY_UP
    //外部通道EXIT0
	//上升沿触发
 	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource0); 
  	EXTI_InitStructure.EXTI_Line=EXTI_Line0;
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  	EXTI_Init(&EXTI_InitStructure);
    //使能按键KEY_UP所在的外部中断通道
    NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
    //抢占优先级2，子优先级0
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;
    //使能外部中断通道
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure); 
}

//外部中断0服务程序 KEY_UP
//用于暂停
void EXTI0_IRQHandler(void)
{
	delay_ms(10);//消抖
	if(WK_UP==1)	 	 //WK_UP按键
	{				 
        
	}
	EXTI_ClearITPendingBit(EXTI_Line0); //清除LINE0上的中断标志位  
}
 
//外部中断3服务程序
void EXTI3_IRQHandler(void)
{
	delay_ms(10);//消抖
	if(KEY1==0)	 //按键KEY1
	{				 
		motor_stop = 1;
	}		 
	EXTI_ClearITPendingBit(EXTI_Line3);  //清除LINE3上的中断标志位  
}

void EXTI4_IRQHandler(void)
{
	delay_ms(10);//消抖
	if(KEY0==0)	 //按键KEY0
	{
		motor_start = 1;
	}		 
	EXTI_ClearITPendingBit(EXTI_Line4);  //清除LINE4上的中断标志位  
}
