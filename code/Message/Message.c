/**
 * @file     Com.c
 * @author   Qiaoyy
 * @date     2017/12/03
 * @version  V1.0
 * @brief    Comunication Level
 */

/* Includes ------------*/
#include "Global.h"
#include "ParseFrame.h"
#include "Device.h"
#include "msg_Qsens.h"
#include "Message.h"

/* Private typedef ------------*/

/* Private macro ------------*/

/* Private variables ---------*/

TRANSPORT_LAYER_DEFINE_BEGIN(USART1_FRAME)
	QSENS_BIN_PROTOCOL_MAPPING_DEFINE
TRANSPORT_LAYER_DEFINE_END
//TRANSPORT_LAYER_DEFINE_BEGIN(USART2_FRAME)
//	QSENS_BIN_PROTOCOL_MAPPING_DEFINE
//TRANSPORT_LAYER_DEFINE_END
TRANSPORT_LAYER_DEFINE_BEGIN(USART3_FRAME)
	QSENS_BIN_PROTOCOL_MAPPING_DEFINE
TRANSPORT_LAYER_DEFINE_END

extern CircularBuffer g_USART1_Buffer;
//extern CircularBuffer g_USART2_Buffer;
extern CircularBuffer g_USART3_Buffer;
//extern SyncCircularBuffer g_USART1_Buffer;
//extern SyncCircularBuffer g_USART2_Buffer;
//extern SyncCircularBuffer g_USART3_Buffer;

CONSTRUCT_PARSE_PARAM_MEMORY_POOL(g_Usart1Parse, 256);
//CONSTRUCT_PARSE_PARAM_MEMORY_POOL(g_Usart2Parse, 256);
CONSTRUCT_PARSE_PARAM_MEMORY_POOL(g_Usart3Parse, 256);

/* Private function prototypes ------*/

/* Private functions ------------*/

void InitMessage()
{
	g_Usart1Parse.TransportFrameDefine = USART1_FRAME;
	g_Usart1Parse.TransportFrameNumber = TRANSPORT_NUMBER(USART1_FRAME);
	
//	g_Usart2Parse.TransportFrameDefine = USART2_FRAME;
//	g_Usart2Parse.TransportFrameNumber = TRANSPORT_NUMBER(USART2_FRAME);
	
	g_Usart3Parse.TransportFrameDefine = USART3_FRAME;
	g_Usart3Parse.TransportFrameNumber = TRANSPORT_NUMBER(USART3_FRAME);
	
	InitFrameParse(&g_Usart1Parse, 0, 0);
//	InitFrameParse(&g_Usart2Parse, 0, 0);
	InitFrameParse(&g_Usart3Parse, 0, 0);
}

void ParseFrameUsart1(void)
{
	ParseFrame(&g_USART1_Buffer, &g_Usart1Parse, 1024);
}
//void ParseFrameUsart1(void)
//{
//	unsigned char byte;
//	unsigned char sync;
//	if (PopSyncCircularBuffer(&byte, &g_USART1_Buffer, &sync) > 0)
//	{
//		if (sync==0x01)
//		{
//			g_Usart1Parse.flagSync = sync;
//		}
//		ParseFrameByte(byte, &g_Usart1Parse);
//	}
//}
//void ParseFrameUsart2(void)
//{
//	ParseFrame(&g_USART2_Buffer, &g_Usart2Parse, 1024);
////	unsigned char byte;
////	unsigned char sync;
////	if (PopSyncCircularBuffer(&byte, &g_USART2_Buffer, &sync) > 0)
////	{
////		if (sync==0x01)
////		{
////			g_Usart2Parse.flagSync = sync;
////		}
////		ParseFrameByte(byte, &g_Usart2Parse);
////	}
//}
void ParseFrameUsart3(void)
{
	ParseFrame(&g_USART3_Buffer, &g_Usart3Parse, 1024);
//	unsigned char byte;
//	unsigned char sync;
//	if (PopSyncCircularBuffer(&byte, &g_USART3_Buffer, &sync) > 0)
//	{
//		if (sync==0x01)
//		{
//			g_Usart3Parse.flagSync = sync;
//		}
//		ParseFrameByte(byte, &g_Usart3Parse);
//	}
}


