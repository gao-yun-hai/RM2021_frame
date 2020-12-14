#ifndef __OFFLINE_CHECK_H
#define __OFFLINE_CHECK_H

/* 包含头文件 ---------------------------------------------------------------*/
#include "myinclude.h"
/* 本模块向外部提供的宏定义 -------------------------------------------------*/
#define OFFLINE_TIME 100 //断线检测时间

#define MyFlagSet(x,y) x=x|(0x00000001<<y) 			//设置标志位  y第几位
#define MyFlagClear(x,y) x=x&~(0x00000001<<y)		//清除标志位  y第几位
#define MyFlagGet(x,y) (x&(0x00000001<<y))
/* 本模块向外部提供的结构体/枚举定义 ----------------------------------------*/
typedef enum
{
		YawGimbalMotor_TOE = 0,		//Yaw轴电机
		PitchGimbalMotor_TOE,			//Pitch轴电机
  	TriggerMotor_TOE,					//拨盘电机
		ChassisMotor_TOE,					//底盘电机
  
		DeviceTotal_TOE	
}DeviceX_DEF;

typedef enum
{
	RemoteDataTask_TOE = 0,	
	VisionDataTask_TOE,
	RefereeDataTask_TOE,
	FrictionDriveTask_TOE,
	TriggerDriveTask_TOE,
	GimbalContrlTask_TOE,
	ChassisControlTask_TOE,
	OutLineCheckTask_TOE,
	
	TaskTotal_TOE	
}TaskX_DEF;

typedef struct
{
	uint8_t mode;											//运行模式
	uint8_t enable;										//使能
	uint8_t state;										//状态
	uint8_t task;											//任务
	uint32_t time;										//System run time mm
	TIM_HandleTypeDef *htim;					//时间计数器句柄
	uint8_t device_offline_flag;			//外设断线标志
	uint8_t task_offline_flag;				//任务断线标志
	
}Off_Line_TypeDef;
/* 本模块向外部提供的变量声明 -----------------------------------------------*/
extern Off_Line_TypeDef  off_line;

/* 本模块向外部提供的自定义数据类型变量声明 ---------------------------------*/

/* 本模块向外部提供的接口函数原型声明 ---------------------------------------*/
uint8_t Off_Line_Detect_Init(void);
void Refresh_Device_OffLine_Time(DeviceX_DEF DevX_TOE);
void Refresh_Task_OffLine_Time(TaskX_DEF Task_TOE);
void Device_OffLine_Check(void);
void Task_OffLine_Check(void);



#endif



