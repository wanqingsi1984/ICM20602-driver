#include "dvc_AT24C01.h"
#include "IMU_EEPROM.h"
/* 
 * 函数名: write_byte(uint8_t* pdata,uint8_t len, uint16_t WriteAddress)
 * 描述  : 写入1串字节数据 
 * 输入  : pdata : 要写入数据缓冲区指针
           len : 待写入长度  
           WriteAddress : 写入地址
                     
 * 输出  : 无
                     
 * 说明  : 跨页写入1串数据
 */
 void write_byte(uint8_t* pdata,uint8_t len, uint16_t WriteAddress)
 {
	 I2C_PageWrite(pdata,len, WriteAddress,0xA0);
 }
 /* 
 * 函数名: void write_uint32(uint32_t* pdata,uint8_t len, uint16_t WriteAddress)
 * 描述  : 写入1串长整型数据 
 * 输入  : pdata : 要写入数据缓冲区指针
           len : 待写入长度  
           WriteAddress : 写入地址
                     
 * 输出  : 无
                     
 * 说明  : 跨页写入1串数据
 */
  void write_uint32(uint32_t* pdata,uint8_t len, uint16_t WriteAddress)
 {
	uint8_t *pf;
	pf=(uint8_t *)pdata;
    I2C_PageWrite(pf,len*4, WriteAddress,0xA0);
 }
  /* 
 * 函数名: void write_float(uint32_t* float,uint8_t len, uint16_t WriteAddress)
 * 描述  : 写入1串浮点型数据 
 * 输入  : pdata : 要写入数据缓冲区指针
           len : 待写入长度  
           WriteAddress : 写入地址
                     
 * 输出  : 无
                     
 * 说明  : 跨页写入1串数据
 */
  void write_float(float* pdata,uint8_t len, uint16_t WriteAddress)
 {
	uint8_t *pf;
	pf=(uint8_t *)pdata;
    I2C_PageWrite(pf,len*4, WriteAddress,0xA0);
 }
  /* 
 * 函数名: write_double(double* pdata,uint8_t len, uint16_t WriteAddress)
 * 描述  : 写入1串双精度数据 
 * 输入  : pdata : 要写入数据缓冲区指针
           len : 待写入长度  
           WriteAddress : 写入地址
                     
 * 输出  : 无
                     
 * 说明  : 跨页写入1串数据
 */
   void write_double(double* pdata,uint8_t len, uint16_t WriteAddress)
 {
	uint8_t *pf;
	pf=(uint8_t *)pdata;
    I2C_PageWrite(pf,len*8, WriteAddress,0xA0);
 }
 /* 
 * 函数名: read_byte(uint8_t* pdata,uint8_t len, uint16_t ReadAddress)
 * 描述  : 读取1串字节数据 
 * 输入  : pdata : 要读取数据缓冲区指针
           len : 待读取的长度  
           ReadAddress : 读取地址
                     
 * 输出  : 无
                     
 * 说明  : 无
 */
 void read_byte(uint8_t* pdata,uint8_t len, uint16_t ReadAddress)
 {
	 I2C_ReadByte(pdata,len,ReadAddress,0xA0);
 }
 /* 
 * 函数名: void read_uint32(uint32_t* pdata,uint8_t len, uint16_t ReadAddress)
 * 描述  : 读取1串长整型数据 
 * 输入  : pdata : 要读取数据缓冲区指针
           len : 待读取的长度  
           ReadAddress : 读取地址
                     
 * 输出  : 无
                     
 * 说明  : 无
 */
  void read_uint32(uint32_t* pdata,uint8_t len, uint16_t ReadAddress)
 {
	uint8_t *pf;
	pf=(uint8_t *)pdata;
    I2C_ReadByte(pf,len*4,ReadAddress,0xA0);
 }
 /* 
 * 函数名: void read_float(float* pdata,uint8_t len, uint16_t ReadAddress)
 * 描述  : 读取1串长整型数据 
 * 输入  : pdata : 要读取数据缓冲区指针
           len : 待读取的长度  
           ReadAddress : 读取地址
                     
 * 输出  : 无
                     
 * 说明  : 无
 */
  void read_float(float* pdata,uint8_t len, uint16_t ReadAddress)
 {
	uint8_t *pf;
	pf=(uint8_t *)pdata;
    I2C_ReadByte(pf,len*4,ReadAddress,0xA0);
 }
 /* 
 * 函数名: read_double(double* pdata,uint8_t len, uint16_t ReadAddress)
 * 描述  : 读取双精度数据 
 * 输入  : pdata : 要读取数据缓冲区指针
           len : 待读取的长度  
           ReadAddress : 读取地址
                     
 * 输出  : 无
                     
 * 说明  : 无
 */
   void read_double(double* pdata,uint8_t len, uint16_t ReadAddress)
 {
	uint8_t *pf;
	pf=(uint8_t *)pdata;
    I2C_ReadByte(pf,len*8, ReadAddress,0xA0);
 }