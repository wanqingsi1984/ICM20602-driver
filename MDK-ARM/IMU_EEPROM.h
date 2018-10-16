#include "main.h"
#include "stm32f1xx_hal.h"
void write_uint32(uint32_t* pdata,uint8_t len, uint16_t WriteAddress);
void read_uint32(uint32_t* pdata,uint8_t len, uint16_t ReadAddress);
void write_float(float* pdata,uint8_t len, uint16_t WriteAddress);
void read_float(float* pdata,uint8_t len, uint16_t ReadAddress);
typedef union
{
    struct
    {
			uint32_t Table_Type;
		    uint32_t Product_ID[2];
		    uint32_t SN[3];
			uint32_t Cal_Date;
			uint32_t Cal_time;
		    uint32_t Cal_ID;
			float    Gyro_Temp0[3];
			float	 Gyro_Temp1[3];
		    float	 Gyro_Install_Err[6];
			float    Gyro_Scale_Err[3];
			float	 Gyro_Zero_Offset[3];
			float    ACC_Temp0[3];
			float	 ACC_Temp1[3];
		    float	 ACC_Install_Err[6];
			float    ACC_Scale_Err[3];
			float	 ACC_Zero_Offset[3];
		    float    Cal_g;
		    uint32_t CRC32;
		
			
    }Data;

    struct
    {
      uint8_t  Data[188];   
      
    }Buf;
	
}Cal_Data;