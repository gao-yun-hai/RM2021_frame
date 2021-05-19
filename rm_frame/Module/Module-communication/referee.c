/*******************************************************************************
                      ��Ȩ���� (C), 2020-,NCUROBOT
 *******************************************************************************
  �� �� ��   : referee.c 
  �� �� ��   : V1.0
  ��    ��   : ���ƺ�
  ��������   : 2020.12.2
  ����޸�   : 
  ��������   : ����ϵͳ���ݽ�������뼰���͡�USART��ʼ����UASRT�жϴ������ݽ��롢���ݷ��͡�
							 ����2020.5�����Ĳ���ϵͳ����Э��V1.1�汾
  �����б�   : 1) RefereeDate_Receive_Init()���ⲿ���ã�bsp.c��
							 2) Referee_UART_IRQHandler() ���ⲿ���ã�stm32f4xx_it.c��USART6�жϷ�������
							 3) SBUS_To_Referee()					���ڲ����ã�Referee_UART_IRQHandler()��
*******************************************************************************/
/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "referee.h"
#include "bsp_usart.h"
#include "crc.h"
#include "offline_check.h"
/* �ڲ��궨�� ----------------------------------------------------------------*/
#define RD_huart  huart6	//�����ϵͳͨ�����ô���
/* �ڲ��Զ����������͵ı��� --------------------------------------------------*/
Referee_Struct  referee;

/* �ڲ����� ------------------------------------------------------------------*/
static uint8_t referee_rx_buf[REFEREE_RX_BUFFER_SIZE];//���ղ���ϵͳԭʼ����
uint16_t referee_rx_date_len;//ʵ�ʽ��յ�����ϵͳ���ݵĳ���
/* �ڲ�����ԭ������ ----------------------------------------------------------*/
static void SBUS_To_Referee(uint8_t *buff, Referee_Struct  *Referee);


/* �������岿�� --------------------------------------------------------------*/
/**
  * @brief				����ϵͳ���ݽ���USART��ʼ������������DMA+����USART�����жϣ�
  * @param[in]		
	* @param[out]		
  * @retval				none
*/
void RefereeDate_Receive_USART_Init(void)
{
    UART_IT_Init(&RD_huart,referee_rx_buf,REFEREE_RX_BUFFER_SIZE);
}


/**
  * @brief				����ϵͳUSART�жϴ���������������DMA����+���ݽ���+��������֪ͨ��
  * @param[in]		
	* @param[out]		
  * @retval				none
*/
void Referee_UART_IRQHandler(void)
{
	static  BaseType_t  pxHigherPriorityTaskWoken;
	/* �ж��Ƿ�Ϊ�����ж� */
  if (__HAL_UART_GET_FLAG(&RD_huart, UART_FLAG_IDLE))
	{
			/* ������б�־������һֱ���ڿ���״̬���ж� */
			__HAL_UART_CLEAR_IDLEFLAG(&RD_huart);

			/* �ر�DMA���� */
			__HAL_DMA_DISABLE(RD_huart.hdmarx);

			/* ������ղ���ϵͳ���ݵĳ��� */
			referee_rx_date_len = (REFEREE_RX_BUFFER_SIZE - RD_huart.hdmarx->Instance->NDTR);
			/* ���ݽ��� */
			SBUS_To_Referee(referee_rx_buf,&referee);
			/* ���߼��ˢ��ʱ�� */
			Refresh_Device_OffLine_Time(Referee_TOE);		
			/* ����֪ͨ */
			vTaskNotifyGiveFromISR(RefereeDataTaskHandle,&pxHigherPriorityTaskWoken);
			portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);	
			/* ��������DMA���� */
			/* ����DMA�ɴ���������ݳ��� */
			__HAL_DMA_SET_COUNTER(RD_huart.hdmarx, REFEREE_RX_BUFFER_SIZE);
			/* ����DMA���� */
			__HAL_DMA_ENABLE(RD_huart.hdmarx);				
			
	}
}


/**
  * @brief				����ϵͳ���ݽ���
  * @param[in]		buff��ָ�����ݽ��������ָ��
	* @param[out]		Referee��ָ��洢����ϵͳ���ݽṹ���ָ��
  * @retval				none
*/
static void SBUS_To_Referee(uint8_t *buff, Referee_Struct  *Referee)
{		
		uint16_t  referee_length = 0;//һ֡�����ܳ���
	
		//�����ݰ��������κδ���
		if (buff == NULL)
		{
			return ;
		}
		
		for(uint8_t i=0;i<referee_rx_date_len;i++)
		{
			//�ж�֡ͷ�����Ƿ�Ϊ0xA5(���յ���һ�������а�����֡���ݣ��ʶ���һ��������Ҫѭ���ж�)
			if(buff[i] == FRAME_HEADER_SOF)
			{
					//д��֡ͷ����,�����ж��Ƿ�ʼ�洢��������
					memcpy(&Referee->frame_header, buff + i, LEN_HEADER);
					//д��������ID����,�����ж����ĸ���������
				 	memcpy(&Referee->cmdID, buff + LEN_HEADER + i, LEN_CMDID);
				  //һ֡�����ܳ��ȣ�֡ͷ���� + ������ID���� + ���ݳ��� + ֡β����
					referee_length = LEN_HEADER + LEN_CMDID + Referee->frame_header.DataLength + LEN_TAIL; 
					//֡ͷCRC8У�鼰֡βCRC16У��
					if(Verify_CRC16_Check_Sum(&buff[0+i], referee_length) && Verify_CRC8_Check_Sum(&buff[0+i],LEN_HEADER))
					{
							switch(Referee->cmdID)
							{
									case ID_GAME_STATE:       								 //0x0001 ����״̬����
										memcpy(&Referee->game_state, (buff + LEN_HEADER + LEN_CMDID + i), LEN_GAME_STATE);
									break;
									
									case ID_GAME_RESULT:      								 //0x0002 �����������
										memcpy(&Referee->game_result, (buff + LEN_HEADER + LEN_CMDID + i), LEN_GAME_RESULT);
									break;

									case ID_GAME_ROBOT_HP:   									 //0x0003 ����������Ѫ������
										memcpy(&Referee->game_robot_HP, (buff + LEN_HEADER + LEN_CMDID + i), LEN_GAME_ROBOT_HP);
									break;
									
									case ID_DART_STATUS:      								 //0x0004 ���ڷ���״̬
										memcpy(&Referee->dart_status, (buff + LEN_HEADER + LEN_CMDID + i), LEN_DART_STATUS);
									break;	

									case ID_ICRA_BUFF_DEBUFF_ZONE_STATUS:      //0x0005 �˹�������ս���ӳ���ͷ���״̬
										memcpy(&Referee->ICRA_buff_debuff_zone_status, (buff + LEN_HEADER + LEN_CMDID + i), LEN_ICRA_BUFF_DEBUFF_ZONE_STATUS);
									break;								
									
									case ID_EVENT_DATE:      									 //0x0101 �����¼�����
										memcpy(&Referee->event_data, (buff + LEN_HEADER + LEN_CMDID + i), LEN_EVENT_DATE);
									break;	
									
									case ID_SUPPLY_PROJRCTILE_ACTION:      		 //0x0102 ���ز���վ������ʶ����
										memcpy(&Referee->supply_projectile_action, (buff + LEN_HEADER + LEN_CMDID+i), LEN_SUPPLY_PROJRCTILE_ACTION);
									break;		

									case ID_REFEREE_WARNING:      		 				 //0x0104 ���о�������
										memcpy(&Referee->referee_warning, (buff + LEN_HEADER + LEN_CMDID + i), LEN_REFEREE_WARNING);
									break;	

									case ID_DART_REMAINING_TIME:      		 		 //0x0105 ���ڷ���ڵ���ʱ
										memcpy(&Referee->dart_remaining_time, (buff + LEN_HEADER + LEN_CMDID + i), LEN_DART_REMAINING_TIME);
									break;									
								
									case ID_GAME_ROBOT_STATUS:      		 		 	 //0x0201 ������״̬����
										memcpy(&Referee->game_robot_status, (buff + LEN_HEADER + LEN_CMDID + i), LEN_GAME_ROBOT_STATUS);
									break;	

									case ID_POWER_HEAT_DATE:      		 		 	   //0x0202 ʵʱ������������
										memcpy(&Referee->power_heat_data, (buff + LEN_HEADER + LEN_CMDID + i), LEN_POWER_HEAT_DATE);
									break;

									case ID_GAME_ROBOT_POS:      		 		 	     //0x0203 ������λ������
										memcpy(&Referee->game_robot_pos, (buff + LEN_HEADER + LEN_CMDID + i), LEN_GAME_ROBOT_POS);
									break;

									case ID_BUFF:      		 		 	    					 //0x0204 ��������������
										memcpy(&Referee->buff, (buff + LEN_HEADER + LEN_CMDID + i), LEN_BUFF);
									break;									
									
									case ID_AERIAL_ROBOT_ENERGY:      		 	   //0x0205 ��������������
										memcpy(&Referee->aerial_robot_energy, (buff + LEN_HEADER + LEN_CMDID + i), LEN_AERIAL_ROBOT_ENERGY);
									break;

									case ID_ROBOT_HURT:      		 	  				   //0x0206 �˺�״̬����
										memcpy(&Referee->robot_hurt, (buff + LEN_HEADER + LEN_CMDID + i), LEN_ROBOT_HURT);
									break;

									case ID_SHOOT_DATE:      		 	  				   //0x0207 ʵʱ�������
										memcpy(&Referee->shoot_data, (buff + LEN_HEADER + LEN_CMDID + i), LEN_SHOOT_DATE);
									break;	

									case ID_BULLET_REAMINING:      		 	  		 //0x0208 ����ʣ�෢����
										memcpy(&Referee->bullet_remaining, (buff + LEN_HEADER + LEN_CMDID + i), LEN_BULLET_REAMINING);
									break;	

									case ID_RFID_STATUS:      		 	  		 		 //0x0209 ������ RFID ״̬
										memcpy(&Referee->RFID_status, (buff + LEN_HEADER + LEN_CMDID + i), LEN_RFID_STATUS);
									break;

									case ID_DART_CLIENT_CMD:      		 	  		 //0x20A ���ڻ����˿ͻ���ָ������
										memcpy(&Referee->dart_client_cmd, (buff + LEN_HEADER + LEN_CMDID + i), LEN_DART_CLIENT_CMD);
									break;								
							}		
							
						i=i+referee_length;	
					}
			}
	 }			
}
/**
  * @brief				��ȡ������ʣ��Ѫ��
  * @param[in]		
	* @param[out]		
  * @retval				
*/
void get_robot_remain_hp(uint16_t *remain_hp)
{
   *remain_hp = referee.game_robot_status.remain_HP;
}

/**
  * @brief				��ȡ����ϵͳ����ӿ�״̬
  * @param[in]		
	* @param[out]		
  * @retval				
*/
void get_robot_mains_power_state(uint8_t *gimbal_output,uint8_t *chassis_output,uint8_t *shooter_output)
{
    *gimbal_output  = referee.game_robot_status.mains_power_gimbal_output;
    *chassis_output = referee.game_robot_status.mains_power_chassis_output;
    *shooter_output = referee.game_robot_status.mains_power_shooter_output;
}


/**
  * @brief				��ȡ���̹�����ʣ�໺������
  * @param[in]		
	* @param[out]		
  * @retval				
*/
void get_chassis_power_and_buffer(fp32 *power, uint16_t *buffer)
{
    *power = referee.power_heat_data.chassis_power;
    *buffer = referee.power_heat_data.chassis_power_buffer;
}

/**
  * @brief				��ȡ1��17mmǹ������������
  * @param[in]		
	* @param[out]		
  * @retval				
*/
void get_shooter_17mm_heat(uint16_t *heat1, uint16_t *heat2, uint16_t *heat1_limit, uint16_t *heat2_limit)
{
    *heat1 = referee.power_heat_data.shooter_id1_17mm_cooling_heat;
    *heat1_limit = referee.game_robot_status.shooter_id1_17mm_cooling_limit;
    *heat2 = referee.power_heat_data.shooter_id2_17mm_cooling_heat;
    *heat2_limit = referee.game_robot_status.shooter_id2_17mm_cooling_limit;

}


/**
  * @brief				��ȡǹ����Ƶ������
  * @param[in]		type��ǹ������
                  id��ǹ��ID
                  freq����Ƶ
                  speed������
	* @param[out]		
  * @retval				
*/
void get_shooter_bullet_freq_speed(uint8_t *type,uint8_t *id,uint8_t *freq,float *speed)
{
    *type = referee.shoot_data.bullet_type;
    *id   = referee.shoot_data.shooter_id;  
    *freq = referee.shoot_data.bullet_freq;
    *speed= referee.shoot_data.bullet_speed;
}

