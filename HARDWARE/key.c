#include "stm32f10x.h"
#include "key.h"
#include "sys.h" 
#include "delay.h"

//////////////////////////////////////////////////////////////////////////////////	 
//������ʼ������������Ҫ�޸ģ�
//KEY0��PE4���ã�KEY1��PE3���ã�KEY_UP��PA0����	
//ע�⹫�ò���AFIO���ã�����Ҫ����ʱ��
//////////////////////////////////////////////////////////////////////////////////  

void KEY_Init(void) //IO��ʼ��
{ 
 	GPIO_InitTypeDef GPIO_InitStructure;
 
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOE,ENABLE);//ʹ��PORTA,PORTE
    //��ʼ��KEY0��KEY1����Ӧ��ʼ��GPIOE4,3
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_4|GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //���ó���������
 	GPIO_Init(GPIOE, &GPIO_InitStructure);//��ʼ��GPIOE4,3

	//��ʼ�� WK_UP-->GPIOA.0	  ��������
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; //PA0���ó����룬Ĭ������	  
	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.0
}
