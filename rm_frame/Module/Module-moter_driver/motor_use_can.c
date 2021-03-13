/*******************************************************************************
                      ��Ȩ���� (C), 2020-,NCUROBOT
 *******************************************************************************
  �� �� ��   : motor_use_can.c
  �� �� ��   : V1.1
  ��    ��   : NCURM
  ��������   : 2018��7��
  ����޸�   : 2020.11.28
  ��������   : �����ģ����ʹ��CAN���п��Ƶĵ����CAN���͡�CAN���ա�CAN�жϻص���
							 ע������ң�������ݼ�����ϵͳ���ͼ����պ����������뿪����
  �����б�   : 1) HAL_CAN_RxFifo0MsgPendingCallback() ��HAL�⺯����CAN�ص�������
							 //CAN����
							 2) Gimbal_Motor_Drive()									���ⲿ���ã�ʹ�ô���
							 3) Gimbal_Motor_Disable()								���ⲿ���ã�ʹ�ô���
							 4) Gimbal_Motor_Calibration()						���ⲿ���ã�ʹ�ô���
							 5) Chassis_Motor_Drive()									���ⲿ���ã�ʹ�ô���
							 6) Chassis_Motor_Disable()								���ⲿ���ã�ʹ�ô���
							 7) Trigger_Motor_Drive()									���ⲿ���ã�ʹ�ô���
							 8) CAN_Send_RefereeData()								���ⲿ���ã�ʹ�ô���
							 9) CAN_Send_RemoteDate()									���ⲿ���ã�ʹ�ô���
							 //CAN����
							 10) Get_Moto_Measure_6623()							���ڲ����ã�CAN�ص��е��á�
							 11) Get_Moto_Measure_3508()							���ڲ����ã�CAN�ص��е��á�
							 12) Get_Moto_Measure_2006()							���ڲ����ã�CAN�ص��е��á�
							 13) Get_Moto_Measure_6020()							���ڲ����ã�CAN�ص��е��á�
							 14) Get_Moto_Offset()										���ڲ����ã�CAN�ص��е��á�
							 15) CAN_GET_Remote()											���ڲ����ã�CAN�ص��е��á�
							 16) CAN_GET_RefereeData()								���ڲ����ã�CAN�ص��е��á�
*******************************************************************************/
/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "motor_use_can.h"
#include "remote_control.h"
#include "offline_check.h"
/* �ڲ��궨�� ----------------------------------------------------------------*/

/* �ڲ��Զ����������͵ı��� --------------------------------------------------*/
motor_measure_t   motor_get[NUM_OF_MOTOR] = {0};

/* �ڲ����� ------------------------------------------------------------------*/


/* �ڲ�����ԭ������ ----------------------------------------------------------*/
void Get_Moto_Measure_3508(motor_measure_t *ptr,uint8_t CAN_RX_date[]);
void Get_Moto_Measure_2006(motor_measure_t *ptr,uint8_t CAN_RX_date[]);
void Get_Moto_Measure_6623(motor_measure_t *ptr,uint8_t CAN_RX_date[]);
void Get_Moto_Measure_6020(motor_measure_t *ptr,uint8_t CAN_RX_date[]);
void Get_Moto_Offset(motor_measure_t *ptr,uint8_t CAN_RX_date[]);
void CAN_GET_Remote(RC_ctrl_t * RC , uint8_t CAN_RX_date[]);
void CAN_GET_RefereeData(uint8_t CAN_RX_date[]);

/* �������岿�� --------------------------------------------------------------*/
//===================================================================================================================//
/****************************************************CAN�жϻص�******************************************************/
//===================================================================================================================//
/**
  * @brief				HAL��CAN�ص�����,���յ������
  * @param[out]		
  * @param[in]		hcan:CAN���ָ��
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
			case CAN_CHASSIS_MOTOR_ID://���̵��
			{		
				Refresh_Device_OffLine_Time(ChassisMotor_TOE);//ˢ��ʱ��
				
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
			
			case CAN_TRIGGER_MOTOR_ID://���̵�� 
			{		
				Refresh_Device_OffLine_Time(TriggerMotor_TOE);//ˢ��ʱ��
				
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
			
			case CAN_YAW_MOTOR_ID://yaw��������
			{		
				Refresh_Device_OffLine_Time(YawGimbalMotor_TOE);//ˢ��ʱ��
				
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
			
			case CAN_PITCH_MOTOR_ID://pitch��������
			{	
				Refresh_Device_OffLine_Time(PitchGimbalMotor_TOE);//ˢ��ʱ��

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
		  case CAN_SEND_REMOTEDATE_ID://����̨���͵�ң����Ϣ
			{
				CAN_GET_Remote(&rc_ctrl,CAN2_RX_date);
			}break;
			
			case CAN_SEND_REFEREEDATE_ID://����̨���͵�ң����Ϣ
			{
				CAN_GET_RefereeData(CAN2_RX_date);//�ڲ�������
			}break;


      default : break;
		}
		
	}
}
//===================================================================================================================//
/************************************************�����ͨ��CAN��������************************************************/
//===================================================================================================================//
/**
  * @brief				��̨�����������(6020��6623�������������ͬ)
  * @param[out]		
  * @param[in]		hcan:Ҫʹ�õ�CAN1
                  yaw:yaw�����ֵ
                  pitch:pitch����ֵ
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
  * @brief				��̨���ʧ�ܺ���(6020��6623���ʧ�ܺ�����ͬ)
  * @param[out]		
  * @param[in]		hcan:Ҫʹ�õ�CAN1
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
  * @brief				��̨���У׼����(��GM6623����)
  * @param[out]		
  * @param[in]		hcan:Ҫʹ�õ�CAN1
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
  * @brief				����3508�����������
  * @param[out]		
  * @param[in]		hcan:Ҫʹ�õ�CAN1
                  iqn:��n�����̵���ĵ���ֵ
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
  * @brief				����3508���ʧ�ܺ���
  * @param[out]		
  * @param[in]		hcan:Ҫʹ�õ�CAN1
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
  * @brief				����2006�����������
  * @param[out]		
  * @param[in]		hcan:Ҫʹ�õ�CAN1
                  value:��������ĵ���ֵ
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
  * @brief				������̨ͨ�ţ�������̨���Ͳ���ϵͳ��Ϣ��
  * @param[out]		
  * @param[in]		hcan��ʹ��CAN2
									date������ϵͳ��Ϣ
  * @retval				
*/
HAL_StatusTypeDef CAN_Send_RefereeData( CAN_HandleTypeDef * hcan, uint16_t data0, uint16_t data1 , uint16_t data2 ,uint16_t data3)//����
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
  * @brief				������̨ͨ�ţ�������̨����ң�������ݣ�
  * @param[out]		
  * @param[in]		hcan��ʹ��CAN2
									date��ң��������
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
/**********************************************�����ͨ��CAN���շ���ֵ************************************************/
//===================================================================================================================//
/**
  * @brief				��ȡCANͨѶ��6623����ķ���ֵ
  * @param[out]		
  * @param[in]		ptr:Ŀ�����ݵ��ڴ��ַ
                  CAN_RX_date:���������CAN�����ݵ�����
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
  * @brief				��ȡCANͨѶ��3508����ķ���ֵ
  * @param[out]		
  * @param[in]		ptr:Ŀ�����ݵ��ڴ��ַ
                  CAN_RX_date:���������CAN�����ݵ�����
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
  * @brief				��ȡCANͨѶ��2006����ķ���ֵ
  * @param[out]		
  * @param[in]		ptr:Ŀ�����ݵ��ڴ��ַ
                  CAN_RX_date:���������CAN�����ݵ�����
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
  * @brief				��ȡCANͨѶ��6020����ķ���ֵ
  * @param[out]		
  * @param[in]		ptr:Ŀ�����ݵ��ڴ��ַ
                  CAN_RX_date:���������CAN�����ݵ�����
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
  * @brief				��ȡ�������ֵ��ƫ��ֵ(��ȡ����ϵ�ʱ�ĽǶ�ֵ)
  * @param[out]		
  * @param[in]		ptr:Ŀ�����ݵ��ڴ��ַ
                  CAN_RX_date:���������CAN�����ݵ�����
  * @retval				
*/
/*this function should be called after system+can init */
void Get_Moto_Offset(motor_measure_t *ptr,uint8_t CAN_RX_date[])
{
	ptr->angle = (uint16_t)(CAN_RX_date[0]<<8 | CAN_RX_date[1]) ;
	ptr->offset_angle = ptr->angle;
}


/**
  * @brief				��ȡң�����������ݣ������ƣ�
  * @param[out]		
  * @param[in]		RC:Ŀ�����ݵ��ڴ��ַ
                  CAN_RX_date:���������CAN�����ݵ�����
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
  * @brief				��ȡ���з������ݣ�����������ڲ���������һ���ṹ�巽�㴫�Σ�
  * @param[out]		
  * @param[in]		CAN_RX_date�����������CAN�����ݵ�����
  * @retval				
*/
void CAN_GET_RefereeData(uint8_t CAN_RX_date[])
{


}	

