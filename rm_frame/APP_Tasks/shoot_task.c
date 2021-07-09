/*******************************************************************************
                      版权所有 (C), 2020-,NCUROBOT
 *******************************************************************************
  文 件 名   : shoot_task.c
  版 本 号   : V1.0
  作    者   : 高云海
  生成日期   : 2020.12.1
  最近修改   :
  功能描述   : 射击任务，包括摩擦轮任务与拨盘任务
  函数列表   : 1) Friction_Drive_Task()		【FreeRTOS函数：操作系统任务调度运行】
							 2) Trigger_Drive_Task()		【FreeRTOS函数：操作系统任务调度运行】
*******************************************************************************/
/* 包含头文件 ----------------------------------------------------------------*/
#include "shoot_task.h"
#include "motor_use_tim.h"
#include "remote_control.h"
#include "motor_use_can.h"
#include "offline_check.h"
#include "referee.h"

/* 内部宏定义 ----------------------------------------------------------------*/

/* 内部自定义数据类型的变量 --------------------------------------------------*/
shoot_control_t shoot_control;          //射击数据结构体

/* 内部变量 ------------------------------------------------------------------*/

/* 内部函数原型声明 ----------------------------------------------------------*/

/* 函数主体部分 --------------------------------------------------------------*/
//===================================================================================================================//
/******************************************************摩擦轮*********************************************************/
//===================================================================================================================//
/**
  * @brief				摩擦轮任务
  * @param[in]		
	* @param[out]		
  * @retval				none
*/
void Friction_Drive_Task(void const *argument)
{

	portTickType xLastWakeTime;
	xLastWakeTime = xTaskGetTickCount();

	//斜坡函数初始化
	ramp_init(&shoot_control.fric1_ramp, 0.01f, 120,100);//0.01为斜坡函数的斜率(时间间隔)  120和100为最大、最小值(最大值也为目标值)
	ramp_init(&shoot_control.fric2_ramp, 0.01f, 120,100);          
	
	for(;;)  
	{
		Refresh_Task_OffLine_Time(FrictionDriveTask_TOE);//记录任务运行的时间点

    //摩擦轮重启（可跟换为按键控制）
    if(rc_ctrl.rc.s2 == RIGHT_S2_UP)
    {
        Shoot_Firction_Motor_Restart();
        shoot_control.fric1_ramp.max_value = 100;
        shoot_control.fric2_ramp.max_value = 100;      
    }
    //摩擦轮转速调节（可跟换为按键控制）
    if(rc_ctrl.rc.s1 == LEFT_S1_UP)
    {
        shoot_control.fric1_ramp.max_value = 120;
        shoot_control.fric2_ramp.max_value = 120;        
    }
    else if(rc_ctrl.rc.s1 == LEFT_S1_DOWN)
    {
        shoot_control.fric1_ramp.max_value = 130;
        shoot_control.fric2_ramp.max_value = 130;    
    }
    else
    {
			shoot_control.fric1_ramp.max_value = 100;
			shoot_control.fric2_ramp.max_value = 100;    
    }
     
 
   ramp_calc(&shoot_control.fric1_ramp, SHOOT_FRIC_PWM_ADD_VALUE);
   ramp_calc(&shoot_control.fric2_ramp, SHOOT_FRIC_PWM_ADD_VALUE);

   Shoot_Firction_Motor(shoot_control.fric1_ramp.out,shoot_control.fric2_ramp.out);
		
	 osDelayUntil(&xLastWakeTime,FRICTION_PERIOD);
		
	}
}



//===================================================================================================================//
/*******************************************************拨盘**********************************************************/
//===================================================================================================================//
/**
  * @brief				拨盘任务
  * @param[in]		
	* @param[out]		
  * @retval				none
*/
void Trigger_Drive_Task(void const * argument)
{
	portTickType xLastWakeTime;
	xLastWakeTime = xTaskGetTickCount();
	
	for(;;)
	{
    Refresh_Task_OffLine_Time(TriggerDriveTask_TOE);//记录任务运行的时间点
    
/****************注意*********************/
    
/*若底盘与云台为不同控制板，可使用拨弹电机驱动函数驱动拨弹电机，对应拨盘ID设为0x203*/
//		Trigger_Motor_Drive();
/*若底盘与云台为相同控制板，则使用云台电机驱动函数驱动拨弹电机，对应拨盘ID设为0x207*/		
//    Gimbal_Motor_Drive();
    
/****************注意*********************/
    
		osDelayUntil(&xLastWakeTime,TRIGGER_PERIOD);		

	}

}

