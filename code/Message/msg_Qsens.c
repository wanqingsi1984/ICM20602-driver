#include "msg_Qsens.h"
#include "ParseFrame.h"
#include "Message.h"
#include "main.h"
#include "rs485.h"
#include "dvc_AT24C01.h"

#if defined(MSG_QSENS_SEND_EPH) || defined(MSG_QSENS_RECIEVE_EPH)
GNSS_EphemDATA g_Eph;
#endif
#if defined(MSG_QSENS_SEND_GPS) || defined(MSG_QSENS_RECIEVE_GPS)
MSG_QSENS_DatagramGpsTypeDef g_GpsData;
#endif
#if defined(MSG_QSENS_SEND_NAV) || defined(MSG_QSENS_RECIEVE_NAV)
MSG_QSENS_DatagramNavTypeDef g_stNavResult;
#endif
#if defined(MSG_QSENS_SEND_OBS) || defined(MSG_QSENS_RECIEVE_OBS)
GNSS_EphemDATA g_Eph;
#endif
#if defined(MSG_QSENS_SEND_IMU) || defined(MSG_QSENS_RECIEVE_IMU) || defined(MSG_QSENS_SEND_IMU_FULL) || defined(MSG_QSENS_RECIEVE_IMU_FULL)
MSG_QSENS_DatagramImuTypeDef g_Imu;
#endif
#if defined(MSG_QSENS_SEND_MAG) || defined(MSG_QSENS_RECIEVE_MAG)
MSG_QSENS_DatagramMagTypeDef g_Eph;
#endif
#if defined(MSG_QSENS_SEND_ALT) || defined(MSG_QSENS_RECIEVE_ALT)
MSG_QSENS_DatagramAltTypeDef g_Eph;
#endif
#if defined(MSG_QSENS_SEND_DR) || defined(MSG_QSENS_RECIEVE_DR)
MSG_QSENS_DatagramDrTypeDef g_Dr;
#endif
FrameConstructor frameConstr;
extern DoubleBuffer g_Usart1OutputBuffer;
extern DoubleBuffer g_Usart2OutputBuffer;
extern DoubleBuffer g_Usart3OutputBuffer;
#include "EnhancedSTM32.h"
extern XL_UART_DMA_OutputStreamTypeDef g_Usart1OutputStream;
extern XL_UART_DMA_OutputStreamTypeDef g_Usart2OutputStream;
extern XL_UART_DMA_OutputStreamTypeDef g_Usart3OutputStream;
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart3;
extern uint8_t SendEnFlag;
extern uint8_t RecCommandFlag;
extern uint8_t Addr_RS485;
extern uint8_t I2cVal_Read[188];
extern uint8_t usart3_txbuf[3600];
extern IMUData_1S_AvgDef ucAverage_1SBuf;
//extern uint8_t SendFlag;
int QSENS_BinInit(unsigned char* pBuffer, int nLength)
{
#if defined(MSG_QSENS_SEND_EPH) || defined(MSG_QSENS_RECIEVE_EPH)
	memset(&g_Eph, 0x00, sizeof(GNSS_EphemDATA));
#endif
#if defined(MSG_QSENS_SEND_GPS) || defined(MSG_QSENS_RECIEVE_GPS)
	memset(&g_GpsData, 0x00, sizeof(MSG_QSENS_DatagramGpsTypeDef));
#endif
#if defined(MSG_QSENS_SEND_NAV) || defined(MSG_QSENS_RECIEVE_NAV)
	memset(&g_stNavResult, 0x00, sizeof(MSG_QSENS_DatagramNavTypeDef));
#endif
#if defined(MSG_QSENS_SEND_OBS) || defined(MSG_QSENS_RECIEVE_OBS)
	memset(&g_Eph, 0x00, sizeof(GNSS_EphemDATA));
#endif
#if defined(MSG_QSENS_SEND_IMU) || defined(MSG_QSENS_RECIEVE_IMU) || defined(MSG_QSENS_SEND_IMU_FULL) || defined(MSG_QSENS_RECIEVE_IMU_FULL)
	memset(&g_Imu, 0x00, sizeof(MSG_QSENS_DatagramImuTypeDef));
#endif
#if defined(MSG_QSENS_SEND_MAG) || defined(MSG_QSENS_RECIEVE_MAG)
	memset(&g_Eph, 0x00, sizeof(MSG_QSENS_DatagramMagTypeDef));
#endif
#if defined(MSG_QSENS_SEND_ALT) || defined(MSG_QSENS_RECIEVE_ALT)
	memset(&g_Eph, 0x00, sizeof(MSG_QSENS_DatagramAltTypeDef));
#endif
#if defined(MSG_QSENS_SEND_DR) || defined(MSG_QSENS_RECIEVE_DR)
	memset(&g_Dr, 0x00, sizeof(MSG_QSENS_DatagramDrTypeDef));
#endif
	memset(&frameConstr, 0x00, sizeof(FrameConstructor));
    return 1;
}
extern ParseParam g_Usart3Parse;

int QSENS_BinFrameLength(unsigned char* pBuffer, int nLength)
{
	unsigned short nFrameLength = 0x00;
	memcpy(&nFrameLength, &pBuffer[4], 2);
	return nFrameLength+3;
}

int QSENS_BinFrameCheck(unsigned char* pBuffer, int nLength)
{
    int i;
    unsigned char crc = 0x00;
	for (i=2;i<nLength-1;i++)
	{
		crc += pBuffer[i];
	}
	if (crc==pBuffer[nLength-1])
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
int QSENS_BinFrameParse(unsigned char* pBuffer, int nLength)
{
   // unsigned short FrameID;
	uint8_t CommandID;
	uint8_t AddrID;
	uint16_t CommandAddr;
	uint8_t Read_CRC,Write_Result,SendOver;
	int i;
	int nCounter;
	static unsigned short usFrameCounter = 0;

    memcpy(&CommandAddr, &pBuffer[2], 2);
	
	memcpy(&CommandID, &pBuffer[3], 1);
	
	memcpy(&AddrID, &pBuffer[2], 1);
	nCounter = 6;
    switch (CommandID)
    {
	case 0x04://写入标定参数（usart3接收，通过usart3发送）
		if(AddrID==Addr_RS485 && SendEnFlag == 0)
		{
			SendEnFlag=1;
			I2C_PageWrite(&pBuffer[8],188,0x00,0xa0);
			I2C_ReadByte(&Read_CRC, 1, 0xBB ,0xa0);
			if(Read_CRC==pBuffer[195])
			{
				Write_Result=1;
			}
			else
			{
				Write_Result=0;
			}
			CommandAddr=(2<<8)|Addr_RS485;
			QSENS_MakeImuFrame2(&Write_Result,1,&g_Usart3OutputBuffer,CommandAddr);
			SendDoubleBuffer(g_Usart3OutputStream.doublebuffer, &huart3);
		}
		break;
	case 0x05://读标定参数（usart3接收，通过usart3发送）
		if(AddrID==Addr_RS485 && SendEnFlag == 0)
		{
			SendEnFlag=1;
			
			I2C_ReadByte(&I2cVal_Read[0], 188, 0x00 ,0xa0);
			
			CommandAddr=(3<<8)|Addr_RS485;
			QSENS_MakeImuFrame2(&I2cVal_Read[0],188,&g_Usart3OutputBuffer,CommandAddr);
			SendDoubleBuffer(g_Usart3OutputStream.doublebuffer, &huart3);;
		}
		break;
	case 0x06://读取IMU数据
		//if(AddrID==Addr_RS485 && SendEnFlag == 0 && ucAverage_1SBuf.Vaild == 1)
		if(AddrID==Addr_RS485)
		{
			RecCommandFlag=1;
//			CommandAddr=(1<<8)|Addr_RS485;
//			HAL_Delay(10);
//			for(i=0;i<100;i++)
//			{
//			   if(SendEnFlag == 0)
//				{
//					QSENS_MakeImuFrame2(&usart3_txbuf[i*36],36,&g_Usart3OutputBuffer,CommandAddr);
//					SendDoubleBuffer(g_Usart3OutputStream.doublebuffer, &huart3);
//				}
//				HAL_Delay(1);
//			}
//			CommandAddr=(9<<8)|Addr_RS485;
//			QSENS_MakeImuFrame2(&SendOver,1,&g_Usart3OutputBuffer,CommandAddr);
//			SendDoubleBuffer(g_Usart3OutputStream.doublebuffer, &huart3);
//			ucAverage_1SBuf.Vaild = 0;
		}
		break;
	default:
		break;
	}
	return 1;
}
int QSENS_BinFrameParse1(unsigned char* pBuffer, int nLength)
{
    unsigned short FrameID;
	int nCounter;

    memcpy(&FrameID, &pBuffer[2], 2);
	nCounter = 6;
    switch (FrameID)
    {
#ifdef MSG_QSENS_RECIEVE_CMD
	case MSG_QSENS_DATAGRAM_CMD:
		if(GetULongContinue((unsigned char*)&pBuffer[nCounter], &nCounter) == 0x00000000)
		{
			RestartAndLoadBin();
		}
		break;
#endif
#ifdef MSG_QSENS_RECIEVE_NAV 
	case MSG_QSENS_DATAGRAM_NAV:
		
		break;
#endif
#ifdef MSG_QSENS_RECIEVE_IMU_FULL
	case MSG_QSENS_DATAGRAM_IMU_FULL:
		g_Imu.counter.current = GetUShortContinue((unsigned char*)&pBuffer[nCounter], &nCounter);
		g_Imu.data.ulFullGyroXCurrent = GetULongContinue((unsigned char*)&pBuffer[nCounter], &nCounter);
		g_Imu.data.ulFullGyroYCurrent = GetULongContinue((unsigned char*)&pBuffer[nCounter], &nCounter);
		g_Imu.data.ulFullGyroZCurrent = GetULongContinue((unsigned char*)&pBuffer[nCounter], &nCounter);
		g_Imu.data.ulFullAccXCurrent = GetULongContinue((unsigned char*)&pBuffer[nCounter], &nCounter);
		g_Imu.data.ulFullAccYCurrent = GetULongContinue((unsigned char*)&pBuffer[nCounter], &nCounter);
		g_Imu.data.ulFullAccZCurrent = GetULongContinue((unsigned char*)&pBuffer[nCounter], &nCounter);
		g_Imu.data.TempretureGyroX = GetShortContinue((unsigned char*)&pBuffer[nCounter], &nCounter);
		if (g_Imu.counter.first == 1)
		{
				// 计算陀螺加表增量并赋值
			g_Imu.data.ulGyroX = g_Imu.data.ulFullGyroXCurrent - g_Imu.data.ulFullGyroXLast;
			g_Imu.data.ulGyroY = g_Imu.data.ulFullGyroYCurrent - g_Imu.data.ulFullGyroYLast;
			g_Imu.data.ulGyroZ = g_Imu.data.ulFullGyroXCurrent - g_Imu.data.ulFullGyroZLast;
			g_Imu.data.ulAccX = g_Imu.data.ulFullAccXCurrent - g_Imu.data.ulFullAccXLast;
			g_Imu.data.ulAccY = g_Imu.data.ulFullAccYCurrent - g_Imu.data.ulFullAccYLast;
			g_Imu.data.ulAccZ = g_Imu.data.ulFullAccZCurrent - g_Imu.data.ulFullAccZLast;
			g_Imu.data.ulFullGyroXCurrent = g_Imu.data.ulFullGyroXLast;
			g_Imu.data.ulFullGyroYCurrent = g_Imu.data.ulFullGyroYLast;
			g_Imu.data.ulFullGyroXCurrent = g_Imu.data.ulFullGyroZLast;
			g_Imu.data.ulFullAccXCurrent = g_Imu.data.ulFullAccXLast;
			g_Imu.data.ulFullAccYCurrent = g_Imu.data.ulFullAccYLast;
			g_Imu.data.ulFullAccZCurrent = g_Imu.data.ulFullAccZLast;
				// 收到有效数据，置标志位
			g_Imu.receive.Status = MESSAGE_STATUS_AVALABEL;
		}
		else
		{
			g_Imu.data.ulFullGyroXCurrent = g_Imu.data.ulFullGyroXLast;
			g_Imu.data.ulFullGyroYCurrent = g_Imu.data.ulFullGyroYLast;
			g_Imu.data.ulFullGyroXCurrent = g_Imu.data.ulFullGyroZLast;
			g_Imu.data.ulFullAccXCurrent = g_Imu.data.ulFullAccXLast;
			g_Imu.data.ulFullAccYCurrent = g_Imu.data.ulFullAccYLast;
			g_Imu.data.ulFullAccZCurrent = g_Imu.data.ulFullAccZLast;
			g_Imu.receive.Status = MESSAGE_STATUS_INIT;
		}
		CalculateDeltaCounterUint16(&g_Imu.counter);
		break;
#endif
#ifdef MSG_QSENS_RECIEVE_IMU
	case MSG_QSENS_DATAGRAM_IMU:
		g_Imu.counter.current = GetUShortContinue((unsigned char*)&pBuffer[nCounter], &nCounter);
		CalculateDeltaCounterUint16(&g_Imu.counter);
		g_Imu.data.lGyroX = GetLongContinue((unsigned char*)&pBuffer[nCounter], &nCounter);
		g_Imu.data.lGyroY = GetLongContinue((unsigned char*)&pBuffer[nCounter], &nCounter);
		g_Imu.data.lGyroZ = GetLongContinue((unsigned char*)&pBuffer[nCounter], &nCounter);
		g_Imu.data.lAccX = GetLongContinue((unsigned char*)&pBuffer[nCounter], &nCounter);
		g_Imu.data.lAccY = GetLongContinue((unsigned char*)&pBuffer[nCounter], &nCounter);
		g_Imu.data.lAccZ = GetLongContinue((unsigned char*)&pBuffer[nCounter], &nCounter);
		g_Imu.data.sTempretureGyroX = GetShortContinue((unsigned char*)&pBuffer[nCounter], &nCounter);
		g_Imu.data.sTempretureGyroY = GetShortContinue((unsigned char*)&pBuffer[nCounter], &nCounter);
		g_Imu.data.sTempretureGyroZ = GetShortContinue((unsigned char*)&pBuffer[nCounter], &nCounter);
		g_Imu.data.sTempretureAccX = GetShortContinue((unsigned char*)&pBuffer[nCounter], &nCounter);
		g_Imu.data.sTempretureAccZ = GetShortContinue((unsigned char*)&pBuffer[nCounter], &nCounter);
		g_Imu.data.sTempretureAccY = GetShortContinue((unsigned char*)&pBuffer[nCounter], &nCounter);

		if (g_Usart3Parse.flagSync==0x01)
		{
			g_Imu.data.sTempretureAccY = 0x01;
			g_Usart3Parse.flagSync = 0;
		}
		else
		{
			g_Imu.data.sTempretureAccY = 0x00;
		}	
		QSENS_MakeImuFrame(&g_Imu, &g_Usart3OutputBuffer);
		g_Imu.receive.Status = MESSAGE_STATUS_AVALABEL;
		break;
#endif
#ifdef MSG_QSENS_RECIEVE_MAG 
	case MSG_QSENS_DATAGRAM_MAG:
		break;
#endif
#ifdef MSG_QSENS_RECIEVE_ALT 
	case MSG_QSENS_DATAGRAM_ALT:
		break;
#endif
#ifdef MSG_QSENS_RECIEVE_DR 
	case MSG_QSENS_DATAGRAM_DR:
		g_Dr.counter.current = GetUShortContinue((unsigned char*)&pBuffer[nCounter], &nCounter);
		CalculateDeltaCounterUint16(&g_Dr.counter);
		g_Dr.data.Vx = GetFloatContinue((unsigned char*)&pBuffer[nCounter], &nCounter);
		g_Dr.data.Vy = GetFloatContinue((unsigned char*)&pBuffer[nCounter], &nCounter);
		g_Dr.data.Temp = GetULongContinue((unsigned char*)&pBuffer[nCounter], &nCounter);
		g_Dr.receive.Status = MESSAGE_STATUS_AVALABEL;
		break;
#endif

#ifdef MSG_QSENS_RECIEVE_GPS
	case MSG_QSENS_DATAGRAM_GPS:
		g_GpsData.counter.current = GetUShortContinue((unsigned char*)&pBuffer[nCounter], &nCounter);
		CalculateDeltaCounterUint16(&g_GpsData.counter);
		g_GpsData.data.ucGGA_Status = pBuffer[nCounter];
		nCounter++;
		g_GpsData.data.ucSvNum = pBuffer[nCounter];
		nCounter++;
		g_GpsData.data.ucBaseAge = pBuffer[nCounter];
		nCounter++;
		g_GpsData.data.ucGPGGA_UpdateFlag = pBuffer[nCounter];
		nCounter++;
		g_GpsData.data.ucGPRMC_UpdateFlag = pBuffer[nCounter];
		nCounter++;
		g_GpsData.data.ucPSAT_UpdateFlag = pBuffer[nCounter];
		nCounter++;
		g_GpsData.data.ucYear = pBuffer[nCounter];
		nCounter++;
		g_GpsData.data.ucMonth = pBuffer[nCounter];
		nCounter++;
		g_GpsData.data.ucDay = pBuffer[nCounter];
		nCounter++;
		g_GpsData.data.ucHour = pBuffer[nCounter];
		nCounter++;
		g_GpsData.data.ucMinite = pBuffer[nCounter];
		nCounter++;
		g_GpsData.data.fSecond = GetFloatContinue((unsigned char*)&pBuffer[nCounter], &nCounter);
		g_GpsData.data.lLongitude_GPS = GetLongContinue((unsigned char*)&pBuffer[nCounter], &nCounter);
		g_GpsData.data.lLatitude_GPS = GetLongContinue((unsigned char*)&pBuffer[nCounter], &nCounter);
		g_GpsData.data.fHightSea_GPS = GetFloatContinue((unsigned char*)&pBuffer[nCounter], &nCounter);
		g_GpsData.data.fHightGeo_GPS = GetFloatContinue((unsigned char*)&pBuffer[nCounter], &nCounter);
		g_GpsData.data.fCourse = GetFloatContinue((unsigned char*)&pBuffer[nCounter], &nCounter);
		g_GpsData.data.fRate = GetFloatContinue((unsigned char*)&pBuffer[nCounter], &nCounter);
		g_GpsData.data.fMagYaw = GetFloatContinue((unsigned char*)&pBuffer[nCounter], &nCounter);
		g_GpsData.data.fDoubleYaw = GetFloatContinue((unsigned char*)&pBuffer[nCounter], &nCounter);
		g_GpsData.data.fHDOP = GetFloatContinue((unsigned char*)&pBuffer[nCounter], &nCounter);
		g_GpsData.data.us1PPS_Counter = GetUShortContinue((unsigned char*)&pBuffer[nCounter], &nCounter);
		g_GpsData.data.usGPGGA_Counter = GetUShortContinue((unsigned char*)&pBuffer[nCounter], &nCounter);
		g_GpsData.data.usGPRMC_Counter = GetUShortContinue((unsigned char*)&pBuffer[nCounter], &nCounter);
		g_GpsData.data.usPSAT_Counter = GetUShortContinue((unsigned char*)&pBuffer[nCounter], &nCounter);
		g_GpsData.data.usRecieverStatus = GetUShortContinue((unsigned char*)&pBuffer[nCounter], &nCounter);
        // ADD NAV Process Code Here
		g_GpsData.receive.Status = MESSAGE_STATUS_AVALABEL;
		break;
#endif

#ifdef MSG_QSENS_RECIEVE_EPH
	case MSG_QSENS_DATAGRAM_EPH:
		break;	
#endif

#ifdef MSG_QSENS_RECIEVE_OBS 
	case MSG_QSENS_DATAGRAM_OBS:
		break;
#endif

	default:
		break;
	}
	return 1;
}

#ifdef MSG_QSENS_SEND_GPS
MSG_QSENS_DatagramGpsTypeDef* QSENS_GetGpsData()
{
	return &g_GpsData;
}

void QSENS_MakeGpsFrame(MSG_QSENS_DatagramGpsTypeDef* gps, DoubleBuffer *buf)
{
	static unsigned short usFrameCounter = 0;
	FrameBegin(buf, &frameConstr);
	PushDoubleBuffer(0xEA, buf);
	PushDoubleBuffer(0x51, buf);
	CheckSectionBegin(buf, &frameConstr);
	LengthSectionBegin(buf, &frameConstr);
	PushWordDoubleBuffer(MSG_QSENS_DATAGRAM_GPS, buf);
	MarkLengthPosition(buf, &frameConstr, 2);
	PUSH_VAR_DOUBLEBUFFER(usFrameCounter, buf);
	usFrameCounter++;
	PUSH_VAR_DOUBLEBUFFER(gps->data.ucGGA_Status, buf);
	PUSH_VAR_DOUBLEBUFFER(gps->data.ucSvNum, buf);
	PUSH_VAR_DOUBLEBUFFER(gps->data.ucBaseAge, buf);
	PUSH_VAR_DOUBLEBUFFER(gps->data.ucGPGGA_UpdateFlag, buf);
	PUSH_VAR_DOUBLEBUFFER(gps->data.ucGPRMC_UpdateFlag, buf);
	PUSH_VAR_DOUBLEBUFFER(gps->data.ucPSAT_UpdateFlag, buf);
    PUSH_VAR_DOUBLEBUFFER(gps->data.ucYear, buf);
    PUSH_VAR_DOUBLEBUFFER(gps->data.ucMonth, buf);
	PUSH_VAR_DOUBLEBUFFER(gps->data.ucDay, buf);
	PUSH_VAR_DOUBLEBUFFER(gps->data.ucHour, buf);
	PUSH_VAR_DOUBLEBUFFER(gps->data.ucMinite, buf);
	PUSH_VAR_DOUBLEBUFFER(gps->data.fSecond, buf);
	PUSH_VAR_DOUBLEBUFFER(gps->data.lLongitude_GPS, buf);
	PUSH_VAR_DOUBLEBUFFER(gps->data.lLatitude_GPS, buf);
	PUSH_VAR_DOUBLEBUFFER(gps->data.fHightSea_GPS, buf);
	PUSH_VAR_DOUBLEBUFFER(gps->data.fHightGeo_GPS, buf);
	PUSH_VAR_DOUBLEBUFFER(gps->data.fCourse, buf);
	PUSH_VAR_DOUBLEBUFFER(gps->data.fRate, buf);
	PUSH_VAR_DOUBLEBUFFER(gps->data.fMagYaw, buf);
	PUSH_VAR_DOUBLEBUFFER(gps->data.fDoubleYaw, buf);
	PUSH_VAR_DOUBLEBUFFER(gps->data.fHDOP, buf);
	PUSH_VAR_DOUBLEBUFFER(gps->data.us1PPS_Counter, buf);
	PUSH_VAR_DOUBLEBUFFER(gps->data.usGPGGA_Counter, buf);
	PUSH_VAR_DOUBLEBUFFER(gps->data.usGPRMC_Counter, buf);
	PUSH_VAR_DOUBLEBUFFER(gps->data.usPSAT_Counter, buf);
	PUSH_VAR_DOUBLEBUFFER(gps->data.usRecieverStatus, buf);
	LengthSectionEnd(buf, &frameConstr);
	CheckSectionEnd(buf, &frameConstr);
	FrameEnd(buf, &frameConstr);
}   
#endif

#ifdef MSG_QSENS_SEND_NAV 
MSG_QSENS_DatagramNavTypeDef* QSENS_GetNavData(void)
{
	return &g_stNavResult;
}

void QSENS_MakeNavResultFrame(MSG_QSENS_DatagramNavTypeDef* nav, DoubleBuffer *buf)
{
	static unsigned short usFrameCounter = 0;
	FrameBegin(buf, &frameConstr);
	PushDoubleBuffer(0xEA, buf);
	PushDoubleBuffer(0x51, buf);
	CheckSectionBegin(buf, &frameConstr);
	LengthSectionBegin(buf, &frameConstr);
	PushWordDoubleBuffer(MSG_QSENS_DATAGRAM_NAV, buf);
	MarkLengthPosition(buf, &frameConstr, 2);
	PUSH_VAR_DOUBLEBUFFER(usFrameCounter, buf);
	usFrameCounter++;
	PUSH_VAR_DOUBLEBUFFER(g_stNavResult.data.dLongitude, buf);
	PUSH_VAR_DOUBLEBUFFER(g_stNavResult.data.dLatitude, buf);
	PUSH_VAR_DOUBLEBUFFER(g_stNavResult.data.fHight, buf);
	PUSH_VAR_DOUBLEBUFFER(g_stNavResult.data.fVe, buf);
	PUSH_VAR_DOUBLEBUFFER(g_stNavResult.data.fVn, buf);
	PUSH_VAR_DOUBLEBUFFER(g_stNavResult.data.fVv, buf);
	PUSH_VAR_DOUBLEBUFFER(g_stNavResult.data.fYaw, buf);
	PUSH_VAR_DOUBLEBUFFER(g_stNavResult.data.fPitch, buf);
	PUSH_VAR_DOUBLEBUFFER(g_stNavResult.data.fRoll, buf);
	PUSH_VAR_DOUBLEBUFFER(g_stNavResult.data.uStatus, buf);
	LengthSectionEnd(buf, &frameConstr);
	CheckSectionEnd(buf, &frameConstr);
	FrameEnd(buf, &frameConstr);
}
#endif

#ifdef MSG_QSENS_SEND_IMU
void QSENS_MakeImuFrame(MSG_QSENS_DatagramImuTypeDef* imu, DoubleBuffer *buf)
{
	static unsigned short usFrameCounter = 0;
	FrameBegin(buf, &frameConstr);
	PushDoubleBuffer(0xEA, buf);
	PushDoubleBuffer(0x51, buf);
	CheckSectionBegin(buf, &frameConstr);
	LengthSectionBegin(buf, &frameConstr);
	PushWordDoubleBuffer(MSG_QSENS_DATAGRAM_IMU, buf);
	MarkLengthPosition(buf, &frameConstr, 2);
	PUSH_VAR_DOUBLEBUFFER(usFrameCounter, buf);
	usFrameCounter++;
	PUSH_VAR_DOUBLEBUFFER(imu->data.lGyroX, buf);
	PUSH_VAR_DOUBLEBUFFER(imu->data.lGyroY, buf);
	PUSH_VAR_DOUBLEBUFFER(imu->data.lGyroZ, buf);
	PUSH_VAR_DOUBLEBUFFER(imu->data.lAccX, buf);
	PUSH_VAR_DOUBLEBUFFER(imu->data.lAccY, buf);
	PUSH_VAR_DOUBLEBUFFER(imu->data.lAccZ, buf);
	PUSH_VAR_DOUBLEBUFFER(imu->data.sTempretureGyroX, buf);
	PUSH_VAR_DOUBLEBUFFER(imu->data.sTempretureGyroY, buf);
	PUSH_VAR_DOUBLEBUFFER(imu->data.sTempretureGyroZ, buf);
	PUSH_VAR_DOUBLEBUFFER(imu->data.sTempretureAccX, buf);
	PUSH_VAR_DOUBLEBUFFER(imu->data.sTempretureAccY, buf);
	PUSH_VAR_DOUBLEBUFFER(imu->data.sTempretureAccZ, buf);
	LengthSectionEnd(buf, &frameConstr);
	CheckSectionEnd(buf, &frameConstr);
	FrameEnd(buf, &frameConstr);
}
void QSENS_MakeImuFrame1(MSG_QSENS_DatagramImuTypeDef* imu, DoubleBuffer *buf,uint16_t CommandAddr)
{
	static unsigned short usFrameCounter = 0;
	FrameBegin(buf, &frameConstr);
	PushDoubleBuffer(0xEA, buf);
	PushDoubleBuffer(0x51, buf);
	CheckSectionBegin(buf, &frameConstr);
	LengthSectionBegin(buf, &frameConstr);
	PushWordDoubleBuffer(CommandAddr, buf);
	MarkLengthPosition(buf, &frameConstr, 2);
	PUSH_VAR_DOUBLEBUFFER(usFrameCounter, buf);
	usFrameCounter++;
	PUSH_VAR_DOUBLEBUFFER(imu->data.lGyroX, buf);
	PUSH_VAR_DOUBLEBUFFER(imu->data.lGyroY, buf);
	PUSH_VAR_DOUBLEBUFFER(imu->data.lGyroZ, buf);
	PUSH_VAR_DOUBLEBUFFER(imu->data.lAccX, buf);
	PUSH_VAR_DOUBLEBUFFER(imu->data.lAccY, buf);
	PUSH_VAR_DOUBLEBUFFER(imu->data.lAccZ, buf);
	PUSH_VAR_DOUBLEBUFFER(imu->data.sTempretureGyroX, buf);
	PUSH_VAR_DOUBLEBUFFER(imu->data.sTempretureGyroY, buf);
	PUSH_VAR_DOUBLEBUFFER(imu->data.sTempretureGyroZ, buf);
	PUSH_VAR_DOUBLEBUFFER(imu->data.sTempretureAccX, buf);
	PUSH_VAR_DOUBLEBUFFER(imu->data.sTempretureAccY, buf);
	PUSH_VAR_DOUBLEBUFFER(imu->data.sTempretureAccZ, buf);
	LengthSectionEnd(buf, &frameConstr);
	CheckSectionEnd(buf, &frameConstr);
	FrameEnd(buf, &frameConstr);
}
void QSENS_MakeImuFrame2(uint8_t *pBuffer,uint16_t len,DoubleBuffer *buf,uint16_t CommandAddr)
{
	static unsigned short usFrameCounter = 0;
	int i;
//	buf->m_nBufferLength[buf->m_nBufferNumber]=0;
	FrameBegin(buf, &frameConstr);
	PushDoubleBuffer(0xEA, buf);
	PushDoubleBuffer(0x51, buf);
	CheckSectionBegin(buf, &frameConstr);
	LengthSectionBegin(buf, &frameConstr);
	PushWordDoubleBuffer(CommandAddr, buf);
	MarkLengthPosition(buf, &frameConstr, 2);
	PUSH_VAR_DOUBLEBUFFER(usFrameCounter, buf);
	usFrameCounter++;
	for(i=0;i<len;i++)
	{
		PushDoubleBuffer(pBuffer[i],buf);
	}
	
	LengthSectionEnd(buf, &frameConstr);
	CheckSectionEnd(buf, &frameConstr);
	FrameEnd(buf, &frameConstr);
}
#endif

#ifdef MSG_QSENS_SEND_IMU_FULL
void QSENS_MakeImuFullFrame(MSG_QSENS_DatagramImuTypeDef* imu, DoubleBuffer *buf)
{
	FrameBegin(buf, &frameConstr);
	PushDoubleBuffer(0xEA, buf);
	PushDoubleBuffer(0x51, buf);
	CheckSectionBegin(buf, &frameConstr);
	PushWordDoubleBuffer(MSG_QSENS_DATAGRAM_IMU, buf);
	MarkLengthPosition(buf, &frameConstr, 2);
	LengthSectionBegin(buf, &frameConstr);
	imu->data.ulFullGyroX += imu->data.ulGyroX;
	imu->data.ulFullGyroY += imu->data.ulGyroY;
	imu->data.ulFullGyroZ += imu->data.ulGyroZ;
	imu->data.ulFullAccX += imu->data.ulAccX;
	imu->data.ulFullAccY += imu->data.ulAccY;
	imu->data.ulFullAccZ += imu->data.ulAccZ;
	PUSH_VAR_DOUBLEBUFFER(imu->data.ulFullGyroX, buf);
	PUSH_VAR_DOUBLEBUFFER(imu->data.ulFullGyroY, buf);
	PUSH_VAR_DOUBLEBUFFER(imu->data.ulFullGyroZ, buf);
	PUSH_VAR_DOUBLEBUFFER(imu->data.ulFullAccX, buf);
	PUSH_VAR_DOUBLEBUFFER(imu->data.ulFullAccY, buf);
	PUSH_VAR_DOUBLEBUFFER(imu->data.ulFullAccZ, buf);
	PUSH_VAR_DOUBLEBUFFER(imu->data.TempretureGyroX, buf);
	PUSH_VAR_DOUBLEBUFFER(imu->data.TempretureGyroY, buf);
	PUSH_VAR_DOUBLEBUFFER(imu->data.TempretureGyroZ, buf);
	PUSH_VAR_DOUBLEBUFFER(imu->data.TempretureAccX, buf);
	PUSH_VAR_DOUBLEBUFFER(imu->data.TempretureAccY, buf);
	PUSH_VAR_DOUBLEBUFFER(imu->data.TempretureAccZ, buf);
	LengthSectionEnd(buf, &frameConstr);
	CheckSectionEnd(buf, &frameConstr);
	FrameEnd(buf, &frameConstr);
}
#endif

#ifdef MSG_QSENS_SEND_MAG 
#endif

#ifdef MSG_QSENS_SEND_ALT 
#endif

#ifdef MSG_QSENS_SEND_DR 
void QSENS_MakeDrFrame(MSG_QSENS_DatagramDrTypeDef* dr, DoubleBuffer *buf)
{
	static unsigned short usFrameCounter = 0;
	FrameBegin(buf, &frameConstr);
	PushDoubleBuffer(0xEA, buf);
	PushDoubleBuffer(0x51, buf);
	CheckSectionBegin(buf, &frameConstr);
	LengthSectionBegin(buf, &frameConstr);
	PushWordDoubleBuffer(MSG_QSENS_DATAGRAM_DR, buf);
	MarkLengthPosition(buf, &frameConstr, 2);
	PUSH_VAR_DOUBLEBUFFER(usFrameCounter, buf);
	usFrameCounter++;
	PUSH_VAR_DOUBLEBUFFER(dr->data.Vx, buf);
	PUSH_VAR_DOUBLEBUFFER(dr->data.Vy, buf);
	PUSH_VAR_DOUBLEBUFFER(dr->data.Temp, buf);
	LengthSectionEnd(buf, &frameConstr);
	CheckSectionEnd(buf, &frameConstr);
	FrameEnd(buf, &frameConstr);
}
#endif

#ifdef MSG_QSENS_SEND_EPH
#endif

#ifdef MSG_QSENS_SEND_OBS 
#endif




