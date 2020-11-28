#include "delay.h"
#include "sys.h"
#include "key.h"
#include "driver.h"
#include "exti.h"
#include "timi.h"
#include "timer.h"
#include "usart.h"

/*
    ���ڼ�¼����ܹ�
    
    TIM3��OC1   z��
    TIM1��OC1   x��
    TIM2��OC1   y��
    TIM4��OC1   ע��
    TIM4��OC3   ����
    
    
    �ӳ���
    1.�趨��ʼƵ��
    2.���������λ����ת�ؽڴ����ϸ�λҪ���е��壩
    3.�ȴ����ڸ���xyλ��
    4.xy���ƶ����޷�����
    5.z�Ỻ���½����ȴ�����������ͣ��
    6.�˲���ת�̶��Ƕ�
    7.���룬ע�䣬���룬��ת����λ
    8.������������λ�ƣ���һ����ȫ��ֵ�����˻���λ
    9.�ȴ����ڸ���xyλ��
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
    ���ȡ4����ʱ����CH1��Ϊ������Ƶ�ʵ�PWM����ˣ�
    #define pulse1 PCout(6) TIM3��OC1   z��
    #define pulse2 PEout(9) TIM1��OC1   x��
    #define pulse3 PAout(15)    TIM2��OC1   y��
    #define pulse4 PDout(12)    TIM4��OC1   ע��
    #define pulse5 PDout(14)    TIM4��OC3   ����
    
    TIM3��OC1   z��
    TIM1��OC1   x��
    TIM2��OC1   y��
    TIM4��OC1   ע��
    TIM4��OC3   ����
    
    ͨ���޸�arr��psc���Ըı�ʱ�ӻ���Ƶ��
    ͨ��TIM_SetComparex(TIMy,ccr)����TIMy��Channelx��PWMռ�ձ�
    
    PF0~7 ���ڿ���8��PWM�ķ���
    PF12  ���ڿ��Ƽ�ͣ��stop=1��Ч��
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
		dir1 = 0;//����
    dir2 = 1;//֧������
    dir3 = 0;//������
    dir4 = 0;
    dir5 = 0;//����
}

void Back(void)
{
		dir1 = 1;//����
    dir2 = 0;//ĩ����֧��
    dir3 = 1;//������
    dir4 = 1;
    dir5 = 1;//����
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
    
    //Ĭ����ͣ
    stop();
    
    //Ĭ��������ת
    //��������������������Ҫ����Ӧ�õ�������λ����
    Back();

    // ��������TIM1��4·PWM
    u32 PSC1=72-1;
    // ������λ0.001ms
    u32 PERIOD1=1000-1;
    // 1000��0.001ms=1msһ�����壬ÿ��1000������
    // ��Ƶ��1000Hz
    u32 HIGHTIME_1_1=800;
    u32 HIGHTIME_1_2=800;
    u32 HIGHTIME_1_3=800;
    u32 HIGHTIME_1_4=800;
 
    // ��������TIM2��4·PWM
    u32 PSC2=72-1;
    u32 PERIOD2=1000-1;
    // ��Ƶ��1000Hz
    u32 HIGHTIME_2_1=800;
    u32 HIGHTIME_2_2=800;
    u32 HIGHTIME_2_3=800;
    u32 HIGHTIME_2_4=800;

    // ��������TIM3��4·PWM
    u32 PSC3=72-1;
    u32 PERIOD3=1000-1;
    // ��Ƶ��1000HZ
    u32 HIGHTIME_3_1=800;
    u32 HIGHTIME_3_2=800;
    u32 HIGHTIME_3_3=800;
    u32 HIGHTIME_3_4=800;

    // ��������TIM4��4·PWM
    u32 PSC4=72-1;
    u32 PERIOD4=1250-1;
    // 1250��0.001ms=1.25msһ�����壬ÿ��800������
    // ��Ƶ��800Hz  (T=1.25ms)
    u32 HIGHTIME_4_1=800;
    u32 HIGHTIME_4_2=800;
    u32 HIGHTIME_4_3=800;
    u32 HIGHTIME_4_4=800;
    
/*	
    TIMx_PWM_Init(arr,psc) ������ TIMx_Init(arr,psc) �� TIMx_GPIO_Init()
    ��ʼ��PWM��PWM1ģʽ�£��ȸߵ�ƽ��͵�ƽ
    arr�������Կ���һ��PWM�����ڣ�psc������ϵͳʱ�ӵķ�Ƶϵ��
*/

    //TIM1���1
    TIM1_PWM_Init(PERIOD1,PSC1,HIGHTIME_1_1,HIGHTIME_1_2,HIGHTIME_1_3,HIGHTIME_1_4);
    //TIM2���2
    TIM2_PWM_Init(PERIOD2,PSC2,HIGHTIME_2_1,HIGHTIME_2_2,HIGHTIME_2_3,HIGHTIME_2_4);
    //TIM3���3
    TIM3_PWM_Init(PERIOD3,PSC3,HIGHTIME_3_1,HIGHTIME_3_2,HIGHTIME_3_3,HIGHTIME_3_4);
    //TIM4���4
    TIM4_PWM_Init(PERIOD4,PSC4,HIGHTIME_4_1,HIGHTIME_4_2,HIGHTIME_4_3,HIGHTIME_4_4);
    
    return;
}

//��λ����
void REST(void)
{
	    start();
	    Back();
	
	    //����
			TIM_Cmd(TIM4,ENABLE);
			delay_ms(1500);
			delay_ms(1500);
			delay_ms(1500);
			delay_ms(500);
			TIM_Cmd(TIM4,DISABLE);
			delay_ms(500);
	
	    //z��
			TIM_Cmd(TIM3,ENABLE);
			delay_ms(1500);
			delay_ms(1500);
			delay_ms(1500);
			delay_ms(1500);
			delay_ms(1500);
			delay_ms(700);
			TIM_Cmd(TIM3,DISABLE);
			delay_ms(500);
	
	    
	    //y��
			TIM_Cmd(TIM1,ENABLE);
			ENA2 = 1;
			delay_ms(1500);
	    delay_ms(1000);
			TIM_Cmd(TIM1,DISABLE);
			ENA2 = 0;
			delay_ms(500);
			
			
	    //x��
			TIM_Cmd(TIM1,ENABLE);
			ENA3 = 1;
			delay_ms(1000);
			TIM_Cmd(TIM1,DISABLE);
			ENA3 = 0;
			delay_ms(500);
			
			//װ��ֹͣ
			stop();
			delay_ms(1500);
	
	
	  /*
    //��ʼ
    //ʹ��TIM4��ʱ��
    TIM_Cmd(TIM4,ENABLE);
		//ʹ��TIM3��ʱ��
    TIM_Cmd(TIM3,ENABLE);
		//ʹ��TIM2��ʱ��
    TIM_Cmd(TIM2,ENABLE);
    //ʹ��TIM1��ʱ��
    TIM_Cmd(TIM1,ENABLE);  
    
    //��λ15s��֤���Ե���λ
    for(int i = 1;i<=15;i++)
    {
        delay_ms(1000);
    }
    
    //ֹͣ
    //ʹ��TIM1��ʱ��
    TIM_Cmd(TIM1,DISABLE);
    //ʹ��TIM2��ʱ��
    TIM_Cmd(TIM2,DISABLE);
    //ʹ��TIM3��ʱ��
    TIM_Cmd(TIM3,DISABLE);
    //ʹ��TIM4��ʱ��
    TIM_Cmd(TIM4,DISABLE);
		*/
}

#define max(a,b) ( ((a)>(b)) ? (a):(b) )
#define min(a,b) ( ((a)>(b)) ? (b):(a) )

extern int motor_start;
extern int motor_stop;

int main()
{
    //  �趨 ��ʱ����ͨ�� �ĳ�ʼƵ��
    para_init();
    
    //  �趨�������ʱ�����
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
			
		//x��
		TIM_Cmd(TIM1,ENABLE);
		ENA3 = 1;
		delay_ms(1000);
		TIM_Cmd(TIM1,DISABLE);
		ENA3 = 0;
		delay_ms(500);
		
		//y��
		TIM_Cmd(TIM1,ENABLE);
		ENA2 = 1;
		delay_ms(1500);
		delay_ms(1000);
		TIM_Cmd(TIM1,DISABLE);
		ENA2 = 0;
		delay_ms(500);
		
		//z��
		TIM_Cmd(TIM3,ENABLE);
		delay_ms(1500);
		delay_ms(1500);
		delay_ms(1500);
		delay_ms(1500);
		delay_ms(1500);
		delay_ms(700);
		TIM_Cmd(TIM3,DISABLE);
		delay_ms(500);
		
		//����
		TIM_Cmd(TIM4,ENABLE);
		delay_ms(1500);
		delay_ms(1500);
		delay_ms(1500);
		delay_ms(500);
		TIM_Cmd(TIM4,DISABLE);
		delay_ms(500);
		
		//װ��ֹͣ
		stop();
		delay_ms(1500);
		delay_ms(1500);
		
		//װ�ù�λ
		REST();
		
		
    //  PWM������ϣ���ʼ��ѭ��
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
			//y��
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

//  1.�ȴ����ڵ�������Ϣ
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
        //  ��ԭ��־λ
			delay_ms(1000);
			start();
        printf("kankan/r/n");  
//  2.���㡢������������Ҫ������ʱ��
//  ��������1000pul/r
//  С������800pul/r
//  ���ݾ�5mm
//  С�ݾ�1mm
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

//  3.���������λ����ת�ؽڴ����ϸ�λҪ���е��壩
        REST(); 
    
//  4.xy���ƶ����޷�����
        //////////////////////// ʵ��xyƽ��
        // x��(TIM2)Ȧ =��xĿ������-��ǰλ�ã�/˿���ݾ�
        // ������ = Ȧ��*ÿȦ��������
        // �����TIM1��TIM2��OC1
				// ���õ��ע����̵ķ���
				GoOn();
        // ʹ��TIM1��ʱ��
        TIM_Cmd(TIM1,ENABLE);
        // ʹ��TIM2��ʱ��
        TIM_Cmd(TIM2,ENABLE);
        // delayʱ�� = ������/����Ƶ��
        
        for(int i = 1;i<=min(Tx,Ty);i++)
        {
         delay_ms(1000);
        }
        
        // y����ͣ
        if(Tx>Ty)
        {
            // �ȹر�TIM2��ʱ��,x�������
            TIM_Cmd(TIM2,DISABLE);
            for(int i = 1;i<=(max(Tx,Ty)-min(Tx,Ty));i++)
            {
             delay_ms(1000);
            }
            // �ر�TIM1��ʱ��
            TIM_Cmd(TIM1,DISABLE);
        }
        else
        {
            // �ȹر�TIM1��ʱ��,y�������            
            TIM_Cmd(TIM1,DISABLE);
            for(int i = 1;i<=(max(Tx,Ty)-min(Tx,Ty));i++)
            {
             delay_ms(1000);
            }
            // �ر�TIM2��ʱ��
            TIM_Cmd(TIM2,DISABLE);
        }
        
//   5.z�Ỻ���½����ȴ�����������ͣ��
        //////////////////////// �����½�z��
        // �򿪵��TIM3��OC1
        // ʹ��TIM3��ʱ��
        TIM_Cmd(TIM3,ENABLE);
        // ��תָ��ʱ��
        for(int i = 1;i<=Tz;i++)
        {
         delay_ms(1000);
        }
        // �ص��TIM3��OC1
        TIM_Cmd(TIM3,DISABLE);
              
//   6.���룬ע�䣬���룬��ת����λ
        //////////////////////// ����
        // ��TIM4��OC1���ر�TIM4��OC3�����ѵ㣩
        TIM_Cmd(TIM4,ENABLE);
        TIM4_CH3_open();
        TIM4_CH1_close();
        // ��תָ��ʱ��
        for(int i = 1;i<=T4_3;i++)
        {
         delay_ms(1000);
        }
        // �ص��
        TIM_Cmd(TIM4,DISABLE);
        //////////////////////// ע��
        // ��TIM4��OC3���ر�TIM4��OC1�����ѵ㣩
        TIM_Cmd(TIM4,ENABLE);
        TIM4_CH1_open();
        TIM4_CH3_close();
        // ��תָ��ʱ��
        for(int i = 1;i<=T4_1;i++)
        {
         delay_ms(1000);
        }
        // �ص��
        TIM_Cmd(TIM4,DISABLE);

//   7.������������λ�ƣ���һ����ȫ��ֵ�����˻���λ
        // ��¼����delay��ʱ��
        // ȫ���������
        // �ı䷽��λ
        // ���ն�Ӧ�Ķ�ʱ��
				Back();
        REST();

//   8.��λ����ʼλ�ã�׼��������

*/

    }
    
/*
����ʹ�õ����жϣ���������usart.c
�����������ݱ�����USART_RX_BUF��
���������printf����USART_SendData
��ֲ�Ļ�ֱ�Ӱ�usart�������ļ�������ȥ����
���ԵĻ�Ҫ�Ȱ�һ�¸�λ
*/
    
}
