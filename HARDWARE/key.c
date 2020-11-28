#include "stm32f10x.h"
#include "key.h"
#include "sys.h" 
#include "delay.h"

//////////////////////////////////////////////////////////////////////////////////	 
//按键初始化函数（不需要修改）
//KEY0与PE4公用，KEY1与PE3公用，KEY_UP与PA0公用	
//注意公用不是AFIO复用，不需要复用时钟
//////////////////////////////////////////////////////////////////////////////////  

void KEY_Init(void) //IO初始化
{ 
 	GPIO_InitTypeDef GPIO_InitStructure;
 
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOE,ENABLE);//使能PORTA,PORTE
    //初始化KEY0和KEY1，对应初始化GPIOE4,3
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_4|GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //设置成上拉输入
 	GPIO_Init(GPIOE, &GPIO_InitStructure);//初始化GPIOE4,3

	//初始化 WK_UP-->GPIOA.0	  下拉输入
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; //PA0设置成输入，默认下拉	  
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.0
}
