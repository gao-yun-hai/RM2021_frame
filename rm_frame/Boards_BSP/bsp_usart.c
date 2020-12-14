/*******************************************************************************
                      版权所有 (C), 2020-,NCUROBOT
 *******************************************************************************
  文 件 名   : bsp_usart.c
  版 本 号   : V1.0
  作    者   : 高云海
  生成日期   : 2020.11.18
  最近修改   :
  功能描述   : USART空闲中断开启相关设置
  函数列表   : 1) UART_IT_init()					【外部调用：需要使用串口空闲处】
							 2) Bsp_UART_Receive_DMA()	【内部调用：UART_IT_init()】
*******************************************************************************/
/* 包含头文件 ----------------------------------------------------------------*/
#include "bsp_usart.h"

/* 内部宏定义 ----------------------------------------------------------------*/

/* 内部自定义数据类型的变量 --------------------------------------------------*/

/* 内部变量 ------------------------------------------------------------------*/

/* 内部函数原型声明 ----------------------------------------------------------*/

/* 函数主体部分 --------------------------------------------------------------*/

/* 函数主体部分 --------------------------------------------------------------*/
/**
  * @brief				使用DMA接收数据并触发串口空闲中断。
									通过非阻塞方式接收数据，数据长度有最大限度，但是在最大限度之内可以接受任意长度的数据。
  * @param[out]		
  * @param[in]		huart: 指向UART_HandleTypeDef结构体的指针，该指针包含了UART的配置信息
                  pData: 指向接受数据缓冲区的指针
									Size: 可接收数据的最大长度
	* @retval				HAL status
*/
HAL_StatusTypeDef UART_IT_Init(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size)
{
	Bsp_UART_Receive_DMA(huart,pData,Size);//利用DMA接受数据	
	__HAL_UART_ENABLE_IT(huart, UART_IT_IDLE);//开启串口空闲中断
	return HAL_OK;

}

/**
  * @brief				为串口开启没有中断的DMA传输，为了减少中断次数为其他中断空出资源，代替库函数HAL_UART_Receive_DMA。
  * @param[out]		
  * @param[in]		hdma: 指向DMA_HandleTypeDef结构体的指针，这个结构体包含了DMA流的配置信息.
  * @retval				HAL status
*/
HAL_StatusTypeDef Bsp_UART_Receive_DMA(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size)
{
  uint32_t *tmp;
  uint32_t tmp1 = 0;
  
  tmp1 = huart->gState;
  if((tmp1 == HAL_UART_STATE_READY) || (tmp1 == HAL_UART_STATE_BUSY_TX))
  {
    if((pData == NULL ) || (Size == 0)) 
    {
      return HAL_ERROR;
    }
		
    
    /* Process Locked */
    __HAL_LOCK(huart);
    
    huart->pRxBuffPtr = pData;
    huart->RxXferSize = Size;
    
    huart->ErrorCode = HAL_UART_ERROR_NONE;
    /* Check if a transmit process is ongoing or not */
    if(huart->gState == HAL_UART_STATE_BUSY_TX)
    {
      huart->gState = HAL_UART_STATE_BUSY_TX_RX;
    }
    else
    {
      huart->gState = HAL_UART_STATE_BUSY_RX;
    }
    
    /* Enable the DMA Stream */
    tmp = (uint32_t*)&pData;
    HAL_DMA_Start(huart->hdmarx, (uint32_t)&huart->Instance->DR, *(uint32_t*)tmp, Size);
    
    /* Enable the DMA transfer for the receiver request by setting the DMAR bit 
    in the UART CR3 register */
    huart->Instance->CR3 |= USART_CR3_DMAR;
    
    /* Process Unlocked */
    __HAL_UNLOCK(huart);
    
    return HAL_OK;
  }
  else
  {
    return HAL_BUSY; 
  }
}
