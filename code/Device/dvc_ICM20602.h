/**
  ******************************************************************************
  * @file    ICM20948.h
  * @author  liun@xlnav.com
  * @version V1.0
  * @date    27-June-2018
  * @brief   Header file for ICM20948.c module.
  
  ******************************************************************************
  */


#ifndef __ICM20602_H
#define __ICM20602_H
#include "Global.h"
// define ICM20948 register address
//****************************************
#define ADDRESS_AD0_LOW     0xD0 //address pin low (GND), default for InvenSense evaluation board
#define ADDRESS_AD0_HIGH    0xD1 //address pin high (VCC)

#define	GYRO_ADDRESS   ADDRESS_AD0_LOW	  //Gyro and Accel device address
#define ACCEL_ADDRESS  ADDRESS_AD0_LOW 
#define DEFAULT_ADDRESS  GYRO_ADDRESS




//Register map
#define	WHO_AM_I		0x75	//identity of the device
#define WHO_AM_I_VAL	0x12 //identity of ICM20948 is 0xEA.

#define	ACCEL_XOUT_H	0x3B
#define	ACCEL_XOUT_L	0x3C
#define	ACCEL_YOUT_H	0x3D
#define	ACCEL_YOUT_L	0x3E
#define	ACCEL_ZOUT_H	0x3F
#define	ACCEL_ZOUT_L	0x40

#define	TEMP_OUT_H		0x41
#define	TEMP_OUT_L		0x42

#define	GYRO_XOUT_H		0x43
#define	GYRO_XOUT_L		0x44	
#define	GYRO_YOUT_H		0x45
#define	GYRO_YOUT_L		0x46
#define	GYRO_ZOUT_H		0x47
#define	GYRO_ZOUT_L		0x48


#define	PWR_MGMT_1		0x6B	//Power Management 1. Typical values:0x00(run mode)
#define	PWR_MGMT_2		0x6C	//Power Management 2. Typical values:0x00(run mode)
#define PWR1_RUN_MODE 		0x01
#define PWN2_DISABLE_ACCE 	0x38
#define PWN2_DISABLE_GYRO 	0x07
#define PWN2_ENABLE_ACCE	0x00
#define PWN2_ENABLE_GYRO	0x00


#define	GYRO_CONFIG	  0x1B	//Gyro Full Scale Select. Typical values:0x10(1000dps)
#define	GYRO_250DPS     0x00
#define	GYRO_500DPS     0x08
#define	GYRO_1000DPS    0x10
#define	GYRO_2000DPS    0x18
#define GYRO_DLPF_ENABLE  0x00

#define CONFIG		0x1A
#define GYRO_DLPF0_NBW307HZ		  0x00
#define GYRO_DLPF1_NBW177HZ		  0x01
#define GYRO_DLPF2_NBW109HZ		  0x02
#define GYRO_DLPF3_NBW59HZ		  0x03
#define GYRO_DLPF4_NBW31HZ		  0x04
#define GYRO_DLPF5_NBW16HZ		  0x05
#define GYRO_DLPF6_NBW8HZ		    0x06
#define GYRO_DLPF7_NBW3451HZ		0x07

#define	ACCEL_CONFIG	0x1C	//Accel Full Scale Select. Typical values:0x01(2g)
#define ACCEL_2G      	0x00
#define ACCEL_4G      	0x08
#define ACCEL_8G		    0x10
#define ACCEL_16G       0x18

#define ACCEL_CONFIG2		0x1D
#define ACCEL_DLPF_DISABLE		0x08
#define ACCEL_DLPF0_NBW235HZ	0x00
#define ACCEL_DLPF1_NBW235HZ	0x01
#define ACCEL_DLPF2_NBW121HZ	0x02
#define ACCEL_DLPF3_NBW62HZ		0x03
#define ACCEL_DLPF4_NBW31HZ		0x04
#define ACCEL_DLPF5_NBW16HZ		0x05
#define ACCEL_DLPF6_NBW8HZ		0x06
#define ACCEL_DLPF7_NBW442HZ	0x07

#define	SMPLRT_DIV		0x19	//Sample Rate Divider. Typical values:0x07(125Hz) 1KHz internal sample rate=1k/(1+SMPLRT_DIV)
#define SAMPLE_RATE_1000HZ	0x00
#define SAMPLE_RATE_500HZ	  0x01
#define SAMPLE_RATE_200HZ	  0x04
#define SAMPLE_RATE_125HZ	  0x07
#define SAMPLE_RATE_100HZ	  0x09
#define SAMPLE_RATE_50HZ	  0x13
#define SAMPLE_RATE_20HZ	  0x31


typedef union
{
    struct
    {
			int16_t ACCEL_X;
			int16_t ACCEL_Y;
			int16_t ACCEL_Z;
			int16_t TEMP;
			int16_t GYRO_X;
			int16_t GYRO_Y;
			int16_t GYRO_Z;
//			int16_t MAG_X;
//			int16_t MAG_Y;
//			int16_t MAG_Z;
    }Data;

    struct
    {
      uint8_t  Data[20];   
      
    }Buf;
	
}IMU_Type;
void ICM20602_Init(void);
//void ICM20948_READ_MAG(int16_t magn[3]);
void ICM20602_READ_Imu(int16_t accel[3], int16_t gyro[3],int16_t *temp);
void ICM20602_Imu_datadeal(int16_t accel[3], int16_t gyro[3],int16_t *temp);
#endif
