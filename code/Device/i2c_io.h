/**
  ******************************************************************************
  * @file    I2C.h
  * @author  Waveshare Team
  * @version V1.0
  * @date    29-August-2014
  * @brief   This file contains all the functions prototypes for the I2C firmware 
  *          library.

  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, WAVESHARE SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  ******************************************************************************
  */


#ifndef _I2C_IO_H_
#define _I2C_IO_H_

#include "stdint.h"
#include "stdbool.h"
#include "gpio.h"

#define I2C_PORT       GPIOA
#define I2C_SCL_PIN    GPIO_PIN_1
#define I2C_SDA_PIN    GPIO_PIN_3
//#define I2C_CS_PIN     GPIO_PIN_4
//#define I2C_AD0_PIN    GPIO_PIN_6

#define I2C_SCL_Set()  HAL_GPIO_WritePin(GPIOB, I2C_SCL_PIN, GPIO_PIN_SET)
#define I2C_SCL_Clr()  HAL_GPIO_WritePin(GPIOB, I2C_SCL_PIN, GPIO_PIN_RESET)

#define I2C_SDA_Set()  HAL_GPIO_WritePin(GPIOA, I2C_SDA_PIN, GPIO_PIN_SET)
#define I2C_SDA_Clr()  HAL_GPIO_WritePin(GPIOA, I2C_SDA_PIN, GPIO_PIN_RESET)

//#define I2C_CS_Set()  HAL_GPIO_WritePin(I2C_PORT, I2C_CS_PIN, GPIO_PIN_SET)
//#define I2C_CS_Clr()  HAL_GPIO_WritePin(I2C_PORT, I2C_CS_PIN, GPIO_PIN_RESET)

//#define I2C_AD0_Set()  HAL_GPIO_WritePin(I2C_PORT, I2C_AD0_PIN, GPIO_PIN_SET)
//#define I2C_AD0_Clr()  HAL_GPIO_WritePin(I2C_PORT, I2C_AD0_PIN, GPIO_PIN_RESET)

#define I2C_SDA_Get()  HAL_GPIO_ReadPin(I2C_PORT, I2C_SDA_PIN)

#ifndef I2C_Direction_Transmitter
	#define  I2C_Direction_Transmitter      ((uint8_t)0x00)
#endif

#ifndef I2C_Direction_Receiver
	#define  I2C_Direction_Receiver         ((uint8_t)0x01)
#endif

enum
{
	I2C_SDA_IN,
	I2C_SDA_OUT
};

enum
{
	I2C_ACK,
	I2C_NACK
};

//extern void I2C_SDAMode(uint8_t Mode);
extern void I2C_Start(void);
extern void I2C_Stop(void);
extern bool I2C_WaiteForAck(void);
extern void I2C_Ack(void);
extern void I2C_NAck(void);
extern bool I2C_WriteOneBit(uint8_t DevAddr, uint8_t RegAddr, uint8_t BitNum, uint8_t Data);
extern bool I2C_WriteBits(uint8_t DevAddr, uint8_t RegAddr, uint8_t BitStart, uint8_t Length, uint8_t Data);
extern void I2C_WriteByte(uint8_t Data);
extern uint8_t I2C_ReadByte(uint8_t Ack);
extern void I2C_WriteOneByte(uint8_t DevAddr, uint8_t RegAddr, uint8_t Data);
extern uint8_t I2C_ReadOneByte(uint8_t DevAddr, uint8_t RegAddr);
extern bool I2C_WriteBuff(uint8_t DevAddr, uint8_t RegAddr, uint8_t Num, uint8_t *pBuff);
extern bool I2C_ReadBuff(uint8_t DevAddr, uint8_t RegAddr, uint8_t Num, uint8_t *pBuff);

extern void I2Cx_Init(void);
extern void Delay_us(uint16_t Time);
extern void Delay_ms(uint16_t Time);

extern void I2C_WriteOneByteSlow(uint8_t DevAddr, uint8_t RegAddr, uint8_t Data);
extern uint8_t I2C_ReadOneByteSlow(uint8_t DevAddr, uint8_t RegAddr);
extern void I2C_StartSlow(void);
extern void I2C_StopSlow(void);
extern bool I2C_WaiteForAckSlow(void);
extern void I2C_WriteByteSlow(uint8_t Data);
extern uint8_t I2C_ReadByteSlow(uint8_t Ack);
#endif

/******************* (C) COPYRIGHT 2014 Waveshare *****END OF FILE*******************/

