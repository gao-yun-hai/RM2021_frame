/*******************************************************************************
                      版权所有 (C), 2017-,NCUROBOT
 *******************************************************************************
  文 件 名   : power_restriction.c
  版 本 号   : 初稿
  作    者   : NCUERM
  生成日期   : 2018年7月
  最近修改   :
  功能描述   : 底盘功率限制
  函数列表   :void power_limit(float  Current_get)
*******************************************************************************/

/* 包含头文件 ----------------------------------------------------------------*/
#include "power_restriction.h"
#include "referee.h"

/* 内部宏定义 ----------------------------------------------------------------*/
#define POWER_LIMIT         80.0f
#define WARNING_POWER       40.0f   
#define WARNING_POWER_BUFF  50.0f  

#define NO_JUDGE_TOTAL_CURRENT_LIMIT    64000.0f  //16000 * 4,    
#define BUFFER_TOTAL_CURRENT_LIMIT      16000.0f
#define POWER_TOTAL_CURRENT_LIMIT       20000.0f

/* 内部常量定义---------------------------------------------------------------*/

/* 外部变量声明 --------------------------------------------------------------*/

/* 外部函数原型声明 ----------------------------------------------------------*/

/* 内部变量 ------------------------------------------------------------------*/

/* 函数原型声明 ----------------------------------------------------------*/
/**
  * @brief          限制功率，主要限制电机电流(底盘共有四个电机)
  * @param[in]      chassis_power_control: 底盘数据
  * @retval         none
  */
void chassis_power_control(float current[4])
{
    fp32 chassis_power = 0.0f;
    fp32 chassis_power_buffer = 0.0f;
    fp32 total_current_limit = 0.0f;
    fp32 total_current = 0.0f;

    get_chassis_power_and_buffer(&chassis_power, &chassis_power_buffer);
    // power > 80w and buffer < 60j, because buffer < 60 means power has been more than 80w
    //功率超过80w 和缓冲能量小于60j,因为缓冲能量小于60意味着功率超过80w
    if(chassis_power_buffer < WARNING_POWER_BUFF)
    {
        fp32 power_scale;
        if(chassis_power_buffer > 5.0f)
        {
            //scale down WARNING_POWER_BUFF
            //缩小WARNING_POWER_BUFF
            power_scale = chassis_power_buffer / WARNING_POWER_BUFF;
        }
        else
        {
            //only left 10% of WARNING_POWER_BUFF
            power_scale = 5.0f / WARNING_POWER_BUFF;
        }
        //scale down
        //缩小
        total_current_limit = BUFFER_TOTAL_CURRENT_LIMIT * power_scale;
    }
    else
    {
        //power > WARNING_POWER
        //功率大于WARNING_POWER
        if(chassis_power > WARNING_POWER)
        {
            fp32 power_scale;
            //power < 80w
            //功率小于80w
            if(chassis_power < POWER_LIMIT)
            {
                //scale down
                //缩小
                power_scale = (POWER_LIMIT - chassis_power) / (POWER_LIMIT - WARNING_POWER);
                
            }
            //power > 80w
            //功率大于80w
            else
            {
                power_scale = 0.0f;
            }
            
            total_current_limit = BUFFER_TOTAL_CURRENT_LIMIT + POWER_TOTAL_CURRENT_LIMIT * power_scale;
        }
        //power < WARNING_POWER
        //功率小于WARNING_POWER
        else
        {
            total_current_limit = BUFFER_TOTAL_CURRENT_LIMIT + POWER_TOTAL_CURRENT_LIMIT;
        }
    }  

     //calculate the original motor current set
    //计算原本电机总电流设定
    for(uint8_t i = 0; i < 4; i++)
    {
        total_current += fabs(current[i]);
    }
    //电流重新分配
    if(total_current > total_current_limit)
    {
        fp32 current_scale = total_current_limit / total_current;
        current[0]*=current_scale;
        current[1]*=current_scale;
        current[2]*=current_scale;
        current[3]*=current_scale;
    }
}


