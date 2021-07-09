/*******************************************************************************
                      ��Ȩ���� (C), 2020-,NCUROBOT
 *******************************************************************************
  �� �� ��   : bsp_usart.c
  �� �� ��   : V1.0
  ��    ��   : ���ƺ�
  ��������   : 2020.11.18
  ����޸�   :
  ��������   : USART�����жϿ����������
  �����б�   : 1) UART_IT_init()					���ⲿ���ã���Ҫʹ�ô��ڿ��д���
							 2) Bsp_UART_Receive_DMA()	���ڲ����ã�UART_IT_init()��
*******************************************************************************/
/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "bsp_usart.h"

/* �ڲ��궨�� ----------------------------------------------------------------*/

/* �ڲ��Զ����������͵ı��� --------------------------------------------------*/

/* �ڲ����� ------------------------------------------------------------------*/

/* �ڲ�����ԭ������ ----------------------------------------------------------*/

/* �������岿�� --------------------------------------------------------------*/

/* �������岿�� --------------------------------------------------------------*/
/**
* @brief				ʹ��HAL���е�DMA���ݽ��ղ��������ڿ����жϡ�(��Ҫʹ�ô��ڷ�����ʹ�øú�������)
  * @param[out]		
  * @param[in]		huart: ����
                  pData: ָ��������ݻ�������ָ��
									Size:  �ɽ������ݵ���󳤶�
	* @retval				HAL status
*/
void HAL_UART_IT_Init(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size)
{
	HAL_UART_Receive_DMA(huart,pData,Size);//����DMA��������	
	__HAL_UART_ENABLE_IT(huart, UART_IT_IDLE);//�������ڿ����ж�
}


/**
  * @brief				ʹ���Զ���DMA�������ݲ��������ڿ����жϡ�(����Ҫʹ�ô��ڷ�����ʹ�øú�������)
									ͨ����������ʽ�������ݣ����ݳ���������޶ȣ�����������޶�֮�ڿ��Խ������ⳤ�ȵ����ݡ�
  * @param[out]		
  * @param[in]		huart: ����
                  pData: ָ��������ݻ�������ָ��
									Size:  �ɽ������ݵ���󳤶�
	* @retval				HAL status
*/
void USER_UART_IT_Init(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size)
{
	USER_UART_Receive_DMA(huart,pData,Size);//����DMA��������	
	__HAL_UART_ENABLE_IT(huart, UART_IT_IDLE);//�������ڿ����ж�
}

/**
  * @brief				Ϊ���ڿ���û���жϵ�DMA���䣬Ϊ�˼����жϴ���Ϊ�����жϿճ���Դ������⺯��HAL_UART_Receive_DMA��
  * @param[out]		
  * @param[in]		huart: ָ��UART_HandleTypeDef�ṹ���ָ�룬��ָ�������UART��������Ϣ
									pData: ָ��������ݻ�������ָ��
									Size: �ɽ������ݵ���󳤶�
  * @retval				HAL status
*/
HAL_StatusTypeDef USER_UART_Receive_DMA(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size)
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



