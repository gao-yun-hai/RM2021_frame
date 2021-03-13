/*******************************************************************************
                      ��Ȩ���� (C), 2020-,NCUROBOT
 *******************************************************************************
  �� �� ��   : pid.c
  �� �� ��   : V1.1
  ��    ��   : ���ƺ�
  ��������   : 2020.12.1
  ����޸�   : ��ԭʼpic��.c/.h�ļ��滻Ϊ��.c/.h�ļ�����Ҫ�Ǽ򻯸������������
							 ԭpid�ļ������� Module -> Module-application -> old_pid �ļ�����
  ��������   : pidʵ�ֺ�����PID��ʼ����PID���㣬PID��������
  �����б�   : 1) PID_Param_Init() 				���ⲿ���ã�ʹ�ô�������ִ��һ�Ρ�
							 2) PID_Calc()							���ⲿ���ã�ʹ�ô���
							 3) PID_Clear()							���ⲿ���ã�ʹ�ô���
*******************************************************************************/
/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "pid.h"

/* �ڲ��궨�� ----------------------------------------------------------------*/

/* �ڲ��Զ����������͵ı��� --------------------------------------------------*/
pid_t  motor_pid[NUM_OF_PID] = {0};

/* �ڲ����� ------------------------------------------------------------------*/

/* �ڲ�����ԭ������ ----------------------------------------------------------*/

/* �������岿�� --------------------------------------------------------------*/
/**
  * @brief				PID�ṹ�������ʼ��
  * @param[out]		*pid: PID�ṹ����ָ��
  * @param[in]		mode: PIDģʽ
									max_out: PID������
									max_iout��PID������������������޷�
									Kp,Ki,Kd��P��I��D����������ʼ��
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
  * @brief				PID����
  * @param[out]		*pid: PID�ṹ����ָ��
  * @param[in]		get: ��������    set: �趨ֵ
  * @retval				pid���
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
    
    if(pid->pid_mode == POSITION_PID) //λ��ʽ
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
    else if(pid->pid_mode == DELTA_PID)//����ʽ
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
  * @brief          pid ������
  * @param[out]     pid: PID�ṹ����ָ��
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
