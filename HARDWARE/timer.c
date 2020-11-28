#include "timer.h"

/*
debug��־��
����˼·
(1)       TIM_DeInit()

(2)       TIM_InternalClockConfig()

(3)       TIM_Perscaler

(4)       TIM_ClockDivision

(5)       TIM_CounterMode

(6)       TIM_Period

(7)       TIM_ARRPerloadConfig()

(8)       TIM_ITConfig()

����(3)-(6)TIM_TimerBaseInitTypeDef
*/

/*
    ʱ�ӻ������ڼ��㷽ʽ��
    Tclk = 72MHz
    pscһ��ȡ1
	Tout = ((arr+1)*(psc+1))/Tclk
*/

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
*/

//������ӳ��״̬
#define NoRemap   0
#define Remap   1
#define PartialRemap 2
#define PartialRemap2 3
#define FullRemap 4

///////////////////////////////////////////////////////////////
//                        �߼���ʱ��TIM1
//       ע��TIM1��GPIO��EXTI���ӵ���Χ������AHB2�����죩
//       TIM2~7���ӵ���Χ������AHB1��������
//       �����趨��ȱ�ٻ������
//       TIM_CtrlPWMOutputs(TIM1, ENABLE);    
///////////////////////////////////////////////////////////////
/*
	�߼���ʱ��TIM1��ͨ�ù��ܹ���4��OutputChannel��
	���Ӧ���ĸ����ŷֱ�Ϊ������ֻ��Ϊ�����
	����ӳ��:
	PA8,PA9,PA10,PA11
	��ӳ��:
	PE9,PE11,PE13,PE14
*/

//ѡ��ǰ״̬-->��ӳ��
#define CURRENT_MODE FullRemap

//TIM1��GPIO����ӳ���ʼ��
void TIM1_GPIO_Init(void)
{
	GPIO_InitTypeDef 	GPIO_InitStructure;
	//1. AFIOʱ��ʹ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
    
	//2. ����ȫ����ӳ��
    /*
    ȫӳ������
    GPIO_PinRemapConfig(GPIO_FullRemap_TIM3, ENABLE); 
    ����ӳ������
    GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE); 
    ӳ�����������ȫ�����ǲ��֣����ݾ���Ӳ��������
    GPIO_PinRemapConfig(GPIO_Remap_TIM3, ENABLE); 
    */
    GPIO_PinRemapConfig(GPIO_FullRemap_TIM1, ENABLE); 
	
	//3. ���ݵ�ǰ��ӳ��ģʽ���ж˿����ã���Ϊ���������50MHz��
	switch(CURRENT_MODE)
	{
		//3.1 ����ӳ�� �� ������ӳ��
		case NoRemap|PartialRemap:
		{
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11;
			GPIO_Init(GPIOA,&GPIO_InitStructure);
			break;
		}
        
        //3.2 ȫ����ӳ��
		case FullRemap:
		{
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE,ENABLE);
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_11|GPIO_Pin_13|GPIO_Pin_14;
			GPIO_Init(GPIOE,&GPIO_InitStructure);
			break;
		}
		
		default:
			break;
	}	
}

//TIM1��װ�ء���Ƶ��ģʽ��ʼ��
void TIM1_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    
    //1. ʹ��APB2ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);
	
	//2. TIM4��װ�ء���Ƶ��ģʽ��ʼ��
    TIM_TimeBaseStructure.TIM_Period = arr;
	TIM_TimeBaseStructure.TIM_Prescaler = psc;
	/*
	TIM_ClockDivision�����������ģʽ�µ�ʱ�ӷ�Ƶ
    ��ʱTIM��Ϊ���������Ƶ
    ��Ƶϵ��=1����TIM_CKD_DIV1
	*/
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //���ϼ���
	TIM_TimeBaseInit(TIM1,&TIM_TimeBaseStructure);
}

//TIM1��OCͨ������ģʽ��ʼ����PWMģʽ�����ڼ�ʱ��ģʽ��GPIO�˿ڳ�ʼ���Ļ���֮�ϣ�
void TIM1_PWM_Init(u16 arr,u16 psc,u32 ht1,u32 ht2,u32 ht3,u32 ht4)
{

    TIM_OCInitTypeDef TIM_OCInitstrcuture;
	//1. ���õײ�ĳ�ʼ������
	TIM1_Init(arr,psc); 
	TIM1_GPIO_Init();
	
	//2. ����PWM����
	/*
	PWM����ģʽ:
		PWM1:
			���ϼ���ʱ����ǰ����ֵС�ڲ���/�ȽϼĴ���CCR�����õ���ֵʱ��
			�����Ч��ƽ���������øߵ�ƽ��Ч���ǵ͵�ƽ��Ч��
		PWM2:
			���ϼ���ʱ��xxx����xxx
	*/
	
	//OCͨ����ģʽΪPWM1���ߵ�ƽΪ��Ч��ƽ��OC���ʹ��
	TIM_OCInitstrcuture.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitstrcuture.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitstrcuture.TIM_OutputNState = TIM_OutputNState_Disable ;
	TIM_OCInitstrcuture.TIM_OCPolarity = TIM_OCPolarity_High;
    
	//4��OCͨ��ȫ����������PWM
	TIM_OC1Init(TIM1,&TIM_OCInitstrcuture);
	TIM_OC2Init(TIM1,&TIM_OCInitstrcuture);
	TIM_OC3Init(TIM1,&TIM_OCInitstrcuture);
	TIM_OC4Init(TIM1,&TIM_OCInitstrcuture);
    
	//����OC�ڵ�Ԥװ��
	TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Disable);
	TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Disable);
	TIM_OC3PreloadConfig(TIM1, TIM_OCPreload_Disable);
	TIM_OC4PreloadConfig(TIM1, TIM_OCPreload_Disable);

    //�����趨��ȱ�ٻ������
    TIM_CtrlPWMOutputs(TIM1, ENABLE);
    
    //�趨PWM�Ƚ���ֵ
    TIM_SetCompare1(TIM1,ht1);
    TIM_SetCompare2(TIM1,ht2);
    TIM_SetCompare3(TIM1,ht3);
    TIM_SetCompare4(TIM1,ht4);
}
///////////////////////////////////////////////////////////////
//                        TIM2
///////////////////////////////////////////////////////////////
/*
	TIM2����4��OutputChannel��
	TIM3_CH1,TIM3_CH2,TIM3_CH3,TIM3_CH4
	���Ӧ���ĸ����ŷֱ�Ϊ������ֻ��Ϊ�����
	����ӳ��:
	PA0,PA1,PA2,PA3
	������ӳ��:
	PA15,PB3,PA2,PA3
    ������ӳ��2:
	PA0,PA1,PB10,PB11
	ȫ����ӳ��:
	PA15,PB3,PB10,PB11

	������ӳ����Ҫ������
	1.����AFIOʱ��
    2.��ӳ������
	2.��Ӧ���ų�ʼ��
*/

//ѡ��ǰ״̬-->ȫ����ӳ��
#define CURRENT_MODE FullRemap

//TIM2��GPIO����ӳ���ʼ��
void TIM2_GPIO_Init(void)
{	
    GPIO_InitTypeDef 	GPIO_InitStructure;
    
	//1. AFIOʱ��ʹ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
    
	//2. ����ȫ����ӳ��
    /*
    ȫӳ������
    GPIO_PinRemapConfig(GPIO_FullRemap_TIM3, ENABLE); 
    ����ӳ������
    GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE); 
    ӳ�����������ȫ�����ǲ��֣����ݾ���Ӳ��������
    GPIO_PinRemapConfig(GPIO_Remap_TIM3, ENABLE); 
    */
    GPIO_PinRemapConfig(GPIO_FullRemap_TIM2, ENABLE); 
	
	//3. ���ݵ�ǰ��ӳ��ģʽ���ж˿����ã���Ϊ���������50MHz��
	switch(CURRENT_MODE)
	{
		//3.1 ����ӳ��
		case NoRemap:
		{            
            RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3;
			GPIO_Init(GPIOA,&GPIO_InitStructure);
			break;
		}
		
		//3.2 ������ӳ��
		case PartialRemap:
		{
            RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB,ENABLE);
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15|GPIO_Pin_2|GPIO_Pin_3;
			GPIO_Init(GPIOA,&GPIO_InitStructure);
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
			GPIO_Init(GPIOB,&GPIO_InitStructure);
			break;
		}
        
        //3.3 ������ӳ��2
		case PartialRemap2:
		{
            RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB,ENABLE);
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;
			GPIO_Init(GPIOA,&GPIO_InitStructure);
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10|GPIO_Pin_11;
			GPIO_Init(GPIOB,&GPIO_InitStructure);
			break;
		}
		
		//3.4 ȫ����ӳ��
		case FullRemap:
		{
            RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB,ENABLE);
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
			GPIO_Init(GPIOA,&GPIO_InitStructure);
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_10;
			GPIO_Init(GPIOB,&GPIO_InitStructure);
			break;
		}
		
		default:
			break;
	}	
}

//TIM2��װ�ء���Ƶ��ģʽ��ʼ��
void TIM2_Init(u16 arr,u16 psc)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    
	//1. ʹ��APB1ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	
	//2. TIM2��װ�ء���Ƶ��ģʽ��ʼ��
    TIM_TimeBaseStructure.TIM_Period = arr;
	TIM_TimeBaseStructure.TIM_Prescaler = psc;
	/*
	TIM_ClockDivision�����������ģʽ�µ�ʱ�ӷ�Ƶ
    ��ʱTIM��Ϊ���������Ƶ
    ��Ƶϵ��=1����TIM_CKD_DIV1
	*/
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //���ϼ���
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseStructure);
}

//TIM2��OCͨ������ģʽ��ʼ����PWMģʽ�����ڼ�ʱ��ģʽ��GPIO�˿ڳ�ʼ���Ļ���֮�ϣ�
void TIM2_PWM_Init(u16 arr,u16 psc,u32 ht1,u32 ht2,u32 ht3,u32 ht4)
{
    TIM_OCInitTypeDef TIM_OCInitstrcuture;
	//1. ���õײ�ĳ�ʼ������
	TIM2_Init(arr,psc); 
	TIM2_GPIO_Init();
	
	//2. ����PWM����
	/*
	PWM����ģʽ:
		PWM1:
			���ϼ���ʱ����ǰ����ֵС�ڲ���/�ȽϼĴ���CCR�����õ���ֵʱ��
			�����Ч��ƽ���������øߵ�ƽ��Ч���ǵ͵�ƽ��Ч��
		PWM2:
			���ϼ���ʱ��xxx����xxx
	*/
	
	//OCͨ����ģʽΪPWM1���ߵ�ƽΪ��Ч��ƽ��OC���ʹ��
	TIM_OCInitstrcuture.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitstrcuture.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitstrcuture.TIM_OCPolarity = TIM_OCPolarity_High;
	
	//4��OCͨ��ȫ����������PWM
	TIM_OC1Init(TIM2,&TIM_OCInitstrcuture);
	TIM_OC2Init(TIM2,&TIM_OCInitstrcuture);
	TIM_OC3Init(TIM2,&TIM_OCInitstrcuture);
	//TIM_OC4Init(TIM2,&TIM_OCInitstrcuture);

	//����OC�ڵ�Ԥװ��
	TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Disable);
	TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Disable);
	TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Disable);
	//TIM_OC4PreloadConfig(TIM2, TIM_OCPreload_Disable);
    
    //�趨PWM�Ƚ���ֵ
    TIM_SetCompare1(TIM2,ht1);
    TIM_SetCompare2(TIM2,ht2);
    TIM_SetCompare3(TIM2,ht3);
    //TIM_SetCompare4(TIM2,ht4);
}

///////////////////////////////////////////////////////////////
//                        TIM3
///////////////////////////////////////////////////////////////
/*
	TIM3����4��OutputChannel��
	TIM3_CH1,TIM3_CH2,TIM3_CH3,TIM3_CH4
	���Ӧ���ĸ����ŷֱ�Ϊ������ֻ��Ϊ�����
	����ӳ��:
	PA6,PA7,PB0,PB1
	������ӳ��:
	PB4,PB5,PB0,PB1
	ȫ����ӳ��:
	PC6,PC7,PC8,PC9	

	������ӳ����Ҫ������
	1.����AFIOʱ��
    2.��ӳ������
	2.��Ӧ���ų�ʼ��
*/

//ѡ��ǰ״̬-->ȫ����ӳ��
#define CURRENT_MODE FullRemap

//TIM3��GPIO����ӳ���ʼ��
void TIM3_GPIO_Init(void)
{
    GPIO_InitTypeDef 	GPIO_InitStructure;
    
	//1. AFIOʱ��ʹ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
    
	//2. ����ȫ����ӳ��
    /*
    ȫӳ������
    GPIO_PinRemapConfig(GPIO_FullRemap_TIM3, ENABLE); 
    ����ӳ������
    GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE); 
    ӳ�����������ȫ�����ǲ��֣����ݾ���Ӳ��������
    GPIO_PinRemapConfig(GPIO_Remap_TIM3, ENABLE); 
    */
    GPIO_PinRemapConfig(GPIO_FullRemap_TIM3, ENABLE); 
	
	//3. ���ݵ�ǰ��ӳ��ģʽ���ж˿����ã���Ϊ���������50MHz��
	switch(CURRENT_MODE)
	{
		//3.1 ����ӳ��
		case NoRemap:
		{
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB,ENABLE);
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;
			GPIO_Init(GPIOA,&GPIO_InitStructure);
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;
			GPIO_Init(GPIOB,&GPIO_InitStructure);
			break;
		}
		
		//3.2 ������ӳ��
		case PartialRemap:
		{
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_4|GPIO_Pin_5;
			GPIO_Init(GPIOB,&GPIO_InitStructure);
			break;
		}
		
		//3.3 ȫ����ӳ��
		case FullRemap:
		{
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_8|GPIO_Pin_9;   //fuck!!!
			GPIO_Init(GPIOC,&GPIO_InitStructure);
			break;
		}
		
		default:
            
			break;
	}	
}

//TIM3��װ�ء���Ƶ��ģʽ��ʼ��
void TIM3_Init(u16 arr,u16 psc)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    
	//1. ʹ��APB1ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
	
	//2. TIM3��װ�ء���Ƶ��ģʽ��ʼ��
    TIM_TimeBaseStructure.TIM_Period = arr;
	TIM_TimeBaseStructure.TIM_Prescaler = psc;
	/*
	TIM_ClockDivision�����������ģʽ�µ�ʱ�ӷ�Ƶ
    ��ʱTIM��Ϊ���������Ƶ
    ��Ƶϵ��=1����TIM_CKD_DIV1
	*/
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //���ϼ���
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseStructure);
}

//TIM3��OCͨ������ģʽ��ʼ����PWMģʽ�����ڼ�ʱ��ģʽ��GPIO�˿ڳ�ʼ���Ļ���֮�ϣ�
void TIM3_PWM_Init(u16 arr,u16 psc,u32 ht1,u32 ht2,u32 ht3,u32 ht4)
{
    TIM_OCInitTypeDef TIM_OCInitstrcuture;
	//1. ���õײ�ĳ�ʼ������
	TIM3_Init(arr,psc); 
	TIM3_GPIO_Init();
	
	//2. ����PWM����
	/*
	PWM����ģʽ:
		PWM1:
			���ϼ���ʱ����ǰ����ֵС�ڲ���/�ȽϼĴ���CCR�����õ���ֵʱ��
			�����Ч��ƽ���������øߵ�ƽ��Ч���ǵ͵�ƽ��Ч��
		PWM2:
			���ϼ���ʱ��xxx����xxx
	*/
	
	//OCͨ����ģʽΪPWM1���ߵ�ƽΪ��Ч��ƽ��OC���ʹ��
	TIM_OCInitstrcuture.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitstrcuture.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitstrcuture.TIM_OCPolarity = TIM_OCPolarity_High;
	
	//4��OCͨ��ȫ����������PWM
	TIM_OC1Init(TIM3,&TIM_OCInitstrcuture);
	//TIM_OC2Init(TIM3,&TIM_OCInitstrcuture);
	TIM_OC3Init(TIM3,&TIM_OCInitstrcuture);
	TIM_OC4Init(TIM3,&TIM_OCInitstrcuture);

	//������OC�ڵ�Ԥװ��
	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Disable);
	//TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Disable);
	TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Disable);
	TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Disable);
    
    //�趨PWM�Ƚ���ֵ
    TIM_SetCompare1(TIM3,ht1);
    //TIM_SetCompare2(TIM3,ht2);
    TIM_SetCompare3(TIM3,ht3);
    TIM_SetCompare4(TIM3,ht4);
}

///////////////////////////////////////////////////////////////
//                        TIM4
///////////////////////////////////////////////////////////////
/*
	TIM4����4��OutputChannel��
	���Ӧ���ĸ����ŷֱ�Ϊ������ֻ��Ϊ�����
	����ӳ��:
	PB6,PB7,PB8,PB9
	��ӳ��:
	PD12,PD13,PD14,PD15
*/

//ѡ��ǰ״̬-->��ӳ��
#define CURRENT_MODE Remap

//TIM4��GPIO����ӳ���ʼ��
void TIM4_GPIO_Init(void)
{
	GPIO_InitTypeDef 	GPIO_InitStructure;
	//1. AFIOʱ��ʹ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
    
	//2. ����ȫ����ӳ��
    /*
    ȫӳ������
    GPIO_PinRemapConfig(GPIO_FullRemap_TIM3, ENABLE); 
    ����ӳ������
    GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE); 
    ӳ�����������ȫ�����ǲ��֣����ݾ���Ӳ��������
    GPIO_PinRemapConfig(GPIO_Remap_TIM3, ENABLE); 
    */
    GPIO_PinRemapConfig(GPIO_Remap_TIM4, ENABLE); 
	
	//3. ���ݵ�ǰ��ӳ��ģʽ���ж˿����ã���Ϊ���������50MHz��
	switch(CURRENT_MODE)
	{
		//3.1 ����ӳ��
		case NoRemap:
		{
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9;
			GPIO_Init(GPIOB,&GPIO_InitStructure);
			break;
		}
		
		//3.2 ��ӳ��
		case Remap:
		{
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD,ENABLE);
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
			GPIO_Init(GPIOD,&GPIO_InitStructure);
			break;
		}
		
		default:
			break;
	}	
}

//TIM4��װ�ء���Ƶ��ģʽ��ʼ��
void TIM4_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    
    //1. ʹ��APB1ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);
	
	//2. TIM4��װ�ء���Ƶ��ģʽ��ʼ��
    TIM_TimeBaseStructure.TIM_Period = arr;
	TIM_TimeBaseStructure.TIM_Prescaler = psc;
	/*
	TIM_ClockDivision�����������ģʽ�µ�ʱ�ӷ�Ƶ
    ��ʱTIM��Ϊ���������Ƶ
    ��Ƶϵ��=1����TIM_CKD_DIV1
	*/
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //���ϼ���
	TIM_TimeBaseInit(TIM4,&TIM_TimeBaseStructure);
}

//TIM4��OCͨ������ģʽ��ʼ����PWMģʽ�����ڼ�ʱ��ģʽ��GPIO�˿ڳ�ʼ���Ļ���֮�ϣ�
void TIM4_PWM_Init(u16 arr,u16 psc,u32 ht1,u32 ht2,u32 ht3,u32 ht4)
{
    TIM_OCInitTypeDef TIM_OCInitstrcuture;
	//1. ���õײ�ĳ�ʼ������
	TIM4_Init(arr,psc); 
	TIM4_GPIO_Init();
	
	//2. ����PWM����
	/*
	PWM����ģʽ:
		PWM1:
			���ϼ���ʱ����ǰ����ֵС�ڲ���/�ȽϼĴ���CCR�����õ���ֵʱ��
			�����Ч��ƽ���������øߵ�ƽ��Ч���ǵ͵�ƽ��Ч��
		PWM2:
			���ϼ���ʱ��xxx����xxx
	*/
	
	//OCͨ����ģʽΪPWM1���ߵ�ƽΪ��Ч��ƽ��OC���ʹ��
	TIM_OCInitstrcuture.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitstrcuture.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitstrcuture.TIM_OCPolarity = TIM_OCPolarity_High;
	
	//4��OCͨ��ȫ����������PWM
	TIM_OC1Init(TIM4,&TIM_OCInitstrcuture);
	TIM_OC2Init(TIM4,&TIM_OCInitstrcuture);
	TIM_OC3Init(TIM4,&TIM_OCInitstrcuture);
	TIM_OC4Init(TIM4,&TIM_OCInitstrcuture);
    
	//������OC�ڵ�Ԥװ��
	TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Disable);
	TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Disable);
	TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Disable);
	TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Disable);
    
    //�趨PWM�Ƚ���ֵ
    TIM_SetCompare1(TIM4,ht1);
    TIM_SetCompare2(TIM4,ht2);
    TIM_SetCompare3(TIM4,ht3);
    TIM_SetCompare4(TIM4,ht4);
}                                                                                                                                                                                        

//////////////////////////////////////////////////////////
//              ����ͨ��
//              CCER��TIM_x��16λ�Ĵ�������0,4,8,12λ����ͨ��ʹ��
////////////////////////////////////////////////////////////
//              TIM3��OC1��OC2
void TIM3_CH1_close(void) 
{
TIM3->CCER&=~(1); 
}
void TIM3_CH1_open(void) 
{
TIM3->CCER|=(1); 
}
void TIM3_CH2_close(void) 
{
TIM3->CCER&=~(1<<4); 
}
void TIM3_CH2_open(void) 
{
TIM3->CCER|=(1<<4); 
}
////////////////////////////////////////////////////////////
//              TIM4��OC1��OC2
void TIM4_CH1_close(void) 
{ 
TIM4->CCER&=~(1); 
} 
void TIM4_CH1_open(void) 
{ 
TIM4->CCER|=(1); 
} 
void TIM4_CH3_close(void) 
{ 
TIM4->CCER&=~(1<<8); 
} 
void TIM4_CH3_open(void) 
{ 
TIM4->CCER|=(1<<8); 
} 
