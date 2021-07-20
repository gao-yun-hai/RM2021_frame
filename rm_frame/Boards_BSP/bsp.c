/*******************************************************************************
                      ��Ȩ���� (C), 2020-,NCUROBOT
 *******************************************************************************
  �� �� ��   : bsp.c
  �� �� ��   : V1.0
  ��    ��   : ���ƺ�
  ��������   : 2020.11.18
  ����޸�   : 
  ��������   : ��������ʹ�ó�ʼ������
  �����б�   : 1) BSP_Init()			���ⲿ���ã�main.c��
							 2) Power_ON()			���ڲ����ã�BSP_Init()��
*******************************************************************************/
/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "bsp.h"

#include "imu.h"
#include "bsp_can.h"
#include "encoder.h"
#include "vision.h"
#include "referee.h"
#include "motor_use_tim.h"
#include "offline_check.h"
#include "remote_control.h"
#include "pid_wireless_debug.h"


/* �ڲ��궨�� ----------------------------------------------------------------*/

/* �ڲ��Զ����������͵ı��� --------------------------------------------------*/

/* �ڲ����� ------------------------------------------------------------------*/

/* �ڲ�����ԭ������ ----------------------------------------------------------*/
void Power_ON (void);

/* �������岿�� --------------------------------------------------------------*/

/**
  * @brief				���������ʼ���������������е��á�
  * @param[out]		
  * @param[in]		
  * @retval				
*/
void BSP_Init (void)
{
	Power_ON();													//�¿���A�壬����24V����ӿ�
	Shoot_Firction_Motor_PWM_Init();		//PWM����Ħ���ֵ����ʼ��		��tim5��
	MPU_Device_SPI_Init();							//���������ǳ�ʼ��					��spi5��(�ڲ�����2s����ʱ)  	  
	Remote_Control_USART_Init();				//ң�������ݽ��ճ�ʼ��			��usart1��
	Wireless_Debug_USART_Init();				//PID���ߵ��γ�ʼ��					��usart2��
	RefereeDate_Receive_USART_Init();		//����ϵͳ���ݽ��ճ�ʼ��		��usart6��	
	VisionDate_Receive_USART_Init();		//�Ӿ����ݽ��ճ�ʼ��				��usart7��
	Encoder_Mini512_TIM_Init();					//mini512��������ʼ��				��tim2��
	Off_Line_Detect_Init();							//���߼���ʼ��						��tim3��
	BSP_CAN_Init(&hcan1);								//CAN1��ʼ��								��can1��
	BSP_CAN_Init(&hcan2);								//CAN2��ʼ��								��can2��
}


/**
  * @brief				�¿���A�壬�����ĸ�24V����ӿ�
  * @param[out]		
  * @param[in]		
  * @retval				
*/
void Power_ON (void)
{
	HAL_GPIO_WritePin(POWER1_GPIO_Port,POWER1_Pin,GPIO_PIN_SET);
	HAL_GPIO_WritePin(POWER2_GPIO_Port,POWER2_Pin,GPIO_PIN_SET);
	HAL_GPIO_WritePin(POWER3_GPIO_Port,POWER3_Pin,GPIO_PIN_SET);
	HAL_GPIO_WritePin(POWER4_GPIO_Port,POWER4_Pin,GPIO_PIN_SET);
}
