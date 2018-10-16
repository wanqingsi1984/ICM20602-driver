//#include "Global.h"
#include "Device.h"
#include "MainEntry.h"
#include "usart.h"
#include "stdio.h"
#include "rs485.h"
#include "dvc_ICM20602.h"
#include "dvc_AT24C01.h"
#include "IMU_EEPROM.h"
#include "EnhancedSTM32.h"
#include "Message.h"
#include "msg_QSens.h"
//#include "i2c_io.h"

extern unsigned char f10ms;
extern unsigned char f10ms_2;
extern IMUData_1S_AvgDef ucAverage_1SBuf;
extern uint16_t TxFrameCnt;
extern RxStateDef mRxDataSta;
extern uint8_t Addr_RS485;
extern uint8_t RecCommandFlag;
extern DoubleBuffer g_Usart3OutputBuffer;
//extern void write_uint32(uint32_t* pdata,uint8_t len, uint16_t WriteAddress);
//extern void read_uint32(uint32_t* pdata,uint8_t len, uint16_t ReadAddress);
//extern void write_float(float* pdata,uint8_t len, uint16_t WriteAddress);
//extern void read_float(float* pdata,uint8_t len, uint16_t ReadAddress);
IMU_Type IMU_Data[100];
uint8_t I2cVal_Read[188] = {0x00};

void HeartPulse(void);

void MainEntry(void)
{
	
	int16_t accel[3],gyro[3];
	int16_t temp;
	CollectIMUDataDef IMU_CollectData;
	uint8_t ucTmpCnt = 0;
	uint8_t I2cVal_Write[188] = {0};
    uint8_t i;
	uint32_t TableType=0x01020304;
	uint32_t ProductID[2]={0x20180901,0x20180902};
	float GyroTemp0[3]={1.1415926,2.1415926,3.1415926};
	uint32_t ReadResult[3]={0};
	float ReadFloat[3]={0};
	uint8_t Write_Result,Read_CRC;
	uint8_t SendOver;
	uint16_t CommandAddr=0;
	
	
	
	
	InitDevice();
	InitMessage();
	
	IMU_CollectData.ACCEL_X = 0;
	IMU_CollectData.ACCEL_Y = 0;
	IMU_CollectData.ACCEL_Z = 0;
	IMU_CollectData.GYRO_X = 0;
	IMU_CollectData.GYRO_Y = 0;
	IMU_CollectData.GYRO_Z = 0;
	IMU_CollectData.TEMP = 0;
	
    ucAverage_1SBuf.Vaild = 0;
//	for(i=0;i<188;i++)
//	{
//		I2cVal_Write[i]=i;
//	}
//	I2cVal_Write[0]=0xFE;
//	I2C_WriteByte(I2cVal_Write[0], 0x00, 0xa0);
//	I2C_ReadByte(&I2cVal_Read[0], 1, 0x00,0xa0);
//	for(i=0;i<8;i++)
//	I2C_WriteByte(I2cVal_Write[i], i, 0xa0);

//	I2C_BufferWrite(&I2cVal_Write[0],8,0x00,0xa0);
//	I2C_PageWrite(&I2cVal_Write[0],188,0x00,0xa0);
//	for(i=0;i<8;i++)
//    I2C_ReadByte(&I2cVal_Read[i], 1, i ,0xa0);
//	I2C_ReadByte(&I2cVal_Read[0], 10, 0x00 ,0xa0);
//	write_uint32(&TableType,1,0x10);
//	write_uint32(&ProductID[0],2,0x14);
//	read_uint32(&ReadResult[0],3,0x10);
//	write_float(&GyroTemp0[0],3,0x1C);
//	read_float(&ReadFloat[0],3,0x1C);
 //  I2C_ReadByte(&I2cVal_Read[0], 188, 0x00 ,0xa0);
    while (1)
    {
		HeartPulse();
		ParseFrameUsart1();
//		ParseFrameUsart2();
		ParseFrameUsart3();
//	if(f10ms_2 == 1)
//		{
//			f10ms_2=0;
//		//	I2C_WriteOneByte(0xa0,0x01,I2cVal_Write);
//		//	I2cVal_Read=I2C_ReadOneByte(0xa0,0x01);
////		I2C_WriteByte(I2cVal_Write[0], 0x01, 0xa0);
////       	I2C_ReadByte(&I2cVal_Read[0], 1, 0x01 ,0xa0);
//		}
		if(ucAverage_1SBuf.Vaild == 1 && SendEnFlag == 0 && RecCommandFlag==1)
//		if(ucAverage_1SBuf.Vaild == 1 && SendEnFlag == 0)
		{
			//SendEnFlag=1;
			//Data_Analytic(&pBuffer[0],10);
			RecCommandFlag=0;
			CommandAddr=(1<<8)|Addr_RS485;
			HAL_Delay(2);
			for(i=0;i<100;i++)
			{
			   while(SendEnFlag == 1);
				{
				  	QSENS_MakeImuFrame2(&usart3_txbuf[i*36],36,&g_Usart3OutputBuffer,CommandAddr);
					SendDoubleBuffer(&g_Usart3OutputBuffer, &huart3);
				}
				//HAL_Delay(1);
			}
			while(SendEnFlag == 1);
			{
				CommandAddr=(9<<8)|Addr_RS485;
				QSENS_MakeImuFrame2(&SendOver,1,&g_Usart3OutputBuffer,CommandAddr);
				SendDoubleBuffer(&g_Usart3OutputBuffer, &huart3);
			}
			ucAverage_1SBuf.Vaild = 0;
		}
		else if(SendEnFlag == 0 && RecCommandFlag==1)
		{
			RecCommandFlag=0;
			SendOver=1;
			while(SendEnFlag == 1);
			{
				CommandAddr=(9<<8)|Addr_RS485;
				QSENS_MakeImuFrame2(&SendOver,1,&g_Usart3OutputBuffer,CommandAddr);
				SendDoubleBuffer(&g_Usart3OutputBuffer, &huart3);
			}
			
		}
  	if (f10ms==1)
		{

//			f10ms = 0;
//		    ICM20602_READ_Imu(&accel[0],&gyro[0],&temp);
//		 //   ICM20602_Imu_datadeal(&accel[0],&gyro[0],&temp);
//			
//			IMU_Data[ucTmpCnt].Data.ACCEL_X=accel[0];
//			IMU_Data[ucTmpCnt].Data.ACCEL_Y=accel[1];
//			IMU_Data[ucTmpCnt].Data.ACCEL_Z=accel[2];
//			IMU_Data[ucTmpCnt].Data.GYRO_X=gyro[0];
//			IMU_Data[ucTmpCnt].Data.GYRO_Y=gyro[1]; 
//			IMU_Data[ucTmpCnt].Data.GYRO_Z=gyro[2];
//			IMU_Data[ucTmpCnt].Data.TEMP=temp;


//			ucTmpCnt++;

//		

//			if(ucTmpCnt == 100)
//			{
//				ucTmpCnt = 0;
//				Data_Store();
//			    ucAverage_1SBuf.Vaild = 1;


//			}

		}
    }
}

void HeartPulse()
{
	static long counter = 0;
	counter++;
	if (counter>100000)
	{
		counter = 0;
		LED_Toggle(LED1);
	}
}
