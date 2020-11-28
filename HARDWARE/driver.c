// #include "driver.h"

//////////////////////////////////////////////////////////////////////////////////	 
/*
    Í¨ÓÃ¶¨Ê±Æ÷TIMx¹²ÓĞ4¸ö£¬±àºÅ2345
    ¸ß¼¶¶¨Ê±Æ÷TIM1ºÍTIM8
    ÕâÀï²ÉÓÃTIM1234¸÷×Ô4¸öOCÍ¨µÀ£¬Ò»¹²16¸öOCÍ¨µÀ
    ¿ÉÒÔÉú³É4¸öÆµÂÊ¶ÀÁ¢µÄ£¬16¸öÕ¼¿Õ±È¶ÀÁ¢µÄPWM²¨
    
    Ã¿Ò»¸ö¶¨Ê±Æ÷µÄPWMÆµÂÊ¿Éµ¥¶Àµ÷½Ú£¬
    ¶ÔÓ¦µÄ4¸öOCÍ¨µÀÕ¼¿Õ±È¿ÉÒÔµ÷½Ú£¬µ«4¸öOCÍ¨µÀÆµÂÊÊÇÒ»ÖÂµÄ
    
    16¸öPWM¶Ë¿Ú·Ö±ğÊÇ
    TIM1µÄPE9,PE11,PE13,PE14
    TIM2µÄPA15,PB3,PB10,PB11	
    TIM3µÄPC6~9
    TIM4µÄPD12~15

    ¶ÔÓÚ²½½øµç»ú£¬PWMÆµÂÊÖØÒª£¬¶øÕ¼¿Õ±È²»ÖØÒª
    Òò´ËÈ¡4¸ö¶¨Ê±Æ÷µÄCH4×÷Îª´ú±íÆäÆµÂÊµÄPWMÊä³ö¶Ë£º
    PE14,PB11,PC9,PD15
    
    Í¨¹ıĞŞ¸ÄarrºÍpsc¿ÉÒÔ¸Ä±äÊ±ÖÓ»ù±¾ÆµÂÊ
    Í¨¹ıTIM_SetComparex(TIMy,ccr)µ÷½ÚTIMyµÄChannelxµÄPWMÕ¼¿Õ±È
    
    PF0~7 ÓÃÓÚ¿ØÖÆ8¸öPWMµÄ·½Ïò
    PF12  ÓÃÓÚ¿ØÖÆ¼±Í££¨stop=1ÓĞĞ§£
*/						  
////////////////////////////////////////////////////////////////////////////////// 	   

// Ê¹ÄÜÉÏÊöÈ«²¿¶Ë¿Ú
//void driver_Init(void)
//{
//    GPIO_InitTypeDef  GPIO_InitStructure;

//    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF, ENABLE);
//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_12;
//    //PFPF0~7£¬PF12 ¶Ë¿ÚÅäÖÃ
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //ÍÆÍìÊä³ö
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO¿ÚËÙ¶ÈÎª50MHz
//    //ÍÆÍìÊä³ö£¬IO¿ÚËÙ¶ÈÎª50MHz
//    GPIO_Init(GPIOF, &GPIO_InitStructure);
//    //¸ù¾İÉè¶¨²ÎÊı³õÊ¼»¯PF0~7£¬PF12

//    //TIM1
//    //PE9,11,13,14    ¶Ë¿ÚÅäÖÃ
//    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_11|GPIO_Pin_13|GPIO_Pin_14;
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 		 //¸´ÓÃÍÆÍìÊä³ö
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO¿ÚËÙ¶ÈÎª50MHz
//    GPIO_Init(GPIOE, &GPIO_InitStructure);
//    
//    //TIM2
//    //PA15	    ¶Ë¿ÚÅäÖÃ
//    //PB3,10,11	    ¶Ë¿ÚÅäÖÃ
//    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB, ENABLE);
//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
//    GPIO_Init(GPIOA, &GPIO_InitStructure);
//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_10|GPIO_Pin_11;
//    GPIO_Init(GPIOB, &GPIO_InitStructure);

//    //TIM3
//    //PC6~9	    ¶Ë¿ÚÅäÖÃ
//    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9;
//    GPIO_Init(GPIOC, &GPIO_InitStructure);

//    //TIM4
//    //PD12~15	¶Ë¿ÚÅäÖÃ  
//    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
//    GPIO_Init(GPIOD, &GPIO_InitStructure);
//      
//}

#include "driver.h"//////////////////////////////////////////////////////////////////////////////////  
/*
    Í¨ÓÃ¶¨Ê±Æ÷TIMx¹²ÓĞ4¸ö£¬±àºÅ2345
    ¸ß¼¶¶¨Ê±Æ÷TIM1ºÍTIM8
    ÕâÀï²ÉÓÃTIM1234¸÷×Ô4¸öOCÍ¨µÀ£¬Ò»¹²16¸öOCÍ¨µÀ
    ¿ÉÒÔÉú³É4¸öÆµÂÊ¶ÀÁ¢µÄ£¬16¸öÕ¼¿Õ±È¶ÀÁ¢µÄPWM²¨
    
    Ã¿Ò»¸ö¶¨Ê±Æ÷µÄPWMÆµÂÊ¿Éµ¥¶Àµ÷½Ú£¬
    ¶ÔÓ¦µÄ4¸öOCÍ¨µÀÕ¼¿Õ±È¿ÉÒÔµ÷½Ú£¬µ«4¸öOCÍ¨µÀÆµÂÊÊÇÒ»ÖÂµÄ
    
    16¸öPWM¶Ë¿Ú·Ö±ğÊÇ
    TIM1µÄPE9,PE11,PE13,PE14
    TIM2µÄPA15,PB3,PB10,PB11 
    TIM3µÄPC6~9
    TIM4µÄPD12~15    ¶ÔÓÚ²½½øµç»ú£¬PWMÆµÂÊÖØÒª£¬¶øÕ¼¿Õ±È²»ÖØÒª
    Òò´ËÈ¡4¸ö¶¨Ê±Æ÷µÄCH4×÷Îª´ú±íÆäÆµÂÊµÄPWMÊä³ö¶Ë£º
    PE14,PB11,PC9,PD15
    
    Í¨¹ıĞŞ¸ÄarrºÍpsc¿ÉÒÔ¸Ä±äÊ±ÖÓ»ù±¾ÆµÂÊ
    Í¨¹ıTIM_SetComparex(TIMy,ccr)µ÷½ÚTIMyµÄChannelxµÄPWMÕ¼¿Õ±È
    
    PF0~7 ÓÃÓÚ¿ØÖÆ8¸öPWMµÄ·½Ïò
    PF12  ÓÃÓÚ¿ØÖÆ¼±Í££¨stop=1ÓĞĞ§?
*/

//////////////////////////////////////////////////////////////////////////////////     // Ê¹ÄÜÉÏÊöÈ«²¿¶Ë¿Ú
void driver_Init(void)
{
	  GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable,ENABLE);
	
    GPIO_InitTypeDef  GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7|GPIO_Pin_1|GPIO_Pin_4|GPIO_Pin_3;   //fuck!!!
    //PFPF0~7£¬PF12 ¶Ë¿ÚÅäÖÃ
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    //ÍÆÍìÊä³ö
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;   //IO¿ÚËÙ¶ÈÎª50MHz
    //ÍÆÍìÊä³ö£¬IO¿ÚËÙ¶ÈÎª50MHz
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    //¸ù¾İÉè¶¨²ÎÊı³õÊ¼»¯PF0~7£¬PF12
   
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
    //PFPF0~7£¬PF12 ¶Ë¿ÚÅäÖÃ
    //GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    //ÍÆÍìÊä³ö
    //GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;   //IO¿ÚËÙ¶ÈÎª50MHz
    //ÍÆÍìÊä³ö£¬IO¿ÚËÙ¶ÈÎª50MHz
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
    //PE9,11,13,14    ¶Ë¿ÚÅäÖÃ
    //RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    //|GPIO_Pin_11|GPIO_Pin_13|GPIO_Pin_14;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;   //¸´ÓÃÍÆÍìÊä³ö
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;   //IO¿ÚËÙ¶ÈÎª50MHz
    GPIO_Init(GPIOE, &GPIO_InitStructure);
    
    //TIM2
    //PA15     ¶Ë¿ÚÅäÖÃ
    //PB3,10,11     ¶Ë¿ÚÅäÖÃ
    //RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    //GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_10|GPIO_Pin_11;
    //GPIO_Init(GPIOB, &GPIO_InitStructure);    //TIM3
    //PC6~9     ¶Ë¿ÚÅäÖÃ
    //RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    //GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    //|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9;
    //GPIO_Init(GPIOC, &GPIO_InitStructure);  //TIM4
    //PD12~15 ¶Ë¿ÚÅäÖÃ  
    //RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_14;
    //|GPIO_Pin_13|GPIO_Pin_15;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
}
