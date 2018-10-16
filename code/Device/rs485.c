/**
  ******************************************************************************
  * @file    rs485.c
  * @author   
  * @version V1.0
  * @date     
  * @brief    
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>

/* USER CODE BEGIN Includes */ 
#include "stdint.h"
#include "usart.h"
#include "gpio.h"

#include "rs485.h"
#include "dvc_ICM20602.h"

/* USER CODE END Includes */

/* Variables -----------------------------------------------------------------*/

/* USER CODE BEGIN Variables */
#define CMD_REQUEST_DATA     0x01      // 请求数据 
#define CMD_RESET            0x02      // 复位

#define DATA_BUF_LEN         10        // 存储区数据长度
#define DATA_LEN             6         // 数据长度

typedef struct
{
	unsigned char CMD1;                  // 帧头1
	unsigned char CMD2;                  // 帧头2
	unsigned char LEN;                   // 数据长度
	unsigned char DataBuf[DATA_LEN];
	unsigned char CHECK;                 // 校验
	
}RS485_TypeDef;

uint16_t TxFrameCnt = 0;
//RS485_TypeDef Data_RS485;

FlagStatus Flag_Start = RESET;
FlagStatus Flag_End = RESET;
unsigned char Receive_Len = 0;

extern uint8_t Addr_RS485;      // RS485物理地址
extern IMU_Type IMU_Data[100];

typedef struct
{
	uint8_t CMD1;
	uint8_t CMD2;
	uint16_t Type;
	uint16_t Len;
	uint16_t Cnt;
	float GYRO_X;
	float GYRO_Y;
	float GYRO_Z;
	float ACCEL_X;
	float ACCEL_Y;
	float ACCEL_Z;
	uint16_t Reserved_1;
	uint16_t Reserved_2;
	uint16_t Reserved_3;
	int16_t Temperature;
	uint16_t Sync;
	uint16_t Reserved_4;
	uint8_t CheckSum;
} TxFrameDef;

typedef struct
{
	float GYRO_X;
	float GYRO_Y;
	float GYRO_Z;
	float ACCEL_X;
	float ACCEL_Y;
	float ACCEL_Z;
	uint16_t Reserved_1;
	uint16_t Reserved_2;
	uint16_t Reserved_3;
	int16_t Temperature;
	uint16_t Sync;
	uint16_t Reserved_4;
} TxFrameDefData;

IMUData_1S_AvgDef ucAverage_1SBuf;


/* USER CODE END Variables */

/* Function prototypes -------------------------------------------------------*/
unsigned char Data_Input(unsigned char *pData, unsigned char DataLen);
void Data_Analytic(unsigned char *pData, unsigned char DataLen);

/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */
/**
  * @brief  Data input tou buffer
  * @param *pIndex:
  * @param DataLen:
  *
  * @retval None
  *               
  */
unsigned char Data_Input(unsigned char *pData_IN, unsigned char DataLen)
{	
  FlagStatus Stare = SET;
	unsigned char i = 0;
	unsigned char sum = 0;
	
	RS485_TypeDef *pData;
	
//	memcpy((void*)&DataBuf, (void*)pData, DataLen);
	
	if(DataLen >= DATA_LEN)
	{
    pData = (RS485_TypeDef*)pData_IN;
		
		if((pData->CMD1 == CMD_F1)&&(pData->CMD2 == CMD_F2))
		{
			Flag_Start = SET;
			for(i = 0;i < pData->LEN; i++)
			{
				sum += pData ->DataBuf[i];
			}
			
			// Check add sum
			if(sum == pData ->DataBuf[pData->LEN])//pData ->CHECK)
			{
				Flag_Start = RESET;
				Flag_End = SET;
				
				// Cammand process
				Data_Analytic(&pData ->DataBuf[0], pData->LEN);
			}
		}
    Stare = RESET;
	}
	
	return Stare;	
}

/**
  * @brief  Data analytic
  * @param *pIndex:
  * @param DataLen:
  *
  * @retval None
  *               
  */
void Data_Store(void)
{
	uint8_t i;
	TxFrameDefData *p;
		p = (TxFrameDefData *)usart3_txbuf;	
		for(i=0;i<100;i++)
		{
			p->GYRO_X = (float)((double)IMU_Data[i].Data.GYRO_X/ 131);
			p->GYRO_Y = (float)((double)IMU_Data[i].Data.GYRO_Y / 131);
			p->GYRO_Z = (float)((double)IMU_Data[i].Data.GYRO_Z / 131);
			p->ACCEL_X = (float)((double)IMU_Data[i].Data.ACCEL_X / 4096 * 9.8);
			p->ACCEL_Y = (float)((double)IMU_Data[i].Data.ACCEL_Y / 4096 * 9.8);
			p->ACCEL_Z = (float)((double)IMU_Data[i].Data.ACCEL_Z / 4096 * 9.8);	
			p->Reserved_1 = 0x00;
			p->Reserved_2 = 0x00;
			p->Reserved_3 = 0x00;
			p->Temperature = (int16_t)((((double)(IMU_Data[i].Data.TEMP-0)/326.8)+25.0)*100);
			p->Sync = 0x00;
			p->Reserved_4 = 0x00;
			p++;
		}
		
	
}
void Data_Analytic(unsigned char *pData, unsigned char DataLen)
{
	uint8_t i,ucTmpChk;
	TxFrameDefData *p;

	if(pData[2] == Addr_RS485)
	{
		p = (TxFrameDefData *)usart3_txbuf;	
		for(i=0;i<100;i++)
		{
			p->GYRO_X = (float)((double)IMU_Data[i].Data.GYRO_X/ 131);
			p->GYRO_Y = (float)((double)IMU_Data[i].Data.GYRO_Y / 131);
			p->GYRO_Z = (float)((double)IMU_Data[i].Data.GYRO_Z / 131);
			p->ACCEL_X = (float)((double)IMU_Data[i].Data.ACCEL_X / 4096 * 9.8);
			p->ACCEL_Y = (float)((double)IMU_Data[i].Data.ACCEL_Y / 4096 * 9.8);
			p->ACCEL_Z = (float)((double)IMU_Data[i].Data.ACCEL_Z / 4096 * 9.8);	
			p->Reserved_1 = 0x00;
			p->Reserved_2 = 0x00;
			p->Reserved_3 = 0x00;
			p->Temperature = (int16_t)((((double)(IMU_Data[i].Data.TEMP-0)/326.8)+25.0)*100);
			p->Sync = 0x00;
			p->Reserved_4 = 0x00;
			p++;
		}
		
	}
}
void Data_Analytic1(unsigned char *pData, unsigned char DataLen)
{
	uint8_t i,ucTmpChk;
	TxFrameDef *p;

	if(pData[2] == Addr_RS485 && ucAverage_1SBuf.Vaild == 1)
	{
		p = (TxFrameDef *)usart3_txbuf;

		p->CMD1 = CMD_F1;
		p->CMD2 = CMD_F2;
		p->Type = (uint16_t)Addr_RS485|(1<<8);
		p->Len = 0x002A;
		p->Cnt = TxFrameCnt;
		TxFrameCnt++;
//		p->GYRO_X = (float)((double)ucAverage_1SBuf.GYRO_X / 131);
//		p->GYRO_Y = (float)((double)ucAverage_1SBuf.GYRO_Y / 131);
//		p->GYRO_Z = (float)((double)ucAverage_1SBuf.GYRO_Z / 131);
//		p->ACCEL_X = (float)((double)ucAverage_1SBuf.ACCEL_X / 2048 * 9.8);
//		p->ACCEL_Y = (float)((double)ucAverage_1SBuf.ACCEL_Y / 2048 * 9.8);
//		p->ACCEL_Z = (float)((double)ucAverage_1SBuf.ACCEL_Z / 2048 * 9.8);
		p->GYRO_X = (float)((double)ucAverage_1SBuf.GYRO_X / 131);
		p->GYRO_Y = (float)((double)ucAverage_1SBuf.GYRO_Y / 131);
		p->GYRO_Z = (float)((double)ucAverage_1SBuf.GYRO_Z / 131);
		p->ACCEL_X = (float)((double)ucAverage_1SBuf.ACCEL_X / 4096 * 9.8);
		p->ACCEL_Y = (float)((double)ucAverage_1SBuf.ACCEL_Y / 4096 * 9.8);
		p->ACCEL_Z = (float)((double)ucAverage_1SBuf.ACCEL_Z / 4096 * 9.8);	
		p->Reserved_1 = 0x00;
		p->Reserved_2 = 0x00;
		p->Reserved_3 = 0x00;
//		p->Temperature = (int16_t)((((double)(ucAverage_1SBuf.TEMP - 0) / 333.87) + 21.0) * 100);
        p->Temperature = (int16_t)((((double)(ucAverage_1SBuf.TEMP-0)/326.8)+25.0)*100);
		p->Sync = 0x00;
		p->Reserved_4 = 0x00;
		
		for(i = 0,ucTmpChk = 0;i < p->Len;i++)
		{
			ucTmpChk += usart3_txbuf[i+2];
		}
		
		p->CheckSum = ucTmpChk;

		// RS485 TX
		HAL_GPIO_WritePin(GPIOE, GPIO_PIN_15, GPIO_PIN_SET);
		
		ucAverage_1SBuf.Vaild = 0;
		SendEnFlag = 1;

		// 输出IMU数据到USART3
		HAL_UART_Transmit_IT(&huart3, usart3_txbuf, p->Len+3);
		// RS485 RX
//		HAL_Delay(1);
//		HAL_GPIO_WritePin(GPIOE, GPIO_PIN_15, GPIO_PIN_RESET);
//		SendEnFlag = 0;
//		HAL_UART_Receive_IT(&huart3, &usart3_Rx_Data, 1);
	}
}
void Data_Response(unsigned char Write_Result)
{
	uint8_t i,ucTmpChk;
    usart3_txbuf[0]=CMD_F1;
	usart3_txbuf[1]=CMD_F2;
	usart3_txbuf[2]=Addr_RS485;
	usart3_txbuf[3]=02;
	usart3_txbuf[4]=0x07;
	usart3_txbuf[5]=00;
	usart3_txbuf[6]=TxFrameCnt;
	usart3_txbuf[7]=TxFrameCnt>>8;
	usart3_txbuf[8]=Write_Result;
	TxFrameCnt++;
	
	for(i = 0,ucTmpChk = 0;i < 7;i++)
	{
		ucTmpChk += usart3_txbuf[i+2];
	}
	
	usart3_txbuf[9] = ucTmpChk;

	// RS485 TX
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_15, GPIO_PIN_SET);
	SendEnFlag = 1;

	// 输出IMU数据到USART3
	HAL_UART_Transmit_IT(&huart3, usart3_txbuf, 10);
	
}
void Data_Cal(unsigned char *pData, unsigned char DataLen)
{
	uint8_t i,ucTmpChk;
    usart3_txbuf[0]=CMD_F1;
	usart3_txbuf[1]=CMD_F2;
	usart3_txbuf[2]=Addr_RS485;
	usart3_txbuf[3]=03;
	usart3_txbuf[4]=0xC2;
	usart3_txbuf[5]=00;
	usart3_txbuf[6]=TxFrameCnt;
	usart3_txbuf[7]=TxFrameCnt>>8;
	for(i=0;i<DataLen;i++)
	{
		usart3_txbuf[8+i]=pData[i];
	}
	
	TxFrameCnt++;
	
	for(i = 0,ucTmpChk = 0;i <194;i++)
	{
		ucTmpChk += usart3_txbuf[i+2];
	}
	
	usart3_txbuf[196] = ucTmpChk;

	// RS485 TX
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_15, GPIO_PIN_SET);
	SendEnFlag = 1;

	// 输出IMU数据到USART3
	HAL_UART_Transmit_IT(&huart3, usart3_txbuf, 197);
	
}