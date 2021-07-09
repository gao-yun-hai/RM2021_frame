/*******************************************************************************
                      ��Ȩ���� (C), 2020-,NCUROBOT
 *******************************************************************************
  �� �� ��   : shoot_task.c
  �� �� ��   : V1.0
  ��    ��   : ���ƺ�
  ��������   : 2020.12.1
  ����޸�   :
  ��������   : ������񣬰���Ħ���������벦������
  �����б�   : 1) Friction_Drive_Task()		��FreeRTOS����������ϵͳ����������С�
							 2) Trigger_Drive_Task()		��FreeRTOS����������ϵͳ����������С�
*******************************************************************************/
/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "shoot_task.h"
#include "motor_use_tim.h"
#include "remote_control.h"
#include "motor_use_can.h"
#include "offline_check.h"
#include "referee.h"

/* �ڲ��궨�� ----------------------------------------------------------------*/

/* �ڲ��Զ����������͵ı��� --------------------------------------------------*/
shoot_control_t shoot_control;          //������ݽṹ��

/* �ڲ����� ------------------------------------------------------------------*/

/* �ڲ�����ԭ������ ----------------------------------------------------------*/

/* �������岿�� --------------------------------------------------------------*/
//===================================================================================================================//
/******************************************************Ħ����*********************************************************/
//===================================================================================================================//
/**
  * @brief				Ħ��������
  * @param[in]		
	* @param[out]		
  * @retval				none
*/
void Friction_Drive_Task(void const *argument)
{

	portTickType xLastWakeTime;
	xLastWakeTime = xTaskGetTickCount();

	//б�º�����ʼ��
	ramp_init(&shoot_control.fric1_ramp, 0.01f, 120,100);//0.01Ϊб�º�����б��(ʱ����)  120��100Ϊ�����Сֵ(���ֵҲΪĿ��ֵ)
	ramp_init(&shoot_control.fric2_ramp, 0.01f, 120,100);          
	
	for(;;)  
	{
		Refresh_Task_OffLine_Time(FrictionDriveTask_TOE);//��¼�������е�ʱ���

    //Ħ�����������ɸ���Ϊ�������ƣ�
    if(rc_ctrl.rc.s2 == RIGHT_S2_UP)
    {
        Shoot_Firction_Motor_Restart();
        shoot_control.fric1_ramp.max_value = 100;
        shoot_control.fric2_ramp.max_value = 100;      
    }
    //Ħ����ת�ٵ��ڣ��ɸ���Ϊ�������ƣ�
    if(rc_ctrl.rc.s1 == LEFT_S1_UP)
    {
        shoot_control.fric1_ramp.max_value = 120;
        shoot_control.fric2_ramp.max_value = 120;        
    }
    else if(rc_ctrl.rc.s1 == LEFT_S1_DOWN)
    {
        shoot_control.fric1_ramp.max_value = 130;
        shoot_control.fric2_ramp.max_value = 130;    
    }
    else
    {
			shoot_control.fric1_ramp.max_value = 100;
			shoot_control.fric2_ramp.max_value = 100;    
    }
     
 
   ramp_calc(&shoot_control.fric1_ramp, SHOOT_FRIC_PWM_ADD_VALUE);
   ramp_calc(&shoot_control.fric2_ramp, SHOOT_FRIC_PWM_ADD_VALUE);

   Shoot_Firction_Motor(shoot_control.fric1_ramp.out,shoot_control.fric2_ramp.out);
		
	 osDelayUntil(&xLastWakeTime,FRICTION_PERIOD);
		
	}
}



//===================================================================================================================//
/*******************************************************����**********************************************************/
//===================================================================================================================//
/**
  * @brief				��������
  * @param[in]		
	* @param[out]		
  * @retval				none
*/
void Trigger_Drive_Task(void const * argument)
{
	portTickType xLastWakeTime;
	xLastWakeTime = xTaskGetTickCount();
	
	for(;;)
	{
    Refresh_Task_OffLine_Time(TriggerDriveTask_TOE);//��¼�������е�ʱ���
    
/****************ע��*********************/
    
/*����������̨Ϊ��ͬ���ư壬��ʹ�ò�����������������������������Ӧ����ID��Ϊ0x203*/
//		Trigger_Motor_Drive();
/*����������̨Ϊ��ͬ���ư壬��ʹ����̨��������������������������Ӧ����ID��Ϊ0x207*/		
//    Gimbal_Motor_Drive();
    
/****************ע��*********************/
    
		osDelayUntil(&xLastWakeTime,TRIGGER_PERIOD);		

	}

}

