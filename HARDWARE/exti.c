#include "exti.h"
#include "delay.h"
#include "usart.h"
#include "key.h"
#include "driver.h"

//////////////////////////////////////////////////////////////////////////////////	 
//              ����KEY_UP�����жϺ���жϷ����ӳ���						  
//////////////////////////////////////////////////////////////////////////////////

int motor_start = 0;
int motor_stop = 0;

//�ⲿ�жϷ������
void EXTIX_Init(void)
{
   	EXTI_InitTypeDef EXTI_InitStructure;
 	NVIC_InitTypeDef NVIC_InitStructure;
    //ʹ�ܸ��ù���ʱ��
  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
    
    //��ʼ������KEY0
    //GPIOE.4 = KEY0
    //�ⲿͨ��EXIT4
	//�½��ش���
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOE,GPIO_PinSource4);
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  	EXTI_InitStructure.EXTI_Line=EXTI_Line4;
  	EXTI_Init(&EXTI_InitStructure);
    //ʹ�ܰ���KEY_UP���ڵ��ⲿ�ж�ͨ��
    NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;
    //��ռ���ȼ�2�������ȼ�1
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
    //ʹ���ⲿ�ж�ͨ��
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure); 
    
    //��ʼ������KEY1
    //GPIOE.3 = KEY1
    //�ⲿͨ��EXIT3
	//�½��ش���
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOE,GPIO_PinSource3);
  	EXTI_InitStructure.EXTI_Line=EXTI_Line3;
  	EXTI_Init(&EXTI_InitStructure);
    //ʹ�ܰ���KEY_1���ڵ��ⲿ�ж�ͨ��
    NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;
    //��ռ���ȼ�2�������ȼ�2
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
    //ʹ���ⲿ�ж�ͨ��
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure); 

    //��ʼ������KEY_UP
    //GPIOA.0 = KEY_UP
    //�ⲿͨ��EXIT0
	//�����ش���
 	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource0); 
  	EXTI_InitStructure.EXTI_Line=EXTI_Line0;
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  	EXTI_Init(&EXTI_InitStructure);
    //ʹ�ܰ���KEY_UP���ڵ��ⲿ�ж�ͨ��
    NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
    //��ռ���ȼ�2�������ȼ�0
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;
    //ʹ���ⲿ�ж�ͨ��
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure); 
}

//�ⲿ�ж�0������� KEY_UP
//������ͣ
void EXTI0_IRQHandler(void)
{
	delay_ms(10);//����
	if(WK_UP==1)	 	 //WK_UP����
	{				 
        
	}
	EXTI_ClearITPendingBit(EXTI_Line0); //���LINE0�ϵ��жϱ�־λ  
}
 
//�ⲿ�ж�3�������
void EXTI3_IRQHandler(void)
{
	delay_ms(10);//����
	if(KEY1==0)	 //����KEY1
	{				 
		motor_stop = 1;
	}		 
	EXTI_ClearITPendingBit(EXTI_Line3);  //���LINE3�ϵ��жϱ�־λ  
}

void EXTI4_IRQHandler(void)
{
	delay_ms(10);//����
	if(KEY0==0)	 //����KEY0
	{
		motor_start = 1;
	}		 
	EXTI_ClearITPendingBit(EXTI_Line4);  //���LINE4�ϵ��жϱ�־λ  
}
