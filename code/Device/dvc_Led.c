
/**
 * @file vc_Led.h
 * @author Qiaoyy@xlnav.com
 * @version v1.0
 * @date 2018-04-26
 * @brief  LED¿ØÖÆ
 * Copyright (c) 2018 XiLang Co. Ltd.
 * All rights reserved.
 */
 
/* Includes ------------------------------------------------------------------*/
#include "Global.h"
#include "dvc_Led.h"
/* Private typedef -----------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
#ifdef LOW_LEVEL_ENABLE
#define SET_LED_GPIO(x,y) HAL_GPIO_WritePin(x,y, GPIO_PIN_RESET)
#define RESET_LED_GPIO(x,y) HAL_GPIO_WritePin(x,y, GPIO_PIN_SET)
#else
#define SET_LED_GPIO(x,y) HAL_GPIO_WritePin(x,y, GPIO_PIN_SET)
#define RESET_LED_GPIO(x,y) HAL_GPIO_WritePin(x,y, GPIO_PIN_RESET)
#endif

#define TOGGLE_LED_GPIO(x,y) HAL_GPIO_TogglePin(x,y)
/* Private variables ---------------------------------------------------------*/
extern LED_TypeDef g_LedTable[];
extern int g_LedNumber;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
 * @brief  
 * @param
 * @return 
*/
void LED_Initialize(void)
{
       	  
}

/**
 * @brief  
 * @param
 * @return 
*/
int LED_GetStauts(int nLED_Number)
{
	if (nLED_Number >= 0 && nLED_Number < g_LedNumber)
	{
		return	HAL_GPIO_ReadPin(g_LedTable[nLED_Number].LED_GPIO_Port, g_LedTable[nLED_Number].LED_GPIO_Pin);
	}
	return 0;
}

/**
 * @brief  
 * @param
 * @return 
*/
void LED_TurnOn(int nLED_Number)
{
	if (nLED_Number >= 0 && nLED_Number < g_LedNumber)
	{
		SET_LED_GPIO(g_LedTable[nLED_Number].LED_GPIO_Port, g_LedTable[nLED_Number].LED_GPIO_Pin);
	}
}

/**
 * @brief  
 * @param
 * @return 
*/
void LED_TurnOnN(char *Info)
{
	int i;
	for (i=0; i<g_LedNumber; i++)
	{
		if (strcmp(Info, g_LedTable[i].LED_Info))
		{
			SET_LED_GPIO(g_LedTable[i].LED_GPIO_Port, g_LedTable[i].LED_GPIO_Pin);
		}
	}
}

/**
 * @brief  
 * @param
 * @return 
*/
void LED_TurnOnAll()
{
	int i;
	for (i=0; i<g_LedNumber; i++)
	{
		SET_LED_GPIO(g_LedTable[i].LED_GPIO_Port, g_LedTable[i].LED_GPIO_Pin);
	}
}

/**
 * @brief  
 * @param
 * @return 
*/
void LED_TurnOff(int nLED_Number)
{
	if (nLED_Number >= 0 && nLED_Number < g_LedNumber)
	{
		RESET_LED_GPIO(g_LedTable[nLED_Number].LED_GPIO_Port, g_LedTable[nLED_Number].LED_GPIO_Pin);
	}
}

/**
 * @brief  
 * @param
 * @return 
*/
void LED_TurnOffAll()
{
	int i;
	for (i=0; i<g_LedNumber; i++)
	{
		RESET_LED_GPIO(g_LedTable[i].LED_GPIO_Port, g_LedTable[i].LED_GPIO_Pin);
	}
}

/**
 * @brief  
 * @param
 * @return 
*/
void LED_Toggle(int nLED_Number)
{
	if (nLED_Number >= 0 && nLED_Number < g_LedNumber)
	{
		TOGGLE_LED_GPIO(g_LedTable[nLED_Number].LED_GPIO_Port, g_LedTable[nLED_Number].LED_GPIO_Pin);
	}
}

/**
 * @brief  
 * @param
 * @return 
*/
void LED_ToggleAll()
{
	int i;
	for (i=0; i<g_LedNumber; i++)
	{
		TOGGLE_LED_GPIO(g_LedTable[i].LED_GPIO_Port, g_LedTable[i].LED_GPIO_Pin);
	}
}

/************************ (C) COPYRIGHT XiLang CO. Ltd. ****** END OF FILE ****/
