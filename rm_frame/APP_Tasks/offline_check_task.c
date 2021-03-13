/*******************************************************************************
                      版权所有 (C), 2020-,NCUROBOT
 *******************************************************************************
  文 件 名   : offline_check_task.c
  版 本 号   : V1.0
  作    者   : 高云海
  生成日期   : 2020.12.10
  最近修改   :
  功能描述   : 断线检测任务，用于调用任务断线检测函数与外设断线检测函数来判断
							 任务或外设是否掉线，若掉线并进行相关处理。
							 注：需要进一步完善
  函数列表   : 1) OffLine_Check_Task()			【FreeRTOS函数，操作系统任务调度运行】
*******************************************************************************/
/* 包含头文件 ----------------------------------------------------------------*/
#include "offline_check_task.h"
#include "offline_check.h"
/* 内部宏定义 ----------------------------------------------------------------*/

/* 内部自定义数据类型的变量 --------------------------------------------------*/

/* 内部变量 ------------------------------------------------------------------*/

/* 内部函数原型声明 ----------------------------------------------------------*/

/* 函数主体部分 --------------------------------------------------------------*/
/*********************************************************
off_line.task_offline_flag为16位数据0000 0000 0000 0000
0000 0000 0000 0001即0x01：遥控器任务掉线
0000 0000 0000 0010即0x02：视觉任务掉线
0000 0000 0000 0100即0x04：裁判系统掉线
0000 0000 0000 1000即0x08：摩擦轮任务掉线
0000 0000 0001 0000即0x10：拨盘任务掉线
0000 0000 0010 0000即0x20：云台任务掉线
0000 0000 0100 0000即0x40：底盘任务掉线
0000 0000 1000 0000即0x80：断线检测任务掉线
----------------------------------------------------------
off_line.device_offline_flag为16位数据0000 0000 0000 0000
0000 0000 0000 0001即0x01：无法接收yaw轴电机返回值
0000 0000 0000 0010即0x02：无法接收pitch轴电机返回值
0000 0000 0000 0100即0x04：无法接收拨盘电机返回值
0000 0000 0000 1000即0x08：无法接收底盘电机返回值
0000 0000 0001 0000即0x10：无法接收遥控器返回值
0000 0000 0010 0000即0x20：无法接收裁判系统返回值
0000 0000 0100 0000即0x40：无法接收视觉返回值
*********************************************************/

/**
  * @brief				断线检测任务
  * @param[in]		
	* @param[out]		
  * @retval				none
*/
void OffLine_Check_Task(void const *argument)
{
	osDelay(1000);

	portTickType xLastWakeTime;
  xLastWakeTime = xTaskGetTickCount();
	for(;;)
	{		
		Refresh_Task_OffLine_Time(OutLineCheckTask_TOE);
		
		Task_OffLine_Check();//任务检测
		Device_OffLine_Check();//断线检测
		
		//任务掉线对应操作
		if((off_line.task_offline_flag & 0x01))//遥控器任务掉线
		{
//			printf("Remote_Data_Task is offline\r\n");
		}
				
		if((off_line.task_offline_flag & 0x02))//视觉任务掉线
		{
//			printf("Vision_Data_Task is offline\r\n");
		} 
		
		if((off_line.task_offline_flag & 0x04))//裁判系统掉线
		{
//			printf("Referee_Data_Task is offline\r\n");
		}	
		
		if((off_line.task_offline_flag & 0x08))//摩擦轮任务掉线
		{
//			printf("Friction_Drive_Task is offline\r\n");
		}
		
		if((off_line.task_offline_flag & 0x10))//拨盘任务掉线
		{
//			printf("Trigger_Drive_Task is offline\r\n");
		}	

		if((off_line.task_offline_flag & 0x20))//云台任务掉线
		{
//			printf("Gimbal_Control_Task is offline\r\n");
		}	
		
		if((off_line.task_offline_flag & 0x40))//底盘任务掉线
		{
//			printf("Chassis_Control_Task is offline\r\n");
		}	
		
		if((off_line.task_offline_flag & 0x80))//断线检测任务掉线
		{
//			printf("OffLine_Check_Task is offline\r\n");
		}
		
		
		//外设掉线对应操作
		if((off_line.device_offline_flag & 0x01))//无法接收yaw轴电机返回值 LED1
		{
			LED1_OFF();
//			printf("Cannot receive yaw_motor return data\r\n");
		}
		else 
			LED1_ON();
		
		if((off_line.device_offline_flag & 0x02))//无法接收pitch轴电机返回值 LED2
		{
			LED2_OFF();
//			printf("Cannot receive pitch_motor return data\r\n");
		}
		else 
			LED2_ON();
		
		if((off_line.device_offline_flag & 0x04))//无法接收拨盘电机返回值 LED3
		{
			LED3_OFF();
//			printf("Cannot receive trigger_motor return data\r\n");
		}
		else 
			LED3_ON();		

		if((off_line.device_offline_flag & 0x08))//无法接收底盘电机返回值 LED4
		{
			LED4_OFF();
//			printf("Cannot receive chassis_motor return data\r\n");
		}
		else 
			LED4_ON();
		
		if((off_line.device_offline_flag & 0x10))//无法接收遥控器返回值 LED5
		{
			LED5_OFF();
//			printf("Cannot receive remote_control return data\r\n");
		}
		else 
			LED5_ON();

		if((off_line.device_offline_flag & 0x20))//无法接收裁判系统返回值 LED6
		{
			LED6_OFF();
//			printf("Cannot receive referee return data\r\n");
		}
		else 
			LED6_ON();		
		
		if((off_line.device_offline_flag & 0x40))//无法接收视觉返回值 LED7
		{
			LED7_OFF();
//			printf("Cannot receive vision return data\r\n");
		}
		else 
			LED7_ON();	
		
		
		osDelayUntil(&xLastWakeTime,OFFLINE_PERIOD);
		
	}
}
