#ifndef __driver_H
#define __driver_H	 
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
/*
    通用定时器TIMx共有4个，编号2345
    高级定时器TIM1和TIM8
    这里采用TIM1234各自4个OC通道，一共16个OC通道
    可以生成4个频率独立的，16个占空比独立的PWM波
    
    每一个定时器的PWM频率可单独调节，
    对应的4个OC通道占空比可以调节，但4个OC通道频率是一致的
    
    16个PWM端口分别是
    TIM1的PE9,PE11,PE13,PE14
    TIM2的PA15,PB3,PB10,PB11	
    TIM3的PC6~9
    TIM4的PD12~15
    
    对于步进电机，PWM频率重要，而占空比不重要
    因此取4个定时器的CH4作为代表其频率的PWM输出端：
    PE14,PB11,PC9,PD15
    
    通过修改arr和psc可以改变时钟基本频率
    通过TIM_SetComparex(TIMy,ccr)调节TIMy的Channelx的PWM占空比
    
    PF0~7 用于控制8个PWM的方向
    PF12  用于控制急停（stop=1有效）
*/						  							  
////////////////////////////////////////////////////////////////////////////////// 

// dir 用于控制8个PWM的方向
#define dir1 PCout(7)//pc7 zdir+,pc6 zdir-
#define dir2 PEout(10)//pa14 xdir+, pa15 xpul+
#define dir3 PFout(2)//pd7 zen+,pg9 zen-
#define dir4 PCout(1)//pg14 zpul-,pg15 zdir-
#define dir5 PFout(5)//pb11 4dir-,
#define mdir1 PGout(15)//pe9 ypul+,pe10 ydir+
#define mdir2 PBout(0)//pf12 yp- pb2 ye+ pb0 yd- pc4 ye-
#define mdir3 PEout(1)//pf1 5e-, pf3 5e+,pf5 5d+,pf7 5d-
#define mdir4 PBout(11)//pc0 4d+,pc2 4p-k
#define mdir5 PFout(7)//pb9 xp-,pe1 xd-,pe3 xe+,pe5 xe-

//pa14 xdir+,pe1 xdir-,pa15 xpul+,pb9 xp-,pe3 xe+,pe5 xe-
//pe10 ydir+,pb0 ydir-,pe9 ypul+,pf12 yp-,pb2 ye+,pc4 ye-
//pc7 zdir+,pc6 zdir-,    pg14 zpul-,pd7 zen+,pg9 zen-
//pb11 4dir-,pc0 4d+,pc2 4p-
//pf1 5e-, pf3 5e+,pf5 5d+,pf7 5d-

#define ENA1 PDout(7)
#define ENA2 PBout(2)
#define ENA3 PEout(3)
#define ENA4 PBout(13)
#define ENA5 PFout(3)
#define mENA1 PGout(9)
#define mENA2 PCout(4)
#define mENA3 PEout(5)
//#define mENA4 GND
#define mENA5 PFout(1)

//  生成4个频率可调PWM，用于控制步进电机
//    #define pulse1 PCout(6)
//    #define pulse2 PEout(9)
//    #define pulse3 PAout(15)
//    #define pulse4 PDout(12)
//    #define pulse5 PDout(14)
#define mpulse1 PGout(14)
#define mpulse2 PFout(12)
#define mpulse3 PBout(9)
#define mpulse4 PCout(3)
//#define mpulse5 GND

void driver_Init(void);//初始化
				    
#endif
