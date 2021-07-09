#ifndef __BSP_USART_H
#define __BSP_USART_H
/* 包含头文件 ---------------------------------------------------------------*/
#include "myinclude.h"

/* 本模块向外部提供的宏定义 -------------------------------------------------*/

/* 本模块向外部提供的结构体/枚举定义 ----------------------------------------*/

/* 本模块向外部提供的变量声明 -----------------------------------------------*/

/* 本模块向外部提供的自定义数据类型变量声明 ---------------------------------*/

/* 本模块向外部提供的接口函数原型声明 ---------------------------------------*/
HAL_StatusTypeDef USER_UART_Receive_DMA(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size);
void USER_UART_IT_Init(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size);
void HAL_UART_IT_Init(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size);

#endif


