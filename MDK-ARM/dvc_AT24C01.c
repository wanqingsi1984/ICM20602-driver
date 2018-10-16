#include "dvc_AT24C01.h"
#define TRUE 1
#define FALSE 0

     
//extern void HAL_Delay(__IO uint32_t nTime);
     
/* 
 * 函数名: SDA_OUT(void)
 * 描述  : 配置SDA引脚为输出引脚
 * 输入  : 无
 * 输出  : 无
 * 说明  : 无
 */
void SDA_OUT(void)
{   
  GPIO_InitTypeDef GPIO_InitStruct;
  GPIO_InitStruct.Pin = I2C_SDA_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct); 
}
/* 
 * 函数名: SDA_IN(void)
 * 描述  : 配置SDA引脚为输入引脚
 * 输入  : 无
 * 输出  : 无
 * 说明  : 无
 */
void SDA_IN(void)
{   
  GPIO_InitTypeDef GPIO_InitStruct;
  GPIO_InitStruct.Pin = I2C_SDA_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);  
}
     
/* 
 * 函数名: void I2C_delay(void)
 * 描述  : 短暂延时
 * 输入  : 无
 * 输出  : 无
 * 说明  : 内部定义的i可以优化速度，经测试最低到5还能写入
 */
static void I2C_delay(void)
{   
    uint8_t i=100; 
    while(i) 
    { 
        i--; 
    } 
}
     
/* 
 * 函数名: uint8_t I2C_Start(void)
 * 描述  : 起始信号
 * 输入  : 无
 * 输出  : TRUE : 成功
                     FALSE : 失败
 * 说明  : 
 */
static uint8_t I2C_Start(void)
{
    SDA_OUT();
	SDA_H;
    SCL_H;
    I2C_delay();
//	SDA_IN();
//    if(!SDA_read)
//        return FALSE;   //SDA线为低电平则总线忙,退出
//  SDA_OUT();
    SDA_L;
    I2C_delay();
//  SDA_IN();
//    if(SDA_read) 
//        return FALSE;   //SDA线为高电平则总线出错,退出
//    SDA_L;
	SCL_L;
    I2C_delay();
    return TRUE;
}
/* 
 * 函数名: static void I2C_Stop(void)
 * 描述  : 终止信号
 * 输入  : 无
 * 输出  : 无
 * 说明  : 
 */
static void I2C_Stop(void)
{
    SCL_L;
    I2C_delay();
	SDA_OUT();
    SDA_L;
    I2C_delay();
    SCL_H;
    I2C_delay();
    SDA_H;
    I2C_delay();
}
/* 
 * 函数名: static void I2C_Ack(void)
 * 描述  : 应答信号
 * 输入  : 无
 * 输出  : 无
 * 说明  : 
 */
static void I2C_Ack(void)
{   
    SCL_L;
    I2C_delay();
	SDA_OUT();
    SDA_L;
    I2C_delay();
    SCL_H;
    I2C_delay();
    SCL_L;
    I2C_delay();
}
/* 
 * 函数名: void I2C_NoAck(void)
 * 描述  : 无应答信号
 * 输入  : 无
 * 输出  : 无
 * 说明  : 
 */
static void I2C_NoAck(void)
{   
	SCL_L;
    I2C_delay();
	SDA_OUT();
    SDA_H;
    I2C_delay();
    SCL_H;
    I2C_delay();
    SCL_L;
    I2C_delay();
}
/* 
 * 函数名: uint8_t I2C_Start(void)
 * 描述  : 等待应答信号
 * 输入  : 无
 * 输出  : TRUE : 有应答
                     FALSE : 无应答
 * 说明  : 
 */
static uint8_t I2C_WaitAck(void)   
{
  //  SDA_OUT();
	uint8_t uscount=0;
	SCL_L;
    I2C_delay();
    SDA_H;          
    I2C_delay();
	SDA_IN();
    SCL_H;
    I2C_delay();

//    if(SDA_read)
//    {
//        SCL_L;
//        return FALSE;
//    }
	while(SDA_read)
	{
		uscount++;
		if(uscount>250)
		{
	       SCL_L;
           return FALSE;
		}
	}
    SCL_L;
//	SDA_OUT();
    return TRUE;
}
/* 
 * 函数名: static void I2C_SendByte(uint8_t SendByte) 
 * 描述  : 发送一个字节
 * 输入  : SendByte : 字节数据
 * 输出  : 无
 * 说明  : 数据从高位到低位
 */
static void I2C_SendByte(uint8_t SendByte) 
{
    uint8_t i=8;
	SDA_OUT();
    while(i--)
    {
        SCL_L;
        I2C_delay();
        if(SendByte&0x80)
            SDA_H;  
        else
            SDA_L;   
        SendByte<<=1;
        I2C_delay();
        SCL_H;
        I2C_delay();
    }
    SCL_L;
}
/* 
 * 函数名: static uint8_t I2C_ReceiveByte(void) 
 * 描述  : 读取一个字节
 * 输入  : 无 
 * 输出  : 字节数据
 * 说明  : ReceiveByte : 数据从高位到低位
 */
static uint8_t I2C_ReceiveByte(void)  
{ 
    uint8_t i=8;
    uint8_t ReceiveByte=0;
//    SDA_OUT();     
//    SDA_H;  
    SDA_IN();	
    while(i--)
    {
        ReceiveByte<<=1;      
        SCL_L;
        I2C_delay();
        SCL_H;
        I2C_delay();    
        if(SDA_read)
        {
            ReceiveByte|=0x01;
        }
    }
    SCL_L;
    return ReceiveByte;
}
     
/* 
 * 函数名: uint8_t I2C_WriteByte(uint8_t SendByte, uint16_t WriteAddress, uint8_t DeviceAddress)
 * 描述  : 写入1字节数据  
 * 输入  : SendByte : 要写入数据
                     WriteAddress : 写入地址
                     DeviceAddress : 器件地址
 * 输出  : TRUE : 成功
                     FALSE : 失败
 * 说明  : 
 */
uint8_t I2C_WriteByte(uint8_t SendByte, uint16_t WriteAddress, uint8_t DeviceAddress)
{       
    if(!I2C_Start())
            return FALSE;
  //  I2C_SendByte(((WriteAddress & 0x0700) >>7) | DeviceAddress & 0xFFFE);//设置高起始地址+器件地址
    I2C_SendByte(DeviceAddress & 0xFE);//发送器件写地址 	
    if(!I2C_WaitAck())
        {
            I2C_Stop(); 
            return FALSE;
        }
    I2C_SendByte((uint8_t)(WriteAddress & 0x00FF));   //设置低起始地址      
    I2C_WaitAck();  
    I2C_SendByte(SendByte);
    I2C_WaitAck();   
    I2C_Stop(); 
         
    HAL_Delay(10);//注意：因为这里要等待EEPROM写完，可以采用查询或延时方式(10ms)
         
        return TRUE;
}
/* 
 * 函数名: uint8_t I2C_WriteByte(uint8_t SendByte, uint16_t WriteAddress, uint8_t DeviceAddress)
 * 描述  : 写入1串数据 
 * 输入  : pBuffer : 要写入数据缓冲区指针
           length : 待写入长度  
                     WriteAddress : 写入地址
                     DeviceAddress : 器件地址
 * 输出  : TRUE : 成功
                     FALSE : 失败
 * 说明  : 注意不能跨页写
 */
uint8_t I2C_BufferWrite(uint8_t* pBuffer, uint8_t length, uint16_t WriteAddress, uint8_t DeviceAddress)
{
    if(!I2C_Start())
            return FALSE;
    //I2C_SendByte(((WriteAddress & 0x0700) >>7) | DeviceAddress & 0xFFFE);//设置高起始地址+器件地址
    I2C_SendByte(DeviceAddress & 0xFE);//发送器件写地址
    if(!I2C_WaitAck())
        {
            I2C_Stop(); 
            return FALSE;
        }
    I2C_SendByte((uint8_t)(WriteAddress & 0x00FF));   //设置低起始地址      
        I2C_WaitAck();  
        while(length--)
        {
            I2C_SendByte(* pBuffer);
            I2C_WaitAck();
            pBuffer++;
        }
        I2C_Stop();
             
        HAL_Delay(10);//注意：因为这里要等待EEPROM写完，可以采用查询或延时方式(10ms)
             
        return TRUE;
}
     
/* 
 * 函数名: uint8_t I2C_WriteByte(uint8_t SendByte, uint16_t WriteAddress, uint8_t DeviceAddress)
 * 描述  : 写入1串数据 
 * 输入  : pBuffer : 要写入数据缓冲区指针
           length : 待写入长度  
                     WriteAddress : 写入地址
                     DeviceAddress : 器件地址
 * 输出  : TRUE : 成功
                     FALSE : 失败
 * 说明  : 跨页写入1串数据
 */
void I2C_PageWrite(  uint8_t* pBuffer, uint8_t length, uint16_t WriteAddress, uint8_t DeviceAddress)
{
    uint8_t NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0;
    Addr  = WriteAddress % I2C_PageSize;      //写入地址是开始页的第几位
    count = I2C_PageSize - Addr;                        //在开始页要写入的个数
    NumOfPage   =  length / I2C_PageSize;     //要写入的页数
    NumOfSingle =  length % I2C_PageSize;     //不足一页的个数
         
    if(Addr == 0)         //写入地址是页的开始 
    {
        if(NumOfPage == 0)  //数据小于一页 
        {
            I2C_BufferWrite(pBuffer,NumOfSingle,WriteAddress,DeviceAddress);   //写少于一页的数据
        }
        else                    //数据大于等于一页  
        {
            while(NumOfPage)//要写入的页数
            {
                I2C_BufferWrite(pBuffer,I2C_PageSize,WriteAddress,DeviceAddress);//写一页的数据
                WriteAddress +=  I2C_PageSize;
                pBuffer      +=  I2C_PageSize;
                NumOfPage--;
                HAL_Delay(10);
            }
            if(NumOfSingle!=0)//剩余数据小于一页
            {
                I2C_BufferWrite(pBuffer,NumOfSingle,WriteAddress,DeviceAddress); //写少于一页的数据
                HAL_Delay(10);
            }
        }
    }
         
    else                  //写入地址不是页的开始 
    {
        //if(NumOfPage== 0)   //数据小于一页 
		if(NumOfPage==0)	//byliun
        {
            if(NumOfSingle<=count)
			    I2C_BufferWrite(pBuffer,NumOfSingle,WriteAddress,DeviceAddress);   //写少于一页的数据
			else
			{
				I2C_BufferWrite(pBuffer,count,WriteAddress,DeviceAddress);      //将开始的空间写满一页
				WriteAddress += count;
				pBuffer      += count;
				I2C_BufferWrite(pBuffer,(NumOfSingle-count),WriteAddress,DeviceAddress);      //将开始的空间写满一页
			}
        }
        else                //数据大于等于一页
        {
            length       -= count;
            NumOfPage     = length / I2C_PageSize;  //重新计算要写入的页数
            NumOfSingle   = length % I2C_PageSize;  //重新计算不足一页的个数   
                 
            if(count != 0)
            {  
                I2C_BufferWrite(pBuffer,count,WriteAddress,DeviceAddress);      //将开始的空间写满一页
                WriteAddress += count;
                pBuffer      += count;
            } 
			while(NumOfPage--)  //要写入的页数
			{
				I2C_BufferWrite(pBuffer,I2C_PageSize,WriteAddress,DeviceAddress);//写一页的数据
				WriteAddress +=  I2C_PageSize;
				pBuffer      +=  I2C_PageSize; 
			}
			if(NumOfSingle != 0)//剩余数据小于一页
			{
				I2C_BufferWrite(pBuffer,NumOfSingle,WriteAddress,DeviceAddress); //写少于一页的数据 
			}
		 
        }
    } 
}
     
/* 
 * 函数名: uint8_t I2C_ReadByte(uint8_t* pBuffer,   uint8_t length,     uint16_t ReadAddress,  uint8_t DeviceAddress)
 * 描述  : 读出1串数据
 * 输入  : pBuffer : 要读取数据缓冲区指针
           length : 待读取长度  
                     WriteAddress : 读取地址
                     DeviceAddress : 器件地址
 * 输出  : TRUE : 成功
                     FALSE : 失败
 * 说明  : 跨页写入1串数据
 */
uint8_t I2C_ReadByte(uint8_t* pBuffer,   uint8_t length,     uint16_t ReadAddress,  uint8_t DeviceAddress)
{       
    if(!I2C_Start())return FALSE;
  //  I2C_SendByte(((ReadAddress & 0x0700) >>7) | DeviceAddress & 0xFFFE);//设置高起始地址+器件地址
    I2C_SendByte(DeviceAddress & 0xFE);//发送器件写地址	
    if(!I2C_WaitAck()){I2C_Stop(); return FALSE;}
    I2C_SendByte((uint8_t)(ReadAddress & 0x00FF));   //设置低起始地址      
    I2C_WaitAck();
    I2C_Start();
    I2C_SendByte(DeviceAddress | 0x01);
    I2C_WaitAck();
    while(length)
    {
        *pBuffer = I2C_ReceiveByte();
        if(length == 1)I2C_NoAck();
        else I2C_Ack(); 
        pBuffer++;
        length--;
    }
    I2C_Stop();
    return TRUE;
}
     
/* 
 * 函数名: void I2C_Test(void)
 * 描述  : 测试函数
 * 输入  : 无 
 * 输出  : 无
 * 说明  : 无
 */
void I2C_Test(void)
{
     
    uint8_t I2cVal_Write = 0xFE;
    uint8_t I2cVal_Read = 0x00;
         
    printf("Start IIC test\r\n");
    printf("The Simulation_IIC has sended data:%d \r\n", I2cVal_Write);
    I2C_WriteByte(I2cVal_Write, 0X01, 0xa0);
         
     
    I2C_ReadByte(&I2cVal_Read, 1, 0x01 ,0xa0);
    printf("The Simulation_IIC has Received data:%d \r\n", I2cVal_Read);
         
    if(I2cVal_Read == I2cVal_Write)
    {
        printf("The Simulation IIC is successful!\r\n");
    }
    else
    {
        printf("The Simulation IIC is failed!\r\n");
    }
     
}