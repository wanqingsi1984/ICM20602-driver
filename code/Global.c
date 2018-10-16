/**
  ******************************************************************************
  * @file Global.c
  * @author  qiaoyy@xlnav.com
  * @version V1.0
  * @date 2018-06-01
  * @brief   全局宏、常量、类型和函数
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
	pBuf->m_ulPushTimes = 0;
	pBuf->m_ulPopTimes = 0;
	pBuf->m_nBufferHeadPosition = 0;
	pBuf->m_nBufferTailPosition = 0;
	pBuf->m_nBufferLength = 0;
	pBuf->m_ucLock = 0;
	pBuf->m_ucErrorCode = BUFFER_NO_ERROR;
	for (j=0; j<pBuf->m_nBufferSize; j++)
	{
		pBuf->m_ucDataBuffer[j] = 0x00;
	}
}

int PushCircularBuffer(uint8_t ucByte, CircularBuffer* pBuf)
{
	if (pBuf->m_ulPushTimes >= pBuf->m_ulPopTimes)
	{
		pBuf->m_nBufferLength = pBuf->m_ulPushTimes - pBuf->m_ulPopTimes;
	}
	else
	{
		pBuf->m_ucErrorCode = BUFFER_ERROR_POP_MORE_THAN_PUSH;
		return 0;
	}
    if (pBuf->m_nBufferLength<pBuf->m_nBufferSize)
    {
        pBuf->m_ucDataBuffer[pBuf->m_nBufferTailPosition] = ucByte;
        pBuf->m_nBufferTailPosition++;
		pBuf->m_ulPushTimes++;

        if (pBuf->m_nBufferTailPosition >= pBuf->m_nBufferSize)
        {
            pBuf->m_nBufferTailPosition = 0;
			
			// 参数一致性检查
			if (pBuf->m_nBufferTailPosition > pBuf->m_nBufferHeadPosition)
			{
				if ((pBuf->m_ulPushTimes - pBuf->m_ulPopTimes) != (pBuf->m_nBufferTailPosition - pBuf->m_nBufferHeadPosition))
				{
					pBuf->m_ucErrorCode = BUFFER_ERROR_PARAM_NO_MATCH;
				}
			}
			else if (pBuf->m_nBufferTailPosition < pBuf->m_nBufferHeadPosition)
			{
				if ((pBuf->m_ulPushTimes - pBuf->m_ulPopTimes) != (pBuf->m_nBufferSize + pBuf->m_nBufferTailPosition - pBuf->m_nBufferHeadPosition))
				{
					pBuf->m_ucErrorCode = BUFFER_ERROR_PARAM_NO_MATCH;
				}
			}
			else
			{
				
			}
			if (pBuf->m_ucLock!=0x01)
			{
				// 计数器预防过零
				while ((pBuf->m_ulPushTimes> pBuf->m_nBufferSize) && (pBuf->m_ulPopTimes > pBuf->m_nBufferSize))
				{
					pBuf->m_ulPushTimes -= pBuf->m_nBufferSize;
					pBuf->m_ulPopTimes -= pBuf->m_nBufferSize;
				}
			}
        }
    }
	else
	{
		pBuf->m_ucErrorCode = BUFFER_ERROR_OVERLAB;
		return 0;
	}
	return 1;
}

int PopCircularBuffer(uint8_t *ucByte, CircularBuffer* pBuf)
{
	
	if (pBuf->m_ulPushTimes >= pBuf->m_ulPopTimes)
	{
		pBuf->m_nBufferLength = pBuf->m_ulPushTimes - pBuf->m_ulPopTimes;
	}
	else
	{
		pBuf->m_ucErrorCode = BUFFER_ERROR_POP_MORE_THAN_PUSH;
		return 0;
	}
    if (pBuf->m_nBufferLength > 0)
    {
        *ucByte = pBuf->m_ucDataBuffer[pBuf->m_nBufferHeadPosition];
        pBuf->m_nBufferHeadPosition++;
		pBuf->m_ucLock = 0x01;
		pBuf->m_ulPopTimes++;
		pBuf->m_ucLock = 0x00;

        if (pBuf->m_nBufferHeadPosition >= pBuf->m_nBufferSize)
        {
            pBuf->m_nBufferHeadPosition = 0;
        }
        return 1;       
    }
    return 0;
}

int PopDoubleBuffer(uint8_t *ucBlock, DoubleBuffer* pBuf)
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

int PushDoubleBuffer(uint8_t ucByte, DoubleBuffer* pBuf)
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

void PushWordDoubleBuffer(uint16_t word, DoubleBuffer* pBuf)
{
	PushDoubleBuffer((uint8_t)(word&0xff), pBuf);
	PushDoubleBuffer((uint8_t)(word>>8), pBuf);
}

void PushBytesDoubleBuffer(uint8_t *pBytes, uint16_t nLength, DoubleBuffer* pBuf)
{
	int i;
	for (i=0; i<nLength; i++)
	{
		PushDoubleBuffer(pBytes[i], pBuf);
	}
}

uint16_t GetDoubleBufferLength(DoubleBuffer* pBuf)
{
	return pBuf->m_nBufferLength[pBuf->m_nBufferNumber];
}

uint8_t* GetDoubleBuffer(DoubleBuffer* pBuf)
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

void ClearSyncCircularBuffer(SyncCircularBuffer* pBuf)
{
    int j;
	pBuf->m_ulPushTimes = 0;
	pBuf->m_ulPopTimes = 0;
	pBuf->m_nBufferHeadPosition = 0;
	pBuf->m_nBufferTailPosition = 0;
	pBuf->m_nBufferLength = 0;
	pBuf->m_ucLock = 0;
	pBuf->m_ucErrorCode = BUFFER_NO_ERROR;
	for (j=0; j<pBuf->m_nBufferSize; j++)
	{
		pBuf->m_ucDataBuffer[j] = 0x00;
		pBuf->m_ucSync[j] = 0x00;
	}
}

int PushSyncCircularBuffer(uint8_t ucByte, SyncCircularBuffer* pBuf)
{
	if (pBuf->m_ulPushTimes >= pBuf->m_ulPopTimes)
	{
		pBuf->m_nBufferLength = pBuf->m_ulPushTimes - pBuf->m_ulPopTimes;
	}
	else
	{
		pBuf->m_ucErrorCode = BUFFER_ERROR_POP_MORE_THAN_PUSH;
		return 0;
	}
    if (pBuf->m_nBufferLength<pBuf->m_nBufferSize)
    {
        pBuf->m_ucDataBuffer[pBuf->m_nBufferTailPosition] = ucByte;
        pBuf->m_nBufferTailPosition++;
		pBuf->m_ulPushTimes++;
		if ( pBuf->m_nBufferTailPosition != pBuf->m_ulPushTimes)
		{
			pBuf->m_ucErrorCode = BUFFER_ERROR_PARAM_NO_MATCH;
		}
        if (pBuf->m_nBufferTailPosition >= pBuf->m_nBufferSize)
        {
            pBuf->m_nBufferTailPosition = 0;
			
			// 参数一致性检查
			if (pBuf->m_nBufferTailPosition > pBuf->m_nBufferHeadPosition)
			{
				if ((pBuf->m_ulPushTimes - pBuf->m_ulPopTimes) != (pBuf->m_nBufferTailPosition - pBuf->m_nBufferHeadPosition))
				{
					pBuf->m_ucErrorCode = BUFFER_ERROR_PARAM_NO_MATCH;
				}
			}
			else if (pBuf->m_nBufferTailPosition < pBuf->m_nBufferHeadPosition)
			{
				if ((pBuf->m_ulPushTimes - pBuf->m_ulPopTimes) != (pBuf->m_nBufferSize + pBuf->m_nBufferTailPosition - pBuf->m_nBufferHeadPosition))
				{
					pBuf->m_ucErrorCode = BUFFER_ERROR_PARAM_NO_MATCH;
				}
			}
			else
			{
				
			}
			if (pBuf->m_ucLock!=0x01)
			{
				// 计数器预防过零
				while ((pBuf->m_ulPushTimes> pBuf->m_nBufferSize) && (pBuf->m_ulPopTimes > pBuf->m_nBufferSize))
				{
					pBuf->m_ulPushTimes -= pBuf->m_nBufferSize;
					pBuf->m_ulPopTimes -= pBuf->m_nBufferSize;
				}
			}
        }
    }
	else
	{
		pBuf->m_ucErrorCode = BUFFER_ERROR_OVERLAB;
		return 0;
	}
	return 1;
}

int PopSyncCircularBuffer(uint8_t *ucByte, SyncCircularBuffer* pBuf, uint8_t *ucSync)
{
	if (pBuf->m_ulPushTimes >= pBuf->m_ulPopTimes)
	{
		pBuf->m_nBufferLength = pBuf->m_ulPushTimes - pBuf->m_ulPopTimes;
	}
	else
	{
		pBuf->m_ucErrorCode = BUFFER_ERROR_POP_MORE_THAN_PUSH;
		return 0;
	}
    if (pBuf->m_nBufferLength > 0)
    {
		*ucSync = pBuf->m_ucSync[pBuf->m_nBufferHeadPosition];
		pBuf->m_ucSync[pBuf->m_nBufferHeadPosition] = 0x00;
        *ucByte = pBuf->m_ucDataBuffer[pBuf->m_nBufferHeadPosition];
        pBuf->m_nBufferHeadPosition++;
		pBuf->m_ucLock = 0x01;
		pBuf->m_ulPopTimes++;
		pBuf->m_ucLock = 0x00;

        if (pBuf->m_nBufferHeadPosition >= pBuf->m_nBufferSize)
        {
            pBuf->m_nBufferHeadPosition = 0;
        }
        return 1;       
    }
    return 0;
}

void MarkSyncCircularBuffer(uint8_t deviceID, SyncCircularBuffer* pBuf)
{
	int markPos = pBuf->m_nBufferTailPosition;
	if (markPos==0)
	{
		markPos = pBuf->m_nBufferSize-1;
	}
	else
	{
		markPos -= 1;
	}
	
	pBuf->m_ucSync[markPos] = 0x01;
}
