#ifndef GLOBAL_H_201704091620
#define GLOBAL_H_201704091620

#include "Includes.h"

#define CONSTRUCT_DOUBLE_BUFFER_MEMORY_POOL(buf, size) unsigned char buf##_memory[2][size];\
                                                DoubleBuffer buf = {{buf##_memory[0], buf##_memory[1]},{0,0}, 0, size};
												
#define CONSTRUCT_CIRCULAR_BUFFER_MEMORY_POOL(buf, size) unsigned char buf##_memory[size];\
                                                  CircularBuffer buf = {buf##_memory, 0, 0, 0, size};									
#define PUSH_VAR_DOUBLEBUFFER(var, buf) PushBytesDoubleBuffer((unsigned char *)(&(var)), sizeof(var), (buf));

#define	BUFFER_NO_ERROR 0x00											  
#define BUFFER_ERROR_OVERLAB 0x01											
												  
#define DEG2RAD(x) ((x)/180*3.14159265358979)
#define RAD2DEG(x) ((x)*180/3.14159265358979)

// 循环缓冲区
typedef struct
{
	unsigned char *m_ucDataBuffer;
	volatile unsigned short m_nBufferHeadPosition;
	volatile unsigned short m_nBufferTailPosition;
	volatile unsigned short m_nBufferLength;
	volatile unsigned short m_nBufferSize;
	volatile unsigned char m_ucErrorCode;
}CircularBuffer;

// 周转双缓冲区
typedef struct
{
	unsigned char *m_ucDataBuffer[2];
	volatile unsigned short m_nBufferLength[2];
	volatile unsigned short m_nBufferNumber;
	volatile unsigned short m_nBufferSize;
	volatile unsigned char m_ucErrorCode;
}DoubleBuffer;


int PopCircularBuffer(unsigned char *ucByte, CircularBuffer* pBuf);
int PushCircularBuffer(unsigned char ucByte, CircularBuffer* pBuf);
void ClearCircularBuffer(CircularBuffer* pBuf);

int PopDoubleBuffer(unsigned char *ucBlock, DoubleBuffer* pBuf);
int PushDoubleBuffer(unsigned char ucByte, DoubleBuffer* pBuf);
void ClearDoubleBuffer(DoubleBuffer* pBuf);
unsigned short GetDoubleBufferLength(DoubleBuffer* pBuf);
unsigned char* GetDoubleBuffer(DoubleBuffer* pBuf);
void PushBytesDoubleBuffer(unsigned char *pBytes, unsigned short nLength, DoubleBuffer* pBuf);
void PushWordDoubleBuffer(unsigned short word, DoubleBuffer* pBuf);

#endif
