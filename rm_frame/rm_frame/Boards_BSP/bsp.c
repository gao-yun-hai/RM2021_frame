/*******************************************************************************
                      版权所有 (C), 2020-,NCUROBOT
 *******************************************************************************
  文 件 名   : bsp.c
  版 本 号   : V1.0
  作    者   : 高云海
  生成日期   : 2020.11.18
  最近修改   : 
  功能描述   : 所用外设初始化汇总
  函数列表   : 1) BSP_Init()			【外部调用：main.c】
							 2) Power_ON()			【内部调用：BSP_Init()】
*******************************************************************************/
/* 包含头文件 ----------------------------------------------------------------*/
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


/* 内部宏定义 ----------------------------------------------------------------*/

/* 内部自定义数据类型的变量 --------------------------------------------------*/

/* 内部变量 ------------------------------------------------------------------*/

/* 内部函数原型声明 ----------------------------------------------------------*/
void Power_ON (void);

/* 函数主体部分 --------------------------------------------------------------*/

/**
  * @brief				所用外设初始化函数【主函数中调用】
  * @param[out]		
  * @param[in]		
  * @retval				
*/
void BSP_Init (void)
{
	Power_ON();													//新开发A板，开启24V供电接口	
	MPU_Device_SPI_Init();							//板载陀螺仪初始化					【spi5】
	Remote_Control_USART_Init();				//遥控器数据接收初始化			【usart1】
	Wireless_Debug_USART_Init();				//PID无线调参初始化					【usart2】
	RefereeDate_Receive_USART_Init();		//裁判系统数据接收初始化		【usart6】
	VisionDate_Receive_USART_Init();		//视觉数据接收初始化				【usart7】
	Encoder_Mini512_TIM_Init();					//mini512编码器初始化				【tim2】
	Off_Line_Detect_Init();							//断线检测初始化						【tim3】	
	Shoot_Firction_Motor_PWM_Init();		//PWM驱动摩擦轮电机初始化		【tim5】
	BSP_CAN_Init(&hcan1);								//CAN1初始化								【can1】
	BSP_CAN_Init(&hcan2);								//CAN2初始化								【can2】
}


/**
  * @brief				新开发A板，开启四个24V供电接口
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
