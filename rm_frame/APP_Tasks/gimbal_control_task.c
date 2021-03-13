/*******************************************************************************
                      ��Ȩ���� (C), 2020-,NCUROBOT
 *******************************************************************************
  �� �� ��   : gimbal_control_task.c
  �� �� ��   : V1.0
  ��    ��   : ���ƺ�
  ��������   : 2020.12.10
  ����޸�   : 
  ��������   : ��̨����,������
  �����б�   : 1) Gimbal_Control_Task()			��FreeRTOS����������ϵͳ����������С�
*******************************************************************************/
/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "gimbal_control_task.h"
#include "offline_check.h"
#include "pid.h"
#include "motor_use_can.h"
#include "usart_printf.h"
/* �ڲ��궨�� ----------------------------------------------------------------*/

/* �ڲ��Զ����������͵ı��� --------------------------------------------------*/

/* �ڲ����� ------------------------------------------------------------------*/

/* �ڲ�����ԭ������ ----------------------------------------------------------*/
void Gimbal_Motor_PID_Init(void);   

/* �������岿�� --------------------------------------------------------------*/
/**
  * @brief				��̨����
  * @param[in]		
	* @param[out]		
  * @retval				none
*/
void Gimbal_Control_Task(void const * argument)
{
	int32_t get_pitch_pos = 0;
	int32_t set_pitch_pos = 0;			  
	static  float pitch_current_value = 0;
	Gimbal_Motor_PID_Init();
	portTickType xLastWakeTime;
	xLastWakeTime = xTaskGetTickCount();
	
	for(;;)
	{
		Refresh_Task_OffLine_Time(GimbalContrlTask_TOE);
		
		set_pitch_pos = 200;
		get_pitch_pos = motor_get[GIMBAL_PITCH_MOTOR].total_angle;
		pitch_current_value = PID_Calc(&motor_pid[PID_PITCH_MOTOR_POS], get_pitch_pos, set_pitch_pos);
		Gimbal_Motor_Drive(&hcan1,0,pitch_current_value);

		osDelayUntil(&xLastWakeTime,GIMBAL_PERIOD);		

	}

}

/**
  * @brief				PID��ʼ��������6020��
  * @param[in]		
	* @param[out]		
  * @retval				none
*/
void Gimbal_Motor_PID_Init(void)   
{
	PID_Param_Init(&motor_pid[PID_PITCH_MOTOR_POS], POSITION_PID, 30000, 30000,
									20.0f, 0.001f, 0.25f);
	
}

