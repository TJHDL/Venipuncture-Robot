#include "delay.h"
#include "usart.h"
#include "timi.h"
/////////////////////////////////////////////////////////////////////////////////
/*
debugÈÕÖ¾£
2020/9/26
²»Í¬ÓÚ²»Í¬¶¨Ê±Æ÷£¬¸ß¼¶¶¨Ê±Æ÷TIM1/TIM8ÔÚÉèÖÃÖÐ¶ÏÊ±ÓÐÁ½ÖÖÖÐ¶ÏÄ£Ê½
TIM_UPºÍTIM_CC
ÐèÒª×¢ÒâÔÚÅäÖÃNVIC_InitStructure.NVIC_IRQChannelºÍÖÐ¶Ïº¯Êývoid TIM1_UP_IRQHandler(void)Ê±
²»ÒªÍü¼Ç£¬·ñÔò²»»á±¨´íµ«ÔËÐÐ»á³öÎÊÌâ
*/
//////////////////////////////////////////////////////////////////////////////////	 
//                          ÄÚ²¿Ê±ÖÓÖÐ¶Ï·þÎñ×Ó³ÌÐò						  
//////////////////////////////////////////////////////////////////////////////////

//Íâ²¿ÖÐ¶Ï·þÎñ³ÌÐò
void TIMI_Init(void)
{
 	NVIC_InitTypeDef NVIC_InitStructure;

    //³õÊ¼»¯TIM1ÖÐ¶Ï£¨×¢ÒâÉèÖÃ±È½ÏÌØÊâ£©
    TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE ); //
    //¸üÐÂÖÐ¶Ï
    NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;
    //±È½ÏÖÐ¶Ï
    //NVIC_InitStructure.NVIC_IRQChannel = TIM1_CC_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    //³õÊ¼»¯TIM2ÖÐ¶Ï
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE ); //
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    //³õÊ¼»¯TIM3ÖÐ¶Ï
    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE ); //
    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    //³õÊ¼»¯TIM4ÖÐ¶Ï
    TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE ); //
    NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

}

void TIM1_UP_IRQHandler(void)   //TIM1
{
    if (TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET)
    {
        TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
        //¿Éµ÷ËÙ£¬Êµ¼Ê¿Éµ÷µÄÊÇÂö³åÖÜÆÚ
        //²½½øµç»úËÙ¶ÈvÕý±ÈÓëÂö³åÆµÂÊf£¬·´±ÈÓÚÖÜÆÚT
        //ÏëÒª¼ÓËÙ¶È£¨³å»÷Á¦£©ÊÇÒ»´Îº¯Êý£¬ÔòËÙ¶ÈÊÇ¶þ´Îº¯Êý
        //ÔòÖÜÆÚTÓ¦¸Ã³Ê1/t^2±ä»¯
        //½¨Á¢²é±í£¬ÉèÖÃÒ»¸öËÙ¶ÈÉÏÏÞ£¬×î¿ªÊ¼µÄÖÜÆÚ×î³¤£¬Öð½¥±ä»¯µ½Ò»¸ö×î¶ÌµÄÖÜÆÚ£¨ËÙ¶ÈÉÏÏÞ£©
        TIM1->ARR++;
        
//      ¿Éµ÷Õ¼¿Õ±È
//      u32 capture;
//      capture = TIM_GetCapture4(TIM1); 
//      capture = capture + 10;
//      TIM_SetCompare4(TIM1, capture); 
    }
}

void TIM2_IRQHandler(void)   //TIM2
{
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
    {
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    }
}

void TIM3_IRQHandler(void)   //TIM3
{
    if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
    {
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
    }
}

void TIM4_IRQHandler(void)   //TIM4
{
    if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
    {
        TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
    }
}
