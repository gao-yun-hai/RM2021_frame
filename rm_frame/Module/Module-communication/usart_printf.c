/*******************************************************************************
                      版权所有 (C), 2020-,NCUROBOT
 *******************************************************************************
  文 件 名   : usart_printf.c
  版 本 号   : V1.0
  作    者   : 高云海
  生成日期   : 2020.11.18
  最近修改   :
  功能描述   : printf重定向与虚拟示波器使用函数库
  函数列表   : 1) fputc()：printf重定向			 【外部直接使用printf】
							 2)	Virtual_Oscilloscope()   	 【外部调用:使用处】
							 3) Vcan_Sendware()			 			 【内部调用：Virtual_Oscilloscope()】
							 4) Usart_SendArray()				   【内部调用：Vcan_Sendware()】
							 5) Usart_SendByte()				   【内部调用：Usart_SendArray()】
*******************************************************************************/
/* 包含头文件 ----------------------------------------------------------------*/
#include "usart_printf.h"

/* 内部宏定义 ----------------------------------------------------------------*/
#define US_huart  huart2   //printf与虚拟示波器使用串口

/* 内部自定义数据类型的变量 --------------------------------------------------*/

/* 内部变量 ------------------------------------------------------------------*/

/* 内部函数原型声明 ----------------------------------------------------------*/
void Usart_SendByte( UART_HandleTypeDef *huart,uint8_t ch );
void Usart_SendArray( UART_HandleTypeDef *huart, uint8_t *array, uint16_t num);
void Vcan_Sendware(uint8_t *wareaddr, uint32_t waresize);

/* 函数主体部分 --------------------------------------------------------------*/
//===================================================================================================================//
/*********************************************** 串口重定向（USART2）*************************************************/
//===================================================================================================================//
/**
  * @brief				重定向c库的printf函数到串口2的发送函数
  * @param[in]		
	* @param[out]		
  * @retval				
*/
int fputc(int ch, FILE* f)
{
		HAL_UART_Transmit(&US_huart, (uint8_t *)&ch, 1, 10); 
    return ch;
}

//===================================================================================================================//
/********************************************** 虚拟示波器（USART2） *************************************************/
//===================================================================================================================//

/**
  * @brief				虚拟示波器传参函数
  * @param[in]		date1-date8：所需打印的数据
	* @param[out]		
  * @retval				none
*/
void Virtual_Oscilloscope(float data1,float data2,float data3,float data4,float data5,float data6,float data7,float data8)
{
	float angle[8];	
	
	angle[0] = data1;
	angle[1] = data2;
	angle[2] = data3;
	angle[3] = data4;	
	angle[4] = data5;
	angle[5] = data6;
	angle[6] = data7;
	angle[7] = data8;
	Vcan_Sendware((uint8_t *)angle,sizeof(angle));
}

/**
  * @brief				虚拟示波器发送函数
	* @param[in]		wareaddr：指向发送数组的指针
	* @param[out]		waresize：发送数据长度
  * @retval				none
*/
void Vcan_Sendware(uint8_t *wareaddr, uint32_t waresize)
{
		#define CMD_WARE     0x03
    uint8_t cmdf[2] = {CMD_WARE, ~CMD_WARE};    //串口调试 使用的前命令
    uint8_t cmdr[2] = {~CMD_WARE, CMD_WARE};    //串口调试 使用的后命令

    Usart_SendArray(&US_huart, cmdf, sizeof(cmdf));    //先发送前命令
    Usart_SendArray(&US_huart, wareaddr, waresize);    //发送数据
    Usart_SendArray(&US_huart, cmdr, sizeof(cmdr));    //发送后命令
}

/******** 发送一个字符 *********/
void Usart_SendByte( UART_HandleTypeDef *huart,uint8_t ch )
{
  HAL_UART_Transmit(huart, (uint8_t *)&ch, 1, 10); 
}
	
/******** 发送8位的数组 *********/
void Usart_SendArray( UART_HandleTypeDef *huart, uint8_t *array, uint16_t num)
{
	uint8_t i;
	for(i=0;i<num;i++)
	{
		Usart_SendByte(huart,array[i]);
	}		
}

