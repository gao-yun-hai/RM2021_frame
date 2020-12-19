/*******************************************************************************
                      版权所有 (C), 2020-,NCUROBOT
 *******************************************************************************
  文 件 名   : remote_control.c
  版 本 号   : V1.0
  作    者   : 高云海
  生成日期   : 2020.11.18
  最近修改   :
  功能描述   : 遥控器数据接收与解码【USART初始化、UASRT中断处理、数据解码】
  函数列表   : 1) Remote_Control_Init()   【外部调用：bsp.c】
							 2) DR16_UART_IRQHandler()	【外部调用：stm32f4xx_it.c的USART1中断服务函数】
							 3) SBUS_To_RC()						【内部调用：DR16_UART_IRQHandler()】
							 4) RC_Restart()						【内部调用：DR16_UART_IRQHandler()】
							 5) RC_Data_Error()					【待开发】
*******************************************************************************/
/* 包含头文件 ----------------------------------------------------------------*/
#include "remote_control.h"
#include "bsp_usart.h"
#include "offline_check.h"
/* 内部宏定义 ----------------------------------------------------------------*/
#define RC_huart  huart1
#define RC_abs(value)    ((value)>=0 ? (value) : (-value))
/* 内部自定义数据类型的变量 --------------------------------------------------*/
RC_ctrl_t rc_ctrl;//遥控器数据结构体

/* 内部变量 ------------------------------------------------------------------*/
static uint8_t sbus_rx_buf[DR16_RX_BUFFER_SIZE];//接收原始数据，为18个字节，给了36个字节长度，防止DMA传输越界

/* 内部函数原型声明 ----------------------------------------------------------*/
static void SBUS_To_RC(volatile const uint8_t *sbus_buf, RC_ctrl_t *rc_ctrl);//遥控器解码函数
static void RC_Restart(uint16_t dma_buf_num);//接收数据异常复位函数
uint8_t RC_Data_Error(void);//接收数据错误判断函数

/* 函数主体部分 --------------------------------------------------------------*/
/**
  * @brief				遥控器数据接收USART空闲中断初始化函数
  * @param[out]		
  * @param[in]		
  * @retval				
*/
void Remote_Control_USART_Init(void)
{
    UART_IT_Init(&RC_huart,sbus_rx_buf,DR16_RX_BUFFER_SIZE);
}


/**
  * @brief				遥控器数据接收的触发的空闲中断后中断处理函数
  * @param[out]		
  * @param[in]		
  * @retval				
*/
uint32_t DR16_UART_IRQHandler(void)
{
	static  BaseType_t  pxHigherPriorityTaskWoken;

 if (__HAL_UART_GET_FLAG(&RC_huart, UART_FLAG_IDLE))
	{
			/* clear idle it flag avoid idle interrupt all the time */
			__HAL_UART_CLEAR_IDLEFLAG(&RC_huart);

			/* clear DMA transfer complete flag */
			__HAL_DMA_DISABLE(RC_huart.hdmarx);

			/* handle dbus data dbus_buf from DMA */
			if ((DR16_RX_BUFFER_SIZE - RC_huart.hdmarx->Instance->NDTR) == DR16_DATA_LEN)
			{
				//数据解码
				SBUS_To_RC(sbus_rx_buf,&rc_ctrl);
				//数据正确性判断
				RC_Data_Error();
				//断线检测刷新时间
				Refresh_Device_OffLine_Time(RemoteControl_TOE);
				//任务通知
				vTaskNotifyGiveFromISR(RemoteDataTaskHandle,&pxHigherPriorityTaskWoken);
				portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);	
			}
			else RC_Restart(DR16_RX_BUFFER_SIZE);
			/* restart dma transmission */
			__HAL_DMA_SET_COUNTER(RC_huart.hdmarx, DR16_RX_BUFFER_SIZE);
			__HAL_DMA_ENABLE(RC_huart.hdmarx);				
			
	}
	return 0;
}

/**
  * @brief				遥控器数据解码函数
  * @param[out]		
  * @param[in]		
  * @retval				
*/
static void SBUS_To_RC(volatile const uint8_t *sbus_buf, RC_ctrl_t *rc_ctrl)
{
	if (sbus_buf == NULL || rc_ctrl == NULL)
	{
			return;
	}
	
	rc_ctrl->rc.ch0 = (sbus_buf[0] | (sbus_buf[1] << 8)) & 0x07ff; //!< Channel 0
	rc_ctrl->rc.ch1 = ((sbus_buf[1] >> 3) | (sbus_buf[2] << 5)) & 0x07ff; //!< Channel 1
	rc_ctrl->rc.ch2 = ((sbus_buf[2] >> 6) | (sbus_buf[3] << 2) | (sbus_buf[4] << 10)) &0x07ff;//!< Channel 2
	rc_ctrl->rc.ch3 = ((sbus_buf[4] >> 1) | (sbus_buf[5] << 7)) & 0x07ff; //!< Channel 3
	
	rc_ctrl->rc.s1 = ((sbus_buf[5] >> 4) & 0x000C) >> 2; //!< Switch left
	rc_ctrl->rc.s2 = ((sbus_buf[5] >> 4) & 0x0003); //!< Switch right
	
	rc_ctrl->mouse.x = sbus_buf[6] | (sbus_buf[7] << 8); //!< Mouse X axis
	rc_ctrl->mouse.y = sbus_buf[8] | (sbus_buf[9] << 8); //!< Mouse Y axis
	rc_ctrl->mouse.z = sbus_buf[10] | (sbus_buf[11] << 8); //!< Mouse Z axis
	
	rc_ctrl->mouse.press_l = sbus_buf[12]; //!< Mouse Left Is Press 
	rc_ctrl->mouse.press_r = sbus_buf[13]; //!< Mouse Right Is Press 
	
	rc_ctrl->key.v = sbus_buf[14] | (sbus_buf[15] << 8); //!< KeyBoard value	
	
	rc_ctrl->rc.ch0 -= 1024;
	rc_ctrl->rc.ch1 -= 1024;
	rc_ctrl->rc.ch2 -= 1024;
	rc_ctrl->rc.ch3 -= 1024;
}

/*----------------------------以下代码暂未使用，待后续开发（参考19年官方代码，断线检测使用）------------------------------*/
/**
  * @brief				判断遥控器数据是否出错
  * @param[out]		
  * @param[in]		
  * @retval				
*/
uint8_t RC_Data_Error(void)
{
    //使用了go to语句 方便出错统一处理遥控器变量数据归零
    if (RC_abs(rc_ctrl.rc.ch0) > 660)
    {
        goto error;
    }
    if (RC_abs(rc_ctrl.rc.ch1) > 660)
    {
        goto error;
    }
    if (RC_abs(rc_ctrl.rc.ch2) > 660)
    {
        goto error;
    }
    if (RC_abs(rc_ctrl.rc.ch3) > 660)
    {
        goto error;
    }
    if (rc_ctrl.rc.s1 == 0)
    {
        goto error;
    }
    if (rc_ctrl.rc.s2 == 0)
    {
        goto error;
    }
    return 0;

error:
    rc_ctrl.rc.ch0 = 0;
    rc_ctrl.rc.ch1 = 0;
    rc_ctrl.rc.ch2 = 0;
    rc_ctrl.rc.ch3 = 0;
    rc_ctrl.rc.s1 = 0;
    rc_ctrl.rc.s2 = 0;
    rc_ctrl.mouse.x = 0;
    rc_ctrl.mouse.y = 0;
    rc_ctrl.mouse.z = 0;
    rc_ctrl.mouse.press_l = 0;
    rc_ctrl.mouse.press_r = 0;
    rc_ctrl.key.v = 0;
    return 1;
}


/**
  * @brief				若接收数据出错，则使用该函数进行复位
  * @param[out]		
  * @param[in]		
  * @retval				
*/
static void RC_Restart(uint16_t dma_buf_num)
{
		//disable UART
	__HAL_UART_DISABLE(&RC_huart);
	//disable DMA
	__HAL_DMA_DISABLE(RC_huart.hdmarx);
	//reset set_data_lenght
	RC_huart.hdmarx->Instance->NDTR = dma_buf_num;
	__HAL_UART_CLEAR_IDLEFLAG(&RC_huart);
	__HAL_DMA_CLEAR_FLAG(RC_huart.hdmarx,DMA_FLAG_TCIF2_6);
	//ensable UART
	__HAL_UART_ENABLE(&RC_huart);
	//ensable DMA
	__HAL_DMA_ENABLE(RC_huart.hdmarx);

}


/*----------------------------以上代码暂未使用，待后续开发（参考19年官方代码，断线检测使用）------------------------------*/
