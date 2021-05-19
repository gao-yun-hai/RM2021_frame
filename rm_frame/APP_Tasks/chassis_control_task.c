/*******************************************************************************
                      ��Ȩ���� (C), 2020-,NCUROBOT
 *******************************************************************************
  �� �� ��   : chassis_control_task.c
  �� �� ��   : V1.0
  ��    ��   : ���ƺ�
  ��������   : 2020.12.10
  ����޸�   : 
  ��������   : �������񣬴�����
  �����б�   : 1) Chassis_Control_Task()		��FreeRTOS����������ϵͳ����������С�
							 2) Chassis_Motor_PID_Init()	���ڲ�������Chassis_Control_Task���á�
*******************************************************************************/
/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "chassis_control_task.h"
#include "pid.h"
#include "encoder.h"
#include "offline_check.h"
#include "motor_use_can.h"

/* �ڲ��궨�� ----------------------------------------------------------------*/

/* �ڲ��Զ����������͵ı��� --------------------------------------------------*/

/* �ڲ����� ------------------------------------------------------------------*/
int32_t mini512_cnt = 0;
/* �ڲ�����ԭ������ ----------------------------------------------------------*/
void Chassis_Motor_PID_Init(void);   

/* �������岿�� --------------------------------------------------------------*/
/**
  * @brief				��������
  * @param[in]		
	* @param[out]		
  * @retval				none
*/
void Chassis_Control_Task(void const * argument)
{
	static int16_t get_speed;
	static int16_t set_speed;			  
	static  float chassis_current_value = 0;
	
	Chassis_Motor_PID_Init();//ʹ�����ߵ���ʱ��Ҫ���ú���ע�͵�
	
	portTickType xLastWakeTime;
	xLastWakeTime = xTaskGetTickCount();

	for(;;)
	{
		Refresh_Task_OffLine_Time(ChassisControlTask_TOE);
		mini512_cnt = Get_TIM_COUNTER();
		set_speed = 500;
		get_speed = motor_get[CHASSIS_MOTOR_LF].speed_rpm;
		chassis_current_value = PID_Calc(&motor_pid[PID_CHASSIS_MOTOR_RF_SPD], get_speed, set_speed);
	
		Chassis_Motor_Drive(&hcan1,chassis_current_value,0,0,0);

		
		osDelayUntil(&xLastWakeTime,CHASSIS_PERIOD);		

	}

}


/**
  * @brief				PID��ʼ��������3508��
  * @param[in]		
	* @param[out]		
  * @retval				none
*/
void Chassis_Motor_PID_Init(void)
{
	PID_Param_Init(&motor_pid[PID_CHASSIS_MOTOR_RF_SPD], POSITION_PID, 1000, 3000,
									10.0f, 0.0f, 0.0f);
}

