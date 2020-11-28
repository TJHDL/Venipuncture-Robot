#ifndef __TIMER_H
#define __TIMER_H
#include "sys.h"

//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK战舰STM32开发板
//定时器 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/4
//版本：V1.1
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
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
