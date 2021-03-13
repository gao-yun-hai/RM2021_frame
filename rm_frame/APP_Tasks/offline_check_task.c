/*******************************************************************************
                      ��Ȩ���� (C), 2020-,NCUROBOT
 *******************************************************************************
  �� �� ��   : offline_check_task.c
  �� �� ��   : V1.0
  ��    ��   : ���ƺ�
  ��������   : 2020.12.10
  ����޸�   :
  ��������   : ���߼���������ڵ���������߼�⺯����������߼�⺯�����ж�
							 ����������Ƿ���ߣ������߲�������ش���
							 ע����Ҫ��һ������
  �����б�   : 1) OffLine_Check_Task()			��FreeRTOS����������ϵͳ����������С�
*******************************************************************************/
/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "offline_check_task.h"
#include "offline_check.h"
/* �ڲ��궨�� ----------------------------------------------------------------*/

/* �ڲ��Զ����������͵ı��� --------------------------------------------------*/

/* �ڲ����� ------------------------------------------------------------------*/

/* �ڲ�����ԭ������ ----------------------------------------------------------*/

/* �������岿�� --------------------------------------------------------------*/
/*********************************************************
off_line.task_offline_flagΪ16λ����0000 0000 0000 0000
0000 0000 0000 0001��0x01��ң�����������
0000 0000 0000 0010��0x02���Ӿ��������
0000 0000 0000 0100��0x04������ϵͳ����
0000 0000 0000 1000��0x08��Ħ�����������
0000 0000 0001 0000��0x10�������������
0000 0000 0010 0000��0x20����̨�������
0000 0000 0100 0000��0x40�������������
0000 0000 1000 0000��0x80�����߼���������
----------------------------------------------------------
off_line.device_offline_flagΪ16λ����0000 0000 0000 0000
0000 0000 0000 0001��0x01���޷�����yaw��������ֵ
0000 0000 0000 0010��0x02���޷�����pitch��������ֵ
0000 0000 0000 0100��0x04���޷����ղ��̵������ֵ
0000 0000 0000 1000��0x08���޷����յ��̵������ֵ
0000 0000 0001 0000��0x10���޷�����ң��������ֵ
0000 0000 0010 0000��0x20���޷����ղ���ϵͳ����ֵ
0000 0000 0100 0000��0x40���޷������Ӿ�����ֵ
*********************************************************/

/**
  * @brief				���߼������
  * @param[in]		
	* @param[out]		
  * @retval				none
*/
void OffLine_Check_Task(void const *argument)
{
	osDelay(1000);

	portTickType xLastWakeTime;
  xLastWakeTime = xTaskGetTickCount();
	for(;;)
	{		
		Refresh_Task_OffLine_Time(OutLineCheckTask_TOE);
		
		Task_OffLine_Check();//������
		Device_OffLine_Check();//���߼��
		
		//������߶�Ӧ����
		if((off_line.task_offline_flag & 0x01))//ң�����������
		{
//			printf("Remote_Data_Task is offline\r\n");
		}
				
		if((off_line.task_offline_flag & 0x02))//�Ӿ��������
		{
//			printf("Vision_Data_Task is offline\r\n");
		} 
		
		if((off_line.task_offline_flag & 0x04))//����ϵͳ����
		{
//			printf("Referee_Data_Task is offline\r\n");
		}	
		
		if((off_line.task_offline_flag & 0x08))//Ħ�����������
		{
//			printf("Friction_Drive_Task is offline\r\n");
		}
		
		if((off_line.task_offline_flag & 0x10))//�����������
		{
//			printf("Trigger_Drive_Task is offline\r\n");
		}	

		if((off_line.task_offline_flag & 0x20))//��̨�������
		{
//			printf("Gimbal_Control_Task is offline\r\n");
		}	
		
		if((off_line.task_offline_flag & 0x40))//�����������
		{
//			printf("Chassis_Control_Task is offline\r\n");
		}	
		
		if((off_line.task_offline_flag & 0x80))//���߼���������
		{
//			printf("OffLine_Check_Task is offline\r\n");
		}
		
		
		//������߶�Ӧ����
		if((off_line.device_offline_flag & 0x01))//�޷�����yaw��������ֵ LED1
		{
			LED1_OFF();
//			printf("Cannot receive yaw_motor return data\r\n");
		}
		else 
			LED1_ON();
		
		if((off_line.device_offline_flag & 0x02))//�޷�����pitch��������ֵ LED2
		{
			LED2_OFF();
//			printf("Cannot receive pitch_motor return data\r\n");
		}
		else 
			LED2_ON();
		
		if((off_line.device_offline_flag & 0x04))//�޷����ղ��̵������ֵ LED3
		{
			LED3_OFF();
//			printf("Cannot receive trigger_motor return data\r\n");
		}
		else 
			LED3_ON();		

		if((off_line.device_offline_flag & 0x08))//�޷����յ��̵������ֵ LED4
		{
			LED4_OFF();
//			printf("Cannot receive chassis_motor return data\r\n");
		}
		else 
			LED4_ON();
		
		if((off_line.device_offline_flag & 0x10))//�޷�����ң��������ֵ LED5
		{
			LED5_OFF();
//			printf("Cannot receive remote_control return data\r\n");
		}
		else 
			LED5_ON();

		if((off_line.device_offline_flag & 0x20))//�޷����ղ���ϵͳ����ֵ LED6
		{
			LED6_OFF();
//			printf("Cannot receive referee return data\r\n");
		}
		else 
			LED6_ON();		
		
		if((off_line.device_offline_flag & 0x40))//�޷������Ӿ�����ֵ LED7
		{
			LED7_OFF();
//			printf("Cannot receive vision return data\r\n");
		}
		else 
			LED7_ON();	
		
		
		osDelayUntil(&xLastWakeTime,OFFLINE_PERIOD);
		
	}
}
