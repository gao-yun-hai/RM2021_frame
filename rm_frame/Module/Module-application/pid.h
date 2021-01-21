#ifndef  __PID_H
#define  __PID_H

/* 包含头文件 ---------------------------------------------------------------*/
#include "myinclude.h"
/* 本模块向外部提供的宏定义 -------------------------------------------------*/

/* 本模块向外部提供的结构体/枚举定义 ----------------------------------------*/

enum PID_MODE
{
		POSITION_PID =0,
		DELTA_PID = 1,
};

enum Time
{
    LLAST	= 0,//上上一次
    LAST 	= 1,//上一次
    NOW 	= 2,//最新一次
};

typedef enum
{
	PID_CHASSIS_3508MOTOR_SPEED,
	PID_CHASSIS_3508MOTOR_POS,
	PID_TRIGGER_2006MOTOR_SPEED,
	PID_TRIGGER_2006MOTOR_POS,
	PID_YAW_3508MOTOR_ID_SPEED,
	PID_YAW_3508MOTOR_ID_POS,
	PID_PITCH_6020MOTOR_ID_SPEED,
	PID_PITCH_6020MOTOR_ID_POS,
	
	NUM_OF_PID//所有电机进行PID运算时所需PID结构体的数量
	
}PID_ID;//电机闭环控制时各电机速度环或位置环PID结构体对应存储的数组编号


typedef struct __pid_t
{
	  uint8_t pid_mode;       	//PID模式

		//PID三参数
    float Kp;			
    float Ki;
    float Kd;

    float set;								//目标值
    float get;								//测量值
    float err[3];							//误差	,包含NOW， LAST，LLAST

    float pout;								//p输出
    float iout;								//i输出
    float dout;								//d输出
    float last_dout;					//上一次d输出
	
		float max_pout;						//比例限幅
		float max_iout;						//积分限幅
		float max_dout;						//微分限幅
		float max_out;						//输出限幅		
		float max_err;						//最大误差
    float deadband;						//死区 

    float pos_out;						//本次位置式输出
    float delta_out;					//本次增量式输出

} pid_t;
/* 本模块向外部提供的变量声明 -----------------------------------------------*/

/* 本模块向外部提供的自定义数据类型变量声明 ---------------------------------*/
extern pid_t   motor_pid[NUM_OF_PID];

/* 本模块向外部提供的接口函数原型声明 ---------------------------------------*/
void PID_Param_Init(pid_t *pid, uint8_t mode, float max_out, float max_iout, float Kp, float Ki, float Kd);
float PID_Calc(pid_t* pid, float get, float set);
void PID_Clear(pid_t *pid);



#endif


