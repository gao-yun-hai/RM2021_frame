/*******************************************************************************
                      版权所有 (C), 2020-,NCUROBOT
 *******************************************************************************
  文 件 名   : motor_use_tim.c
  版 本 号   : V1.0
  作    者   : NCURM
  生成日期   : 2018年7月
  最近修改   : 2020.11.28
  功能描述   : 电机库模块中使用TIM进行控制的电机【PWM初始化、比较值设定】
  函数列表   : 1) TIM_Compare_Value_Set()				   【内部调用】
							 2) Shoot_Firction_Motor()				   【外部调用：使用处】
							 3) Shoot_Firction_Motor_Stop()	     【外部调用：使用处】
							 4) Snail_PWM_Init()								 【外部调用：bsp.c】
*******************************************************************************/
/* 包含头文件 ----------------------------------------------------------------*/
#include "motor_use_tim.h"

/* 内部宏定义 ----------------------------------------------------------------*/
#define PWM_htim  htim5
/* 内部自定义数据类型的变量 --------------------------------------------------*/

/* 内部变量 ------------------------------------------------------------------*/


/* 内部函数原型声明 ----------------------------------------------------------*/
void TIM_Compare_Value_Set(uint32_t value1,uint32_t value2);

/* 函数主体部分 --------------------------------------------------------------*/
/*
TIM4：通道1-通道4对应 -> PD12/PD13/PD14/PD15
TIM5：通道1-通道4对应 -> PH10/PH11/PH12/PI0
目前只使用TIM5通道1与通道2，其余均完成测试但未使用到。
*/
/**
  * @brief				TIM捕获/比较寄存器的预装载值设定
  * @param[out]		
  * @param[in]		value1：通道CC1的值。value2：通道CC2的值									
  * @retval				
*/
void TIM_Compare_Value_Set(uint32_t value1,uint32_t value2)
{
	  PWM_htim.Instance->CCR1=value1;
	  PWM_htim.Instance->CCR2=value2;
}

/**
  * @brief				摩擦轮电机驱动函数（通过设定比较值进而进行设定转速）
  * @param[out]		
  * @param[in]		两轮转速:wheel1；wheel2
  * @retval				
*/
void Shoot_Firction_Motor(uint32_t wheel1,uint32_t wheel2)
{
  TIM_Compare_Value_Set(wheel1,wheel2);
}

/**
  * @brief				关闭摩擦轮
  * @param[out]		
  * @param[in]		
  * @retval				none
*/
void Shoot_Firction_Motor_Stop(void)
{
  PWM_htim.Instance->CNT = 0;
	PWM_htim.Instance->CCR1 = 0;
  PWM_htim.Instance->CCR2 = 0;
	__HAL_TIM_DISABLE(&PWM_htim);  //关闭定时器
  HAL_TIM_PWM_Stop(&PWM_htim,TIM_CHANNEL_1);
  HAL_TIM_PWM_Stop(&PWM_htim,TIM_CHANNEL_2);	
}

/**
  * @brief				摩擦轮重启
  * @param[in]		
	* @param[out]		
  * @retval				none
*/
void Shoot_Firction_Motor_Restart(void)
{
    Shoot_Firction_Motor_Stop();
    osDelay(2000);
    __HAL_TIM_ENABLE(&PWM_htim);									//开启定时器
    HAL_TIM_PWM_Start(&PWM_htim,TIM_CHANNEL_1);   //PH10,右侧摩擦轮(TIM5)
    HAL_TIM_PWM_Start(&PWM_htim,TIM_CHANNEL_2);   //PH11,左侧摩擦轮(TIM5)
}

/**
  * @brief				摩擦轮电机初始化函数
  * @param[out]		
  * @param[in]		
  * @retval				none
*/
void Shoot_Firction_Motor_PWM_Init(void)
{
	__HAL_TIM_ENABLE(&PWM_htim);									//开启定时器
	HAL_TIM_PWM_Start(&PWM_htim,TIM_CHANNEL_1);   //PH10,右侧摩擦轮(TIM5)
	HAL_TIM_PWM_Start(&PWM_htim,TIM_CHANNEL_2);   //PH11,左侧摩擦轮(TIM5)
	Shoot_Firction_Motor (HIGH_SPEED,HIGH_SPEED);
	Shoot_Firction_Motor (LOW_SPEED,LOW_SPEED); 
}
