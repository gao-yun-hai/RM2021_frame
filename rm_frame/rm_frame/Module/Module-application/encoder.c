/*******************************************************************************
                      版权所有 (C), 2020-,NCUROBOT
 *******************************************************************************
  文 件 名   : bsp_tim.c
  版 本 号   : V1.0
  作    者   : 高云海
  生成日期   : 2020.12.8
  最近修改   : 
  功能描述   : 外置编码器TIM初始化、TIM计数及编码器检测方向所用IO口外部中断回调函数
  函数列表   : 1) Encoder_Mini512_TIM_Init()			【外部调用：bsp.c】
							 2) Get_TIM_COUNTER()								【外部调用：chassis_control_task.c】
							 3) HAL_GPIO_EXTI_Callback()				【HAL库函数，外部中断回调函数】							 								
*******************************************************************************/
/* 包含头文件 ----------------------------------------------------------------*/
#include "encoder.h"
/* 内部宏定义 ----------------------------------------------------------------*/
#define EM_htim    htim2//编码器计数所用定时器

/* 内部自定义数据类型的变量 --------------------------------------------------*/

/* 内部变量 ------------------------------------------------------------------*/

/* 内部函数原型声明 ----------------------------------------------------------*/

/* 函数主体部分 --------------------------------------------------------------*/
/**
  * @brief				mini512编码器初始化
  * @param[out]		
  * @param[in]		
  * @retval				
*/
void Encoder_Mini512_TIM_Init(void)
{
	HAL_TIM_Base_Start(&EM_htim);				//启动定时器进行外部脉冲计数
}

/**
  * @brief				获取tim的计数值
  * @param[out]		
  * @param[in]		
  * @retval				
*/
int32_t Get_TIM_COUNTER(void)
{	
	return  __HAL_TIM_GET_COUNTER(&EM_htim);	
}

/**
  * @brief				外部中断回调函数，检测编码器转动方向
  * @param[out]		
  * @param[in]		
  * @retval				
*/
void	HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if(GPIO_Pin == Encoder_Direction_Pin)
	{
		if(HAL_GPIO_ReadPin(Encoder_Direction_GPIO_Port,Encoder_Direction_Pin)==1)
			EM_htim.Instance->CR1 &=~(1<<4);//对CR1寄存器的DIR位置0，即向下计数。
			
		else if(HAL_GPIO_ReadPin(Encoder_Direction_GPIO_Port,Encoder_Direction_Pin)==0)
			EM_htim.Instance->CR1 |=(1<<4);//对CR1寄存器的DIR位置1，即向上计数。
	}
}

