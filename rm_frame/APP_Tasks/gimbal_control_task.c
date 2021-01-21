/*******************************************************************************
                      版权所有 (C), 2020-,NCUROBOT
 *******************************************************************************
  文 件 名   : gimbal_control_task.c
  版 本 号   : V1.0
  作    者   : 高云海
  生成日期   : 2020.12.10
  最近修改   : 
  功能描述   : 云台任务,待完善
  函数列表   : 1) Gimbal_Control_Task()			【FreeRTOS函数：操作系统任务调度运行】
*******************************************************************************/
/* 包含头文件 ----------------------------------------------------------------*/
#include "gimbal_control_task.h"
#include "offline_check.h"
#include "pid.h"
#include "motor_use_can.h"
#include "usart_printf.h"
/* 内部宏定义 ----------------------------------------------------------------*/

/* 内部自定义数据类型的变量 --------------------------------------------------*/

/* 内部变量 ------------------------------------------------------------------*/

/* 内部函数原型声明 ----------------------------------------------------------*/
void Motor6623_PID_Init(void);   

/* 函数主体部分 --------------------------------------------------------------*/
/**
  * @brief				云台任务
  * @param[in]		
	* @param[out]		
  * @retval				none
*/
void Gimbal_Control_Task(void const * argument)
{
	int16_t get_pitch_pos;
	float 	set_pitch_pos;			  //电机转速设定值
	static  float pitch_current_value = 0;
	Motor6623_PID_Init();
	portTickType xLastWakeTime;
	xLastWakeTime = xTaskGetTickCount();
	
	for(;;)
	{
		Refresh_Task_OffLine_Time(GimbalContrlTask_TOE);
		
		set_pitch_pos = 200;
		get_pitch_pos = motor_get[PITCH_6020MOTOR].total_angle;//正面看逆时针为正
		pitch_current_value = PID_Calc(&motor_pid[PID_PITCH_6020MOTOR_ID_POS], get_pitch_pos, set_pitch_pos);
		Gimbal_Motor6020(&hcan1,0,pitch_current_value);
		
		osDelayUntil(&xLastWakeTime,GIMBAL_PERIOD);		

	}

}

/**
  * @brief				PID初始化函数（6020）
  * @param[in]		
	* @param[out]		
  * @retval				none
*/
void Motor6623_PID_Init(void)   
{
	PID_Param_Init(&motor_pid[PID_PITCH_6020MOTOR_ID_POS], DELTA_PID, 30000, 30000,
									20.0f, 0.001f, 0.25f);
	
}

