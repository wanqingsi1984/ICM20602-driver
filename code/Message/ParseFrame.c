/**
 * @file ParseFrame.c
 * @author Qiaoyy@xlnav.com
 * @version V2.0
 * @date 2018-4-4
 * @brief  通讯帧解析框架
 * Copyright (c) 2018 XiLang Co. Ltd.
 * All rights reserved.
 */

 /* Includes ------------------------------------------------------------------*/
#include "Global.h"
#include <string.h>
#include "ParseFrame.h"
/* Private typedef -----------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
//extern FrameTemplet g_TransportFrameDefine[];
//extern int32_t g_TransportFrameNumber;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
 * @brief  初始化帧解析框架
 * @param
 * @return
*/
void InitFrameParse(ParseParam* io_ParseParam, uint8_t* i_pcBuffer, int32_t i_nBufferSize)
{
	int32_t i;
	for (i = 0; i < io_ParseParam->TransportFrameNumber; i++)
	{
		io_ParseParam->TransportFrameDefine[i].InitFunc(i_pcBuffer, i_nBufferSize);
	}
}

/**
* @brief  初始化帧解析结构体
* @param
* @return
*/
void InitParseParam(ParseParam* io_ParseParam, uint8_t* pBuffer, int32_t nBufferSize)
{
	int32_t i;
	for (i = 0; i < io_ParseParam->TransportFrameNumber; i++)
	{
		io_ParseParam->cSelect[i] = 0x00;
	}
	io_ParseParam->nTailCounter = 0;
	io_ParseParam->nStep = PARSE_INITIAL;
	io_ParseParam->nReadCounter = 0;
	io_ParseParam->nFrameNumber = 0;
	io_ParseParam->nFrameCounter = 0;
	io_ParseParam->nBufferSize = nBufferSize;
	io_ParseParam->nErrorCode = 0;
	io_ParseParam->pBuffer = pBuffer;
	io_ParseParam->nFrameLength = 0;
	for (i = 0; i < io_ParseParam->nBufferSize; i++)
	{
		io_ParseParam->pBuffer[i] = 0x00;
	}
}

/**
 * @brief  清除帧解析结构体
 * @param
 * @return
*/
void ClearParseParam(ParseParam* io_ParseParam)
{
	int32_t i;
	for (i = 0; i < io_ParseParam->TransportFrameNumber; i++)
	{
		io_ParseParam->cSelect[i] = 0;
	}
	io_ParseParam->nTailCounter = 0;
	io_ParseParam->nStep = PARSE_INITIAL;
	io_ParseParam->nReadCounter = 0;
	io_ParseParam->nFrameNumber = 0;
	io_ParseParam->nFrameCounter = 0;
	io_ParseParam->nFrameLength = 0;
	io_ParseParam->flagSync = 0;
}

/**
 * @brief
 * @param
 * @return
*/
void ParseFrame(CircularBuffer* buf, ParseParam* pParseParam, unsigned int nMaxReadTimes)
{
	unsigned char byte;
	uint32_t times = 0;
	unsigned char flag = 0x00;
	while ((PopCircularBuffer(&byte, buf) > 0) && (times < nMaxReadTimes))
	{
		times++;
		flag = ParseFrameByte(byte, pParseParam);
		if (flag!=0)
		{
			break;
		}
	}
}

/**
 * @brief
 * @param
 * @return
 * @todo 返回值处理 
 * @todo 错误码定义
*/
int32_t ParseFrameByte(uint8_t i_ucByte, ParseParam* io_pParseParam)
{
	int32_t j = 0;
	int32_t nFrameID = 0;
	int32_t nGetAll = 0;
	int8_t nFoundOne = 0;

	if (io_pParseParam->nStep == PARSE_FOUND_HEAD)
	{
		// 校验帧头部首字节后字节
		nFoundOne = 0;
		for (j = 0; j < io_pParseParam->TransportFrameNumber; j++)
		{
			// 对于帧头首字节不符的跳过
			if (io_pParseParam->cSelect[j] != 1)
			{
				continue;
			}

			if (io_pParseParam->TransportFrameDefine[j].FrameHead[io_pParseParam->nFrameCounter] == (int8_t)i_ucByte)
			{
				nFoundOne = 1;
				io_pParseParam->cSelect[j] = 1;
			}
			else
			{
				io_pParseParam->cSelect[j] = 0;
			}
		}
		if (nFoundOne == 1)
		{
			io_pParseParam->pBuffer[io_pParseParam->nFrameCounter] = i_ucByte;
			io_pParseParam->nFrameCounter++;
			for (j = 0; j < io_pParseParam->TransportFrameNumber; j++)
			{
				if (io_pParseParam->cSelect[j] == 1)
				{
					if (io_pParseParam->TransportFrameDefine[j].FrameHeadLength == io_pParseParam->nFrameCounter)
					{
						io_pParseParam->nStep = PARSE_RECIEVE_DATA;
						io_pParseParam->nFrameNumber = j;
					}
				}
			}
		}
		else
		{
			ClearParseParam(io_pParseParam);
		}
	}
	else if (io_pParseParam->nStep == PARSE_RECIEVE_DATA)
	{
		// 接收数据
		io_pParseParam->pBuffer[io_pParseParam->nFrameCounter] = i_ucByte;
		io_pParseParam->nFrameCounter++;

		// 根据帧类型采用不同方法寻找帧尾
		if (io_pParseParam->TransportFrameDefine[io_pParseParam->nFrameNumber].nFrameType == FIXED_LENGTH_FRAME)
		{
			// 固定长度帧 FIXED_LENGTH_FRAME
			if (io_pParseParam->nFrameCounter == io_pParseParam->TransportFrameDefine[io_pParseParam->nFrameNumber].FrameParamLength)
			{
				nGetAll = 1;
			}
		}
		else if (io_pParseParam->TransportFrameDefine[io_pParseParam->nFrameNumber].nFrameType == SETTING_LENGTH_FRAME)
		{
			// 变长帧 SETTING_LENGTH_FRAME
			if (io_pParseParam->nFrameCounter > io_pParseParam->TransportFrameDefine[io_pParseParam->nFrameNumber].FrameParamLength)
			{			
				if (io_pParseParam->nFrameLength>0 && io_pParseParam->nFrameCounter == io_pParseParam->nFrameLength)
				{
					// 首先帧长度不能为0，当接收到的字节数与帧长度相同时则获得完整帧
					nGetAll = 1;
				}
				else if (io_pParseParam->nFrameLength == 0)
				{
					io_pParseParam->nFrameLength = io_pParseParam->TransportFrameDefine[io_pParseParam->nFrameNumber].LengthFunc(io_pParseParam->pBuffer, io_pParseParam->nFrameCounter);
				}
				else
				{
					// DO_NOTHING
				}
			}
			else if (io_pParseParam->nFrameCounter == io_pParseParam->TransportFrameDefine[io_pParseParam->nFrameNumber].FrameParamLength)
			{
				// 当到达设定的字节数时，取得帧的长度
				io_pParseParam->nFrameLength = io_pParseParam->TransportFrameDefine[io_pParseParam->nFrameNumber].LengthFunc(io_pParseParam->pBuffer, io_pParseParam->nFrameCounter);
			}
			else
			{
				// DO_NOTHING
			}

		}
		else if (io_pParseParam->TransportFrameDefine[io_pParseParam->nFrameNumber].nFrameType == UNFIXED_LENGTH_FRAME)
		{
			// 不固定长度帧 UNFIXED_LENGTH_FRAME
			// 检测是否收到帧尾标志首字节
			if (i_ucByte == io_pParseParam->TransportFrameDefine[io_pParseParam->nFrameNumber].FrameTail[0])
			{
				io_pParseParam->nStep = PARSE_FOUND_TAIL;
				io_pParseParam->nTailCounter++;
			}
		}

		if (io_pParseParam->nFrameCounter > io_pParseParam->nBufferSize)
		{
			ClearParseParam(io_pParseParam);
			io_pParseParam->nErrorCode = 1;
			return 0;
		}
	}
	else if (io_pParseParam->nStep == PARSE_FOUND_TAIL)
	{
		// 收到帧尾标志并进行校验、解码
		io_pParseParam->pBuffer[io_pParseParam->nFrameCounter] = i_ucByte;
		io_pParseParam->nFrameCounter++;
		if (i_ucByte == io_pParseParam->TransportFrameDefine[io_pParseParam->nFrameNumber].FrameTail[io_pParseParam->nTailCounter])
		{
			io_pParseParam->nTailCounter++;
		}
		else
		{
			if (i_ucByte == io_pParseParam->TransportFrameDefine[io_pParseParam->nFrameNumber].FrameTail[0])
			{
				io_pParseParam->nTailCounter = 1;
			}
			else
			{
				io_pParseParam->nTailCounter = 0;
				io_pParseParam->nStep = PARSE_RECIEVE_DATA;
			}
		}
		if (io_pParseParam->TransportFrameDefine[io_pParseParam->nFrameNumber].FrameParamLength == io_pParseParam->nTailCounter)
		{
			nGetAll = 1;
		}
	}
	// 寻找帧首个字节
	if (io_pParseParam->nStep == PARSE_INITIAL)
	{
		nFoundOne = 0;
		for (j = 0; j < io_pParseParam->TransportFrameNumber; j++)
		{
			if (io_pParseParam->TransportFrameDefine[j].FrameHead[0] == (int8_t)i_ucByte)
			{
				// 遍历各类型帧头并标记可能
				if (io_pParseParam->TransportFrameDefine[j].FrameHeadLength > 1)
				{
					nFoundOne = 1;
					io_pParseParam->cSelect[j] = 1;
				}
				// 对于帧头只有一字节的情况则直接进入接收状态
				else if (io_pParseParam->TransportFrameDefine[j].FrameHeadLength == 1)
				{
					nFoundOne = 2;
					io_pParseParam->cSelect[j] = 1;
					io_pParseParam->nFrameNumber = j;
					break;
				}
				else
				{
					ClearParseParam(io_pParseParam);
				}				
			}
		}

		if (nFoundOne == 1)
		{
			io_pParseParam->nStep = PARSE_FOUND_HEAD;
			io_pParseParam->pBuffer[0] = i_ucByte;
			io_pParseParam->nFrameCounter = 1;
			nFoundOne = 0;
		}
		else if (nFoundOne == 2)
		{
			io_pParseParam->nStep = PARSE_RECIEVE_DATA;
			io_pParseParam->pBuffer[0] = i_ucByte;
			io_pParseParam->nFrameCounter = 1;
		}
		else
		{
			ClearParseParam(io_pParseParam);
		}
	}
	if (nGetAll == 1)
	{
		// 进行帧校验
		if (io_pParseParam->TransportFrameDefine[io_pParseParam->nFrameNumber].CheckFunc(io_pParseParam->pBuffer, io_pParseParam->nFrameCounter) == 1)
		{
			// 进行解码
			io_pParseParam->TransportFrameDefine[io_pParseParam->nFrameNumber].ParseFunc(io_pParseParam->pBuffer, io_pParseParam->nFrameCounter);

			nFrameID = io_pParseParam->nFrameNumber;
		}
		else
		{
			// 校验错误则返回负的帧号
			nFrameID = -io_pParseParam->nFrameNumber;
		}

		ClearParseParam(io_pParseParam);
		return nFrameID;
	}
	return 0;
}


/**
 * @brief
 * @param
 * @return
 * @warning 读后 pBuffer 不移动
 * @warning 有越界风险，nBeginBit小于pBuffer字节数*8， nBitNumber 小于等于（pBuffer字节数*8-nBeginBit）
*/
uint32_t GetBit(uint8_t* pBuffer, int32_t nBeginBit, int32_t nBitNumber)
{
	int32_t i;
	if (nBitNumber > 8 * sizeof(uint32_t))
	{
		return 0;
	}
	int32_t nFirstByte = nBeginBit / 8; // 计算第一个字节位置
	int32_t nBitInBeginByte = nBeginBit % 8; // 计算第一个字节位置中的起始位
	int32_t nLeftBits = nBitNumber - (8 - nBitInBeginByte); // 读完第一个字节的位后还有多少位要读
	int32_t nLeftBytes = nLeftBits / 8; // 剩余位数对应的整字节数
	int32_t nLastBits = nLeftBits % 8; // 最后一个字节对应位数
	uint32_t result = (uint32_t)0x00;

	result = pBuffer[nFirstByte] >> nBitInBeginByte; // 读第一个字节中的位数
	for (i = 0; i < nLeftBytes; i++)
	{
		result = result | ((uint32_t)(pBuffer[nFirstByte + i + 1])) << (8 - nBitInBeginByte + i * 8);
	}

	if (nLeftBits > 0)
	{
		result = result | ((uint32_t)(pBuffer[nFirstByte + nLeftBytes + 1] & (0xff >> (8 - nLastBits))) << (nLeftBytes * 8 + 8 - nBitInBeginByte));
	}
	else if (nLeftBits < 0)
	{
		result = result & (0xff >> (-nLastBits + nBitInBeginByte));
	}
	else
	{

	}

	return result;
}

/**
 * @brief
 * @param
 * @return
*/
uint16_t GetUShort(uint8_t* pBuffer)
{
	uint16_t retval;
	memcpy(&retval, pBuffer, sizeof(uint16_t));
	return retval;
}

/**
 * @brief
 * @param
 * @return
*/
int16_t GetShort(uint8_t* pBuffer)
{
	int16_t retval;
	memcpy(&retval, pBuffer, sizeof(int16_t));
	return retval;
}

/**
 * @brief
 * @param
 * @return
*/
uint32_t GetULong(uint8_t* pBuffer)
{
	uint32_t retval;
	memcpy(&retval, pBuffer, sizeof(uint32_t));
	return retval;
}

/**
 * @brief
 * @param
 * @return
*/
int32_t GetLong(uint8_t* pBuffer)
{
	int32_t retval;
	memcpy(&retval, pBuffer, sizeof(int32_t));
	return retval;
}

/**
 * @brief
 * @param
 * @return
*/
double GetDouble(uint8_t* pBuffer)
{
	double retval;
	memcpy(&retval, pBuffer, sizeof(double));
	return retval;
}

/**
 * @brief
 * @param
 * @return
*/
float GetFloat(uint8_t* pBuffer)
{
	float retval;
	memcpy(&retval, pBuffer, sizeof(float));
	return retval;
}

/**
 * @brief
 * @param
 * @return
*/
uint16_t GetUShortContinue(uint8_t* pBuffer, int32_t* nCounter)
{
	uint16_t retval;
	memcpy(&retval, pBuffer, sizeof(uint16_t));
	*nCounter = *nCounter + sizeof(uint16_t);
	return retval;
}

/**
 * @brief
 * @param
 * @return
*/
int16_t GetShortContinue(uint8_t* pBuffer, int32_t* nCounter)
{
	int16_t retval;
	memcpy(&retval, pBuffer, sizeof(int16_t));
	*nCounter = *nCounter + sizeof(int16_t);
	return retval;
}

/**
 * @brief
 * @param
 * @return
*/
uint32_t GetULongContinue(uint8_t* pBuffer, int32_t* nCounter)
{
	uint32_t retval;
	memcpy(&retval, pBuffer, sizeof(uint32_t));
	*nCounter = *nCounter + sizeof(uint32_t);
	return retval;
}

/**
 * @brief
 * @param
 * @return
*/
int32_t GetLongContinue(uint8_t* pBuffer, int32_t* nCounter)
{
	int32_t retval;
	memcpy(&retval, pBuffer, sizeof(int32_t));
	*nCounter = *nCounter + sizeof(int32_t);
	return retval;
}

/**
 * @brief
 * @param
 * @return
*/
double GetDoubleContinue(uint8_t* pBuffer, int32_t* nCounter)
{
	double retval;
	memcpy(&retval, pBuffer, sizeof(double));
	*nCounter = *nCounter + sizeof(double);
	return retval;
	//return *((double*)pBuffer);
}

/**
 * @brief
 * @param
 * @return
*/
float GetFloatContinue(uint8_t* pBuffer, int32_t* nCounter)
{
	float retval;
	memcpy(&retval, pBuffer, sizeof(float));
	*nCounter = *nCounter + sizeof(float);
	return retval;
}

/**
 * @brief
 * @param
 * @return
*/
uint16_t GetUShortBigEndContinue(uint8_t* pBuffer, int32_t* nCounter)
{
	uint16_t retval;
	int32_t size = sizeof(uint16_t);
	uint8_t tempBuffer[sizeof(uint16_t)];
	int32_t i;
	for (i = 0; i < size; i++)
	{
		tempBuffer[i] = pBuffer[size - i - 1];
	}
	memcpy(&retval, tempBuffer, size);
	*nCounter = *nCounter + size;
	return retval;
}

/**
 * @brief
 * @param
 * @return
*/
int16_t GetShortBigEndContinue(uint8_t* pBuffer, int32_t* nCounter)
{
	int16_t retval;
	int32_t size = sizeof(int16_t);
	uint8_t tempBuffer[sizeof(int16_t)];
	int32_t i;
	for (i = 0; i < size; i++)
	{
		tempBuffer[i] = pBuffer[size - i - 1];
	}
	memcpy(&retval, tempBuffer, size);
	*nCounter = *nCounter + size;
	return retval;
}

/**
 * @brief
 * @param
 * @return
*/
uint32_t GetULongBigEndContinue(uint8_t* pBuffer, int32_t* nCounter)
{
	uint32_t retval;
	int32_t size = sizeof(uint32_t);
	uint8_t tempBuffer[sizeof(uint32_t)];
	int32_t i;
	for (i = 0; i < size; i++)
	{
		tempBuffer[i] = pBuffer[size - i - 1];
	}
	memcpy(&retval, tempBuffer, size);
	*nCounter = *nCounter + size;
	return retval;
}

/**
 * @brief
 * @param
 * @return
*/
int32_t GetLongBigEndContinue(uint8_t* pBuffer, int32_t* nCounter)
{
	int32_t retval;
	int32_t size = sizeof(int32_t);
	uint8_t tempBuffer[sizeof(int32_t)];
	int32_t i;
	for (i = 0; i < size; i++)
	{
		tempBuffer[i] = pBuffer[size - i - 1];
	}
	memcpy(&retval, tempBuffer, size);
	*nCounter = *nCounter + size;
	return retval;
}


/**
* @brief
* @param
* @return
*/
uint16_t GetUShortBigEnd(uint8_t* pBuffer)
{
	uint16_t retval;
	int32_t size = sizeof(uint16_t);
	uint8_t tempBuffer[sizeof(uint16_t)];
	int32_t i;
	for (i = 0; i < size; i++)
	{
		tempBuffer[i] = pBuffer[size - i - 1];
	}
	memcpy(&retval, tempBuffer, size);
	return retval;
}

/**
* @brief
* @param
* @return
*/
int16_t GetShortBigEnd(uint8_t* pBuffer)
{
	int16_t retval;
	int32_t size = sizeof(int16_t);
	uint8_t tempBuffer[sizeof(int16_t)];
	int32_t i;
	for (i = 0; i < size; i++)
	{
		tempBuffer[i] = pBuffer[size - i - 1];
	}
	memcpy(&retval, tempBuffer, size);
	return retval;
}

/**
* @brief
* @param
* @return
*/
uint32_t GetULongBigEnd(uint8_t* pBuffer)
{
	uint32_t retval;
	int32_t size = sizeof(uint32_t);
	uint8_t tempBuffer[sizeof(uint32_t)];
	int32_t i;
	for (i = 0; i < size; i++)
	{
		tempBuffer[i] = pBuffer[size - i - 1];
	}
	memcpy(&retval, tempBuffer, size);
	return retval;
}

/**
* @brief
* @param
* @return
*/
int32_t GetLongBigEnd(uint8_t* pBuffer)
{
	int32_t retval;
	int32_t size = sizeof(int32_t);
	uint8_t tempBuffer[sizeof(int32_t)];
	int32_t i;
	for (i = 0; i < size; i++)
	{
		tempBuffer[i] = pBuffer[size - i - 1];
	}
	memcpy(&retval, tempBuffer, size);
	return retval;
}



unsigned char CalculateCheckSum(unsigned char* pBuffer, int start, int end)
{
    unsigned char crc = 0x00;
    int i;
    for(i=start; i<end; i++)
    {
        crc += pBuffer[i];
    }
    return crc;
}

unsigned short CheckSum16(unsigned char* pBuffer, int nLength)
{
	unsigned short retval = 0x0000;
	int i;
	for(i=0;i<nLength;i++)
	{
		retval += (unsigned short)(pBuffer[i]);
	}
	return retval;
}

unsigned char CheckSum8(unsigned char* pBuffer, int nLength)
{
	unsigned char retval = 0x00;
	int i;
	for(i=0;i<nLength;i++)
	{
		retval += pBuffer[i];
	}
	return retval;
}

void RecieveMessage(RecieveParam* mnt)
{
	mnt->Status = MESSAGE_STATUS_REVIEVED;
}

void CheckError(RecieveParam* mnt)
{
	mnt->usCheckErrorTimes += 1;
	mnt->ucErrorCode = MESSAGE_ERROR_CHECK;
}

void CheckRight(RecieveParam* mnt)
{
	mnt->nUpdateTimes += 1;
	mnt->Status = MESSAGE_STATUS_UPDATED;
}

void DataAvalabel(RecieveParam* mnt)
{
	mnt->Status = MESSAGE_STATUS_AVALABEL;
}

void DataNotAvalabel(RecieveParam* mnt)
{
	mnt->usAvalabelErrorTimes += 1;
	mnt->ucErrorCode = MESSAGE_ERROR_AVALABEL;
}

void ReadData(RecieveParam* mnt)
{
	if (mnt->Status == MESSAGE_STATUS_AVALABEL)
	{
		
	}
}

// X8+X2+X+1 0X07
// 
unsigned char CheckCRC8(unsigned char* pBuffer, int nLength)
{
	unsigned char retval = 0x00000000;
	return retval;
}

// X^16+X^15+X^2+1 0X8005
// Bisync, Modbus, USB, ANSI X3.28, SIA DC-07, many others; also known as CRC-16 and CRC-16-ANSI
#define POLY16 0x1021 
#define CRC_SEED   0xFFFF
unsigned short CheckCRC16(unsigned char* pBuffer, int nLength)
{
	unsigned short shift,data,val;
	int i;
	shift = CRC_SEED;

	for(i=0;i<nLength;i++) 
	{
		if((i % 8) == 0)
		  data = (*pBuffer++)<<8;
		val = shift ^ data;
		shift = shift<<1;
		data = data <<1;
		if(val&0x8000)
		  shift = shift ^ POLY16;
	}
	return shift;
}

// X^16+X^12+X^5+1 0X1021
// ISO HDLC, ITU X.25, V.34/V.41/V.42, PPP-FCS
unsigned short CheckCRC_CCITT(unsigned char* pBuffer, int nLength)
{
	unsigned short retval = 0x00000000;
	return retval;
}

// 32Î»CRCÐ£Ñé
// X^32+X^26+X^23+X^22+X^16+X^12+X^11+X^10+X^8+X^7+X^5+X^4+X^2+X+1 0x04C11DB7
// ZIP, RAR, IEEE 802 LAN/FDDI, IEEE 1394, PPP-FCS
static uint32_t table[256];
//ÂëÄæ×ª
static uint32_t bitrev(uint32_t input, int bw)  
{  
    int i;  
    uint32_t var;  
    var = 0;  
    for(i=0; i<bw; i++)  
    {  
        if(input & 0x01)  
        {  
            var |= 1<<(bw - 1 - i);  
        }  
        input >>= 1;  
    }  
    return var;  
}  
  
//Âë±íÉú³É 
void crc32_init(uint32_t poly)  
{  
    int i;  
    int j;  
    uint32_t c;  
  
  
    poly = bitrev(poly, 32);  
    for(i=0; i<256; i++)  
    {  
        c = i;  
        for (j=0; j<8; j++)  
        {  
            c = (c & 1) ? (poly ^ (c >> 1)) : (c >> 1);  
        }  
        table[i] = c;  
    }  
}  
  
  
//Éú³ÉCRC
uint32_t CheckCRC32(uint32_t crc, void* input, int len)  
{  
    int i;  
    uint8_t index;  
    uint8_t *p;  
    p = (uint8_t*)input;  
    for(i=0; i<len; i++)  
    {  
        index = (*p ^ crc);  
        crc = (crc >> 8) ^ table[index];  
        p++;  
    }  
    return crc;  
}

void CalculateDeltaCounterUint16(FrameCounterUint16* counter)
{
	if (counter->first == 1)
	{
		counter->delta = ((long)counter->current - (long)counter->last);
		if (counter->delta <= -65485)
		{
			counter->delta += 65536;
		}

		counter->last = counter->current;
	}
	else
	{
		counter->first = 1;
		counter->last = counter->current;
		counter->delta = 0;
	}
}

void CalculateDeltaCounterUint8(FrameCounterUint8* counter)
{
	if (counter->first == 1)
	{
		counter->delta = ((short)counter->current - (short)counter->last);
		if (counter->delta <= -250)
		{
			counter->delta += 255;
		}

		counter->last = counter->current;
	}
	else
	{
		counter->first = 1;
		counter->last = counter->current;
		counter->delta = 0;
	}
}

void FrameBegin(DoubleBuffer* buf, FrameConstructor* frm)
{
	ClearFrameConstructor(frm);
}
void FrameEnd(DoubleBuffer* buf, FrameConstructor* frm)
{
	ClearFrameConstructor(frm);
}
void CheckSectionBegin(DoubleBuffer* buf, FrameConstructor* frm)
{
	frm->nCheckBegin = GetDoubleBufferLength(buf);
}
void CheckSectionEnd(DoubleBuffer* buf, FrameConstructor* frm)
{
	frm->check = CalculateCheckSum(GetDoubleBuffer(buf), frm->nCheckBegin ,GetDoubleBufferLength(buf));
	PushDoubleBuffer(frm->check, (buf));
}

void LengthSectionBegin(DoubleBuffer* buf, FrameConstructor* frm)
{
	frm->nFrameLengthBegin = GetDoubleBufferLength(buf);
}
void LengthSectionEnd(DoubleBuffer* buf, FrameConstructor* frm)
{
	unsigned short Length;
	Length = GetDoubleBufferLength(buf)- frm->nFrameLengthBegin;
	memcpy(&(GetDoubleBuffer(buf)[frm->nFrameLengthPos]), &Length, frm->nFrameLengthSize);
}
void MarkLengthPosition(DoubleBuffer* buf, FrameConstructor* frm, int nLength)
{
	int i;
	frm->nFrameLengthPos = GetDoubleBufferLength(buf);
	frm->nFrameLengthSize = nLength;
	for (i=0; i<nLength; i++)
	{
		PushDoubleBuffer(0x00, buf);
	}
}

void ClearFrameConstructor(FrameConstructor* frm)
{
	frm->check = 0x00;
	frm->nFrameLengthBegin = 0x0000;
	frm->nFrameLengthPos = 0x0000;
	frm->nFrameLengthSize = 0x0000;
	frm->nCheckBegin = 0x0000;
	frm->nStep = 0x0000;
}
/************************ (C) COPYRIGHT XiLang CO. Ltd. ****** END OF FILE ****/
