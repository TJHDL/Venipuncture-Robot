#ifndef __TIMER_H
#define __TIMER_H
#include "sys.h"

//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEKս��STM32������
//��ʱ�� ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/4
//�汾��V1.1
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//********************************************************************************

//void TIM1_Int_Init(u16 arr,u16 psc);
void TIM1_PWM_Init(u16 arr,u16 psc,u32 ht1,u32 ht2,u32 ht3,u32 ht4);
//void TIM2_Int_Init(u16 arr,u16 psc);
void TIM2_PWM_Init(u16 arr,u16 psc,u32 ht1,u32 ht2,u32 ht3,u32 ht4);
//void TIM3_Int_Init(u16 arr,u16 psc);
void TIM3_PWM_Init(u16 arr,u16 psc,u32 ht1,u32 ht2,u32 ht3,u32 ht4);
//void TIM4_Int_Init(u16 arr,u16 psc);
void TIM4_PWM_Init(u16 arr,u16 psc,u32 ht1,u32 ht2,u32 ht3,u32 ht4);
void TIM3_CH1_close(void);
void TIM3_CH1_open(void);
void TIM3_CH2_close(void);
void TIM3_CH2_open(void);
void TIM4_CH1_close(void);
void TIM4_CH1_open(void);
void TIM4_CH3_close(void);
void TIM4_CH3_open(void);
#endif
