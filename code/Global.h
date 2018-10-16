#ifndef GLOBAL_H_201704091620
#define GLOBAL_H_201704091620 
/**
* @file
* @author Qiaoyy@xlnav.com
* @date
* @brief
* Copyright (c) 2018 XiLang Co. Ltd.
* All rights reserved.
*/

/* Includes ------------------------------------------------------------------*/
#include "Includes.h"

/* Exported macro ------------------------------------------------------------*/
/**
  * @brief 为双缓冲区分配静态内存
  * @param buf 双缓冲区
  * @param size 双缓冲区大小，必须为大于0的整数
  * @warning 只作为全局变量声明使用，不可在函数内部调用
  */
#define CONSTRUCT_DOUBLE_BUFFER_MEMORY_POOL(buf, size) uint8_t buf##_memory[2][size];\
                                                DoubleBuffer buf = {{buf##_memory[0], buf##_memory[1]},{0,0}, 0, size};
 /**
  * @brief 为循环缓冲区分配静态内存
  * @param buf 循环缓冲区
  * @param size 循环缓冲区大小，必须为大于0的整数
  * @warning 只作为全局变量声明使用，不可在函数内部调用
  */
#define CONSTRUCT_CIRCULAR_BUFFER_MEMORY_POOL(buf, size) uint8_t buf##_memory[size];\
                                                  CircularBuffer buf = {buf##_memory, 0, 0, 0, 0, 0, size, 0, 0};									
#define CONSTRUCT_SYNC_CIRCULAR_BUFFER_MEMORY_POOL(buf, size) unsigned char buf##_memory[size];\
														 unsigned char buf##_sync_memory[size];\
                                                  SyncCircularBuffer buf = {buf##_memory, buf##_sync_memory, 0, 0, 0, 0, 0, size, 0, 0};													  
#define PUSH_VAR_DOUBLEBUFFER(var, buf) PushBytesDoubleBuffer((uint8_t *)(&(var)), sizeof(var), (buf));

#define	BUFFER_NO_ERROR 0x00											  
#define BUFFER_ERROR_OVERLAB 0x01	
#define BUFFER_ERROR_PARAM_NO_MATCH 0x02
#define	BUFFER_ERROR_POP_MORE_THAN_PUSH 0x03		

 /**
  * @brief 角度值转为弧度值
  * @param x 角度值
  */
#define DEG2RAD(x) ((x)/180*3.14159265358979)

 /**
  * @brief 弧度值转为角度值
  * @param x 弧度值
  */
#define RAD2DEG(x) ((x)*180/3.14159265358979)
												  
#define FUNC_OK	0x00										
#define	FUNC_ERROR 0x01	 							

#define TO_STRING__(x) #x
#define TO_STRING(x) TO_STRING__(x) 
/* Exported types ------------------------------------------------------------*/
typedef enum
{
  RETURN_OK = 0U,
  RETURN_ERROR = !RETURN_OK
} ReturnState;
												  
												  
#if (!defined __STM32F7xx_H) && (!defined __STM32F4xx_H) && (!defined __STM32F1xx_H)												  
//typedef enum
//{
//  RESET = 0U,
//  SET = !RESET
//} FlagStatus, ITStatus;

//typedef enum
//{
//  DISABLE = 0U,
//  ENABLE = !DISABLE
//} FunctionalState;
#define IS_FUNCTIONAL_STATE(STATE) (((STATE) == DISABLE) || ((STATE) == ENABLE))

//typedef enum
//{
//  ERROR = 0U,
//  SUCCESS = !ERROR
//} ErrorStatus;												  
							
#define SET_BIT(REG, BIT)     ((REG) |= (BIT))

#define CLEAR_BIT(REG, BIT)   ((REG) &= ~(BIT))

#define READ_BIT(REG, BIT)    ((REG) & (BIT))

#define CLEAR_REG(REG)        ((REG) = (0x0))

#define WRITE_REG(REG, VAL)   ((REG) = (VAL))

#define READ_REG(REG)         ((REG))

#define MODIFY_REG(REG, CLEARMASK, SETMASK)  WRITE_REG((REG), (((READ_REG(REG)) & (~(CLEARMASK))) | (SETMASK)))

#define POSITION_VAL(VAL)     (__CLZ(__RBIT(VAL)))
#endif												  
/**
  * @brief  循环缓冲区
  */
typedef struct
{
	uint8_t *m_ucDataBuffer;
	volatile uint16_t m_ulPopTimes;
	volatile uint16_t m_ulPushTimes;
	volatile uint16_t m_nBufferHeadPosition;
	volatile uint16_t m_nBufferTailPosition;
	volatile uint16_t m_nBufferLength;
	volatile uint16_t m_nBufferSize;
	volatile uint8_t m_ucErrorCode;
	volatile uint8_t m_ucLock;
}CircularBuffer;

/**
  * @brief  带有同步标志的循环缓冲区
  */
typedef struct
{
	uint8_t *m_ucDataBuffer;
	uint8_t *m_ucSync;
	volatile uint16_t m_ulPopTimes;
	volatile uint16_t m_ulPushTimes;
	volatile uint16_t m_nBufferHeadPosition;
	volatile uint16_t m_nBufferTailPosition;
	volatile uint16_t m_nBufferLength;
	volatile uint16_t m_nBufferSize;
	volatile uint8_t m_ucErrorCode;
	volatile uint8_t m_ucLock;
}SyncCircularBuffer;

/**
  * @brief  双缓冲区
  */
typedef struct
{
	uint8_t *m_ucDataBuffer[2];
	volatile uint16_t m_nBufferLength[2];
	volatile uint16_t m_nBufferNumber;
	volatile uint16_t m_nBufferSize;
	volatile uint8_t m_ucErrorCode;
}DoubleBuffer;
/* Exported constants --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
/* 循环数组函数 *****************************************************************/
int PopCircularBuffer(uint8_t *ucByte, CircularBuffer* pBuf);
int PushCircularBuffer(uint8_t ucByte, CircularBuffer* pBuf);
void ClearCircularBuffer(CircularBuffer* pBuf);
/* 带有同步标志的循环数组函数 *****************************************************/
int PopSyncCircularBuffer(uint8_t *ucByte, SyncCircularBuffer* pBuf, uint8_t *ucSync);
int PushSyncCircularBuffer(uint8_t ucByte, SyncCircularBuffer* pBuf);
void ClearSyncCircularBuffer(SyncCircularBuffer* pBuf);
void MarkSyncCircularBuffer(uint8_t deviceID, SyncCircularBuffer* pBuf);
/* 双缓冲区函数 *****************************************************************/
int PopDoubleBuffer(uint8_t *ucBlock, DoubleBuffer* pBuf);
int PushDoubleBuffer(uint8_t ucByte, DoubleBuffer* pBuf);
void ClearDoubleBuffer(DoubleBuffer* pBuf);
uint16_t GetDoubleBufferLength(DoubleBuffer* pBuf);
uint8_t* GetDoubleBuffer(DoubleBuffer* pBuf);
void PushBytesDoubleBuffer(uint8_t *pBytes, uint16_t nLength, DoubleBuffer* pBuf);
void PushWordDoubleBuffer(uint16_t word, DoubleBuffer* pBuf);
#endif
/************************ (C) COPYRIGHT XiLang CO. Ltd. *******END OF FILE*****/
