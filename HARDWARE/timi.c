#include "delay.h"
#include "usart.h"
#include "timi.h"
/////////////////////////////////////////////////////////////////////////////////
/*
debug��־�
2020/9/26
��ͬ�ڲ�ͬ��ʱ�����߼���ʱ��TIM1/TIM8�������ж�ʱ�������ж�ģʽ
TIM_UP��TIM_CC
��Ҫע��������NVIC_InitStructure.NVIC_IRQChannel���жϺ���void TIM1_UP_IRQHandler(void)ʱ
��Ҫ���ǣ����򲻻ᱨ�����л������
*/
//////////////////////////////////////////////////////////////////////////////////	 
//                          �ڲ�ʱ���жϷ����ӳ���						  
//////////////////////////////////////////////////////////////////////////////////

//�ⲿ�жϷ������
void TIMI_Init(void)
{
 	NVIC_InitTypeDef NVIC_InitStructure;

    //��ʼ��TIM1�жϣ�ע�����ñȽ����⣩
    TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE ); //
    //�����ж�
    NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;
    //�Ƚ��ж�
    //NVIC_InitStructure.NVIC_IRQChannel = TIM1_CC_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    //��ʼ��TIM2�ж�
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE ); //
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    //��ʼ��TIM3�ж�
    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE ); //
    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    //��ʼ��TIM4�ж�
    TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE ); //
    NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

}

void TIM1_UP_IRQHandler(void)   //TIM1
{
    if (TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET)
    {
        TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
        //�ɵ��٣�ʵ�ʿɵ�������������
        //��������ٶ�v����������Ƶ��f������������T
        //��Ҫ���ٶȣ����������һ�κ��������ٶ��Ƕ��κ���
        //������TӦ�ó�1/t^2�仯
        //�����������һ���ٶ����ޣ��ʼ����������𽥱仯��һ����̵����ڣ��ٶ����ޣ�
        TIM1->ARR++;
        
//      �ɵ�ռ�ձ�
//      u32 capture;
//      capture = TIM_GetCapture4(TIM1); 
//      capture = capture + 10;
//      TIM_SetCompare4(TIM1, capture); 
    }
}

void TIM2_IRQHandler(void)   //TIM2
{
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
    {
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    }
}

void TIM3_IRQHandler(void)   //TIM3
{
    if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
    {
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
    }
}

void TIM4_IRQHandler(void)   //TIM4
{
    if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
    {
        TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
    }
}
