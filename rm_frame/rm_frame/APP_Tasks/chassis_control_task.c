/*******************************************************************************
                      版权所有 (C), 2020-,NCUROBOT
 *******************************************************************************
  文 件 名   : chassis_control_task.c
  版 本 号   : V1.0
  作    者   : 高云海
  生成日期   : 2020.12.10
  最近修改   : 
  功能描述   : 底盘任务，待完善
  函数列表   : 1) Chassis_Control_Task()		【FreeRTOS函数：操作系统任务调度运行】
							 2) motor_3508_pid_init()			【内部函数：Chassis_Control_Task调用】
*******************************************************************************/
/* 包含头文件 ----------------------------------------------------------------*/
#include "chassis_control_task.h"
#include "pid.h"
#include "encoder.h"
#include "offline_check.h"
#include "motor_use_can.h"

/* 内部宏定义 ----------------------------------------------------------------*/

/* 内部自定义数据类型的变量 --------------------------------------------------*/

/* 内部变量 ------------------------------------------------------------------*/
int32_t mini512_cnt = 0;
/* 内部函数原型声明 ----------------------------------------------------------*/
void Motor3508_PID_Init(void);   

/* 函数主体部分 --------------------------------------------------------------*/
/**
  * @brief				底盘任务
  * @param[out]		
  * @param[in]		
  * @retval				
*/
void Chassis_Control_Task(void const * argument)
{
	int16_t get_speed_z;
	float 	set_speed_z;			  //电机转速设定值
	static  float Current_Value = 0;
	
	Motor3508_PID_Init();//使用无线调参时需要将该函数注释掉
	
	portTickType xLastWakeTime;
	xLastWakeTime = xTaskGetTickCount();

	for(;;)
	{
		Refresh_Task_OffLine_Time(ChassisControlTask_TOE);
		mini512_cnt = Get_TIM_COUNTER();
		set_speed_z = 500;
		get_speed_z = motor_get[CHASSIS_3508MOTOR].speed_rpm;//正面看逆时针为正
		PID_Calc(&motor_pid[PID_CHASSIS_3508MOTOR_SPEED], get_speed_z, set_speed_z);
		Current_Value = (motor_pid[PID_CHASSIS_3508MOTOR_SPEED].pos_out);
	
		Chassis_Motor3508(&hcan1,Current_Value,0,0,0);
		
		
		osDelayUntil(&xLastWakeTime,CHASSIS_PERIOD);		

	}

}

/**
  * @brief				PID初始化函数（3508）
  * @param[out]		
  * @param[in]		
  * @retval				
*/
void Motor3508_PID_Init(void)   
{
	PID_Param_Init(&motor_pid[PID_CHASSIS_3508MOTOR_SPEED], POSITION_PID, 1000, 3000,
									10.0f, 0.0f, 0.0f);
}

