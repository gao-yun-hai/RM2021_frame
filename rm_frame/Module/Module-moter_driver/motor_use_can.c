/*******************************************************************************
                      版权所有 (C), 2020-,NCUROBOT
 *******************************************************************************
  文 件 名   : motor_use_can.c
  版 本 号   : V1.1
  作    者   : NCURM
  生成日期   : 2018年7月
  最近修改   : 2020.11.28
  功能描述   : 电机库模块中使用CAN进行控制的电机【CAN发送、CAN接收、CAN中断回调】
							 注：其中遥控器数据及裁判系统发送及接收函数待完善与开发。
  函数列表   : 1) HAL_CAN_RxFifo0MsgPendingCallback() 【HAL库函数，CAN回调函数】
							 //CAN发送
							 2) Gimbal_Motor_Drive()									【外部调用：使用处】
							 3) Gimbal_Motor_Disable()								【外部调用：使用处】
							 4) Gimbal_Motor_Calibration()						【外部调用：使用处】
							 5) Chassis_Motor_Drive()									【外部调用：使用处】
							 6) Chassis_Motor_Disable()								【外部调用：使用处】
							 7) Trigger_Motor_Drive()									【外部调用：使用处】
							 8) CAN_Send_RefereeData()								【外部调用：使用处】
							 9) CAN_Send_RemoteDate()									【外部调用：使用处】
							 //CAN接收
							 10) Get_Moto_Measure_6623()							【内部调用：CAN回调中调用】
							 11) Get_Moto_Measure_3508()							【内部调用：CAN回调中调用】
							 12) Get_Moto_Measure_2006()							【内部调用：CAN回调中调用】
							 13) Get_Moto_Measure_6020()							【内部调用：CAN回调中调用】
							 14) Get_Moto_Offset()										【内部调用：CAN回调中调用】
							 15) CAN_GET_Remote()											【内部调用：CAN回调中调用】
							 16) CAN_GET_RefereeData()								【内部调用：CAN回调中调用】
*******************************************************************************/
/* 包含头文件 ----------------------------------------------------------------*/
#include "motor_use_can.h"
#include "remote_control.h"
#include "offline_check.h"
/* 内部宏定义 ----------------------------------------------------------------*/

/* 内部自定义数据类型的变量 --------------------------------------------------*/
motor_measure_t   motor_get[NUM_OF_MOTOR] = {0};

/* 内部变量 ------------------------------------------------------------------*/


/* 内部函数原型声明 ----------------------------------------------------------*/
void Get_Moto_Measure_3508(motor_measure_t *ptr,uint8_t CAN_RX_date[]);
void Get_Moto_Measure_2006(motor_measure_t *ptr,uint8_t CAN_RX_date[]);
void Get_Moto_Measure_6623(motor_measure_t *ptr,uint8_t CAN_RX_date[]);
void Get_Moto_Measure_6020(motor_measure_t *ptr,uint8_t CAN_RX_date[]);
void Get_Moto_Offset(motor_measure_t *ptr,uint8_t CAN_RX_date[]);
void CAN_GET_Remote(RC_ctrl_t * RC , uint8_t CAN_RX_date[]);
void CAN_GET_RefereeData(uint8_t CAN_RX_date[]);

/* 函数主体部分 --------------------------------------------------------------*/
//===================================================================================================================//
/****************************************************CAN中断回调******************************************************/
//===================================================================================================================//
/**
  * @brief				HAL库CAN回调函数,接收电机数据
  * @param[out]		
  * @param[in]		hcan:CAN句柄指针
  * @retval				none
*/
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
	CAN_RxHeaderTypeDef  CAN1_Rx_Header;
	CAN_RxHeaderTypeDef  CAN2_Rx_Header;
	uint8_t CAN1_RX_date[8];
	uint8_t CAN2_RX_date[8];
	
	if(hcan == &hcan1)
	{
		HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &CAN1_Rx_Header, CAN1_RX_date);
		switch(CAN1_Rx_Header.StdId)
		{
			case CAN_CHASSIS_MOTOR_ID://底盘电机
			{		
				Refresh_Device_OffLine_Time(ChassisMotor_TOE);//刷新时间
				
				if(motor_get[CHASSIS_MOTOR].msg_cnt++ <= 50)	
				{
					Get_Moto_Offset(&motor_get[CHASSIS_MOTOR],CAN1_RX_date);
				}
				else
				{	
					motor_get[CHASSIS_MOTOR].msg_cnt = 51;	
					Get_Moto_Measure_3508(&motor_get[CHASSIS_MOTOR],CAN1_RX_date);
				}
			}break;
			
			case CAN_TRIGGER_MOTOR_ID://拨盘电机 
			{		
				Refresh_Device_OffLine_Time(TriggerMotor_TOE);//刷新时间
				
				if(motor_get[TRIGGER_MOTOR].msg_cnt++ <= 50)	
				{
					Get_Moto_Offset(&motor_get[TRIGGER_MOTOR],CAN1_RX_date);
				}
				else
				{		
				motor_get[TRIGGER_MOTOR].msg_cnt=51;	
				Get_Moto_Measure_2006(&motor_get[TRIGGER_MOTOR], CAN1_RX_date);
				}
			}break;			
			
			case CAN_YAW_MOTOR_ID://yaw轴电机反馈
			{		
				Refresh_Device_OffLine_Time(YawGimbalMotor_TOE);//刷新时间
				
				if(motor_get[GIMBAL_YAW_MOTOR].msg_cnt++ <= 50)
				{
					Get_Moto_Offset(&motor_get[GIMBAL_YAW_MOTOR],CAN1_RX_date);
				}
				else
				{
					motor_get[GIMBAL_YAW_MOTOR].msg_cnt = 51;
					Get_Moto_Measure_3508(&motor_get[GIMBAL_YAW_MOTOR],CAN1_RX_date);
				}								
			}break;
			
			case CAN_PITCH_MOTOR_ID://pitch轴电机反馈
			{	
				Refresh_Device_OffLine_Time(PitchGimbalMotor_TOE);//刷新时间

				if(motor_get[GIMBAL_PITCH_MOTOR].msg_cnt++ <= 50)
				{
					Get_Moto_Offset(&motor_get[GIMBAL_PITCH_MOTOR],CAN1_RX_date);
				}
				else
				{
					motor_get[GIMBAL_PITCH_MOTOR].msg_cnt = 51;
					Get_Moto_Measure_6020(&motor_get[GIMBAL_PITCH_MOTOR],CAN1_RX_date);
				}				
			}break;
			default: break;
		}
	}
	
	else if(hcan == &hcan2)
	{
	  HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &CAN2_Rx_Header, CAN2_RX_date);
		switch(CAN2_Rx_Header.StdId)
		{			
		  case CAN_SEND_REMOTEDATE_ID://上云台发送的遥控信息
			{
				CAN_GET_Remote(&rc_ctrl,CAN2_RX_date);
			}break;
			
			case CAN_SEND_REFEREEDATE_ID://上云台发送的遥控信息
			{
				CAN_GET_RefereeData(CAN2_RX_date);//内部待完善
			}break;


      default : break;
		}
		
	}
}
//===================================================================================================================//
/************************************************各电机通过CAN发送数据************************************************/
//===================================================================================================================//
/**
  * @brief				云台电机驱动函数(6020与6623电机驱动函数相同)
  * @param[out]		
  * @param[in]		hcan:要使用的CAN1
                  yaw:yaw轴电流值
                  pitch:pitch电流值
  * @retval				none
*/
HAL_StatusTypeDef 	Gimbal_Motor_Drive(CAN_HandleTypeDef * hcan,int16_t yaw,int16_t	pitch)
{
	static CAN_TxHeaderTypeDef  Gimbal_TX_Message;
	uint8_t CAN_TX_DATA[8];

	Gimbal_TX_Message.StdId = 0x1FF;
	Gimbal_TX_Message.IDE = CAN_ID_STD;
	Gimbal_TX_Message.RTR = CAN_RTR_DATA;
	Gimbal_TX_Message.DLC = 0X08;
	
	CAN_TX_DATA[0] = yaw >> 8;
	CAN_TX_DATA[1] = yaw;
	CAN_TX_DATA[2] = pitch >> 8;
	CAN_TX_DATA[3] = pitch;
	CAN_TX_DATA[4] = 0x00;
	CAN_TX_DATA[5] = 0x00;
	CAN_TX_DATA[6] = 0x00;
	CAN_TX_DATA[7] = 0x00;

	if (HAL_CAN_AddTxMessage(hcan, &Gimbal_TX_Message, CAN_TX_DATA, (uint32_t *)CAN_TX_MAILBOX0) == HAL_OK)		
		return HAL_OK;
	else 
		return HAL_ERROR;	
}
/**
  * @brief				云台电机失能函数(6020与6623电机失能函数相同)
  * @param[out]		
  * @param[in]		hcan:要使用的CAN1
  * @retval				
*/
HAL_StatusTypeDef Gimbal_Motor_Disable(CAN_HandleTypeDef * hcan)
{
	static CAN_TxHeaderTypeDef  Gimbal_TX_Message;
	uint8_t CAN_TX_DATA[8];

	Gimbal_TX_Message.StdId = 0x1FF;
	Gimbal_TX_Message.IDE = CAN_ID_STD;
	Gimbal_TX_Message.RTR = CAN_RTR_DATA;
	Gimbal_TX_Message.DLC = 0X08;
	
	CAN_TX_DATA[0] = 0x00;
	CAN_TX_DATA[1] = 0x00;
	CAN_TX_DATA[2] = 0x00;
	CAN_TX_DATA[3] = 0x00;
	CAN_TX_DATA[4] = 0x00;
	CAN_TX_DATA[5] = 0x00;
	CAN_TX_DATA[6] = 0x00;
	CAN_TX_DATA[7] = 0x00;

	if (HAL_CAN_AddTxMessage(hcan, &Gimbal_TX_Message, CAN_TX_DATA, (uint32_t *)CAN_TX_MAILBOX0) == HAL_OK)		
		return HAL_OK;
	else 
		return HAL_ERROR;	
}

/**
  * @brief				云台电机校准函数(仅GM6623可用)
  * @param[out]		
  * @param[in]		hcan:要使用的CAN1
  * @retval				
*/
HAL_StatusTypeDef Gimbal_Motor_Calibration(CAN_HandleTypeDef * hcan)
{
	static CAN_TxHeaderTypeDef  Gimbal_TX_Message;
	uint8_t CAN_TX_DATA[8];

	Gimbal_TX_Message.StdId = 0x3F0;
	Gimbal_TX_Message.IDE = CAN_ID_STD;
	Gimbal_TX_Message.RTR = CAN_RTR_DATA;
	Gimbal_TX_Message.DLC = 0X08;
	
	CAN_TX_DATA[0] = 'c' ;
	CAN_TX_DATA[1] = 0x00;
	CAN_TX_DATA[2] = 0x00;
	CAN_TX_DATA[3] = 0x00;
	CAN_TX_DATA[4] = 0x00;
	CAN_TX_DATA[5] = 0x00;
	CAN_TX_DATA[6] = 0x00;
	CAN_TX_DATA[7] = 0x00;

	if (HAL_CAN_AddTxMessage(hcan, &Gimbal_TX_Message, CAN_TX_DATA, (uint32_t *)CAN_TX_MAILBOX0) == HAL_OK)		
		return HAL_OK;
	else 
		return HAL_ERROR;	
}

/**
  * @brief				底盘3508电机驱动函数
  * @param[out]		
  * @param[in]		hcan:要使用的CAN1
                  iqn:第n个底盘电机的电流值
  * @retval				
*/
HAL_StatusTypeDef Chassis_Motor_Drive( CAN_HandleTypeDef * hcan, int16_t motor1, int16_t motor2, int16_t motor3, int16_t motor4)
{
	static CAN_TxHeaderTypeDef	Chassis_TX_Message;
	uint8_t CAN_TX_DATA[8];

	Chassis_TX_Message.DLC = 0x08;
	Chassis_TX_Message.IDE = CAN_ID_STD;
	Chassis_TX_Message.RTR = CAN_RTR_DATA;
	Chassis_TX_Message.StdId = 0x200;

	CAN_TX_DATA[0] = motor1 >> 8;
	CAN_TX_DATA[1] = motor1;
	CAN_TX_DATA[2] = motor2 >> 8;
	CAN_TX_DATA[3] = motor2;
	CAN_TX_DATA[4] = motor3 >> 8;
	CAN_TX_DATA[5] = motor3;
	CAN_TX_DATA[6] = motor4 >> 8;
	CAN_TX_DATA[7] = motor4;

	if (HAL_CAN_AddTxMessage(hcan, &Chassis_TX_Message, CAN_TX_DATA, (uint32_t *)CAN_TX_MAILBOX0) == HAL_OK)		
		return HAL_OK;
	else 
		return HAL_ERROR;		
}	

/**
  * @brief				底盘3508电机失能函数
  * @param[out]		
  * @param[in]		hcan:要使用的CAN1
  * @retval				
*/
HAL_StatusTypeDef Chassis_Motor_Disable( CAN_HandleTypeDef * hcan)
{
	static CAN_TxHeaderTypeDef	Chassis_TX_Message;
	uint8_t CAN_TX_DATA[8];

	Chassis_TX_Message.DLC = 0x08;
	Chassis_TX_Message.IDE = CAN_ID_STD;
	Chassis_TX_Message.RTR = CAN_RTR_DATA;
	Chassis_TX_Message.StdId = 0x200;

	CAN_TX_DATA[0] = 0x00;
	CAN_TX_DATA[1] = 0x00;
	CAN_TX_DATA[2] = 0x00;
	CAN_TX_DATA[3] = 0x00;
	CAN_TX_DATA[4] = 0x00;
	CAN_TX_DATA[5] = 0x00;
	CAN_TX_DATA[6] = 0x00;
	CAN_TX_DATA[7] = 0x00;

	if( HAL_CAN_AddTxMessage(hcan, &Chassis_TX_Message, CAN_TX_DATA, (uint32_t *)CAN_TX_MAILBOX0) == HAL_OK)
		return HAL_OK;
	else 
		return HAL_ERROR;		
}	
/**
  * @brief				拨弹2006电机驱动函数
  * @param[out]		
  * @param[in]		hcan:要使用的CAN1
                  value:拨弹电机的电流值
  * @retval				
*/
HAL_StatusTypeDef Trigger_Motor_Drive(CAN_HandleTypeDef * hcan,int16_t value)
{
	static CAN_TxHeaderTypeDef  Trigger_TX_Message;
	uint8_t CAN_TX_DATA[8];

	Trigger_TX_Message.DLC = 0x08;
	Trigger_TX_Message.IDE = CAN_ID_STD;
	Trigger_TX_Message.RTR = CAN_RTR_DATA;
	Trigger_TX_Message.StdId = 0x200;

	CAN_TX_DATA[0] = 0;
	CAN_TX_DATA[1] = 0;
	CAN_TX_DATA[2] = 0;
	CAN_TX_DATA[3] = 0;
	CAN_TX_DATA[4] = value >> 8;
	CAN_TX_DATA[5] = value;
	CAN_TX_DATA[6] = 0;
	CAN_TX_DATA[7] = 0;

	if (HAL_CAN_AddTxMessage(hcan, &Trigger_TX_Message, CAN_TX_DATA, (uint32_t *)CAN_TX_MAILBOX0) == HAL_OK)		
		return HAL_OK;
	else 
		return HAL_ERROR;		
}

/**
  * @brief				上下云台通信（给下云台发送裁判系统信息）
  * @param[out]		
  * @param[in]		hcan：使用CAN2
									date：裁判系统信息
  * @retval				
*/
HAL_StatusTypeDef CAN_Send_RefereeData( CAN_HandleTypeDef * hcan, uint16_t data0, uint16_t data1 , uint16_t data2 ,uint16_t data3)//待续
{
	static CAN_TxHeaderTypeDef  Referee_TX_Message;
	uint8_t CAN_TX_DATA[8];

	Referee_TX_Message.DLC = 0x08;
	Referee_TX_Message.IDE = CAN_ID_STD;
	Referee_TX_Message.RTR = CAN_RTR_DATA;
	Referee_TX_Message.StdId = 0x120;

	CAN_TX_DATA[0] = data0 >>8;
	CAN_TX_DATA[1] = data0;
	CAN_TX_DATA[2] = data1 >>8;
	CAN_TX_DATA[3] = data1;
	CAN_TX_DATA[4] = data2>>8;
	CAN_TX_DATA[5] = data2;
	CAN_TX_DATA[6] = data3>>8;
	CAN_TX_DATA[7] = data3;

	if (HAL_CAN_AddTxMessage(hcan, &Referee_TX_Message, CAN_TX_DATA, (uint32_t *)CAN_TX_MAILBOX0) == HAL_OK)		
		return HAL_OK;
	else 
		return HAL_ERROR;	
}	

/**
  * @brief				上下云台通信（给下云台发送遥控器数据）
  * @param[out]		
  * @param[in]		hcan：使用CAN2
									date：遥控器数据
  * @retval				
*/
HAL_StatusTypeDef CAN_Send_RemoteDate( CAN_HandleTypeDef * hcan,
									                int16_t key_v, int16_t rc_ch0, int16_t rc_ch1, uint8_t rc_s1, uint8_t rc_s2)
{
	static CAN_TxHeaderTypeDef  Remote_TX_Message;
	uint8_t CAN_TX_DATA[8];

	Remote_TX_Message.DLC = 0x08;
	Remote_TX_Message.IDE = CAN_ID_STD;
	Remote_TX_Message.RTR = CAN_RTR_DATA;
	Remote_TX_Message.StdId = 0x110;

	CAN_TX_DATA[0] = key_v >> 8;
	CAN_TX_DATA[1] = key_v;
	CAN_TX_DATA[2] = rc_ch0 >> 8;
	CAN_TX_DATA[3] = rc_ch0;
	CAN_TX_DATA[4] = rc_ch1 >> 8;
	CAN_TX_DATA[5] = rc_ch1;
	CAN_TX_DATA[6] = rc_s1;
	CAN_TX_DATA[7] = rc_s2;

	if (HAL_CAN_AddTxMessage(hcan, &Remote_TX_Message, CAN_TX_DATA, (uint32_t *)CAN_TX_MAILBOX0) == HAL_OK)		
		return HAL_OK;
	else 
		return HAL_ERROR;	
}	

//===================================================================================================================//
/**********************************************各电机通过CAN接收返回值************************************************/
//===================================================================================================================//
/**
  * @brief				获取CAN通讯的6623电机的返回值
  * @param[out]		
  * @param[in]		ptr:目标数据的内存地址
                  CAN_RX_date:保存的来自CAN的数据的数组
  * @retval				
*/
void Get_Moto_Measure_6623(motor_measure_t *ptr,uint8_t CAN_RX_date[])
{
	ptr->last_angle = ptr->angle;
	ptr->angle = (uint16_t)(CAN_RX_date[0]<<8 | CAN_RX_date[1]) ;
	ptr->real_current  = (int16_t)(CAN_RX_date[2]<<8 | CAN_RX_date[3]);
	ptr->given_current = (int16_t)(CAN_RX_date[4]<<8 | CAN_RX_date[5]);
	
	if(ptr->angle - ptr->last_angle > 4096)
		ptr->round_cnt --;
	else if (ptr->angle - ptr->last_angle < -4096)
		ptr->round_cnt ++;
	ptr->total_angle = ptr->round_cnt * 8192 + ptr->angle - ptr->offset_angle;
}

/**
  * @brief				获取CAN通讯的3508电机的返回值
  * @param[out]		
  * @param[in]		ptr:目标数据的内存地址
                  CAN_RX_date:保存的来自CAN的数据的数组
  * @retval				
*/
void Get_Moto_Measure_3508(motor_measure_t *ptr,uint8_t CAN_RX_date[])
{
	ptr->last_angle = ptr->angle;
	ptr->angle = (uint16_t)(CAN_RX_date[0]<<8 | CAN_RX_date[1]) ;
	ptr->speed_rpm  = (int16_t)(CAN_RX_date[2]<<8 | CAN_RX_date[3]);
	ptr->real_current = (int16_t)(CAN_RX_date[4]<<8 | CAN_RX_date[5]);
	ptr->temperature = CAN_RX_date[6];
	
	if(ptr->angle - ptr->last_angle > 4096)
		ptr->round_cnt --;
	else if (ptr->angle - ptr->last_angle < -4096)
		ptr->round_cnt ++;
	ptr->total_angle = ptr->round_cnt * 8192 + ptr->angle - ptr->offset_angle;
}

/**
  * @brief				获取CAN通讯的2006电机的返回值
  * @param[out]		
  * @param[in]		ptr:目标数据的内存地址
                  CAN_RX_date:保存的来自CAN的数据的数组
  * @retval				
*/
void Get_Moto_Measure_2006(motor_measure_t *ptr,uint8_t CAN_RX_date[])
{
	ptr->last_angle = ptr->angle;
	ptr->angle = (uint16_t)(CAN_RX_date[0]<<8 | CAN_RX_date[1]) ;
	ptr->speed_rpm  = (int16_t)(CAN_RX_date[2]<<8 | CAN_RX_date[3]);
	ptr->real_current = (int16_t)(CAN_RX_date[4]<<8 | CAN_RX_date[5]);
	
	if(ptr->angle - ptr->last_angle > 4096)
		ptr->round_cnt --;
	else if (ptr->angle - ptr->last_angle < -4096)
		ptr->round_cnt ++;
	ptr->total_angle = ptr->round_cnt * 8192 + ptr->angle - ptr->offset_angle;
}

/**
  * @brief				获取CAN通讯的6020电机的返回值
  * @param[out]		
  * @param[in]		ptr:目标数据的内存地址
                  CAN_RX_date:保存的来自CAN的数据的数组
  * @retval				
*/
void Get_Moto_Measure_6020(motor_measure_t *ptr,uint8_t CAN_RX_date[])
{
	ptr->last_angle = ptr->angle;
	ptr->angle = (uint16_t)(CAN_RX_date[0]<<8 | CAN_RX_date[1]) ;
	ptr->speed_rpm  = (int16_t)(CAN_RX_date[2]<<8 | CAN_RX_date[3]);
	ptr->given_current = (int16_t)(CAN_RX_date[4]<<8 | CAN_RX_date[5]);
	ptr->temperature = CAN_RX_date[6];

	if(ptr->angle - ptr->last_angle > 4096)
		ptr->round_cnt --;
	else if (ptr->angle - ptr->last_angle < -4096)
		ptr->round_cnt ++;
	ptr->total_angle = ptr->round_cnt * 8192 + ptr->angle - ptr->offset_angle;
}

/**
  * @brief				获取电机返回值的偏差值(获取电机上电时的角度值)
  * @param[out]		
  * @param[in]		ptr:目标数据的内存地址
                  CAN_RX_date:保存的来自CAN的数据的数组
  * @retval				
*/
/*this function should be called after system+can init */
void Get_Moto_Offset(motor_measure_t *ptr,uint8_t CAN_RX_date[])
{
	ptr->angle = (uint16_t)(CAN_RX_date[0]<<8 | CAN_RX_date[1]) ;
	ptr->offset_angle = ptr->angle;
}


/**
  * @brief				获取遥控器返回数据（待完善）
  * @param[out]		
  * @param[in]		RC:目标数据的内存地址
                  CAN_RX_date:保存的来自CAN的数据的数组
  * @retval				
*/

void CAN_GET_Remote(RC_ctrl_t * RC , uint8_t CAN_RX_date[])
{
			RC->key.v =  (CAN_RX_date[0]<<8 | CAN_RX_date[1]) ;
	    RC->rc.ch0 = (CAN_RX_date[2]<<8 | CAN_RX_date[3]) ;
	    RC->rc.ch1 = (CAN_RX_date[4]<<8 | CAN_RX_date[5]) ;
	    RC->rc.s1  =  CAN_RX_date[6];
	    RC->rc.s2  =  CAN_RX_date[7];
}	


/**
  * @brief				获取裁判返回数据（待开发，入口参数可增加一个结构体方便传参）
  * @param[out]		
  * @param[in]		CAN_RX_date：保存的来自CAN的数据的数组
  * @retval				
*/
void CAN_GET_RefereeData(uint8_t CAN_RX_date[])
{


}	

