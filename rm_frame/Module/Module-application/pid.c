/*******************************************************************************
                      版权所有 (C), 2020-,NCUROBOT
 *******************************************************************************
  文 件 名   : pid.c
  版 本 号   : V1.1
  作    者   : 高云海
  生成日期   : 2020.12.1
  最近修改   : 将原始pic的.c/.h文件替换为新.c/.h文件。主要是简化各函数与变量。
							 原pid文件放置于 Module -> Module-application -> old_pid 文件夹中
  功能描述   : pid实现函数【PID初始化，PID计算，PID输出清除】
  函数列表   : 1) PID_Param_Init() 				【外部调用：使用处，仅需执行一次】
							 2) PID_Calc()							【外部调用：使用处】
							 3) PID_Clear()							【外部调用：使用处】
*******************************************************************************/
/* 包含头文件 ----------------------------------------------------------------*/
#include "pid.h"

/* 内部宏定义 ----------------------------------------------------------------*/

/* 内部自定义数据类型的变量 --------------------------------------------------*/
pid_t  motor_pid[NUM_OF_PID] = {0};

/* 内部变量 ------------------------------------------------------------------*/

/* 内部函数原型声明 ----------------------------------------------------------*/

/* 函数主体部分 --------------------------------------------------------------*/
/**
  * @brief				PID结构体参数初始化
  * @param[out]		*pid: PID结构数据指针
  * @param[in]		mode: PID模式
									max_out: PID最大输出
									max_iout：PID最大积分输出，即积分限幅
									Kp,Ki,Kd：P、I、D三个参数初始化
  * @retval				
*/
void PID_Param_Init(pid_t *pid, uint8_t mode, float max_out, float max_iout, float Kp, float Ki, float Kd)
{   
	  if (pid == NULL)
    {
        return;
    }
    pid->max_iout = max_iout;
    pid->max_out = max_out;
    pid->pid_mode = mode;
    
    pid->Kp = Kp;
    pid->Ki = Ki;
    pid->Kd = Kd;
		
		pid->err[LLAST] = pid->err[LAST] = pid->err[NOW] = 0.0f;
    pid->pout = pid->iout = pid->dout = pid->last_dout = 0.0f;
		pid->pos_out = pid->delta_out = 0.0f;

}

/**
  * @brief				PID计算
  * @param[out]		*pid: PID结构数据指针
  * @param[in]		get: 反馈数据    set: 设定值
  * @retval				pid输出
*/                
float PID_Calc(pid_t* pid, float get, float set)
{
	  if (pid == NULL)
    {
        return 0.0f;
    }
	
    pid->get = get;
    pid->set = set;
    pid->err[NOW] = set - get;
		
    if (pid->max_err != 0 && fabs(pid->err[NOW]) > pid->max_err)
		return 0;
		if (pid->deadband != 0 && fabs(pid->err[NOW]) < pid->deadband)
		return 0;
    
    if(pid->pid_mode == POSITION_PID) //位置式
    {
        pid->pout = pid->Kp * pid->err[NOW];
        pid->iout += pid->Ki * pid->err[NOW];
        pid->dout = pid->Kd * (pid->err[NOW] - pid->err[LAST] );			  
		    pid->dout = LPF_1st(pid->last_dout,pid->dout,0.6);
				pid->last_dout = pid->dout;
			
        abs_limit(&(pid->iout), pid->max_iout);
        pid->pos_out = pid->pout + pid->iout + pid->dout;
        abs_limit(&(pid->pos_out), pid->max_out);

    }
    else if(pid->pid_mode == DELTA_PID)//增量式
    {
        pid->pout = pid->Kp * (pid->err[NOW] - pid->err[LAST]);
        pid->iout = pid->Ki * pid->err[NOW];
        pid->dout = pid->Kd * (pid->err[NOW] - 2*pid->err[LAST] + pid->err[LLAST]);
				pid->dout = LPF_1st(pid->last_dout,pid->dout,0.6);
				pid->last_dout = pid->dout;
        
        abs_limit(&(pid->iout), pid->max_iout);
        pid->delta_out += pid->pout + pid->iout + pid->dout;
        abs_limit(&(pid->delta_out), pid->max_out);
    }
		
    pid->err[LAST] = pid->err[NOW];
    pid->err[LLAST] = pid->err[LAST];   
		
    return pid->pid_mode==POSITION_PID ? pid->pos_out : pid->delta_out;
	
}

/**
  * @brief          pid 输出清除
  * @param[out]     pid: PID结构数据指针
	* @param[in]
  * @retval         none
  */
void PID_Clear(pid_t *pid)
{
    if (pid == NULL)
    {
        return;
    }

    pid->err[LLAST] = pid->err[LAST] = pid->err[NOW] = 0.0f;
		
		if(pid->pid_mode == POSITION_PID)
		{
			pid->pout = pid->iout = pid->dout = pid->last_dout = 0.0f;
			pid->pos_out  = 0.0f;
		}
		else
		{
			pid->pout = pid->iout = pid->dout = 0.0f;
			pid->delta_out = 0.0f;		
		}		
}
