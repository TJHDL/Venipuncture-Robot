// #include "driver.h"

//////////////////////////////////////////////////////////////////////////////////	 
/*
    ͨ�ö�ʱ��TIMx����4�������2345
    �߼���ʱ��TIM1��TIM8
    �������TIM1234����4��OCͨ����һ��16��OCͨ��
    ��������4��Ƶ�ʶ����ģ�16��ռ�ձȶ�����PWM��
    
    ÿһ����ʱ����PWMƵ�ʿɵ������ڣ�
    ��Ӧ��4��OCͨ��ռ�ձȿ��Ե��ڣ���4��OCͨ��Ƶ����һ�µ�
    
    16��PWM�˿ڷֱ���
    TIM1��PE9,PE11,PE13,PE14
    TIM2��PA15,PB3,PB10,PB11	
    TIM3��PC6~9
    TIM4��PD12~15

    ���ڲ��������PWMƵ����Ҫ����ռ�ձȲ���Ҫ
    ���ȡ4����ʱ����CH4��Ϊ������Ƶ�ʵ�PWM����ˣ�
    PE14,PB11,PC9,PD15
    
    ͨ���޸�arr��psc���Ըı�ʱ�ӻ���Ƶ��
    ͨ��TIM_SetComparex(TIMy,ccr)����TIMy��Channelx��PWMռ�ձ�
    
    PF0~7 ���ڿ���8��PWM�ķ���
    PF12  ���ڿ��Ƽ�ͣ��stop=1��Ч�
*/						  
////////////////////////////////////////////////////////////////////////////////// 	   

// ʹ������ȫ���˿�
//void driver_Init(void)
//{
//    GPIO_InitTypeDef  GPIO_InitStructure;

//    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF, ENABLE);
//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_12;
//    //PFPF0~7��PF12 �˿�����
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
//    //���������IO���ٶ�Ϊ50MHz
//    GPIO_Init(GPIOF, &GPIO_InitStructure);
//    //�����趨������ʼ��PF0~7��PF12

//    //TIM1
//    //PE9,11,13,14    �˿�����
//    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_11|GPIO_Pin_13|GPIO_Pin_14;
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 		 //�����������
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
//    GPIO_Init(GPIOE, &GPIO_InitStructure);
//    
//    //TIM2
//    //PA15	    �˿�����
//    //PB3,10,11	    �˿�����
//    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB, ENABLE);
//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
//    GPIO_Init(GPIOA, &GPIO_InitStructure);
//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_10|GPIO_Pin_11;
//    GPIO_Init(GPIOB, &GPIO_InitStructure);

//    //TIM3
//    //PC6~9	    �˿�����
//    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9;
//    GPIO_Init(GPIOC, &GPIO_InitStructure);

//    //TIM4
//    //PD12~15	�˿�����  
//    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
//    GPIO_Init(GPIOD, &GPIO_InitStructure);
//      
//}

#include "driver.h"//////////////////////////////////////////////////////////////////////////////////  
/*
    ͨ�ö�ʱ��TIMx����4�������2345
    �߼���ʱ��TIM1��TIM8
    �������TIM1234����4��OCͨ����һ��16��OCͨ��
    ��������4��Ƶ�ʶ����ģ�16��ռ�ձȶ�����PWM��
    
    ÿһ����ʱ����PWMƵ�ʿɵ������ڣ�
    ��Ӧ��4��OCͨ��ռ�ձȿ��Ե��ڣ���4��OCͨ��Ƶ����һ�µ�
    
    16��PWM�˿ڷֱ���
    TIM1��PE9,PE11,PE13,PE14
    TIM2��PA15,PB3,PB10,PB11 
    TIM3��PC6~9
    TIM4��PD12~15    ���ڲ��������PWMƵ����Ҫ����ռ�ձȲ���Ҫ
    ���ȡ4����ʱ����CH4��Ϊ������Ƶ�ʵ�PWM����ˣ�
    PE14,PB11,PC9,PD15
    
    ͨ���޸�arr��psc���Ըı�ʱ�ӻ���Ƶ��
    ͨ��TIM_SetComparex(TIMy,ccr)����TIMy��Channelx��PWMռ�ձ�
    
    PF0~7 ���ڿ���8��PWM�ķ���
    PF12  ���ڿ��Ƽ�ͣ��stop=1��Ч?
*/

//////////////////////////////////////////////////////////////////////////////////     // ʹ������ȫ���˿�
void driver_Init(void)
{
	  GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable,ENABLE);
	
    GPIO_InitTypeDef  GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7|GPIO_Pin_1|GPIO_Pin_4|GPIO_Pin_3;   //fuck!!!
    //PFPF0~7��PF12 �˿�����
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    //�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;   //IO���ٶ�Ϊ50MHz
    //���������IO���ٶ�Ϊ50MHz
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    //�����趨������ʼ��PF0~7��PF12
   
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
    //PFPF0~7��PF12 �˿�����
    //GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    //�������
    //GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;   //IO���ٶ�Ϊ50MHz
    //���������IO���ٶ�Ϊ50MHz
    GPIO_Init(GPIOA, &GPIO_InitStructure);
 
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_13|GPIO_Pin_0|GPIO_Pin_11|GPIO_Pin_9;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
 
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10|GPIO_Pin_3|GPIO_Pin_1|GPIO_Pin_5;
    GPIO_Init(GPIOE, &GPIO_InitStructure);
  
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_3|GPIO_Pin_7|GPIO_Pin_1|GPIO_Pin_12|GPIO_Pin_0|GPIO_Pin_2;
    GPIO_Init(GPIOF, &GPIO_InitStructure);
  
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14|GPIO_Pin_9|GPIO_Pin_15;
    GPIO_Init(GPIOG, &GPIO_InitStructure);
 
    //TIM1
    //PE9,11,13,14    �˿�����
    //RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    //|GPIO_Pin_11|GPIO_Pin_13|GPIO_Pin_14;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;   //�����������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;   //IO���ٶ�Ϊ50MHz
    GPIO_Init(GPIOE, &GPIO_InitStructure);
    
    //TIM2
    //PA15     �˿�����
    //PB3,10,11     �˿�����
    //RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    //GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_10|GPIO_Pin_11;
    //GPIO_Init(GPIOB, &GPIO_InitStructure);    //TIM3
    //PC6~9     �˿�����
    //RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    //GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    //|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9;
    //GPIO_Init(GPIOC, &GPIO_InitStructure);  //TIM4
    //PD12~15 �˿�����  
    //RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_14;
    //|GPIO_Pin_13|GPIO_Pin_15;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
}
