#ifndef __driver_H
#define __driver_H	 
#include "sys.h"
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
    PF12  ���ڿ��Ƽ�ͣ��stop=1��Ч��
*/						  							  
////////////////////////////////////////////////////////////////////////////////// 

// dir ���ڿ���8��PWM�ķ���
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

//  ����4��Ƶ�ʿɵ�PWM�����ڿ��Ʋ������
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

void driver_Init(void);//��ʼ��
				    
#endif
