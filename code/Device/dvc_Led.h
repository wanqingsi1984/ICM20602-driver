#ifndef LED_H_20170823
#define LED_H_20170823
/**
 * @file dvc_Led.h
 * @author Qiaoyy@xlnav.com
 * @date 2018-04-26
 * @brief  LED¿ØÖÆ
 * Copyright (c) 2018 XiLang Co. Ltd.
 * All rights reserved.
 */
 
/* Includes ------------------------------------------------------------------*/
#include "Global.h"
/* Exported macro ------------------------------------------------------------*/
#define LED1 0
#define LED2 1
#define LED3 2
#define LED4 3
#define LED5 4
#define LED6 5
#define LED7 6
#define LED8 7

#define LED_DEFINE_TABLE_BEGIN  LED_TypeDef g_LedTable[] = {
#define LED_DEFINE_TABLE_END {0x0000,0x0000}};uint16_t g_LedNumber = ((sizeof(g_LedTable)/sizeof(LED_TypeDef))-1);

#define LOW_LEVEL_ENABLE
/* Exported types ------------------------------------------------------------*/ 
typedef struct
{
	char LED_Info[8];
	uint16_t LED_GPIO_Pin;
	GPIO_TypeDef* LED_GPIO_Port;
}LED_TypeDef;
/* Exported constants --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
void LED_Initialize(void);
void LED_TurnOff(int nLED_Number);
void LED_TurnOffAll(void);
void LED_TurnOn(int nLED_Number);
void LED_TurnOnAll(void);
void LED_Toggle(int nLED_Number);
void LED_ToggleAll(void);
#endif
/************************ (C) COPYRIGHT XiLang CO. Ltd. *******END OF FILE*****/


