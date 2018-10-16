/**
  ******************************************************************************
  * @file 
  * @author  
  * @version 
  * @date    
  * @brief   
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "Global.h"
/* Private typedef -----------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

void ClearCircularBuffer(CircularBuffer* pBuf)
{
    int j;
	pBuf->m_nBufferHeadPosition = 0;
	pBuf->m_nBufferTailPosition = 0;
	pBuf->m_nBufferLength = 0;
	pBuf->m_ucErrorCode = BUFFER_NO_ERROR;
	for (j=0; j<pBuf->m_nBufferSize; j++)
	{
		pBuf->m_ucDataBuffer[j] = 0x00;
	}
}

int PushCircularBuffer(unsigned char ucByte, CircularBuffer* pBuf)
{
    if (pBuf->m_nBufferLength<pBuf->m_nBufferSize)
    {
        pBuf->m_ucDataBuffer[pBuf->m_nBufferTailPosition] = ucByte;
        pBuf->m_nBufferTailPosition++;
        if (pBuf->m_nBufferTailPosition >= pBuf->m_nBufferSize)
        {
            pBuf->m_nBufferTailPosition = 0;
        }
        pBuf->m_nBufferLength++;
    }
	else
	{
		pBuf->m_ucErrorCode = BUFFER_ERROR_OVERLAB;
		return 0;
	}
	return 1;
}

int PopCircularBuffer(unsigned char *ucByte, CircularBuffer* pBuf)
{
    if (pBuf->m_nBufferLength > 0)
    {
        *ucByte = pBuf->m_ucDataBuffer[pBuf->m_nBufferHeadPosition];
		pBuf->m_nBufferLength--;
        pBuf->m_nBufferHeadPosition++;
        if (pBuf->m_nBufferHeadPosition >= pBuf->m_nBufferSize)
        {
            pBuf->m_nBufferHeadPosition = 0;
        }
        return 1;       
    }
    return 0;
}

int PopDoubleBuffer(unsigned char *ucBlock, DoubleBuffer* pBuf)
{
	int i;
	int length = 0;
	if (pBuf->m_nBufferNumber == 1)
    {
        pBuf->m_nBufferNumber = 0;
    }
    else if (pBuf->m_nBufferNumber == 0)
    {
        pBuf->m_nBufferNumber = 1;
    }
    else
    {
        pBuf->m_nBufferNumber = 0;
    }
	length = pBuf->m_nBufferLength[1-pBuf->m_nBufferNumber];
	for (i=0;i<length;i++)
	{
		ucBlock[i] = pBuf->m_ucDataBuffer[1 - pBuf->m_nBufferNumber][i];
	}
	pBuf->m_nBufferLength[1-pBuf->m_nBufferNumber] = 0;
	return length;
}

int PushDoubleBuffer(unsigned char ucByte, DoubleBuffer* pBuf)
{
	if (pBuf->m_nBufferLength[pBuf->m_nBufferNumber] < pBuf->m_nBufferSize)
	{
		pBuf->m_ucDataBuffer[pBuf->m_nBufferNumber][pBuf->m_nBufferLength[pBuf->m_nBufferNumber]] = ucByte;
		pBuf->m_nBufferLength[pBuf->m_nBufferNumber]++;
		return 1;
	}
	else
	{
		pBuf->m_ucErrorCode = BUFFER_ERROR_OVERLAB;
	}
	return 0;
}

void PushWordDoubleBuffer(unsigned short word, DoubleBuffer* pBuf)
{
	PushDoubleBuffer((unsigned char)(word&0xff), pBuf);
	PushDoubleBuffer((unsigned char)(word>>8), pBuf);
}

void PushBytesDoubleBuffer(unsigned char *pBytes, unsigned short nLength, DoubleBuffer* pBuf)
{
	int i;
	for (i=0; i<nLength; i++)
	{
		PushDoubleBuffer(pBytes[i], pBuf);
	}
}

unsigned short GetDoubleBufferLength(DoubleBuffer* pBuf)
{
	return pBuf->m_nBufferLength[pBuf->m_nBufferNumber];
}

unsigned char* GetDoubleBuffer(DoubleBuffer* pBuf)
{
	return pBuf->m_ucDataBuffer[pBuf->m_nBufferNumber];
}

void ClearDoubleBuffer(DoubleBuffer* pBuf)
{
	pBuf->m_nBufferLength[0] = 0;
	pBuf->m_nBufferLength[1] = 0;
	pBuf->m_nBufferNumber = 0;
	pBuf->m_ucErrorCode = BUFFER_NO_ERROR;
}

