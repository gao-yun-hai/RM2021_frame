/*******************************************************************************
                      版权所有 (C), 2020-,NCUROBOT
 *******************************************************************************
  文 件 名   : vision.c
  版 本 号   : V1.0
  作    者   : 高云海
  生成日期   : 2020.12.8
  最近修改   : 
  功能描述   : 视觉数据接收与解码及发送【USART初始化、UASRT中断处理、数据解码、数据发送】
  函数列表   : 1) VisionDate_Receive_Init()  【外部调用：bsp.c】
							 2) Vision_UART_IRQHandler()	 【外部调用：stm32f4xx_it.c的USART7中断服务函数】
							 3) SBUS_To_Vision()					 【内部调用：Vision_UART_IRQHandler()】
*******************************************************************************/
/* 包含头文件 ----------------------------------------------------------------*/
#include "vision.h"
#include "bsp_usart.h"
#include "offline_check.h"
/* 内部宏定义 ----------------------------------------------------------------*/
#define VD_huart  huart7 //与视觉通信所用串口
/* 内部自定义数据类型的变量 --------------------------------------------------*/
Minipc_Rx_Struct minipc_rx; //接收来自minipc的数据

/* 内部变量 ------------------------------------------------------------------*/
static uint8_t vision_rx_buf[VISION_RX_BUFFER_SIZE];//接收视觉原始数据的数组


/* 内部函数原型声明 ----------------------------------------------------------*/
static void SBUS_To_Vision(volatile const uint8_t *buff, Minipc_Rx_Struct *Minipc_Rx);

/* 函数主体部分 --------------------------------------------------------------*/
/**
  * @brief				视觉数据接收USART初始化函数（开启DMA+开启USART空闲中断）
  * @param[in]		
	* @param[out]		
  * @retval				none
*/
void VisionDate_Receive_USART_Init(void)
{
    USER_UART_IT_Init(&VD_huart,vision_rx_buf,VISION_RX_BUFFER_SIZE);
}


/**
  * @brief				视觉USART中断处理函数（重新设置DMA参数+数据解码+发送任务通知）
  * @param[in]		
	* @param[out]		
  * @retval				none
*/
void Vision_UART_IRQHandler(void)
{
	static  BaseType_t  pxHigherPriorityTaskWoken;
	/* 判断是否为空闲中断 */
	if (__HAL_UART_GET_FLAG(&VD_huart, UART_FLAG_IDLE))
	{
			/* 清除空闲标志，避免一直处于空闲状态的中断 */
			__HAL_UART_CLEAR_IDLEFLAG(&VD_huart);

			/* 关闭DMA传输 */
			__HAL_DMA_DISABLE(VD_huart.hdmarx);

			/* 计算接收到的视觉数据长度与发送的是否一致 */
			if ((VISION_RX_BUFFER_SIZE - VD_huart.hdmarx->Instance->NDTR) == VISION_DATA_LEN)
			{
				/* 数据解码 */
				SBUS_To_Vision(vision_rx_buf,&minipc_rx);
				/* 断线检测刷新时间 */
				Refresh_Device_OffLine_Time(Vision_TOE);
				/* 任务通知 */
				vTaskNotifyGiveFromISR(VisionDataTaskHandle,&pxHigherPriorityTaskWoken);
				portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);	
			}
			/* 重新启动DMA传输 */
			/* 设置DMA可传输最大数据长度 */
			__HAL_DMA_SET_COUNTER(VD_huart.hdmarx, VISION_RX_BUFFER_SIZE);
			/* 开启DMA传输 */
			__HAL_DMA_ENABLE(VD_huart.hdmarx);				
			
	}
}


/**
  * @brief				视觉数据解码函数
  * @param[in]		buff：指向数据接收数组的指针
	* @param[out]		Minipc_Rx：指向存储视觉数据结构体的指针
  * @retval				
*/
static void SBUS_To_Vision(volatile const uint8_t *buff, Minipc_Rx_Struct *Minipc_Rx)
{
	if (buff == NULL || Minipc_Rx == NULL)
	{
			return;
	}
	
	Minipc_Rx->frame_header = buff[0];
	Minipc_Rx->frame_tail 	= buff[8];
	
	if((Minipc_Rx->frame_header == 0xFF) && (Minipc_Rx->frame_tail == 0xFE))
	{
		Minipc_Rx->angle_yaw  = (int16_t)(buff[1] << 8 | buff[2]);
		Minipc_Rx->angle_pit  = (int16_t)(buff[3] << 8 | buff[4]);
		Minipc_Rx->state_flag = buff[5];
		Minipc_Rx->distance   = buff[6]<<8|buff[7];
	}
	
}


