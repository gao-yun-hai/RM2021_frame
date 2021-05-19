#ifndef  __PID_H
#define  __PID_H

/* ����ͷ�ļ� ---------------------------------------------------------------*/
#include "myinclude.h"
/* ��ģ�����ⲿ�ṩ�ĺ궨�� -------------------------------------------------*/

/* ��ģ�����ⲿ�ṩ�Ľṹ��/ö�ٶ��� ----------------------------------------*/

enum PID_MODE
{
		POSITION_PID =0,
		DELTA_PID = 1,
};

enum Time
{
    LLAST	= 0,//����һ��
    LAST 	= 1,//��һ��
    NOW 	= 2,//����һ��
};

typedef enum
{
	PID_CHASSIS_MOTOR_RF_SPD,
	PID_CHASSIS_MOTOR_LF_SPD,
  PID_CHASSIS_MOTOR_LB_SPD,
	PID_CHASSIS_MOTOR_RB_SPD,
  
	PID_TRIGGER_MOTOR_SPD,
	PID_TRIGGER_MOTOR_POS,
	PID_YAW_MOTOR_SPD,
	PID_YAW_MOTOR_POS,
	PID_PITCH_MOTOR_SPD,
	PID_PITCH_MOTOR_POS,
	
	NUM_OF_PID//���е������PID����ʱ����PID�ṹ�������
	
}PID_ID;//����ջ�����ʱ������ٶȻ���λ�û�PID�ṹ���Ӧ�洢��������


typedef struct __pid_t
{
	  uint8_t pid_mode;       	//PIDģʽ

		//PID������
    float Kp;			
    float Ki;
    float Kd;

    float set;								//Ŀ��ֵ
    float get;								//����ֵ
    float err[3];							//���	,����NOW�� LAST��LLAST

    float pout;								//p���
    float iout;								//i���
    float dout;								//d���
    float last_dout;					//��һ��d���
	
		float max_pout;						//�����޷�
		float max_iout;						//�����޷�
		float max_dout;						//΢���޷�
		float max_out;						//����޷�		
		float max_err;						//������
    float deadband;						//���� 

    float pos_out;						//����λ��ʽ���
    float delta_out;					//��������ʽ���

} pid_t;
/* ��ģ�����ⲿ�ṩ�ı������� -----------------------------------------------*/

/* ��ģ�����ⲿ�ṩ���Զ����������ͱ������� ---------------------------------*/
extern pid_t   motor_pid[NUM_OF_PID];

/* ��ģ�����ⲿ�ṩ�Ľӿں���ԭ������ ---------------------------------------*/
void PID_Param_Init(pid_t *pid, uint8_t mode, float max_out, float max_iout, float Kp, float Ki, float Kd);
float PID_Calc(pid_t* pid, float get, float set);
void PID_Clear(pid_t *pid);



#endif


