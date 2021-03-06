// #include "driver.h"

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
    PF12  用于控制急停（stop=1有效�
*/						  
////////////////////////////////////////////////////////////////////////////////// 	   

// 使能上述全部端口
//void driver_Init(void)
//{
//    GPIO_InitTypeDef  GPIO_InitStructure;

//    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF, ENABLE);
//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_12;
//    //PFPF0~7，PF12 端口配置
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
//    //推挽输出，IO口速度为50MHz
//    GPIO_Init(GPIOF, &GPIO_InitStructure);
//    //根据设定参数初始化PF0~7，PF12

//    //TIM1
//    //PE9,11,13,14    端口配置
//    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_11|GPIO_Pin_13|GPIO_Pin_14;
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 		 //复用推挽输出
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
//    GPIO_Init(GPIOE, &GPIO_InitStructure);
//    
//    //TIM2
//    //PA15	    端口配置
//    //PB3,10,11	    端口配置
//    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB, ENABLE);
//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
//    GPIO_Init(GPIOA, &GPIO_InitStructure);
//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_10|GPIO_Pin_11;
//    GPIO_Init(GPIOB, &GPIO_InitStructure);

//    //TIM3
//    //PC6~9	    端口配置
//    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9;
//    GPIO_Init(GPIOC, &GPIO_InitStructure);

//    //TIM4
//    //PD12~15	端口配置  
//    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
//    GPIO_Init(GPIOD, &GPIO_InitStructure);
//      
//}

#include "driver.h"//////////////////////////////////////////////////////////////////////////////////  
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
    TIM4的PD12~15    对于步进电机，PWM频率重要，而占空比不重要
    因此取4个定时器的CH4作为代表其频率的PWM输出端：
    PE14,PB11,PC9,PD15
    
    通过修改arr和psc可以改变时钟基本频率
    通过TIM_SetComparex(TIMy,ccr)调节TIMy的Channelx的PWM占空比
    
    PF0~7 用于控制8个PWM的方向
    PF12  用于控制急停（stop=1有效?
*/

//////////////////////////////////////////////////////////////////////////////////     // 使能上述全部端口
void driver_Init(void)
{
	  GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable,ENABLE);
	
    GPIO_InitTypeDef  GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7|GPIO_Pin_1|GPIO_Pin_4|GPIO_Pin_3;   //fuck!!!
    //PFPF0~7，PF12 端口配置
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;   //IO口速度为50MHz
    //推挽输出，IO口速度为50MHz
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    //根据设定参数初始化PF0~7，PF12
   
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
    //PFPF0~7，PF12 端口配置
    //GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    //推挽输出
    //GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;   //IO口速度为50MHz
    //推挽输出，IO口速度为50MHz
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
    //PE9,11,13,14    端口配置
    //RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    //|GPIO_Pin_11|GPIO_Pin_13|GPIO_Pin_14;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;   //复用推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;   //IO口速度为50MHz
    GPIO_Init(GPIOE, &GPIO_InitStructure);
    
    //TIM2
    //PA15     端口配置
    //PB3,10,11     端口配置
    //RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    //GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_10|GPIO_Pin_11;
    //GPIO_Init(GPIOB, &GPIO_InitStructure);    //TIM3
    //PC6~9     端口配置
    //RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    //GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    //|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9;
    //GPIO_Init(GPIOC, &GPIO_InitStructure);  //TIM4
    //PD12~15 端口配置  
    //RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_14;
    //|GPIO_Pin_13|GPIO_Pin_15;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
}
