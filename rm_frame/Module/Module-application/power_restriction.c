/*******************************************************************************
                      ��Ȩ���� (C), 2017-,NCUROBOT
 *******************************************************************************
  �� �� ��   : power_restriction.c
  �� �� ��   : ����
  ��    ��   : NCUERM
  ��������   : 2018��7��
  ����޸�   :
  ��������   : ���̹�������
  �����б�   :void power_limit(float  Current_get)
*******************************************************************************/

/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "power_restriction.h"
#include "referee.h"

/* �ڲ��궨�� ----------------------------------------------------------------*/
#define POWER_LIMIT         80.0f
#define WARNING_POWER       40.0f   
#define WARNING_POWER_BUFF  50.0f  

#define NO_JUDGE_TOTAL_CURRENT_LIMIT    64000.0f  //16000 * 4,    
#define BUFFER_TOTAL_CURRENT_LIMIT      16000.0f
#define POWER_TOTAL_CURRENT_LIMIT       20000.0f

/* �ڲ���������---------------------------------------------------------------*/

/* �ⲿ�������� --------------------------------------------------------------*/

/* �ⲿ����ԭ������ ----------------------------------------------------------*/

/* �ڲ����� ------------------------------------------------------------------*/

/* ����ԭ������ ----------------------------------------------------------*/
/**
  * @brief          ���ƹ��ʣ���Ҫ���Ƶ������(���̹����ĸ����)
  * @param[in]      chassis_power_control: ��������
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
    //���ʳ���80w �ͻ�������С��60j,��Ϊ��������С��60��ζ�Ź��ʳ���80w
    if(chassis_power_buffer < WARNING_POWER_BUFF)
    {
        fp32 power_scale;
        if(chassis_power_buffer > 5.0f)
        {
            //scale down WARNING_POWER_BUFF
            //��СWARNING_POWER_BUFF
            power_scale = chassis_power_buffer / WARNING_POWER_BUFF;
        }
        else
        {
            //only left 10% of WARNING_POWER_BUFF
            power_scale = 5.0f / WARNING_POWER_BUFF;
        }
        //scale down
        //��С
        total_current_limit = BUFFER_TOTAL_CURRENT_LIMIT * power_scale;
    }
    else
    {
        //power > WARNING_POWER
        //���ʴ���WARNING_POWER
        if(chassis_power > WARNING_POWER)
        {
            fp32 power_scale;
            //power < 80w
            //����С��80w
            if(chassis_power < POWER_LIMIT)
            {
                //scale down
                //��С
                power_scale = (POWER_LIMIT - chassis_power) / (POWER_LIMIT - WARNING_POWER);
                
            }
            //power > 80w
            //���ʴ���80w
            else
            {
                power_scale = 0.0f;
            }
            
            total_current_limit = BUFFER_TOTAL_CURRENT_LIMIT + POWER_TOTAL_CURRENT_LIMIT * power_scale;
        }
        //power < WARNING_POWER
        //����С��WARNING_POWER
        else
        {
            total_current_limit = BUFFER_TOTAL_CURRENT_LIMIT + POWER_TOTAL_CURRENT_LIMIT;
        }
    }  

     //calculate the original motor current set
    //����ԭ������ܵ����趨
    for(uint8_t i = 0; i < 4; i++)
    {
        total_current += fabs(current[i]);
    }
    //�������·���
    if(total_current > total_current_limit)
    {
        fp32 current_scale = total_current_limit / total_current;
        current[0]*=current_scale;
        current[1]*=current_scale;
        current[2]*=current_scale;
        current[3]*=current_scale;
    }
}


