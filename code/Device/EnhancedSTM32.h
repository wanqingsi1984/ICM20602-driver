#ifndef STM32_H_201803151748
#define STM32_H_201803151748
/**
 * @file 
 * @author Qiaoyy@xlnav.com
 * @date 
 * @brief  
 * Copyright (c) 2018 XiLang Co. Ltd.
 * All rights reserved.
 */
 
/* Includes ------------------------------------------------------------------*/
#include "Global.h"
#include "ParseFrame.h"
/* Exported types ------------------------------------------------------------*/ 
typedef struct 
{
	volatile unsigned int readCounter;
	volatile unsigned int writeCounter;
	volatile unsigned int readCircle;
	volatile unsigned int writeCircle;
	volatile unsigned int lastWriteCircle;
	volatile unsigned int length;
	volatile unsigned int maxSize;
	volatile unsigned int head;
	volatile unsigned int tail;
	volatile unsigned int maxLength;
	unsigned char* buffer;
	UART_HandleTypeDef *husart;
	volatile unsigned char overLapFlag;
	volatile unsigned char protact;
} DMA_CircularBuffer;

typedef struct
{
	unsigned char ucTemp;
	UART_HandleTypeDef * huart;
	CircularBuffer *pCircularBuffer;
	ParseParam *pParseParam;
} XL_UART_IT_InputStreamTypeDef;

typedef struct
{
	DMA_CircularBuffer *circularbuffer;
	ParseParam *pParseParam;
} XL_UART_DMA_InputStreamTypeDef;

typedef struct
{
	UART_HandleTypeDef * huart;
	DoubleBuffer *doublebuffer;
} XL_UART_DMA_OutputStreamTypeDef;

/** @defgroup LED LED¿ØÖÆ
  * @{
  */
/** 
  * @brief LED ¿ØÖÆ¾ä±ú
  */
typedef struct
{
	char LED_Info[8];
	uint16_t LED_GPIO_Pin;
	GPIO_TypeDef* LED_GPIO_Port;
}LED_TypeDef;
/**
  * @}
  */ 
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/** @addtogroup LED
* @{
*/
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
/**
* @}
*/
/* Exported functions --------------------------------------------------------*/
void LED_Initialize(void);
void LED_TurnOff(int nLED_Number);
void LED_TurnOffAll(void);
void LED_TurnOn(int nLED_Number);
void LED_TurnOnAll(void);
void LED_Toggle(int nLED_Number);
void LED_ToggleAll(void);

int XL_PopDmaCircularBuffer(uint8_t *value, DMA_CircularBuffer* buffer);
void XL_DmaCircularBufferIrqHandle(DMA_CircularBuffer* buffer);
void XL_InitDmaCircularBuffer(DMA_CircularBuffer* buffer);
void DmaCircularBufferIrqHandle(DMA_CircularBuffer* buffer);

void SendDoubleBuffer(DoubleBuffer *pBuf, UART_HandleTypeDef* huart);
void UART_LinkInputStream_IT(XL_UART_IT_InputStreamTypeDef* stream);
void UART_LinkInputStream_DMA(XL_UART_DMA_InputStreamTypeDef* stream);
void RecieveInputStream_IT(UART_HandleTypeDef *huart, XL_UART_IT_InputStreamTypeDef* stream);
void ParseFrame_DMA(XL_UART_DMA_InputStreamTypeDef* stream, unsigned int nMaxReadTimes);

#endif
/************************ (C) COPYRIGHT XiLang CO. Ltd. *******END OF FILE*****/

