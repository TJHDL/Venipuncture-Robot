#include "timer.h"

/*
debug日志：
配置思路
(1)       TIM_DeInit()

(2)       TIM_InternalClockConfig()

(3)       TIM_Perscaler

(4)       TIM_ClockDivision

(5)       TIM_CounterMode

(6)       TIM_Period

(7)       TIM_ARRPerloadConfig()

(8)       TIM_ITConfig()

其中(3)-(6)TIM_TimerBaseInitTypeDef
*/

/*
    时钟基本周期计算方式：
    Tclk = 72MHz
    psc一般取1
	Tout = ((arr+1)*(psc+1))/Tclk
*/

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
*/

//定义重映射状态
#define NoRemap   0
#define Remap   1
#define PartialRemap 2
#define PartialRemap2 3
#define FullRemap 4

///////////////////////////////////////////////////////////////
//                        高级定时器TIM1
//       注意TIM1、GPIO、EXTI连接的外围总线是AHB2（更快）
//       TIM2~7连接的外围总线是AHB1（更慢）
//       特殊设定，缺少会出问题
//       TIM_CtrlPWMOutputs(TIM1, ENABLE);    
///////////////////////////////////////////////////////////////
/*
	高级定时器TIM1的通用功能共有4个OutputChannel：
	其对应的四个引脚分别为（建议只作为输出）
	无重映射:
	PA8,PA9,PA10,PA11
	重映射:
	PE9,PE11,PE13,PE14
*/

//选择当前状态-->重映射
#define CURRENT_MODE FullRemap

//TIM1的GPIO口重映射初始化
void TIM1_GPIO_Init(void)
{
	GPIO_InitTypeDef 	GPIO_InitStructure;
	//1. AFIO时钟使能
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
    
	//2. 开启全部重映射
    /*
    全映射命令
    GPIO_PinRemapConfig(GPIO_FullRemap_TIM3, ENABLE); 
    部分映射命令
    GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE); 
    映射命令（不区分全部还是部分，依据具体硬件而定）
    GPIO_PinRemapConfig(GPIO_Remap_TIM3, ENABLE); 
    */
    GPIO_PinRemapConfig(GPIO_FullRemap_TIM1, ENABLE); 
	
	//3. 根据当前重映射模式进行端口配置（均为推挽输出，50MHz）
	switch(CURRENT_MODE)
	{
		//3.1 无重映射 或 部分重映射
		case NoRemap|PartialRemap:
		{
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11;
			GPIO_Init(GPIOA,&GPIO_InitStructure);
			break;
		}
        
        //3.2 全部重映射
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

//TIM1的装载、分频、模式初始化
void TIM1_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    
    //1. 使能APB2时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);
	
	//2. TIM4的装载、分频、模式初始化
    TIM_TimeBaseStructure.TIM_Period = arr;
	TIM_TimeBaseStructure.TIM_Prescaler = psc;
	/*
	TIM_ClockDivision用于输出捕获模式下的时钟分频
    此时TIM作为输出，不分频
    分频系数=1，即TIM_CKD_DIV1
	*/
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //向上计数
	TIM_TimeBaseInit(TIM1,&TIM_TimeBaseStructure);
}

//TIM1的OC通道工作模式初始化（PWM模式）（在计时器模式和GPIO端口初始化的基础之上）
void TIM1_PWM_Init(u16 arr,u16 psc,u32 ht1,u32 ht2,u32 ht3,u32 ht4)
{

    TIM_OCInitTypeDef TIM_OCInitstrcuture;
	//1. 调用底层的初始化函数
	TIM1_Init(arr,psc); 
	TIM1_GPIO_Init();
	
	//2. 设置PWM参数
	/*
	PWM两种模式:
		PWM1:
			向上计数时，当前计数值小于捕获/比较寄存器CCR里设置的阈值时，
			输出有效电平（可以设置高电平有效还是低电平有效）
		PWM2:
			向上计数时，xxx大于xxx
	*/
	
	//OC通道：模式为PWM1，高电平为有效电平，OC输出使能
	TIM_OCInitstrcuture.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitstrcuture.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitstrcuture.TIM_OutputNState = TIM_OutputNState_Disable ;
	TIM_OCInitstrcuture.TIM_OCPolarity = TIM_OCPolarity_High;
    
	//4个OC通道全部用于生成PWM
	TIM_OC1Init(TIM1,&TIM_OCInitstrcuture);
	TIM_OC2Init(TIM1,&TIM_OCInitstrcuture);
	TIM_OC3Init(TIM1,&TIM_OCInitstrcuture);
	TIM_OC4Init(TIM1,&TIM_OCInitstrcuture);
    
	//允许OC口的预装载
	TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Disable);
	TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Disable);
	TIM_OC3PreloadConfig(TIM1, TIM_OCPreload_Disable);
	TIM_OC4PreloadConfig(TIM1, TIM_OCPreload_Disable);

    //特殊设定，缺少会出问题
    TIM_CtrlPWMOutputs(TIM1, ENABLE);
    
    //设定PWM比较阈值
    TIM_SetCompare1(TIM1,ht1);
    TIM_SetCompare2(TIM1,ht2);
    TIM_SetCompare3(TIM1,ht3);
    TIM_SetCompare4(TIM1,ht4);
}
///////////////////////////////////////////////////////////////
//                        TIM2
///////////////////////////////////////////////////////////////
/*
	TIM2共有4个OutputChannel：
	TIM3_CH1,TIM3_CH2,TIM3_CH3,TIM3_CH4
	其对应的四个引脚分别为（建议只作为输出）
	无重映射:
	PA0,PA1,PA2,PA3
	部分重映射:
	PA15,PB3,PA2,PA3
    部分重映射2:
	PA0,PA1,PB10,PB11
	全部重映射:
	PA15,PB3,PB10,PB11

	开启重映射需要做到：
	1.开启AFIO时钟
    2.重映射命令
	2.对应引脚初始化
*/

//选择当前状态-->全部重映射
#define CURRENT_MODE FullRemap

//TIM2的GPIO口重映射初始化
void TIM2_GPIO_Init(void)
{	
    GPIO_InitTypeDef 	GPIO_InitStructure;
    
	//1. AFIO时钟使能
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
    
	//2. 开启全部重映射
    /*
    全映射命令
    GPIO_PinRemapConfig(GPIO_FullRemap_TIM3, ENABLE); 
    部分映射命令
    GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE); 
    映射命令（不区分全部还是部分，依据具体硬件而定）
    GPIO_PinRemapConfig(GPIO_Remap_TIM3, ENABLE); 
    */
    GPIO_PinRemapConfig(GPIO_FullRemap_TIM2, ENABLE); 
	
	//3. 根据当前重映射模式进行端口配置（均为推挽输出，50MHz）
	switch(CURRENT_MODE)
	{
		//3.1 无重映射
		case NoRemap:
		{            
            RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3;
			GPIO_Init(GPIOA,&GPIO_InitStructure);
			break;
		}
		
		//3.2 部分重映射
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
        
        //3.3 部分重映射2
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
		
		//3.4 全部重映射
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

//TIM2的装载、分频、模式初始化
void TIM2_Init(u16 arr,u16 psc)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    
	//1. 使能APB1时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	
	//2. TIM2的装载、分频、模式初始化
    TIM_TimeBaseStructure.TIM_Period = arr;
	TIM_TimeBaseStructure.TIM_Prescaler = psc;
	/*
	TIM_ClockDivision用于输出捕获模式下的时钟分频
    此时TIM作为输出，不分频
    分频系数=1，即TIM_CKD_DIV1
	*/
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //向上计数
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseStructure);
}

//TIM2的OC通道工作模式初始化（PWM模式）（在计时器模式和GPIO端口初始化的基础之上）
void TIM2_PWM_Init(u16 arr,u16 psc,u32 ht1,u32 ht2,u32 ht3,u32 ht4)
{
    TIM_OCInitTypeDef TIM_OCInitstrcuture;
	//1. 调用底层的初始化函数
	TIM2_Init(arr,psc); 
	TIM2_GPIO_Init();
	
	//2. 设置PWM参数
	/*
	PWM两种模式:
		PWM1:
			向上计数时，当前计数值小于捕获/比较寄存器CCR里设置的阈值时，
			输出有效电平（可以设置高电平有效还是低电平有效）
		PWM2:
			向上计数时，xxx大于xxx
	*/
	
	//OC通道：模式为PWM1，高电平为有效电平，OC输出使能
	TIM_OCInitstrcuture.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitstrcuture.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitstrcuture.TIM_OCPolarity = TIM_OCPolarity_High;
	
	//4个OC通道全部用于生成PWM
	TIM_OC1Init(TIM2,&TIM_OCInitstrcuture);
	TIM_OC2Init(TIM2,&TIM_OCInitstrcuture);
	TIM_OC3Init(TIM2,&TIM_OCInitstrcuture);
	//TIM_OC4Init(TIM2,&TIM_OCInitstrcuture);

	//允许OC口的预装载
	TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Disable);
	TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Disable);
	TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Disable);
	//TIM_OC4PreloadConfig(TIM2, TIM_OCPreload_Disable);
    
    //设定PWM比较阈值
    TIM_SetCompare1(TIM2,ht1);
    TIM_SetCompare2(TIM2,ht2);
    TIM_SetCompare3(TIM2,ht3);
    //TIM_SetCompare4(TIM2,ht4);
}

///////////////////////////////////////////////////////////////
//                        TIM3
///////////////////////////////////////////////////////////////
/*
	TIM3共有4个OutputChannel：
	TIM3_CH1,TIM3_CH2,TIM3_CH3,TIM3_CH4
	其对应的四个引脚分别为（建议只作为输出）
	无重映射:
	PA6,PA7,PB0,PB1
	部分重映射:
	PB4,PB5,PB0,PB1
	全部重映射:
	PC6,PC7,PC8,PC9	

	开启重映射需要做到：
	1.开启AFIO时钟
    2.重映射命令
	2.对应引脚初始化
*/

//选择当前状态-->全部重映射
#define CURRENT_MODE FullRemap

//TIM3的GPIO口重映射初始化
void TIM3_GPIO_Init(void)
{
    GPIO_InitTypeDef 	GPIO_InitStructure;
    
	//1. AFIO时钟使能
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
    
	//2. 开启全部重映射
    /*
    全映射命令
    GPIO_PinRemapConfig(GPIO_FullRemap_TIM3, ENABLE); 
    部分映射命令
    GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE); 
    映射命令（不区分全部还是部分，依据具体硬件而定）
    GPIO_PinRemapConfig(GPIO_Remap_TIM3, ENABLE); 
    */
    GPIO_PinRemapConfig(GPIO_FullRemap_TIM3, ENABLE); 
	
	//3. 根据当前重映射模式进行端口配置（均为推挽输出，50MHz）
	switch(CURRENT_MODE)
	{
		//3.1 无重映射
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
		
		//3.2 部分重映射
		case PartialRemap:
		{
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_4|GPIO_Pin_5;
			GPIO_Init(GPIOB,&GPIO_InitStructure);
			break;
		}
		
		//3.3 全部重映射
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

//TIM3的装载、分频、模式初始化
void TIM3_Init(u16 arr,u16 psc)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    
	//1. 使能APB1时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
	
	//2. TIM3的装载、分频、模式初始化
    TIM_TimeBaseStructure.TIM_Period = arr;
	TIM_TimeBaseStructure.TIM_Prescaler = psc;
	/*
	TIM_ClockDivision用于输出捕获模式下的时钟分频
    此时TIM作为输出，不分频
    分频系数=1，即TIM_CKD_DIV1
	*/
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //向上计数
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseStructure);
}

//TIM3的OC通道工作模式初始化（PWM模式）（在计时器模式和GPIO端口初始化的基础之上）
void TIM3_PWM_Init(u16 arr,u16 psc,u32 ht1,u32 ht2,u32 ht3,u32 ht4)
{
    TIM_OCInitTypeDef TIM_OCInitstrcuture;
	//1. 调用底层的初始化函数
	TIM3_Init(arr,psc); 
	TIM3_GPIO_Init();
	
	//2. 设置PWM参数
	/*
	PWM两种模式:
		PWM1:
			向上计数时，当前计数值小于捕获/比较寄存器CCR里设置的阈值时，
			输出有效电平（可以设置高电平有效还是低电平有效）
		PWM2:
			向上计数时，xxx大于xxx
	*/
	
	//OC通道：模式为PWM1，高电平为有效电平，OC输出使能
	TIM_OCInitstrcuture.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitstrcuture.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitstrcuture.TIM_OCPolarity = TIM_OCPolarity_High;
	
	//4个OC通道全部用于生成PWM
	TIM_OC1Init(TIM3,&TIM_OCInitstrcuture);
	//TIM_OC2Init(TIM3,&TIM_OCInitstrcuture);
	TIM_OC3Init(TIM3,&TIM_OCInitstrcuture);
	TIM_OC4Init(TIM3,&TIM_OCInitstrcuture);

	//不允许OC口的预装载
	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Disable);
	//TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Disable);
	TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Disable);
	TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Disable);
    
    //设定PWM比较阈值
    TIM_SetCompare1(TIM3,ht1);
    //TIM_SetCompare2(TIM3,ht2);
    TIM_SetCompare3(TIM3,ht3);
    TIM_SetCompare4(TIM3,ht4);
}

///////////////////////////////////////////////////////////////
//                        TIM4
///////////////////////////////////////////////////////////////
/*
	TIM4共有4个OutputChannel：
	其对应的四个引脚分别为（建议只作为输出）
	无重映射:
	PB6,PB7,PB8,PB9
	重映射:
	PD12,PD13,PD14,PD15
*/

//选择当前状态-->重映射
#define CURRENT_MODE Remap

//TIM4的GPIO口重映射初始化
void TIM4_GPIO_Init(void)
{
	GPIO_InitTypeDef 	GPIO_InitStructure;
	//1. AFIO时钟使能
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
    
	//2. 开启全部重映射
    /*
    全映射命令
    GPIO_PinRemapConfig(GPIO_FullRemap_TIM3, ENABLE); 
    部分映射命令
    GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE); 
    映射命令（不区分全部还是部分，依据具体硬件而定）
    GPIO_PinRemapConfig(GPIO_Remap_TIM3, ENABLE); 
    */
    GPIO_PinRemapConfig(GPIO_Remap_TIM4, ENABLE); 
	
	//3. 根据当前重映射模式进行端口配置（均为推挽输出，50MHz）
	switch(CURRENT_MODE)
	{
		//3.1 无重映射
		case NoRemap:
		{
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9;
			GPIO_Init(GPIOB,&GPIO_InitStructure);
			break;
		}
		
		//3.2 重映射
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

//TIM4的装载、分频、模式初始化
void TIM4_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    
    //1. 使能APB1时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);
	
	//2. TIM4的装载、分频、模式初始化
    TIM_TimeBaseStructure.TIM_Period = arr;
	TIM_TimeBaseStructure.TIM_Prescaler = psc;
	/*
	TIM_ClockDivision用于输出捕获模式下的时钟分频
    此时TIM作为输出，不分频
    分频系数=1，即TIM_CKD_DIV1
	*/
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //向上计数
	TIM_TimeBaseInit(TIM4,&TIM_TimeBaseStructure);
}

//TIM4的OC通道工作模式初始化（PWM模式）（在计时器模式和GPIO端口初始化的基础之上）
void TIM4_PWM_Init(u16 arr,u16 psc,u32 ht1,u32 ht2,u32 ht3,u32 ht4)
{
    TIM_OCInitTypeDef TIM_OCInitstrcuture;
	//1. 调用底层的初始化函数
	TIM4_Init(arr,psc); 
	TIM4_GPIO_Init();
	
	//2. 设置PWM参数
	/*
	PWM两种模式:
		PWM1:
			向上计数时，当前计数值小于捕获/比较寄存器CCR里设置的阈值时，
			输出有效电平（可以设置高电平有效还是低电平有效）
		PWM2:
			向上计数时，xxx大于xxx
	*/
	
	//OC通道：模式为PWM1，高电平为有效电平，OC输出使能
	TIM_OCInitstrcuture.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitstrcuture.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitstrcuture.TIM_OCPolarity = TIM_OCPolarity_High;
	
	//4个OC通道全部用于生成PWM
	TIM_OC1Init(TIM4,&TIM_OCInitstrcuture);
	TIM_OC2Init(TIM4,&TIM_OCInitstrcuture);
	TIM_OC3Init(TIM4,&TIM_OCInitstrcuture);
	TIM_OC4Init(TIM4,&TIM_OCInitstrcuture);
    
	//不允许OC口的预装载
	TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Disable);
	TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Disable);
	TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Disable);
	TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Disable);
    
    //设定PWM比较阈值
    TIM_SetCompare1(TIM4,ht1);
    TIM_SetCompare2(TIM4,ht2);
    TIM_SetCompare3(TIM4,ht3);
    TIM_SetCompare4(TIM4,ht4);
}                                                                                                                                                                                        

//////////////////////////////////////////////////////////
//              开关通道
//              CCER是TIM_x的16位寄存器，第0,4,8,12位控制通道使能
////////////////////////////////////////////////////////////
//              TIM3的OC1和OC2
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
//              TIM4的OC1和OC2
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
