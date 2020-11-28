#include "sys.h"
#include "usart.h"	  
////////////////////////////////////////////////////////////////////////////////// 	 
//如果使用ucos,则包括下面的头文件即可.
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos 使用	  
#endif
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//串口1初始化		   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/8/18
//版本：V1.5
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved
//********************************************************************************
//V1.3修改说明 
//支持适应不同频率下的串口波特率设置.
//加入了对printf的支持
//增加了串口接收命令功能.
//修正了printf第一个字符丢失的bug
//V1.4修改说明
//1,修改串口初始化IO的bug
//2,修改了USART_RX_STA,使得串口最大接收字节数为2的14次方
//3,增加了USART_REC_LEN,用于定义串口最大允许接收的字节数(不大于2的14次方)
//4,修改了EN_USART1_RX的使能方式
//V1.5修改说明
//1,增加了对UCOSII的支持
////////////////////////////////////////////////////////////////////////////////// 	  
 static uint8_t uart1_flag_rx_ok = 0; //接收成功标志
static uint8_t uart1_rx_con=0;  //接收计数器
//static uint8_t uart2_rx_checksum; //帧头部分校验和
static uint8_t uart1_rx_buf[40];  //接收缓冲，数据内容小于等于32Byte
static uint8_t uart1_tx_buf[40];  //发送缓冲

//////////////////////////////////////////////////////////////////
//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
    USART1->DR = (u8) ch;      
	return ch;
}
#endif 

/*使用microLib的方法*/
 /* 
int fputc(int ch, FILE *f)
{
	USART_SendData(USART1, (uint8_t) ch);

	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET) {}	
   
    return ch;
}
int GetKey (void)  { 

    while (!(USART1->SR & USART_FLAG_RXNE));

    return ((int)(USART1->DR & 0x1FF));
}
*/
 
#if EN_USART1_RX   //如果使能了接收
//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误   	
u8 USART_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 USART_RX_STA=0;       //接收状态标记	  
  
void uart_init(u32 bound)
{
    //GPIO端口设置
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//使能USART1，GPIOA时钟

    //USART1_TX   GPIOA.9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
    GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.9

    //USART1_RX	  GPIOA.10初始化
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.10  

    //Usart1 NVIC 配置
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
    NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器

    //USART 初始化设置

    USART_InitStructure.USART_BaudRate = bound;//串口波特率
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
    USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
    USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

    USART_Init(USART1, &USART_InitStructure); //初始化串口1
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启串口接受中断
    USART_Cmd(USART1, ENABLE);                    //使能串口1 
}

//void USART1_IRQHandler(void)                	//串口1中断服务程序
//	{
//	u8 Res;
//#if SYSTEM_SUPPORT_OS 		//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
//	OSIntEnter();    
//#endif
//	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
//		{
//		Res =USART_ReceiveData(USART1);	//读取接收到的数据
//		
//		if((USART_RX_STA&0x8000)==0)//接收未完成
//			{
//			if(USART_RX_STA&0x4000)//接收到了0x0d
//				{
//				if(Res!=0x0a)USART_RX_STA=0;//接收错误,重新开始
//				else USART_RX_STA|=0x8000;	//接收完成了 
//				}
//			else //还没收到0X0D
//				{	
//				if(Res==0x0d)USART_RX_STA|=0x4000;
//				else
//					{
//					USART_RX_BUF[USART_RX_STA&0X3FFF]=Res ;
//					USART_RX_STA++;
//					if(USART_RX_STA>(USART_REC_LEN-1))USART_RX_STA=0;//接收数据错误,重新开始接收	  
//					}		 
//				}
//			}   		 
//     } 
//#if SYSTEM_SUPPORT_OS 	//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
//	OSIntExit();  											 
//#endif
//} 
//#endif



/*
* 帧定义：AA 55 | 0B  | 01   03 E8 FC 18 00 0A | AA
*        帧头   帧长   帧码  数据                            校验和（帧尾）
*/
void USART1_IRQHandler(void)                 //串口1中断服务程序
{
 uint8_t Res;
 
 if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断
 {
   Res =USART_ReceiveData(USART1); 
  
   if(uart1_rx_con < 3)    //==接收帧头 + 长度
   {
    if(uart1_rx_con == 0)  //接收帧头1 0xAA
    {
     if(Res == 0xAA)
     {
      uart1_rx_buf[0] = Res;
      uart1_rx_con = 1;     
     }
     else
     {
      
     }
    }else if(uart1_rx_con == 1) //接收帧头2 0x55
    {
     if(Res == 0x55)
     {
      uart1_rx_buf[1] = Res;
      uart1_rx_con = 2;
     }
     else
     {
      uart1_rx_con = 0;      
     }    
    }
    else  //接收数据长度
    {
     uart1_rx_buf[2] = Res;
     uart1_rx_con = 3;
     //uart2_rx_checksum = (0xAA+0x55) + Res; //计算校验和
    }
   }
   else    //==接收数据
   {
    if(uart1_rx_con < (uart1_rx_buf[2]-1) )
    {
     uart1_rx_buf[uart1_rx_con] = Res;
     uart1_rx_con++;
     //uart2_rx_checksum = uart2_rx_checksum + Res;     
    }
    else    //判断最后1位
    {
     //数据校验
     if( Res == 0xAA )  //校验正确
     { 
      //此处进行数据解析
      uart1_flag_rx_ok = 1;
      
        //printf("U2 %2x %2x \r\n",uart2_rx_buf[2],uart2_rx_buf[3]);
      
      //接收完成，恢复初始状态
      uart1_rx_con = 0;     
     } 
     
    }
   }
        USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
 } 
}/**
  * @简  述  获取接收的数据
  * @参  数  *pbuf：接收数据指针,第1个字节为帧编码，后面为数据
  * @返回值  0-无数据接收，other-需要读取的数据字节个数
  */
uint8_t AX_UART_PI_GetRxData(uint8_t *pbuf)
{
 uint8_t cnt,i;
 
 if(uart1_flag_rx_ok != 0)
 {
  cnt = uart1_rx_buf[2]-4;
  
  for(i=0; i<cnt; i++)
  {
   *(pbuf+i) = uart1_rx_buf[3+i];
  }
  
  uart1_flag_rx_ok = 0;
  return cnt;
 }
 else
 {
  return 0;
 } 
}
#endif 
