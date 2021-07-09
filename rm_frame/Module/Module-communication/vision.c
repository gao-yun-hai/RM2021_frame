/*******************************************************************************
                      ��Ȩ���� (C), 2020-,NCUROBOT
 *******************************************************************************
  �� �� ��   : vision.c
  �� �� ��   : V1.0
  ��    ��   : ���ƺ�
  ��������   : 2020.12.8
  ����޸�   : 
  ��������   : �Ӿ����ݽ�������뼰���͡�USART��ʼ����UASRT�жϴ������ݽ��롢���ݷ��͡�
  �����б�   : 1) VisionDate_Receive_Init()  ���ⲿ���ã�bsp.c��
							 2) Vision_UART_IRQHandler()	 ���ⲿ���ã�stm32f4xx_it.c��USART7�жϷ�������
							 3) SBUS_To_Vision()					 ���ڲ����ã�Vision_UART_IRQHandler()��
*******************************************************************************/
/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "vision.h"
#include "bsp_usart.h"
#include "offline_check.h"
/* �ڲ��궨�� ----------------------------------------------------------------*/
#define VD_huart  huart7 //���Ӿ�ͨ�����ô���
/* �ڲ��Զ����������͵ı��� --------------------------------------------------*/
Minipc_Rx_Struct minipc_rx; //��������minipc������

/* �ڲ����� ------------------------------------------------------------------*/
static uint8_t vision_rx_buf[VISION_RX_BUFFER_SIZE];//�����Ӿ�ԭʼ���ݵ�����


/* �ڲ�����ԭ������ ----------------------------------------------------------*/
static void SBUS_To_Vision(volatile const uint8_t *buff, Minipc_Rx_Struct *Minipc_Rx);

/* �������岿�� --------------------------------------------------------------*/
/**
  * @brief				�Ӿ����ݽ���USART��ʼ������������DMA+����USART�����жϣ�
  * @param[in]		
	* @param[out]		
  * @retval				none
*/
void VisionDate_Receive_USART_Init(void)
{
    USER_UART_IT_Init(&VD_huart,vision_rx_buf,VISION_RX_BUFFER_SIZE);
}


/**
  * @brief				�Ӿ�USART�жϴ���������������DMA����+���ݽ���+��������֪ͨ��
  * @param[in]		
	* @param[out]		
  * @retval				none
*/
void Vision_UART_IRQHandler(void)
{
	static  BaseType_t  pxHigherPriorityTaskWoken;
	/* �ж��Ƿ�Ϊ�����ж� */
	if (__HAL_UART_GET_FLAG(&VD_huart, UART_FLAG_IDLE))
	{
			/* ������б�־������һֱ���ڿ���״̬���ж� */
			__HAL_UART_CLEAR_IDLEFLAG(&VD_huart);

			/* �ر�DMA���� */
			__HAL_DMA_DISABLE(VD_huart.hdmarx);

			/* ������յ����Ӿ����ݳ����뷢�͵��Ƿ�һ�� */
			if ((VISION_RX_BUFFER_SIZE - VD_huart.hdmarx->Instance->NDTR) == VISION_DATA_LEN)
			{
				/* ���ݽ��� */
				SBUS_To_Vision(vision_rx_buf,&minipc_rx);
				/* ���߼��ˢ��ʱ�� */
				Refresh_Device_OffLine_Time(Vision_TOE);
				/* ����֪ͨ */
				vTaskNotifyGiveFromISR(VisionDataTaskHandle,&pxHigherPriorityTaskWoken);
				portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);	
			}
			/* ��������DMA���� */
			/* ����DMA�ɴ���������ݳ��� */
			__HAL_DMA_SET_COUNTER(VD_huart.hdmarx, VISION_RX_BUFFER_SIZE);
			/* ����DMA���� */
			__HAL_DMA_ENABLE(VD_huart.hdmarx);				
			
	}
}


/**
  * @brief				�Ӿ����ݽ��뺯��
  * @param[in]		buff��ָ�����ݽ��������ָ��
	* @param[out]		Minipc_Rx��ָ��洢�Ӿ����ݽṹ���ָ��
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


