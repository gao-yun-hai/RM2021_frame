#ifndef __BSP_USART_H
#define __BSP_USART_H
/* ����ͷ�ļ� ---------------------------------------------------------------*/
#include "myinclude.h"

/* ��ģ�����ⲿ�ṩ�ĺ궨�� -------------------------------------------------*/

/* ��ģ�����ⲿ�ṩ�Ľṹ��/ö�ٶ��� ----------------------------------------*/

/* ��ģ�����ⲿ�ṩ�ı������� -----------------------------------------------*/

/* ��ģ�����ⲿ�ṩ���Զ����������ͱ������� ---------------------------------*/

/* ��ģ�����ⲿ�ṩ�Ľӿں���ԭ������ ---------------------------------------*/
HAL_StatusTypeDef USER_UART_Receive_DMA(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size);
void USER_UART_IT_Init(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size);
void HAL_UART_IT_Init(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size);

#endif


