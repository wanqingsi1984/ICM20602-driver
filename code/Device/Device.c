#include "main.h"
//#include "dvc_Led.h"
#include "Device.h"
#include "rs485.h"
#include "Message.h"
#include "msg_Qsens.h"
#include "EnhancedSTM32.h"
extern TIM_HandleTypeDef htim6;
extern TIM_HandleTypeDef htim7;

extern UART_HandleTypeDef huart1; //!< <-> Main Port
extern UART_HandleTypeDef huart3; //!< <-> Debug Port
extern SPI_HandleTypeDef hspi1;
extern SPI_HandleTypeDef hspi4;

uint8_t Addr_RS485 = 0;                        // RS485 physical address

uint8_t usart1_rxbuf[USART1_RXBUFF_SIZE] = {0};
uint8_t usart3_rxbuf[USART3_RXBUFF_SIZE] = {0};

uint16_t usart1_rxlen = 0;
uint16_t usart3_rxlen = 0;

uint8_t usart1_Rx_Data = 0;
uint8_t usart3_Rx_Data = 0;

uint8_t t10msRx;
RxStateDef mRxDataSta = MRX_STBY;
uint8_t RxDataStart = 0;
uint8_t RxBakBuf[200] = {0};
uint8_t SendEnFlag = 0;
uint8_t RecCommandFlag=0;
uint8_t usart3_txbuf[3600] = {0};
int make_crc32_table();
long make_crc(unsigned long crc, unsigned char *string, long size);
uint8_t make_crc_sum( unsigned char *string, long size);
void RcvDataProcessCal(void);

LED_DEFINE_TABLE_BEGIN
{ "Live", GPIO_PIN_5,GPIOE },
{ "IN",   GPIO_PIN_4,GPIOE },
{ "OUT1",  GPIO_PIN_3,GPIOE },
{ "OUT2",  GPIO_PIN_2,GPIOE },
LED_DEFINE_TABLE_END



CONSTRUCT_CIRCULAR_BUFFER_MEMORY_POOL(g_USART1_Buffer, 256);
//CONSTRUCT_CIRCULAR_BUFFER_MEMORY_POOL(g_USART2_Buffer, 256);
CONSTRUCT_CIRCULAR_BUFFER_MEMORY_POOL(g_USART3_Buffer, 256)

CONSTRUCT_DOUBLE_BUFFER_MEMORY_POOL(g_Usart1OutputBuffer, 1024);
//CONSTRUCT_DOUBLE_BUFFER_MEMORY_POOL(g_Usart2OutputBuffer, 1024);
CONSTRUCT_DOUBLE_BUFFER_MEMORY_POOL(g_Usart3OutputBuffer, 1024);
extern ParseParam g_Usart1Parse;
//extern ParseParam g_Usart2Parse;
extern ParseParam g_Usart3Parse;

XL_UART_IT_InputStreamTypeDef g_Usart1InputStream;
//XL_UART_IT_InputStreamTypeDef g_Usart2InputStream;
XL_UART_IT_InputStreamTypeDef g_Usart3InputStream;

XL_UART_DMA_OutputStreamTypeDef g_Usart1OutputStream;
//XL_UART_DMA_OutputStreamTypeDef g_Usart2OutputStream;
XL_UART_DMA_OutputStreamTypeDef g_Usart3OutputStream;
uint8_t usart1Temp;
//uint8_t usart2Temp;
uint8_t usart3Temp;


void InitDevice(void)
{	
	//	
    uint8_t ucTmpBuff[11] = {0x41,0x70,0x70,0x20,0x61,0x64,0x64,0x72,0x3A,0x00,0x00};
	// Read the input IO
	Addr_RS485 = 0;
	Addr_RS485 |=  HAL_GPIO_ReadPin(IN1_GPIO_Port, IN1_Pin);
	Addr_RS485 <<= 1;
	Addr_RS485 |=  HAL_GPIO_ReadPin(IN2_GPIO_Port, IN2_Pin);
	Addr_RS485 <<= 1;
	Addr_RS485 |=  HAL_GPIO_ReadPin(IN3_GPIO_Port, IN3_Pin);
	Addr_RS485 <<= 1;
	Addr_RS485 |=  HAL_GPIO_ReadPin(IN4_GPIO_Port, IN4_Pin);
	
	ucTmpBuff[9] = Addr_RS485 + 0x30;
	ucTmpBuff[10] = 0x0A;
    HAL_UART_Transmit_IT(&huart1, ucTmpBuff, 11);
	
	// RS485 RX
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_15, GPIO_PIN_RESET);
	
//	HAL_UART_Receive_IT(&huart3, &usart3_Rx_Data, 1);
	
	LED_TurnOnAll();
	ICM20602_Init();
    make_crc32_table();	
	HAL_TIM_Base_Start_IT(&htim6);
	//HAL_TIM_Base_Start_IT(&htim7);
	
	//
	g_Usart1InputStream.huart = &huart1;
	g_Usart1InputStream.pCircularBuffer = &g_USART1_Buffer;
	g_Usart1InputStream.pParseParam = &g_Usart1Parse;
	UART_LinkInputStream_IT(&g_Usart1InputStream);

//	g_Usart2InputStream.huart = &huart2;
//	g_Usart2InputStream.pCircularBuffer = &g_USART2_Buffer;
//	g_Usart2InputStream.pParseParam = &g_Usart2Parse;
//	UART_LinkInputStream_IT(&g_Usart2InputStream);
	
	g_Usart3InputStream.huart = &huart3;
	g_Usart3InputStream.pCircularBuffer = &g_USART3_Buffer;
	g_Usart3InputStream.pParseParam = &g_Usart3Parse;
	UART_LinkInputStream_IT(&g_Usart3InputStream);


	
	g_Usart1OutputStream.huart = &huart1;
	g_Usart1OutputStream.doublebuffer = &g_Usart1OutputBuffer;
//	g_Usart2OutputStream.huart = &huart2;
//	g_Usart2OutputStream.doublebuffer = &g_Usart2OutputBuffer;
    g_Usart3OutputStream.huart = &huart3;
	g_Usart3OutputStream.doublebuffer = &g_Usart3OutputBuffer;
	
}

unsigned char f10ms = 0;
unsigned char f10ms_2 = 0;
extern IMU_Type IMU_Data[100];
extern IMUData_1S_AvgDef ucAverage_1SBuf;
uint8_t ucTmpCnt=0;
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	int16_t accel[3],gyro[3],temp;
	if (htim->Instance == htim6.Instance)
	{
		///@todo 在这里补充输出函数
		f10ms = 1;
		
			//f10ms = 0;

		ICM20602_READ_Imu(&accel[0],&gyro[0],&temp);
	 //   ICM20602_Imu_datadeal(&accel[0],&gyro[0],&temp);
		
		IMU_Data[ucTmpCnt].Data.ACCEL_X=accel[0];
		IMU_Data[ucTmpCnt].Data.ACCEL_Y=accel[1];
		IMU_Data[ucTmpCnt].Data.ACCEL_Z=accel[2];
		IMU_Data[ucTmpCnt].Data.GYRO_X=gyro[0];
		IMU_Data[ucTmpCnt].Data.GYRO_Y=gyro[1]; 
		IMU_Data[ucTmpCnt].Data.GYRO_Z=gyro[2];
		IMU_Data[ucTmpCnt].Data.TEMP=temp;


		ucTmpCnt++;

	

		if(ucTmpCnt == 100)
		{
			ucTmpCnt = 0;
			Data_Store();
			ucAverage_1SBuf.Vaild = 1;


		}
		

	}
	else if (htim->Instance == htim7.Instance)
	{
		///@todo 在这里补充输出函数
		f10ms_2 = 1;

	}
}
void RcvDataProcess(void);
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	
	if (huart->Instance == USART1)
	{
		RecieveInputStream_IT(huart, &g_Usart1InputStream);
//		PushSyncCircularBuffer(usart1Temp, &g_USART1_Buffer);
//		HAL_UART_Receive_IT(&huart1, &usart1Temp, 1);
	}
	
	else if (huart->Instance == USART3)
	{
		RecieveInputStream_IT(huart, &g_Usart3InputStream);
//		PushSyncCircularBuffer(usart3Temp, &g_USART3_Buffer);
//		HAL_UART_Receive_IT(&huart3, &usart3Temp, 1);		
	}
	else
	{
	}
}

void SendDataProcess(void)
{
	if(SendEnFlag == 1)
	{
		// RS485 RX
		HAL_GPIO_WritePin(GPIOE, GPIO_PIN_15, GPIO_PIN_RESET);
		SendEnFlag = 0;
	//	HAL_UART_Receive_IT(&huart3, &(g_Usart3InputStream.ucTemp), 1);
	//	HAL_UART_Receive_IT(&huart3, &usart3_Rx_Data, 1);
	}
}
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
  /* Prevent unused argument(s) compilation warning */
 // UNUSED(huart);
  /* NOTE: This function should not be modified, when the callback is needed,
           the HAL_UART_TxCpltCallback can be implemented in the user file
   */ 
	if(huart->Instance == USART3 && SendEnFlag==1)
		{
			// RS485 RX
//		  HAL_Delay(5);
		  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_15, GPIO_PIN_RESET);
		  HAL_UART_Receive_IT(&huart3, &(g_Usart3InputStream.ucTemp), 1);
		  SendEnFlag=0;
//		  g_Usart3OutputBuffer.m_nBufferLength[1 - g_Usart3OutputBuffer.m_nBufferNumber] = 0;
		}
//	SendDataProcess();
}
unsigned long crc32_table[256];
int make_crc32_table()  
{  
    unsigned long c;  
    int i = 0;  
    int bit = 0;  
      
    for(i = 0; i < 256; i++)  
    {  
        c  = (unsigned long)i;  
          
        for(bit = 0; bit < 8; bit++)  
        {  
            if(c&1)  
            {  
				//0xEDB88320//0x4C11DB7
                c = (c >> 1)^(0xEDB88320);  
            }  
            else  
            {  
                c =  c >> 1;  
            }  
              
        }  
        crc32_table[i] = c;  
    }  
      
  return 0;
}  
  
long make_crc(unsigned long crc, unsigned char *string, long size)  
{  
  
    while(size--)  
        crc = (crc >> 8)^(crc32_table[(crc ^ *string++)&0xff]);  
  
    return crc^0xFFFFFFFF;  
} 
uint8_t make_crc_sum( unsigned char *string, long size)  
{  
  uint8_t i=0,crc_sum=0;
   for(i = 0;i < size;i++)
   {
		crc_sum += string[i];
   }  
   return crc_sum;
} 
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
	if (HAL_UART_GetError(huart) == HAL_UART_ERROR_ORE)
	{
		huart->RxState = HAL_UART_STATE_READY;
	}
	else if (HAL_UART_GetError(huart) == HAL_UART_ERROR_DMA)
	{
		huart->RxState = HAL_UART_STATE_READY;
	}
	else
	{
	}

	if (huart->Instance == USART1)
	{
		HAL_UART_Receive_IT(&huart1, &(g_Usart1InputStream.ucTemp), 1);
	//	HAL_UART_Receive_IT(&huart1, &usart1Temp, 1);
	}
	else if (huart->Instance == USART3)
	{
		HAL_UART_Receive_IT(&huart3, &(g_Usart3InputStream.ucTemp), 1);
		//HAL_UART_Receive_IT(&huart3, &usart3Temp, 1);
	}
	else
	{
	}
}

int fputc(int ch, FILE* f)
{
	HAL_UART_Transmit(&huart3, (uint8_t*)&ch, 1, 100);
	return ch;
}

unsigned char rxCpltFlag = 0;
void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi)
{
	if(hspi->Instance == SPI1)
	{
		
	}
}

void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi)
{
	if(hspi->Instance == SPI1)
	{
		
	}
}

void HAL_SPI_ErrorCallback(SPI_HandleTypeDef *hspi)
{
	if(hspi->Instance == SPI1)
	{
	}
}
