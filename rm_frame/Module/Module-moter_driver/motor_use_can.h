#ifndef __MOTOR_USE_CAN_H
#define __MOTOR_USE_CAN_H
/* ����ͷ�ļ� ---------------------------------------------------------------*/
#include "myinclude.h"

/* ��ģ�����ⲿ�ṩ�ĺ궨�� -------------------------------------------------*/


/* ��ģ�����ⲿ�ṩ�Ľṹ��/ö�ٶ��� ----------------------------------------*/
typedef enum
{
	CHASSIS_MOTOR_RF,
  CHASSIS_MOTOR_LF,
  CHASSIS_MOTOR_LB,
  CHASSIS_MOTOR_RB,
  
	TRIGGER_MOTOR,
	GIMBAL_YAW_MOTOR,
	GIMBAL_PITCH_MOTOR,
	
	NUM_OF_MOTOR      //���������
}motor_ID;//������ݽ��սṹ���Ӧ�洢��������

typedef enum
{
  CAN_CHASSIS_ALL_ID = 0x200,
	CAN_CHASSIS_MOTOR_RF_ID = 0x201,  //��ǰ
	CAN_CHASSIS_MOTOR_LF_ID = 0x202,  //��ǰ
	CAN_CHASSIS_MOTOR_LB_ID = 0x203,  //���
	CAN_CHASSIS_MOTOR_RB_ID = 0x204,  //�Һ�
  
  CAN_GIMBAL_ALL_ID = 0x1FF,  
	CAN_YAW_MOTOR_ID = 0x205,
	CAN_PITCH_MOTOR_ID = 0x206,
	CAN_TRIGGER_MOTOR_ID = 0x207,
  
	CAN_SEND_REMOTEDATE_ID = 0x110,
	CAN_SEND_REFEREEDATE_ID = 0x120,
	
}CAN_Message_ID;//CANͨ��ʱ��������ӦID��

typedef struct{
	int16_t	 			speed_rpm;			//ת��ת��
	int16_t  			real_current; 	//ʵ��ת�ص���
	int16_t  			given_current;	//����ת�ص���
	uint8_t  			temperature;		//����¶�
	uint16_t 			angle;					//ת�ӻ�е�Ƕ�abs angle range:[0,8191]
	uint16_t 			last_angle;			//��һ��ת�ӻ�е�Ƕ�
	uint16_t			offset_angle;		//����ϵ�ת�ӳ�ʼ��е�Ƕ�
	int32_t				round_cnt;			//���ת��ת��Ȧ��
	int32_t				total_angle;		//�����ת��Ȧ��
	uint8_t				msg_cnt;				//CAN�������ݴ��������ڻ�ȡ���ת�ӳ�ʼ�Ƕȣ�
}motor_measure_t;
/* ��ģ�����ⲿ�ṩ�ı������� -----------------------------------------------*/

/* ��ģ�����ⲿ�ṩ���Զ����������ͱ������� ---------------------------------*/
extern motor_measure_t  motor_get[NUM_OF_MOTOR];

/* ��ģ�����ⲿ�ṩ�Ľӿں���ԭ������ ---------------------------------------*/
HAL_StatusTypeDef 	Gimbal_Motor_Drive(CAN_HandleTypeDef * hcan, int16_t yaw, int16_t	pitch, int16_t shoot);
HAL_StatusTypeDef Gimbal_Motor_Disable(CAN_HandleTypeDef * hcan);
HAL_StatusTypeDef Gimbal_Motor_Calibration(CAN_HandleTypeDef * hcan);
HAL_StatusTypeDef Chassis_Motor_Drive(CAN_HandleTypeDef * hcan,int16_t motor1, int16_t motor2, int16_t motor3, int16_t motor4);
HAL_StatusTypeDef Chassis_Motor_Disable(CAN_HandleTypeDef * hcan);
HAL_StatusTypeDef Trigger_Motor_Drive(CAN_HandleTypeDef * hcan,int16_t value);
HAL_StatusTypeDef CAN_Send_RemoteDate(CAN_HandleTypeDef * hcan,int16_t key_v, int16_t rc_ch0, int16_t rc_ch1, uint8_t rc_s1, uint8_t rc_s2);
HAL_StatusTypeDef CAN_Send_RefereeData(CAN_HandleTypeDef * hcan, uint16_t data0, uint16_t data1 , uint16_t data2 ,uint16_t data3);

#endif
