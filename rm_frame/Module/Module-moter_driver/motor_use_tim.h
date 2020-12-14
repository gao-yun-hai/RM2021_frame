#ifndef  __Motor_USE_TIM_H
#define  __Motor_USE_TIM_H
/* 包含头文件 ---------------------------------------------------------------*/
#include "myinclude.h"

/* 本模块向外部提供的宏定义 -------------------------------------------------*/
#define HIGH_SPEED 200
#define LOW_SPEED  100

/* 本模块向外部提供的结构体/枚举定义 ----------------------------------------*/

/* 本模块向外部提供的变量声明 -----------------------------------------------*/

/* 本模块向外部提供的自定义数据类型变量声明 ---------------------------------*/

/* 本模块向外部提供的接口函数原型声明 ---------------------------------------*/
void Shoot_Firction_Motor_PWM_Init(void);
void Shoot_Firction_Motor(uint32_t wheelone,uint32_t wheeltwo);
void Shoot_Firction_Motor_Stop(void);


#endif
