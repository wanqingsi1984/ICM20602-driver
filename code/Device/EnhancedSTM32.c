/**
 * @file
 * @author Qiaoyy@xlnav.com
 * @version
 * @date
 * @brief
 * Copyright (c) 2018 XiLang Co. Ltd.
 * All rights reserved.
 */

 /* Includes ------------------------------------------------------------------*/
#include "Global.h"
#include "EnhancedSTM32.h"
/* Private typedef -----------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/** @addtogroup LED
  * @{
  */
#ifdef LOW_LEVEL_ENABLE
#define SET_LED_GPIO(x,y) HAL_GPIO_WritePin(x,y, GPIO_PIN_RESET)
#define RESET_LED_GPIO(x,y) HAL_GPIO_WritePin(x,y, GPIO_PIN_SET)
#else
#define SET_LED_GPIO(x,y) HAL_GPIO_WritePin(x,y, GPIO_PIN_SET)
#define RESET_LED_GPIO(x,y) HAL_GPIO_WritePin(x,y, GPIO_PIN_RESET)
#endif
#define TOGGLE_LED_GPIO(x,y) HAL_GPIO_TogglePin(x,y)
/**
  * @}
  */
/* Private variables ---------------------------------------------------------*/
/** @addtogroup LED
  * @{
  */
extern LED_TypeDef g_LedTable[];
extern int g_LedNumber;
extern uint8_t SendEnFlag;
extern UART_HandleTypeDef huart3;
extern XL_UART_IT_InputStreamTypeDef g_Usart3InputStream;
/**
  * @}
  */
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
 * @brief
 * @param
 * @return
*/
void InitDmaCircularBuffer(DMA_CircularBuffer* buffer)
{
	buffer->readCounter = 0;
	buffer->writeCounter = 0;
	buffer->readCircle = 0;
	buffer->writeCircle = 0;
	buffer->length = 0;
	buffer->head = 0;
	buffer->tail = 0;
	buffer->overLapFlag = 0;
	buffer->maxLength = 0;
	buffer->protact = 0;
	buffer->lastWriteCircle = 0;
	HAL_UART_Receive_DMA(buffer->husart, buffer->buffer, buffer->maxSize);
}

/**
 * @brief
 * @param
 * @return
*/
int PopDmaCircularBuffer(uint8_t *value, DMA_CircularBuffer* buffer)
{
	volatile unsigned int writeCircle;
	volatile unsigned int head;
	// 防止写入字节数溢出并处理过零问题
	writeCircle = buffer->writeCircle;
#ifdef STM32F429xx
	head = buffer->husart->hdmarx->Instance->NDTR;
#endif
#ifdef STM32F1
	head = buffer->husart->hdmarx->Instance->CNDTR;
#endif
	if (writeCircle == buffer->lastWriteCircle
		&& (head < 1 || head > buffer->maxSize - 1))
	{
		//		printf("protact1 %d %d %d %d %d\n", buffer->maxLength, buffer->head, buffer->tail, buffer->writeCounter, buffer->readCounter);
		return 0;
	}
	if (head > 1 && head < buffer->maxSize - 1)
	{
		buffer->lastWriteCircle = writeCircle;
	}


	if ((ULONG_MAX / buffer->maxSize) < (writeCircle + 10))
	{
		if (buffer->readCounter < writeCircle * buffer->maxSize)
		{
			buffer->writeCircle -= buffer->readCounter / buffer->maxSize;
			buffer->readCounter = buffer->readCounter % buffer->maxSize;
		}
		else
		{
			buffer->writeCircle -= writeCircle;
			buffer->readCounter -= writeCircle * buffer->maxSize;
		}
	}
	// 计算头部位置和总写入字节数
	buffer->head = buffer->maxSize - head;
	buffer->writeCounter = writeCircle * buffer->maxSize + buffer->head;

	// 计算缓存区中未读出的字节数
	buffer->length = buffer->writeCounter - buffer->readCounter;

	if (buffer->writeCounter < buffer->readCounter)
	{
		//		printf("Error %d %d %d %d %d %d %d %d %d\n", buffer->maxLength, buffer->head, buffer->tail, buffer->writeCounter, buffer->readCounter, head, buffer->lastWriteCircle, buffer->writeCircle, writeCircle);
	}

	// 检查是否溢出并重定位队尾位置
	if (buffer->length > buffer->maxSize)
	{
		buffer->overLapFlag = 1;
		buffer->length = buffer->maxSize;
		buffer->readCounter = buffer->writeCounter - buffer->length;
		if (buffer->head == buffer->maxSize - 1)
		{
			buffer->tail = 0;
		}
		else
		{
			buffer->tail = buffer->head + 1;
		}
		//		printf("Overlap %d %d %d %d %d %d\n", buffer->maxLength, buffer->head, buffer->tail, buffer->writeCounter, buffer->readCounter, head);

	}

	// 执行推送动作
	if (buffer->length > 0)
	{
		if (buffer->length >= buffer->maxLength)
		{

			buffer->maxLength = buffer->length;
			//			printf("Max: %d %d %d %d %d %d\n", buffer->maxLength, buffer->head, buffer->tail, buffer->writeCounter, buffer->readCounter, head);
		}
		*value = buffer->buffer[buffer->tail];
		buffer->readCounter++;
		buffer->tail++;
		buffer->length--;
		if (buffer->tail >= buffer->maxSize)
		{
			buffer->tail = 0;
			buffer->readCircle++;
		}
		return 1;
	}
	else
	{
		*value = 0x00;
		return 0;
	}
}

/**
 * @brief
 * @param
 * @return
*/
void DmaCircularBufferIrqHandle(DMA_CircularBuffer* buffer)
{
	buffer->protact = 1;
	buffer->writeCircle++;
	buffer->protact = 0;
}

/**
 * @brief
 * @param
 * @return
*/
void SendDoubleBuffer(DoubleBuffer *pBuf, UART_HandleTypeDef* huart)
{
	if (huart->gState != HAL_UART_STATE_BUSY_TX && pBuf->m_nBufferLength[pBuf->m_nBufferNumber] > 0)
	{
		if (pBuf->m_nBufferNumber == 1)
		{
			pBuf->m_nBufferNumber = 0;
		}
		else if (pBuf->m_nBufferNumber == 0)
		{
			pBuf->m_nBufferNumber = 1;
		}
		else
		{
			pBuf->m_nBufferNumber = 0;
		}
        if(huart->Instance == USART3)
		{
			// RS485 TX
			 HAL_GPIO_WritePin(GPIOE, GPIO_PIN_15, GPIO_PIN_SET);
		//	 HAL_Delay(10);
		}
		{
	    	SendEnFlag=1;
			HAL_UART_Transmit_DMA(huart, pBuf->m_ucDataBuffer[1 - pBuf->m_nBufferNumber],pBuf->m_nBufferLength[1 - pBuf->m_nBufferNumber]);
		//	HAL_UART_Transmit_IT(huart, pBuf->m_ucDataBuffer[1 - pBuf->m_nBufferNumber],pBuf->m_nBufferLength[1 - pBuf->m_nBufferNumber]);
		}
//		if(huart->Instance == USART3)
//		{
//			// RS485 RX
//		  HAL_Delay(5);
//		  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_15, GPIO_PIN_RESET);
//		  HAL_UART_Receive_IT(&huart3, &(g_Usart3InputStream.ucTemp), 1);
//		  SendEnFlag=0;
//		}
		pBuf->m_nBufferLength[1 - pBuf->m_nBufferNumber] = 0;
	}
}

void UART_LinkInputStream_IT(XL_UART_IT_InputStreamTypeDef* stream)
{
	//assert(stream->huart != NULL);
	//assert(stream->pCircularBuffer != NULL);
	//assert(stream->pParseParam != NULL);
	HAL_UART_Receive_IT(stream->huart, &(stream->ucTemp), 1);
	ClearCircularBuffer(stream->pCircularBuffer);
	ClearParseParam(stream->pParseParam);
}

void RecieveInputStream_IT(UART_HandleTypeDef *huart, XL_UART_IT_InputStreamTypeDef* stream)
{
	if(huart->Instance == stream->huart->Instance)
	{
		PushCircularBuffer(stream->ucTemp, stream->pCircularBuffer);
		HAL_UART_Receive_IT(stream->huart, &(stream->ucTemp), 1);
	}
}

void UART_LinkInputStream_DMA(XL_UART_DMA_InputStreamTypeDef* stream)
{
	InitDmaCircularBuffer(stream->circularbuffer);
	ClearParseParam(stream->pParseParam);
}

void ParseFrame_DMA(XL_UART_DMA_InputStreamTypeDef* stream, unsigned int nMaxReadTimes)
{
	unsigned char byte;
	uint32_t times = 0;
	unsigned char flag = 0x00;
	while ((PopDmaCircularBuffer(&byte, stream->circularbuffer) > 0) && (times < nMaxReadTimes))
	{
		times++;
		flag = ParseFrameByte(byte, stream->pParseParam);
		if (flag!=0)
		{
			break;
		}
	}
}


/** @addtogroup LED
  * @{
  */
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
	for (i = 0; i<g_LedNumber; i++)
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
	for (i = 0; i<g_LedNumber; i++)
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
	for (i = 0; i<g_LedNumber; i++)
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
	for (i = 0; i<g_LedNumber; i++)
	{
		TOGGLE_LED_GPIO(g_LedTable[i].LED_GPIO_Port, g_LedTable[i].LED_GPIO_Pin);
	}
}
/**
  * @}
  */

/************************ (C) COPYRIGHT XiLang CO. Ltd. ****** END OF FILE ****/

