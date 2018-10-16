/* USER CODE BEGIN Private defines */
#include "Global.h"
#define USART1_RXBUFF_SIZE             200
#define USART3_RXBUFF_SIZE             200
#define I2C1_RXBUFF_SIZE               100  

#define USART2_TXBUFF_SIZE             200 
#define FLASH_App_START_ADDR    ((uint32_t)0x08020000)
#define CMD_F1               0xEA      // ??1
#define CMD_F2               0x51      // ??2

typedef struct
{
	int8_t Vaild;
	int32_t ACCEL_X;
	int32_t ACCEL_Y;
	int32_t ACCEL_Z;
	int32_t TEMP;
	int32_t GYRO_X;
	int32_t GYRO_Y;
	int32_t GYRO_Z;	
}CollectIMUDataDef;

typedef struct
{
	int8_t Vaild;
	float ACCEL_X;
	float ACCEL_Y;
	float ACCEL_Z;
	float TEMP;
	float GYRO_X;
	float GYRO_Y;
	float GYRO_Z;	
}IMUData_1S_AvgDef;

typedef enum 
{
	MRX_STBY = 0x01,
  MRX_DATA,
  MRX_Cal,
  MRX_Read_Cal,
  MRX_ERROR
} RxStateDef;

extern uint8_t RxDataStart;
extern uint8_t RxBakBuf[200];
extern uint8_t t10msRx;
extern RxStateDef mRxDataSta;
extern uint8_t SendEnFlag;
extern uint8_t usart3_txbuf[3600];
extern uint8_t usart1_rxbuf[USART1_RXBUFF_SIZE];
extern uint8_t usart3_rxbuf[USART3_RXBUFF_SIZE];

extern uint16_t usart1_rxlen;
extern uint16_t usart3_rxlen;

extern uint8_t usart1_Rx_Data;
extern uint8_t usart3_Rx_Data;

/* USER CODE END Private defines */

extern void _Error_Handler(char *, int);

void MX_USART1_UART_Init(void);
void MX_USART3_UART_Init(void);

/* USER CODE BEGIN Prototypes */
void SendDataProcess(void);
void RcvDataProcess(void);
void Uart3ErrHandle(void);
void Data_Analytic(unsigned char *pData, unsigned char DataLen);
void Data_Response(unsigned char Write_Result);
void Data_Cal(unsigned char *pData, unsigned char DataLen);
void Data_Store(void);
/* USER CODE END Prototypes */
