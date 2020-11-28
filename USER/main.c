#include "delay.h"
#include "sys.h"
#include "key.h"
#include "driver.h"
#include "exti.h"
#include "timi.h"
#include "timer.h"
#include "usart.h"

/*
    用于记录整体架构
    
    TIM3的OC1   z轴
    TIM1的OC1   x轴
    TIM2的OC1   y轴
    TIM4的OC1   注射
    TIM4的OC3   扎针
    
    
    子程序：
    1.设定初始频率
    2.各个电机复位（旋转关节处向上复位要求有挡板）
    3.等待串口给出xy位移
    4.xy轴移动（无反馈）
    5.z轴缓慢下降（等待串口命令暂停）
    6.端部旋转固定角度
    7.扎针，注射，退针，旋转回零位
    8.按照所给正向位移，加一个安全阈值反向退回零位
    9.等待串口给出xy位移
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
    因此取4个定时器的CH1作为代表其频率的PWM输出端：
    #define pulse1 PCout(6) TIM3的OC1   z轴
    #define pulse2 PEout(9) TIM1的OC1   x轴
    #define pulse3 PAout(15)    TIM2的OC1   y轴
    #define pulse4 PDout(12)    TIM4的OC1   注射
    #define pulse5 PDout(14)    TIM4的OC3   扎针
    
    TIM3的OC1   z轴
    TIM1的OC1   x轴
    TIM2的OC1   y轴
    TIM4的OC1   注射
    TIM4的OC3   扎针
    
    通过修改arr和psc可以改变时钟基本频率
    通过TIM_SetComparex(TIMy,ccr)调节TIMy的Channelx的PWM占空比
    
    PF0~7 用于控制8个PWM的方向
    PF12  用于控制急停（stop=1有效）
*/

void start(void)
{
    ENA1 = 0;
    ENA2 = 0;
    ENA3 = 0;
    ENA4 = 0;
    ENA5 = 0;
    return; 
}

void stop(void)
{
    ENA1 = 1;
    ENA2 = 1;
    ENA3 = 1;
    ENA4 = 1;
    ENA5 = 1;
    return; 
}

void GoOn(void)
{
		dir1 = 0;//向下
    dir2 = 1;//支架向外
    dir3 = 0;//向门里
    dir4 = 0;
    dir5 = 0;//扎针
}

void Back(void)
{
		dir1 = 1;//向上
    dir2 = 0;//末端向支架
    dir3 = 1;//向门外
    dir4 = 1;
    dir5 = 1;//退针
}

void para_init(void)
{
    delay_init();
	  driver_Init();
    //EXTIX_Init();
    TIMI_Init();
    //KEY_Init();
    //usart_Init(115200);
	  uart_init(115200);
    
    mdir1 = 0;
    mdir2 = 0;
    mdir3 = 0;
    mdir4 = 0;
    mdir5 = 0;
    mENA1 = 0;
    mENA2 = 0;
    mENA3 = 0;
    //mENA4 = 0;
    mENA5 = 0;
    mpulse1 = 0;
    mpulse2 = 0;
    mpulse3 = 0;
    mpulse4 = 0;
    //mpulse5 = 0;
    
    //默认暂停
    stop();
    
    //默认正向旋转
    //！！！！！！！！！需要调试应该调整至复位方向
    Back();

    // 用于设置TIM1的4路PWM
    u32 PSC1=72-1;
    // 基本单位0.001ms
    u32 PERIOD1=1000-1;
    // 1000个0.001ms=1ms一个脉冲，每秒1000个脉冲
    // 分频后1000Hz
    u32 HIGHTIME_1_1=800;
    u32 HIGHTIME_1_2=800;
    u32 HIGHTIME_1_3=800;
    u32 HIGHTIME_1_4=800;
 
    // 用于设置TIM2的4路PWM
    u32 PSC2=72-1;
    u32 PERIOD2=1000-1;
    // 分频后1000Hz
    u32 HIGHTIME_2_1=800;
    u32 HIGHTIME_2_2=800;
    u32 HIGHTIME_2_3=800;
    u32 HIGHTIME_2_4=800;

    // 用于设置TIM3的4路PWM
    u32 PSC3=72-1;
    u32 PERIOD3=1000-1;
    // 分频后1000HZ
    u32 HIGHTIME_3_1=800;
    u32 HIGHTIME_3_2=800;
    u32 HIGHTIME_3_3=800;
    u32 HIGHTIME_3_4=800;

    // 用于设置TIM4的4路PWM
    u32 PSC4=72-1;
    u32 PERIOD4=1250-1;
    // 1250个0.001ms=1.25ms一个脉冲，每秒800个脉冲
    // 分频后800Hz  (T=1.25ms)
    u32 HIGHTIME_4_1=800;
    u32 HIGHTIME_4_2=800;
    u32 HIGHTIME_4_3=800;
    u32 HIGHTIME_4_4=800;
    
/*	
    TIMx_PWM_Init(arr,psc) 集成了 TIMx_Init(arr,psc) 和 TIMx_GPIO_Init()
    初始化PWM，PWM1模式下，先高电平后低电平
    arr参数可以控制一个PWM的周期，psc参数是系统时钟的分频系数
*/

    //TIM1电机1
    TIM1_PWM_Init(PERIOD1,PSC1,HIGHTIME_1_1,HIGHTIME_1_2,HIGHTIME_1_3,HIGHTIME_1_4);
    //TIM2电机2
    TIM2_PWM_Init(PERIOD2,PSC2,HIGHTIME_2_1,HIGHTIME_2_2,HIGHTIME_2_3,HIGHTIME_2_4);
    //TIM3电机3
    TIM3_PWM_Init(PERIOD3,PSC3,HIGHTIME_3_1,HIGHTIME_3_2,HIGHTIME_3_3,HIGHTIME_3_4);
    //TIM4电机4
    TIM4_PWM_Init(PERIOD4,PSC4,HIGHTIME_4_1,HIGHTIME_4_2,HIGHTIME_4_3,HIGHTIME_4_4);
    
    return;
}

//复位程序
void REST(void)
{
	    start();
	    Back();
	
	    //扎针
			TIM_Cmd(TIM4,ENABLE);
			delay_ms(1500);
			delay_ms(1500);
			delay_ms(1500);
			delay_ms(500);
			TIM_Cmd(TIM4,DISABLE);
			delay_ms(500);
	
	    //z轴
			TIM_Cmd(TIM3,ENABLE);
			delay_ms(1500);
			delay_ms(1500);
			delay_ms(1500);
			delay_ms(1500);
			delay_ms(1500);
			delay_ms(700);
			TIM_Cmd(TIM3,DISABLE);
			delay_ms(500);
	
	    
	    //y轴
			TIM_Cmd(TIM1,ENABLE);
			ENA2 = 1;
			delay_ms(1500);
	    delay_ms(1000);
			TIM_Cmd(TIM1,DISABLE);
			ENA2 = 0;
			delay_ms(500);
			
			
	    //x轴
			TIM_Cmd(TIM1,ENABLE);
			ENA3 = 1;
			delay_ms(1000);
			TIM_Cmd(TIM1,DISABLE);
			ENA3 = 0;
			delay_ms(500);
			
			//装置停止
			stop();
			delay_ms(1500);
	
	
	  /*
    //开始
    //使能TIM4的时钟
    TIM_Cmd(TIM4,ENABLE);
		//使能TIM3的时钟
    TIM_Cmd(TIM3,ENABLE);
		//使能TIM2的时钟
    TIM_Cmd(TIM2,ENABLE);
    //使能TIM1的时钟
    TIM_Cmd(TIM1,ENABLE);  
    
    //复位15s保证可以到零位
    for(int i = 1;i<=15;i++)
    {
        delay_ms(1000);
    }
    
    //停止
    //使能TIM1的时钟
    TIM_Cmd(TIM1,DISABLE);
    //使能TIM2的时钟
    TIM_Cmd(TIM2,DISABLE);
    //使能TIM3的时钟
    TIM_Cmd(TIM3,DISABLE);
    //使能TIM4的时钟
    TIM_Cmd(TIM4,DISABLE);
		*/
}

#define max(a,b) ( ((a)>(b)) ? (a):(b) )
#define min(a,b) ( ((a)>(b)) ? (b):(a) )

extern int motor_start;
extern int motor_stop;

int main()
{
    //  设定 定时器、通道 的初始频率
    para_init();
    
    //  设定电机运行时间参数
    u32 x;
    u32 y;
    u32 Tx;
    u32 Ty;
    u32 Tz = 15;
    u32 T4_1 = 3;
    u32 T4_3 = 3;
    uint8_t data[32] ;
    
		start();
    
	
	  GoOn();
			
		//x轴
		TIM_Cmd(TIM1,ENABLE);
		ENA3 = 1;
		delay_ms(1000);
		TIM_Cmd(TIM1,DISABLE);
		ENA3 = 0;
		delay_ms(500);
		
		//y轴
		TIM_Cmd(TIM1,ENABLE);
		ENA2 = 1;
		delay_ms(1500);
		delay_ms(1000);
		TIM_Cmd(TIM1,DISABLE);
		ENA2 = 0;
		delay_ms(500);
		
		//z轴
		TIM_Cmd(TIM3,ENABLE);
		delay_ms(1500);
		delay_ms(1500);
		delay_ms(1500);
		delay_ms(1500);
		delay_ms(1500);
		delay_ms(700);
		TIM_Cmd(TIM3,DISABLE);
		delay_ms(500);
		
		//扎针
		TIM_Cmd(TIM4,ENABLE);
		delay_ms(1500);
		delay_ms(1500);
		delay_ms(1500);
		delay_ms(500);
		TIM_Cmd(TIM4,DISABLE);
		delay_ms(500);
		
		//装置停止
		stop();
		delay_ms(1500);
		delay_ms(1500);
		
		//装置归位
		REST();
		
		
    //  PWM设置完毕，开始死循环
    while(1)
    {
			/*
			GoOn();
			TIM_Cmd(TIM4,ENABLE);
		  delay_ms(1500);
			delay_ms(1500);
			delay_ms(1500);
			delay_ms(1500);
		  TIM_Cmd(TIM4,DISABLE);
		  delay_ms(1000);
			Back();
			TIM_Cmd(TIM4,ENABLE);
		  delay_ms(1500);
			delay_ms(1500);
			delay_ms(1500);
			delay_ms(1500);
		  TIM_Cmd(TIM4,DISABLE);
		  delay_ms(1000);
			*/
			/*
			GoOn();
			TIM_Cmd(TIM3,ENABLE);
		  delay_ms(1500);
			delay_ms(1500);
			delay_ms(1500);
			delay_ms(1500);
			delay_ms(1500);
			delay_ms(800);
		  TIM_Cmd(TIM3,DISABLE);
		  delay_ms(1000);
			Back();
			TIM_Cmd(TIM3,ENABLE);
		  delay_ms(1500);
			delay_ms(1500);
			delay_ms(1500);
			delay_ms(1500);
			delay_ms(1500);
			delay_ms(800);
		  TIM_Cmd(TIM3,DISABLE);
		  delay_ms(1000);
			*/
			
			/*
			//y轴
			GoOn();
			TIM_Cmd(TIM1,ENABLE);
			ENA2 = 1;
			delay_ms(1500);
			TIM_Cmd(TIM1,DISABLE);
			ENA2 = 0;
			delay_ms(500);
			Back();
			TIM_Cmd(TIM1,ENABLE);
			ENA2 = 1;
			delay_ms(1500);
			TIM_Cmd(TIM1,DISABLE);
			ENA2 = 0;
			delay_ms(500);
			*/

//  1.等待串口的坐标信息
//  AX_UART_PI_GetRxData(data)
//  AX_UART_PI_GetRxData()
			/*
        while(1)
        {
            //if(AX_UART_PI_GetRxData(data))
					  if(flag == 1)
            {
                printf("start/r/n");
							  flag = 0;
                start();
                break;
            }
        }
        //  复原标志位
			delay_ms(1000);
			start();
        printf("kankan/r/n");  
//  2.计算、储存各个电机需要工作的时间
//  大驱动器1000pul/r
//  小驱动器800pul/r
//  大螺距5mm
//  小螺距1mm
//      u32 x;
//      u32 y;
//      u32 Tx;
//      u32 Ty;
//      u32 Tz;
//      u32 T4_1;
//      u32 T4_3;
        
//        Tx = (x-x_0)/5.0;
//        Ty = (y-y_0)/5.0;
        Tx = 5;
        Ty = 10;

//  3.各个电机复位（旋转关节处向上复位要求有挡板）
        REST(); 
    
//  4.xy轴移动（无反馈）
        //////////////////////// 实现xy平移
        // x轴(TIM2)圈 =（x目标坐标-当前位置）/丝杆螺距
        // 脉冲数 = 圈数*每圈的脉冲数
        // 开电机TIM1和TIM2的OC1
				// 设置电机注射过程的方向
				GoOn();
        // 使能TIM1的时钟
        TIM_Cmd(TIM1,ENABLE);
        // 使能TIM2的时钟
        TIM_Cmd(TIM2,ENABLE);
        // delay时间 = 脉冲数/脉冲频率
        
        for(int i = 1;i<=min(Tx,Ty);i++)
        {
         delay_ms(1000);
        }
        
        // y轴先停
        if(Tx>Ty)
        {
            // 先关闭TIM2的时钟,x轴继续动
            TIM_Cmd(TIM2,DISABLE);
            for(int i = 1;i<=(max(Tx,Ty)-min(Tx,Ty));i++)
            {
             delay_ms(1000);
            }
            // 关闭TIM1的时钟
            TIM_Cmd(TIM1,DISABLE);
        }
        else
        {
            // 先关闭TIM1的时钟,y轴继续动            
            TIM_Cmd(TIM1,DISABLE);
            for(int i = 1;i<=(max(Tx,Ty)-min(Tx,Ty));i++)
            {
             delay_ms(1000);
            }
            // 关闭TIM2的时钟
            TIM_Cmd(TIM2,DISABLE);
        }
        
//   5.z轴缓慢下降（等待串口命令暂停）
        //////////////////////// 缓慢下降z轴
        // 打开电机TIM3的OC1
        // 使能TIM3的时钟
        TIM_Cmd(TIM3,ENABLE);
        // 旋转指定时间
        for(int i = 1;i<=Tz;i++)
        {
         delay_ms(1000);
        }
        // 关电机TIM3的OC1
        TIM_Cmd(TIM3,DISABLE);
              
//   6.扎针，注射，退针，旋转回零位
        //////////////////////// 扎针
        // 打开TIM4的OC1，关闭TIM4的OC3（困难点）
        TIM_Cmd(TIM4,ENABLE);
        TIM4_CH3_open();
        TIM4_CH1_close();
        // 旋转指定时间
        for(int i = 1;i<=T4_3;i++)
        {
         delay_ms(1000);
        }
        // 关电机
        TIM_Cmd(TIM4,DISABLE);
        //////////////////////// 注射
        // 打开TIM4的OC3，关闭TIM4的OC1（困难点）
        TIM_Cmd(TIM4,ENABLE);
        TIM4_CH1_open();
        TIM4_CH3_close();
        // 旋转指定时间
        for(int i = 1;i<=T4_1;i++)
        {
         delay_ms(1000);
        }
        // 关电机
        TIM_Cmd(TIM4,DISABLE);

//   7.按照所给正向位移，加一个安全阈值反向退回零位
        // 记录所有delay的时间
        // 全部逆向操作
        // 改变方向位
        // 开闭对应的定时器
				Back();
        REST();

//   8.复位到初始位置，准备红外检测

*/

    }
    
/*
发送使用的是中断，服务函数在usart.c
发送来的数据保存在USART_RX_BUF中
输出可以用printf或者USART_SendData
移植的话直接把usart的两个文件拷贝过去即可
调试的话要先按一下复位
*/
    
}
