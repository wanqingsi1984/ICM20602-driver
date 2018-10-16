#ifndef QSENS_H_2017621735
#define QSENS_H_2017621735
/**
* @file
* @author Qiaoyy@xlnav.com
* @date
* @brief
* Copyright (c) 2018 XiLang Co. Ltd.
* All rights reserved.
*/
#define MSG_QSENS_SEND_IMU
#define MSG_QSENS_RECIEVE_IMU 
// #define MSG_QSENS_SEND_IMU_FULL 
// #define MSG_QSENS_RECIEVE_IMU_FULL 
/* #define MSG_QSENS_SEND_MAG */
/* #define MSG_QSENS_RECIEVE_MAG */
/* #define MSG_QSENS_SEND_DR */
/* #define MSG_QSENS_RECIEVE_DR */
/* #define MSG_QSENS_SEND_ALTIMETER */
/* #define MSG_QSENS_RECIEVE_ALTIMETER */
// #define MSG_QSENS_SEND_NAV 
/* #define MSG_QSENS_RECIEVE_NAV */
// #define MSG_QSENS_SEND_GPS 
// #define MSG_QSENS_RECIEVE_GPS 
/* #define MSG_QSENS_SEND_OBS */
/* #define MSG_QSENS_RECIEVE_OBS */
/* #define MSG_QSENS_SEND_EPH */
/* #define MSG_QSENS_RECIEVE_EPH */
/* Includes ------------------------------------------------------------------*/
#include "Global.h"
#include "ParseFrame.h"
#if defined(MSG_QSENS_SEND_EPH) || defined(MSG_QSENS_RECIEVE_EPH) || defined(MSG_QSENS_SEND_OBS) || defined(MSG_QSENS_RECIEVE_OBS)
#include "msg_GNSS.h"
#endif
/* Exported macro ------------------------------------------------------------*/
#define QSENS_BIN_PROTOCOL_MAPPING_DEFINE {SETTING_LENGTH_FRAME, {0xEA,0x51}, "", 2, 6, QSENS_BinInit, QSENS_BinFrameLength, QSENS_BinFrameCheck, QSENS_BinFrameParse},

#define MSG_QSENS_DATAGRAM_CMD 0x0000U
#define MSG_QSENS_DATAGRAM_NAV 0x0001U
#define MSG_QSENS_DATAGRAM_IMU 0x0201U
#define MSG_QSENS_DATAGRAM_IMU_MTI 0x0211U
#define MSG_QSENS_DATAGRAM_IMU_FULL 0x0202U
#define MSG_QSENS_DATAGRAM_MAG 0x0203U
#define MSG_QSENS_DATAGRAM_ALT 0x0204U
#define MSG_QSENS_DATAGRAM_DR  0x0205U
#define MSG_QSENS_DATAGRAM_GPS 0x0101U
#define MSG_QSENS_DATAGRAM_EPH 0x0102U
#define MSG_QSENS_DATAGRAM_OBS 0x0103U

/* Exported types ------------------------------------------------------------*/
typedef struct
{
	double dDeltaLatitude;
	double dDeltaLongitude;
	float fSecond;
	float fHDOP; //!< 
	long lLatitude_GPS; //!< 
	long lLongitude_GPS; //!< 
	float fHightSea_GPS; //!< 
	float fHightGeo_GPS; //!< 
	float fCourse; //!< 
	float fRate; //!< 
	float fMagYaw;
	float fDoubleYaw;
	unsigned short us1PPS_Counter; //!< 
	unsigned short usGPGGA_Counter; //!< 
	unsigned short usGPRMC_Counter; //!< 
	unsigned short usPSAT_Counter;
	unsigned short usRecieverStatus; //!< 
	unsigned char ucGPGGA_RecieveFlag; //!< 
	unsigned char ucGPRMC_RecieveFlag; //!<
	unsigned char ucPSAT_RecieveFlag;
	unsigned char ucGPGGA_UpdateFlag; //!< 
	unsigned char ucGPRMC_UpdateFlag; //!< 
	unsigned char ucPSAT_UpdateFlag;
	unsigned char ucYear;
	unsigned char ucMonth;
	unsigned char ucDay;
	unsigned char ucHour;
	unsigned char ucMinite;
	unsigned char ucGGA_Status;//!< 
	unsigned char ucSvNum; //!< 
	unsigned char ucBaseAge; //!< 
}MSG_QSENS_GpsData;

typedef struct
{
	long lGyroX;
	long lGyroY;
	long lGyroZ;
	long lAccX;
	long lAccY;
	long lAccZ;
#ifdef MSG_QSENS_RECIEVE_IMU_FULL
	long lFullGyroXLast;
	long lFullGyroYLast;
	long lFullGyroZLast;
	long lFullAccXLast;
	long lFullAccYLast;
	long lFullAccZLast;
	long lFullGyroXCurrent;
	long lFullGyroYCurrent;
	long lFullGyroZCurrent;
	long lFullAccXCurrent;
	long lFullAccYCurrent;
	long lFullAccZCurrent;
#endif
#ifdef MSG_QSENS_SEND_IMU_FULL
	long lFullGyroX;
	long lFullGyroY;
	long lFullGyroZ;
	long lFullAccX;
	long lFullAccY;
	long lFullAccZ;
#endif
	short sTempretureGyroX;
	short sTempretureGyroY;
	short sTempretureGyroZ;
	short sTempretureAccX;
	short sTempretureAccY;
	short sTempretureAccZ;
}MSG_QSENS_ImuData;

typedef struct
{
	unsigned long unMagX;
	unsigned long unMagY;
	unsigned long unMagZ;
}MSG_QSENS_MagData;

typedef struct
{
	float Vx;
	float Vy;
	unsigned long Temp;
}MSG_QSENS_DrData;

typedef struct
{
	unsigned int unPressure;
	unsigned int unTempreture;
	unsigned int unHight;
}MSG_QSENS_AltData;

typedef struct
{
	int a;
}MSG_QSENS_ObsData;

typedef struct
{
	double dLongitude;
	double dLatitude;
	float fHight;
	float fYaw;
	float fPitch;
	float fRoll;
	float fVe;
	float fVn;
	float fVv;
	unsigned short uStatus;
}MSG_QSENS_NavData;

typedef struct
{
#ifdef MSG_QSENS_RECIEVE_GPS
	RecieveParam receive;
#endif
	FrameCounterUint16 counter;
	MSG_QSENS_GpsData data;
}MSG_QSENS_DatagramGpsTypeDef;

typedef struct
{
#ifdef MSG_QSENS_RECIEVE_IMU
	RecieveParam receive;
#endif
	FrameCounterUint16 counter;
	MSG_QSENS_ImuData data;
}MSG_QSENS_DatagramImuTypeDef;

typedef struct
{
#ifdef MSG_QSENS_RECIEVE_MAG
	RecieveParam receive;
#endif
	FrameCounterUint16 counter;
	MSG_QSENS_MagData data;
}MSG_QSENS_DatagramMagTypeDef;

typedef struct
{
#ifdef MSG_QSENS_RECIEVE_DR
	RecieveParam receive;
#endif
	FrameCounterUint16 counter;
	MSG_QSENS_DrData data;
}MSG_QSENS_DatagramDrTypeDef;

typedef struct
{
#ifdef MSG_QSENS_RECIEVE_ALT
	RecieveParam receive;
#endif
	FrameCounterUint16 counter;
	MSG_QSENS_AltData data;
}MSG_QSENS_DatagramAltTypeDef;

typedef struct
{
#ifdef MSG_QSENS_RECIEVE_OBS
	RecieveParam receive;
#endif
	FrameCounterUint16 counter;
	MSG_QSENS_ObsData data;
}MSG_QSENS_DatagramObsTypeDef;

typedef struct
{
#ifdef MSG_QSENS_RECIEVE_NAV
	RecieveParam receive;
#endif
	FrameCounterUint16 counter;
	MSG_QSENS_NavData data;
}MSG_QSENS_DatagramNavTypeDef;

/* Exported constants --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
int QSENS_BinFrameLength(unsigned char* pBuffer, int nLength);
int QSENS_BinFrameCheck(unsigned char* pBuffer, int nLength);
int QSENS_BinFrameParse(unsigned char* pBuffer, int nLength);
int QSENS_BinInit(unsigned char* pBuffer, int nLength);

#ifdef MSG_QSENS_SEND_GPS
MSG_QSENS_DatagramGpsTypeDef* QSENS_GetGpsData(void);
void QSENS_MakeGpsFrame(MSG_QSENS_DatagramGpsTypeDef* gps, DoubleBuffer *buf);
#endif

#ifdef MSG_QSENS_SEND_NAV 
MSG_QSENS_DatagramNavTypeDef* QSENS_GetNavData(void);
void QSENS_MakeNavResultFrame(MSG_QSENS_DatagramNavTypeDef* nav, DoubleBuffer *buf);
#endif

#ifdef MSG_QSENS_SEND_IMU
void QSENS_MakeImuFrame(MSG_QSENS_DatagramImuTypeDef* imu, DoubleBuffer *buf);
void QSENS_MakeImuFrame1(MSG_QSENS_DatagramImuTypeDef* imu, DoubleBuffer *buf,uint16_t CommandAddr);
void QSENS_MakeImuFrame2(uint8_t *pBuffer,uint16_t len,DoubleBuffer *buf,uint16_t CommandAddr);
#endif

#ifdef MSG_QSENS_SEND_MAG 
#endif

#ifdef MSG_QSENS_SEND_ALT 
#endif

#ifdef MSG_QSENS_SEND_DR 
#endif

#ifdef MSG_QSENS_SEND_GPS
#endif

#ifdef MSG_QSENS_SEND_EPH
#endif

#ifdef MSG_QSENS_SEND_OBS 
#endif


#endif
/************************ (C) COPYRIGHT XiLang CO. Ltd. *******END OF FILE*****/
