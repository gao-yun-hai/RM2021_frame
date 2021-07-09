/*******************************************************************************
                      ��Ȩ���� (C), 2020-,NCUROBOT
 *******************************************************************************
  �� �� ��   : remote_control.c
  �� �� ��   : V1.0
  ��    ��   : ���ƺ�
  ��������   : 2020.11.18
  ����޸�   :
  ��������   : ң�������ݽ�������롾USART��ʼ����UASRT�жϴ������ݽ��롿
  �����б�   : 1) Remote_Control_Init()   ���ⲿ���ã�bsp.c��
							 2) DR16_UART_IRQHandler()	���ⲿ���ã�stm32f4xx_it.c��USART1�жϷ�������
							 3) SBUS_To_RC()						���ڲ����ã�DR16_UART_IRQHandler()��
							 4) RC_Restart()						���ڲ����ã�DR16_UART_IRQHandler()��
							 5) RC_Data_Error()					����������
*******************************************************************************/
/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "remote_control.h"
#include "bsp_usart.h"
#include "offline_check.h"
/* �ڲ��궨�� ----------------------------------------------------------------*/
#define RC_huart  huart1
#define RC_abs(value)    ((value)>=0 ? (value) : (-value))
/* �ڲ��Զ����������͵ı��� --------------------------------------------------*/
RC_ctrl_t rc_ctrl;//ң�������ݽṹ��

/* �ڲ����� ------------------------------------------------------------------*/
static uint8_t sbus_rx_buf[DR16_RX_BUFFER_SIZE];//����ԭʼ���ݣ�Ϊ18���ֽڣ�����36���ֽڳ��ȣ���ֹDMA����Խ��

/* �ڲ�����ԭ������ ----------------------------------------------------------*/
static void SBUS_To_RC(volatile const uint8_t *sbus_buf, RC_ctrl_t *rc_ctrl);//ң�������뺯��
static void RC_Restart(uint16_t dma_buf_num);//���������쳣��λ����
uint8_t RC_Data_Error(void);//�������ݴ����жϺ���

/* �������岿�� --------------------------------------------------------------*/
/**
  * @brief				ң�������ݽ���USART��ʼ������������DMA+����USART�����жϣ�
  * @param[in]		
	* @param[out]		
  * @retval				none
*/
void Remote_Control_USART_Init(void)
{
    USER_UART_IT_Init(&RC_huart,sbus_rx_buf,DR16_RX_BUFFER_SIZE);
}


/**
  * @brief				ң����USART�жϴ���������������DMA����+���ݽ���+��������֪ͨ��
  * @param[in]		
	* @param[out]		
  * @retval				none
*/
void DR16_UART_IRQHandler(void)
{
	static  BaseType_t  pxHigherPriorityTaskWoken;
 /* �ж��Ƿ�Ϊ�����ж� */
  if (__HAL_UART_GET_FLAG(&RC_huart, UART_FLAG_IDLE))
	{
			/* ������б�־������һֱ���ڿ���״̬���ж� */
			__HAL_UART_CLEAR_IDLEFLAG(&RC_huart);

			/* �ر�DMA���� */
			__HAL_DMA_DISABLE(RC_huart.hdmarx);

			/* ������յ���ң�������ݳ����뷢�͵��Ƿ�һ�� */
			if ((DR16_RX_BUFFER_SIZE - RC_huart.hdmarx->Instance->NDTR) == DR16_DATA_LEN)
			{
				/* ���ݽ��� */
				SBUS_To_RC(sbus_rx_buf,&rc_ctrl);
				/* ������ȷ���ж� */
				RC_Data_Error();
				/* ���߼��ˢ��ʱ�� */
				Refresh_Device_OffLine_Time(RemoteControl_TOE);
				/* ����֪ͨ */
				vTaskNotifyGiveFromISR(RemoteDataTaskHandle,&pxHigherPriorityTaskWoken);
				portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);	
			}
			else RC_Restart(DR16_RX_BUFFER_SIZE);
			/* ��������DMA���� */
			/* ����DMA�ɴ���������ݳ��� */
			__HAL_DMA_SET_COUNTER(RC_huart.hdmarx, DR16_RX_BUFFER_SIZE);
			/* ����DMA���� */
			__HAL_DMA_ENABLE(RC_huart.hdmarx);				
			
	}
}

/**
  * @brief				ң�������ݽ��뺯��
  * @param[in]		sbus_buf��ָ�����ݽ��������ָ��
	* @param[out]		rc_ctrl��	ָ��洢ң�������ݽṹ���ָ��
  * @retval				none
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

/**
  * @brief				�ж�ң���������Ƿ����
  * @param[in]		
	* @param[out]		
  * @retval				0�����ݴ���1����������
*/
uint8_t RC_Data_Error(void)
{
    //ʹ����go to��� �������ͳһ����ң�����������ݹ���
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
  * @brief				�������¿���
  * @param[in]		dma_buf_num��DMA�ɽ������ݵ���󳤶�
	* @param[out]		
  * @retval				none
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
