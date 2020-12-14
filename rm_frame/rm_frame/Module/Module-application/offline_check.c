/*******************************************************************************
                      版权所有 (C), 2020-,NCUROBOT
 *******************************************************************************
  文 件 名   : offline_check.c
  版 本 号   : V1.0
  作    者   : 高云海
  生成日期   : 2020.12.13
  最近修改   :
  功能描述   : 断线检测所用TIM初始化及断线检测相关函数
  函数列表   : 1) Off_Line_Detect_Init()						【外部调用：bsp.c】
							 2) Refresh_SysTime()									【内部调用：HAL_TIM_PeriodElapsedCallback】
							 3) HAL_TIM_PeriodElapsedCallback()		【HAL库函数，TIM中断回调函数】
							 //使用CubeMX修改工程并生成程序时需要将main中的该函数注释
							 4) Refresh_Device_OffLine_Time()			【外部调用：使用处】
							 5) Refresh_Task_OffLine_Time()				【外部调用：使用处】
							 6) Device_OffLine_Check()						【外部调用：offline_check_task.c】
							 7) Task_OffLine_Check()							【外部调用：offline_check_task.c】
	
*******************************************************************************/
/* 包含头文件 ----------------------------------------------------------------*/
#include "offline_check.h"
/* 内部宏定义 ----------------------------------------------------------------*/
#define OL_htim    htim3//断线检测计时所用定时器

/* 内部自定义数据类型的变量 --------------------------------------------------*/
Off_Line_TypeDef  off_line;

/* 内部变量 ------------------------------------------------------------------*/
float g_Time_Device_OffLine[DeviceTotal_TOE] = {0};//外设最近一次通信时间数组
float g_Time_Task_OffLine[TaskTotal_TOE] = {0};//任务最近一次通信时间数组

/* 内部函数原型声明 ----------------------------------------------------------*/
inline float Get_System_Timer(void);
void Refresh_SysTime(void);

/* 函数主体部分 --------------------------------------------------------------*/
/**
  * @brief				断线检测初始化
  * @param[out]		
  * @param[in]		
  * @retval				
*/
uint8_t Off_Line_Detect_Init(void)
{
	int state;
	off_line.enable = 0;
	off_line.state = 0;
	off_line.task = 0;
	off_line.time = 0;
	off_line.htim = &OL_htim;//计时器请设定 每 10us 记一个数  重载值为 100-1 (1ms)  例如 Timer3 主频168M 预分频 (840-1) 重载值 (100-1)
	state=HAL_TIM_Base_Start_IT(off_line.htim);//启动时间计数器
	
  return state;
}

/**
  * @brief				更新系统时间 ms(中断刷新中调用)
  * @param[out]		
  * @param[in]		
  * @retval				
*/
void Refresh_SysTime(void)
{
		off_line.time += 1;
}
/**
  * @brief				TIM中断回调函数（需要将main.c中cubemx生成的TIM中断回调函数注释）
  * @param[out]		
  * @param[in]		
  * @retval				
*/
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance == TIM1) 
	{
    HAL_IncTick();
  }

	else if(htim == (&OL_htim))
	{
		Refresh_SysTime();
	}	
}
/**
  * @brief				获得任务当前运行时间
  * @param[out]		
  * @param[in]		
  * @retval				
*/
float Get_System_Timer(void)
{
	return off_line.htim->Instance->CNT / 100.0 + off_line.time;   //经计算，得到的最终时间单位为ms
}

/**
  * @brief				刷新外设通信时间数组
  * @param[out]		
  * @param[in]		
  * @retval				
*/
void Refresh_Device_OffLine_Time(DeviceX_DEF DevX_TOE)
{	
	g_Time_Device_OffLine[DevX_TOE] = Get_System_Timer();	
}

/**
  * @brief				刷新任务时间数组
  * @param[out]		
  * @param[in]		
  * @retval				
*/
void Refresh_Task_OffLine_Time(TaskX_DEF TaskX_TOE)
{	
	g_Time_Task_OffLine[TaskX_TOE] = Get_System_Timer();	
}

/**
  * @brief				外设断线检测检测
  * @param[out]		
  * @param[in]		
  * @retval				
*/
void Device_OffLine_Check(void)
{
	uint8_t num=0;//临时变量累加用
	float time=Get_System_Timer();//当前系统时间

	for(num=0; num<DeviceTotal_TOE; num++)
	{
		if(time-g_Time_Device_OffLine[num] > OFFLINE_TIME)
		{
			MyFlagSet(off_line.device_offline_flag,(num));//设置断线标志
		}
		else
		{
			MyFlagClear(off_line.device_offline_flag,(num));//清除断线标志
		}
	}
}

/**
  * @brief				任务断线检测
  * @param[out]		
  * @param[in]		
  * @retval				
*/
void Task_OffLine_Check(void)
{
	short num=0;//临时变量累加用
	float time=Get_System_Timer();//当前系统时间

	for(num=0;num<TaskTotal_TOE;num++)
	{
		if(time-g_Time_Task_OffLine[num] > OFFLINE_TIME)
		{
			MyFlagSet(off_line.task_offline_flag,(num));//设置断线标志
		}
		else
		{
			MyFlagClear(off_line.task_offline_flag,(num));//清除断线标志
		}
	}
}
