
/***********************************************************************************
 * 文件名  ：i2c_soft.h
 * 描述    ：使用I/0模拟I2C接口         
 * 实验平台：神舟III号
 * 库版本  ：ST3.0.0
 *
 * 作者    ：xiayufeng  xiayufeng90520@163.com 
 * 博客    ：http://hi.baidu.com/xiayufeng520
**********************************************************************************/
#ifndef __I2C_SOFT_H
#define __I2C_SOFT_H
    
//#include "stm32f10x.h"
#include "stm32f1xx_hal.h"
#include "main.h"
#define SCL_PIN        GPIO_Pin_1
#define SDA_PIN        GPIO_Pin_3
#define SCL_PORT       GPIOB
#define SDA_PORT       GPIOA
#define SCL_RCC_CLOCK  RCC_APB2Periph_GPIOB
#define SDA_RCC_CLOCK  RCC_APB2Periph_GPIOB
    
//#define SCL_H         GPIOB->BSRR = GPIO_Pin_6
//#define SCL_L         GPIOB->BRR  = GPIO_Pin_6 
//       
//#define SDA_H         GPIOB->BSRR = GPIO_Pin_7
//#define SDA_L         GPIOB->BRR  = GPIO_Pin_7


//#define SCL_read      GPIOB->IDR  & GPIO_Pin_6
//#define SDA_read      GPIOB->IDR  & GPIO_Pin_7


#define SCL_H         HAL_GPIO_WritePin(I2C_SCL_GPIO_Port, I2C_SCL_Pin, GPIO_PIN_SET)
#define SCL_L         HAL_GPIO_WritePin(I2C_SCL_GPIO_Port, I2C_SCL_Pin, GPIO_PIN_RESET)
       
#define SDA_H         HAL_GPIO_WritePin(GPIOA, I2C_SDA_Pin, GPIO_PIN_SET)
#define SDA_L         HAL_GPIO_WritePin(GPIOA, I2C_SDA_Pin, GPIO_PIN_RESET)
    
#define SCL_read      HAL_GPIO_ReadPin(GPIOB, I2C_SCL_Pin)
#define SDA_read      HAL_GPIO_ReadPin(GPIOA, I2C_SDA_Pin)
    
#define I2C_PageSize  8  //24C02每页8字节
    
void I2C_GPIO_Config(void);
uint8_t I2C_WriteByte(uint8_t SendByte, uint16_t WriteAddress, uint8_t DeviceAddress);
uint8_t I2C_BufferWrite(uint8_t* pBuffer, uint8_t length, uint16_t WriteAddress, uint8_t DeviceAddress);
void I2C_PageWrite(uint8_t* pBuffer, uint8_t length, uint16_t WriteAddress, uint8_t DeviceAddress);
uint8_t I2C_ReadByte(uint8_t* pBuffer, uint8_t length, uint16_t ReadAddress, uint8_t DeviceAddress);
void I2C_Test(void);
    
#endif