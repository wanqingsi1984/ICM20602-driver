/**
  ******************************************************************************
  * @file    ICM20602.c
  * @author  liun@xlnav.com
  * @version V1.0
  * @date    21-June-2018
  * @brief   This file includes the ICM20602 driver functions
  
  ******************************************************************************
  */

//#include "Debug.h"
//#include "i2c.h"
#include "dvc_ICM20602.h"
#include "spi.h"
void i2c_Mag_write(uint8_t reg,uint8_t value);
/**
  * @brief  Read bytes by SPI
  * @param  REG_Add:register address will be writen
	*         *pData: the point to the result of reading
	*				  Size:the zize of the reading bytes
  * @retval None
  */
void ICM20602_DevieceSPI_ReadBytes(uint8_t REG_Add, uint8_t *pData, uint16_t Size)
{
  REG_Add=REG_Add|0x80;
	HAL_GPIO_WritePin(CS_20602_GPIO_Port, CS_20602_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(CS_20602_GPIO_Port, CS_20602_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi1,&REG_Add,1,100);
	HAL_SPI_Receive(&hspi1,pData,Size,100);
  HAL_GPIO_WritePin(CS_20602_GPIO_Port, CS_20602_Pin, GPIO_PIN_SET);	
}
/**
  * @brief  Write bytes by SPI
  * @param  REG_Add:register address will be writen
	*        *pData: the point to the date which will be writen
	* 				Size:the size of the reading bytes
  * @retval None
  */

void ICM20602_DevieceSPI_WriteBytes(uint8_t REG_Add, uint8_t *pData, uint16_t Size)
{
	REG_Add=REG_Add&0x7F;
	HAL_GPIO_WritePin(CS_20602_GPIO_Port, CS_20602_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi1,&REG_Add,1,100);
	HAL_SPI_Transmit(&hspi1,pData,Size,100);
	HAL_GPIO_WritePin(CS_20602_GPIO_Port, CS_20602_Pin, GPIO_PIN_SET);
	
}
/**
  * @brief  Read one byte by SPI
  * @param  REG_Add:register address will be writen
	*         *pData: the point to the redult of reading
  * @retval None
  */
void ICM20602_DevieceSPI_ReadOneByte(uint8_t REG_Add, uint8_t* pData)
{
	REG_Add=REG_Add|0x80;
	HAL_GPIO_WritePin(CS_20602_GPIO_Port, CS_20602_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi1,&REG_Add,1,100);
	HAL_SPI_Receive(&hspi1,pData,1,100);
	HAL_GPIO_WritePin(CS_20602_GPIO_Port, CS_20602_Pin, GPIO_PIN_SET);
}
/**
  * @brief  Write one byte by SPI
  * @param  REG_Add:register address will be writen
	*         *pData: the point to the date which will be writen
  * @retval None
  */
void ICM20602_DevieceSPI_WriteOneByte(uint8_t REG_Add, uint8_t Data)
{
	REG_Add=REG_Add&0x7F;
	HAL_GPIO_WritePin(CS_20602_GPIO_Port, CS_20602_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi1,&REG_Add,1,100);
	HAL_SPI_Transmit(&hspi1,&Data,1,100);
	HAL_GPIO_WritePin(CS_20602_GPIO_Port, CS_20602_Pin, GPIO_PIN_SET);
}

/**
  * @brief  Initializes ICM20602
  * @param  None
  * @retval None
  */
void ICM20602_Init(void)
{
    unsigned char i2c_dev = 0;
//	  uint8_t recbuf[10];
	//  ICM2602 POWER ON
    ICM20602_DevieceSPI_WriteOneByte(PWR_MGMT_1, 0x01);
	HAL_Delay(10);
	ICM20602_DevieceSPI_WriteOneByte(PWR_MGMT_2, (PWN2_DISABLE_ACCE | PWN2_DISABLE_GYRO));
	HAL_Delay(10);
	ICM20602_DevieceSPI_WriteOneByte(PWR_MGMT_2, PWN2_ENABLE_ACCE | PWN2_ENABLE_GYRO);
	HAL_Delay(10);
	// check ID
	//while(1)
   ICM20602_DevieceSPI_ReadOneByte( WHO_AM_I, &i2c_dev);
	HAL_Delay(10);
 	if (i2c_dev == WHO_AM_I_VAL)
	{

		// GYRO config
		ICM20602_DevieceSPI_WriteOneByte(GYRO_CONFIG, (GYRO_250DPS|GYRO_DLPF_ENABLE));
		HAL_Delay(10);
		ICM20602_DevieceSPI_WriteOneByte(CONFIG,GYRO_DLPF6_NBW8HZ);
    HAL_Delay(10);
		
		// Accelerate config
		ICM20602_DevieceSPI_WriteOneByte(ACCEL_CONFIG, ACCEL_8G);
		HAL_Delay(10);
		ICM20602_DevieceSPI_WriteOneByte(ACCEL_CONFIG2,ACCEL_DLPF5_NBW16HZ);
		HAL_Delay(10);
		// Set sample rate 1000Hz
		ICM20602_DevieceSPI_WriteOneByte(SMPLRT_DIV, SAMPLE_RATE_100HZ);
		HAL_Delay(10);
   }
}
	
/**
  * @brief Get accelerometer and gyroscope datas
  * @param  accel: the value of accelerometer
  *         gyro:the value of gyroscope
  *         temp:the pointer to temperature
  * @retval None
  */
void ICM20602_READ_Imu(int16_t accel[3], int16_t gyro[3],int16_t *temp)
{ 
	unsigned char BUF[14];
    ICM20602_DevieceSPI_ReadBytes( ACCEL_XOUT_H, BUF, 14);
	//ICM20602_DevieceSPI_ReadBytes( GYRO_XOUT_H, BUF, 14);
	//ICM20602_DevieceSPI_ReadOneByte( ACCEL_XOUT_H, BUF);
	accel[0]= (BUF[0]<<8)|BUF[1];
	accel[1]= (BUF[2]<<8)|BUF[3];
	accel[2]= (BUF[4]<<8)|BUF[5];
	*temp=(BUF[6]<<8)|BUF[7];
	gyro[0] = (BUF[8]<<8)|BUF[9];	
	gyro[1] = (BUF[10]<<8)|BUF[11];
	gyro[2] = (BUF[12]<<8)|BUF[13];	
}
float acc_f[3],gyro_f[3];
	
int16_t temp_f;
/**
  * @brief  Deal with accelerometer gyroscope and temperature datas
  * @param  accel: the value of accelerometer
  *         gyro:the value of gyroscope
  *         temp:the pointer to temperature
  * @retval None
  */
void ICM20602_Imu_datadeal(int16_t accel[3], int16_t gyro[3],int16_t *temp)
{
	int i;
	for(i=0;i<3;i++)
	{
		acc_f[i]=(float)((double)accel[i]/4096*9.8);
	}
	for(i=0;i<3;i++)
	{
		gyro_f[i]=(float)((double)gyro[i]/131);
	}
	temp_f=(int16_t)((((double)(*temp-0)/326.8)+25.0)*100);
}

