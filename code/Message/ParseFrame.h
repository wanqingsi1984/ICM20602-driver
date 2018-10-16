#ifndef PARSEFRAME_H_20175201932
#define PARSEFRAME_H_20175201932
/**
 * @file ParseFrame.h
 * @author Qiaoyy@xlnav.com
 * @date 2018-4-4
 * @brief  通讯帧解析框架
 * Copyright (c) 2018 XiLang Co. Ltd.
 * All rights reserved.
   
                      ##### 如何使用通讯帧解析框架 #####
  ==============================================================================
  [..] 
      (#) FLASH Memory Erase functions: 
           (++) Lock and Unlock the FLASH interface using HAL_FLASH_Unlock() and 
                HAL_FLASH_Lock() functions
           (++) Erase function: Erase sector, erase all sectors
           (++) There are two modes of erase :
             (+++) Polling Mode using HAL_FLASHEx_Erase()
             (+++) Interrupt Mode using HAL_FLASHEx_Erase_IT()
             
      (#) Option Bytes Programming functions: Use HAL_FLASHEx_OBProgram() to :
           (++) Set/Reset the write protection
           (++) Set the Read protection Level
           (++) Set the BOR level
           (++) Program the user Option Bytes
      (#) Advanced Option Bytes Programming functions: Use HAL_FLASHEx_AdvOBProgram() to :  
       (++) Extended space (bank 2) erase function
       (++) Full FLASH space (2 Mo) erase (bank 1 and bank 2)
       (++) Dual Boot activation
       (++) Write protection configuration for bank 2
       (++) PCROP protection configuration and control for both banks
 ******************************************************************************* 
 */
 
/* Includes ------------------------------------------------------------------*/
#include "Global.h"
/* Exported macro ------------------------------------------------------------*/
#define MAX_FRAME_TYPE 8
#define MAX_FRAME_HEAD_LENGTH 8
#define MAX_FRAME_TAIL_LENGTH 8
#define MAX_PARSE_BUFFER_SIZE 1024
#define TRANSPORT_LAYER_DEFINE_BEGIN(table) FrameTemplet table[] = {
#define TRANSPORT_LAYER_DEFINE_END {FIXED_LENGTH_FRAME, "", "", 0,0,0,0}};
#define TRANSPORT_NUMBER(table) ((sizeof(table)/sizeof(FrameTemplet))-1)                             
#define STD_FRAME_HEAD {0x55,0xaa}
#define STD_FRAME_TAIL {0x0d,0x0a}

#define CONSTRUCT_PARSE_PARAM_MEMORY_POOL(buf, size) unsigned char buf##_memory[size];\
                                                  ParseParam buf = {0,PARSE_INITIAL,0,0,0,(size),0,0,{0},buf##_memory,0,0,0};
												  
// 数据监控
// 设计：
#define MESSAGE_STATUS_INIT			0x00 
#define MESSAGE_STATUS_REVIEVED 	0x01
#define MESSAGE_STATUS_UPDATED	 	0x02
#define MESSAGE_STATUS_AVALABEL	 	0x03

#define MESSAGE_ERROR_CHECK 	0x00
#define MESSAGE_ERROR_AVALABEL 	0x01												  
/* Exported types ------------------------------------------------------------*/
typedef int32_t (*FrameParseCallBack)(uint8_t* pBuffer, int32_t nLength);

enum ParseStep
{
	PARSE_INITIAL = 0,
	PARSE_FOUND_HEAD,
	PARSE_RECIEVE_DATA,
	PARSE_FOUND_TAIL
};

enum FrameType{FIXED_LENGTH_FRAME=0, UNFIXED_LENGTH_FRAME,  SETTING_LENGTH_FRAME};

/**
 *  帧模型  FrameTemplet
 **/
typedef struct 
{
	enum FrameType nFrameType;				//!<
    int8_t FrameHead[MAX_FRAME_HEAD_LENGTH]; 	//!<
    int8_t FrameTail[MAX_FRAME_TAIL_LENGTH]; 	//!<
	uint16_t FrameHeadLength;			//!<
	uint16_t FrameParamLength; 		//!<
    FrameParseCallBack InitFunc;			//!<
	FrameParseCallBack LengthFunc;			//!<
    FrameParseCallBack CheckFunc;			//!<
	FrameParseCallBack ParseFunc;			//!<
}FrameTemplet;

/**
 *  帧解析结构体  ParseParam
 **/
typedef struct  
{
    uint16_t nReadCounter; 
    enum ParseStep nStep;
    uint16_t nFrameCounter;
    uint16_t nTailCounter;
    uint16_t nFrameNumber;
	uint16_t nBufferSize;
	uint16_t nErrorCode;
	uint16_t nFrameLength;
    int8_t cSelect[MAX_FRAME_TYPE];
    uint8_t* pBuffer;
	FrameTemplet* TransportFrameDefine;
	uint16_t TransportFrameNumber;
	uint8_t flagSync;
} ParseParam;

/**
 * 接收参数结构体
 **/
typedef struct
{
	unsigned long nUpdateTimes;			//!< 数据更新次数
	unsigned long nReadTimes;			//!< 数据读取次数
	unsigned short usCheckErrorTimes;	//!< 校验和错误
	unsigned short usAvalabelErrorTimes;//!< 校验和错误
	unsigned char Status;				//!< 数据帧获取标志位
	unsigned char ucErrorCode;			//!< 错误码
}RecieveParam;

/**
 *  8位帧计数
 **/
typedef struct
{
	unsigned char last;
	short delta;
	unsigned char first;
	unsigned char current;
}FrameCounterUint8;

/**
 *  16位帧计数
 **/
typedef struct
{
	unsigned short current;
	unsigned short last;
    long delta;
	unsigned short first;
}FrameCounterUint16;

/**
 *  帧构造结构体
 **/
typedef struct
{
	uint16_t nFrameLengthBegin;
	uint16_t nFrameLengthPos;
	uint16_t nFrameLengthSize;
	uint16_t nCheckBegin;
	uint16_t nStep;
	uint8_t check;
}FrameConstructor;


/* Exported constants --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
void GetBytes(uint8_t* pBuffer,uint8_t* pBytes, int32_t nNumber);
void ClearParseParam(ParseParam* io_ParseParam);
void ParseFrameBlock(uint8_t* i_pcBuffer, int32_t i_nBufferSize, ParseParam* io_pParseParam);
int32_t ParseFrameByte(uint8_t i_ucByte, ParseParam* io_pParseParam);
void InitFrameParse(ParseParam* io_ParseParam, uint8_t* i_pcBuffer, int32_t i_nBufferSize);
void ParseFrame(CircularBuffer* buf, ParseParam* pParseParam, unsigned int nMaxReadTimes);

uint32_t GetBit(uint8_t* pBuffer, int32_t nBeginBit, int32_t nBitNumber);

int16_t GetShort(uint8_t* pBuffer);
uint32_t GetULong(uint8_t* pBuffer);
int32_t GetLong(uint8_t* pBuffer);
uint16_t GetUShort(uint8_t* pBuffer);
int16_t GetShortBigEnd(uint8_t* pBuffer);
uint16_t GetUShortBigEnd(uint8_t* pBuffer);
int32_t GetLongBigEnd(uint8_t* pBuffer);
uint32_t GetULongBigEnd(uint8_t* pBuffer);
float GetFloat(uint8_t* pBuffer);
double GetDouble(uint8_t* pBuffer);

uint32_t GetULongContinue(uint8_t* pBuffer, int32_t* nCounter);
int16_t GetShortContinue(uint8_t* pBuffer, int32_t* nCounter);
int32_t GetLongContinue(uint8_t* pBuffer, int32_t* nCounter);
uint16_t GetUShortContinue(uint8_t* pBuffer, int32_t* nCounter);
int16_t GetShortBigEndContinue(uint8_t* pBuffer, int32_t* nCounter);
uint16_t GetUShortBigEndContinue(uint8_t* pBuffer, int32_t* nCounter);
int32_t GetLongBigEndContinue(uint8_t* pBuffer, int32_t* nCounter);
uint32_t GetULongBigEndContinue(uint8_t* pBuffer, int32_t* nCounter);
float GetFloatContinue(uint8_t* pBuffer, int32_t* nCounter);
double GetDoubleContinue(uint8_t* pBuffer, int32_t* nCounter);

void ReadData(RecieveParam* mnt);
void DataNotAvalabel(RecieveParam* mnt);
void DataAvalabel(RecieveParam* mnt);
void CheckRight(RecieveParam* mnt);
void CheckError(RecieveParam* mnt);
void RecieveMessage(RecieveParam* mnt);

unsigned char CalculateCheckSum(unsigned char* pBuffer, int start, int end);
unsigned short CheckSum16(unsigned char* pBuffer, int nLength);
unsigned char CheckSum8(unsigned char* pBuffer, int nLength);

void CalculateDeltaCounterUint16(FrameCounterUint16* counter);
void CalculateDeltaCounterUint8(FrameCounterUint8* counter);

void FrameBegin(DoubleBuffer* buf, FrameConstructor* frm);
void FrameEnd(DoubleBuffer* buf, FrameConstructor* frm);
void CheckSectionBegin(DoubleBuffer* buf, FrameConstructor* frm);
void CheckSectionEnd(DoubleBuffer* buf, FrameConstructor* frm);
void LengthSectionBegin(DoubleBuffer* buf, FrameConstructor* frm);
void LengthSectionEnd(DoubleBuffer* buf, FrameConstructor* frm);
void MarkLengthPosition(DoubleBuffer* buf, FrameConstructor* frm, int nLength);
void ClearFrameConstructor(FrameConstructor* frm);
#endif
/************************ (C) COPYRIGHT XiLang CO. Ltd. *******END OF FILE*****/
