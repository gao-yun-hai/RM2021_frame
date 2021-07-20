/*******************************************************************************
                      ��Ȩ���� (C), 2020-,NCUROBOT
 *******************************************************************************
  �� �� ��   : motor_use_tim.c
  �� �� ��   : V1.0
  ��    ��   : NCURM
  ��������   : 2018��7��
  ����޸�   : 2020.11.28
  ��������   : �����ģ����ʹ��TIM���п��Ƶĵ����PWM��ʼ�����Ƚ�ֵ�趨��
  �����б�   : 1) TIM_Compare_Value_Set()				   ���ڲ����á�
							 2) Shoot_Firction_Motor()				   ���ⲿ���ã�ʹ�ô���
							 3) Shoot_Firction_Motor_Stop()	     ���ⲿ���ã�ʹ�ô���
							 4) Snail_PWM_Init()								 ���ⲿ���ã�bsp.c��
*******************************************************************************/
/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "motor_use_tim.h"

/* �ڲ��궨�� ----------------------------------------------------------------*/
#define PWM_htim  htim5
/* �ڲ��Զ����������͵ı��� --------------------------------------------------*/

/* �ڲ����� ------------------------------------------------------------------*/


/* �ڲ�����ԭ������ ----------------------------------------------------------*/
void TIM_Compare_Value_Set(uint32_t value1,uint32_t value2);

/* �������岿�� --------------------------------------------------------------*/
/*
TIM4��ͨ��1-ͨ��4��Ӧ -> PD12/PD13/PD14/PD15
TIM5��ͨ��1-ͨ��4��Ӧ -> PH10/PH11/PH12/PI0
Ŀǰֻʹ��TIM5ͨ��1��ͨ��2���������ɲ��Ե�δʹ�õ���
*/
/**
  * @brief				TIM����/�ȽϼĴ�����Ԥװ��ֵ�趨
  * @param[out]		
  * @param[in]		value1��ͨ��CC1��ֵ��value2��ͨ��CC2��ֵ									
  * @retval				
*/
void TIM_Compare_Value_Set(uint32_t value1,uint32_t value2)
{
	  PWM_htim.Instance->CCR1=value1;
	  PWM_htim.Instance->CCR2=value2;
}

/**
  * @brief				Ħ���ֵ������������ͨ���趨�Ƚ�ֵ���������趨ת�٣�
  * @param[out]		
  * @param[in]		����ת��:wheel1��wheel2
  * @retval				
*/
void Shoot_Firction_Motor(uint32_t wheel1,uint32_t wheel2)
{
  TIM_Compare_Value_Set(wheel1,wheel2);
}

/**
  * @brief				�ر�Ħ����
  * @param[out]		
  * @param[in]		
  * @retval				none
*/
void Shoot_Firction_Motor_Stop(void)
{
  PWM_htim.Instance->CNT = 0;
	PWM_htim.Instance->CCR1 = 0;
  PWM_htim.Instance->CCR2 = 0;
	__HAL_TIM_DISABLE(&PWM_htim);  //�رն�ʱ��
  HAL_TIM_PWM_Stop(&PWM_htim,TIM_CHANNEL_1);
  HAL_TIM_PWM_Stop(&PWM_htim,TIM_CHANNEL_2);	
}

/**
  * @brief				Ħ��������
  * @param[in]		
	* @param[out]		
  * @retval				none
*/
void Shoot_Firction_Motor_Restart(void)
{
    Shoot_Firction_Motor_Stop();
    osDelay(2000);
    __HAL_TIM_ENABLE(&PWM_htim);									//������ʱ��
    HAL_TIM_PWM_Start(&PWM_htim,TIM_CHANNEL_1);   //PH10,�Ҳ�Ħ����(TIM5)
    HAL_TIM_PWM_Start(&PWM_htim,TIM_CHANNEL_2);   //PH11,���Ħ����(TIM5)
}

/**
  * @brief				Ħ���ֵ����ʼ������
  * @param[out]		
  * @param[in]		
  * @retval				none
*/
void Shoot_Firction_Motor_PWM_Init(void)
{
	__HAL_TIM_ENABLE(&PWM_htim);									//������ʱ��
	HAL_TIM_PWM_Start(&PWM_htim,TIM_CHANNEL_1);   //PH10,�Ҳ�Ħ����(TIM5)
	HAL_TIM_PWM_Start(&PWM_htim,TIM_CHANNEL_2);   //PH11,���Ħ����(TIM5)
	Shoot_Firction_Motor (HIGH_SPEED,HIGH_SPEED);
	Shoot_Firction_Motor (LOW_SPEED,LOW_SPEED); 
}
