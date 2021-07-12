/*******************************************************************************
                      ��Ȩ���� (C), 2020-,NCUROBOT
 *******************************************************************************
  �� �� ��   : referee.c 
  �� �� ��   : V1.0 �� V1.1
  ��    ��   : ���ƺ�
  ��������   : 2020.12.2
  ����޸�   : 2021.7.9
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
Referee_Struct  referee_receive;          //����ϵͳ��������
Referee_Used_Struct referee_used;         //����ϵͳ��������-�ⲿʹ��
Referee_Struct  referee_send;             //����ϵͳ���ݷ���-�����˽���
Referee_Struct  referee_client;           //����ϵͳ���ݷ���-�Զ���UI

/* �ڲ����� ------------------------------------------------------------------*/
static uint8_t referee_rx_buf[REFEREE_RX_BUFFER_SIZE];//���ղ���ϵͳԭʼ����
uint16_t referee_rx_date_len;//ʵ�ʽ��յ�����ϵͳ���ݵĳ���
/* �ڲ�����ԭ������ ----------------------------------------------------------*/
static void SBUS_To_Referee(uint8_t *buff, Referee_Struct  *Referee);
void Referee_To_Use(void);

/* �������岿�� --------------------------------------------------------------*/
//===================================================================================================================//
/******************************************* �� ����ϵͳ���ݽ��ղ��� �� ************************************************/
//===================================================================================================================//
/**
  * @brief				����ϵͳ���ݽ���USART��ʼ������������DMA+����USART�����жϣ�
  * @param[in]		
	* @param[out]		
  * @retval				none
*/
void RefereeDate_Receive_USART_Init(void)
{
    HAL_UART_IT_Init(&RD_huart,referee_rx_buf,REFEREE_RX_BUFFER_SIZE);
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
			SBUS_To_Referee(referee_rx_buf,&referee_receive);
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
                  
									case ID_STUDENT_INTERACTIVE_HEADER:				 //0x0301 �����˼佻������
                    //���л����˷��͸��ڱ�
                    if(Referee->student_interactive_header_data.data_cmd_id ==  ID_AERIAL_ROBOT_TO_SENTRY_ROBOT)
                    {
                      memcpy(&Referee->aerial_to_sentry_data, (buff + LEN_HEADER + LEN_CMDID + LEN_ALL_ID + i),LEN_AERIAL_ROBOT_TO_SENTRY_ROBOT);                      
                    }
                    //���л����˷��͸�����
                    else if(Referee->student_interactive_header_data.data_cmd_id == ID_AERIAL_ROBOT_TO_DART_ROBOT)
                    {
                      memcpy(&Referee->aerial_to_dart_data, (buff + LEN_HEADER + LEN_CMDID + LEN_ALL_ID + i),LEN_AERIAL_ROBOT_TO_DART_ROBOT);                    
                    }                      
									break;                  
							}		
							
						i=i+referee_length;	
					}
			}
	 }	
    
   Referee_To_Use();//���ⲿ��Ҫʹ�õĲ���ϵͳ���ݷ�ֵ��referee_used�ṹ��
   
}
/**
  * @brief				�ⲿ��Ҫʹ�õĲ���ϵͳ���ݣ�������������������referee_used�ṹ������Ӻ���з�ֵ��
  * @param[in]		
	* @param[out]		
  * @retval				
*/
void Referee_To_Use(void)
{
  referee_used.robot_id =  referee_receive.game_robot_status.robot_id;//������ID

  referee_used.robot_remain_hp = referee_receive.game_robot_status.remain_HP;//������ʣ��Ѫ��

  referee_used.chassis_power = referee_receive.power_heat_data.chassis_power;//����ʵʱ����
  referee_used.chassis_power_buffer = referee_receive.power_heat_data.chassis_power_buffer;//���̻�������
  referee_used.chassis_power_limit = referee_receive.game_robot_status.chassis_power_limit;//���̹�������

  referee_used.shooter1_17mm_heat_limit = referee_receive.game_robot_status.shooter_id1_17mm_cooling_limit;//17mm �������1 ǹ����������
  referee_used.shooter2_17mm_heat_limit = referee_receive.game_robot_status.shooter_id2_17mm_cooling_limit;//17mm �������2 ǹ����������
  referee_used.shooter1_42mm_heat_limit = referee_receive.game_robot_status.shooter_id1_42mm_cooling_limit;//42mm �������1 ǹ����������
  
  referee_used.shooter1_17mm_speed_limit = referee_receive.game_robot_status.shooter_id1_17mm_speed_limit;//17mm �������1 ǹ����������
  referee_used.shooter2_17mm_speed_limit = referee_receive.game_robot_status.shooter_id2_17mm_speed_limit;//17mm �������2 ǹ����������
  referee_used.shooter1_42mm_speed_limit = referee_receive.game_robot_status.shooter_id1_42mm_speed_limit;//42mm �������1 ǹ����������

  referee_used.shooter1_17mm_heat = referee_receive.power_heat_data.shooter_id1_17mm_cooling_heat;//17mm �������1 ǹ������
  referee_used.shooter2_17mm_heat = referee_receive.power_heat_data.shooter_id2_17mm_cooling_heat;//17mm �������2 ǹ������
  referee_used.shooter1_42mm_heat = referee_receive.power_heat_data.shooter_id1_42mm_cooling_heat;//42mm �������1 ǹ������

  if(referee_receive.shoot_data.bullet_type == BULLET_17MM)
  {
    if(referee_receive.shoot_data.shooter_id == SHOOTER1_17MM)
    {
       referee_used.shooter1_17mm_freq  = referee_receive.shoot_data.bullet_freq; //17mm �������1 ��Ƶ
       referee_used.shooter1_17mm_speed = referee_receive.shoot_data.bullet_speed;//17mm �������1 ����
    }

    else if(referee_receive.shoot_data.shooter_id == SHOOTER2_17MM)
    {    
       referee_used.shooter2_17mm_freq  = referee_receive.shoot_data.bullet_freq; //17mm �������2 ��Ƶ
       referee_used.shooter2_17mm_speed = referee_receive.shoot_data.bullet_speed;//17mm �������2 ����
    }
  }
  else if(referee_receive.shoot_data.bullet_type == BULLET_42MM && referee_receive.shoot_data.shooter_id == SHOOTER2_17MM)
  {
     referee_used.shooter1_42mm_freq  = referee_receive.shoot_data.bullet_freq; //42mm �������1 ��Ƶ
     referee_used.shooter1_42mm_speed = referee_receive.shoot_data.bullet_speed;//42mm �������1 ����
  }
  
  referee_used.bullet_17mm_remaining_num = referee_receive.bullet_remaining.bullet_remaining_num_17mm;//17mm �ӵ�ʣ�෢����Ŀ���Կ��������л��������ڱ����ã�
  referee_used.coin_remaining_num = referee_receive.bullet_remaining.coin_remaining_num;//ʣ��������
  
  referee_used.chassis_output = referee_receive.game_robot_status.mains_power_chassis_output;//��̨�����
  referee_used.gimbal_output  = referee_receive.game_robot_status.mains_power_gimbal_output; //���̿����
  referee_used.shooter_output = referee_receive.game_robot_status.mains_power_shooter_output;//��������  
}
//===================================================================================================================//
/******************************************* �� ����ϵͳ���ݽ��ղ��� �� ************************************************/
//===================================================================================================================//


//===================================================================================================================//
/*********************************************** ��  �����Զ���UI  �� **************************************************/
//===================================================================================================================//
/**
  * @brief				��ȡ�����Զ���UI �ͻ���ID
  * @param[in]		
	* @param[out]		
  * @retval				
*/
uint16_t Get_Receive_Client_Side_ID(void)
{
  static uint16_t receive_client_side_id;
  if(referee_used.robot_id >= 1 &&referee_used.robot_id <= 7)
  {
    receive_client_side_id = 0x0100 + referee_used.robot_id;
  }
  else if(referee_used.robot_id >= 101 &&referee_used.robot_id <= 107)
  {
    receive_client_side_id = 0x0164 + (referee_used.robot_id - 100);
  }
  return receive_client_side_id;
}

/**
  * @brief				�Զ���ͼ�Σ���ֱ�߻��߾��λ�����Բ
  * @param[in]		ͼ�θ���num    ͼ�ε�����name    ͼ�β���operate   ͼ�ε�����type   ͼ�β���layer
                  ͼ����ɫcolor  ͼ�εĿ�� width  ��ʼ����s_x,s_y   �յ�����e_x,e_y  
	* @param[out]		
  * @retval				
*/
void Draw_StraightLine_or_Rectangle_or_Ellipse(uint8_t num, uint8_t name, uint8_t operate, uint8_t type, 
     uint8_t layer, uint8_t color, uint8_t width , uint16_t s_x, uint16_t s_y, uint16_t e_x, uint16_t e_y)
{
  unsigned char cliend_tx_buffer[LEN_SEND_CLIENT_SIDE_DATE_MAX];
	static uint8_t data_length;

  //����ͷ֡����
  referee_client.frame_header.SOF = FRAME_HEADER_SOF;//ͷ֡
  referee_client.frame_header.DataLength = LEN_ALL_ID + LEN_SEND_CLIENT_SIDE_BUFFER*LEN_SEND_CLIENT_SIDE_BUFFER_NUM;//���ݳ���
  referee_client.frame_header.Seq = 0;
  //��ͷ֡���ݸ��Ƶ�������������
	memcpy(cliend_tx_buffer, &referee_client.frame_header, LEN_HEADER);
  //���CRC8��ͷ֡��β
  Append_CRC8_Check_Sum(cliend_tx_buffer, LEN_HEADER);
  
  //����CmdID
  referee_client.cmdID = ID_STUDENT_INTERACTIVE_HEADER;
  
  //�����Զ���UI���贫������
  //�������� ID �ٷ��̶�,��ǰΪ2��ͼ�Σ�����idΪ0x0102
  referee_client.student_interactive_header_data.data_cmd_id = ID_CLIENT_CUSTOM_GRAPHIC_DOUBLE;
  //���÷����ߵ� ID
  referee_client.student_interactive_header_data.sender_ID = referee_used.robot_id;
  //���ý����ߵ� ID
  referee_client.student_interactive_header_data.receiver_ID = Get_Receive_Client_Side_ID();
  //���÷��͵�����
  //ͼ�ε����ƣ��������ŵ���name������ÿһλ
  referee_client.graphic_data_struct[num].graphic_name[0] = name%10;
	referee_client.graphic_data_struct[num].graphic_name[1] = (name/10)%10;
	referee_client.graphic_data_struct[num].graphic_name[2] = (name/100)%10;
  //����������ɾ�������ӵȲ���
  referee_client.graphic_data_struct[num].operate_tpye = operate;
  //ͼ�����ֱࣻ�ߡ����λ�����Բ
  referee_client.graphic_data_struct[num].graphic_tpye = type;
	//ͼ�������൱�ڿ��Բ��ϸ���
  referee_client.graphic_data_struct[num].layer = layer;
  //ͼ����ɫ
  referee_client.graphic_data_struct[num].color = color;
  //���
  referee_client.graphic_data_struct[num].width = width;
  //��ʼ���ꣻֱ�߻��߾��ε���ʼλ��x,y����ԲԲ�ĵ�x,y����
  referee_client.graphic_data_struct[num].start_x = s_x;
  referee_client.graphic_data_struct[num].start_y = s_y;
	//ֱ�ߵ�ĩβλ�ã����ζԶ���x,y�����꣬��Բx���᳤�Ⱥ�y���᳤��
  referee_client.graphic_data_struct[num].end_x = e_x;
  referee_client.graphic_data_struct[num].end_y = e_y;    
  //�������˽������ݸ��Ƶ�������������
  memcpy(cliend_tx_buffer + LEN_HEADER, (uint8_t *)&referee_client.cmdID, LEN_CMDID + referee_client.frame_header.DataLength);
  //���������ܳ���
  data_length = LEN_HEADER + LEN_CMDID + referee_client.frame_header.DataLength + LEN_TAIL;
  //���CRC16�����ݽ�β
  Append_CRC16_Check_Sum(cliend_tx_buffer, data_length);
  
  for(int i=0; i<data_length; i++)
  {
  	HAL_UART_Transmit(&RD_huart, &cliend_tx_buffer[i], 1, 1);  
  }
  
}

/**
  * @brief				�Զ���ͼ�Σ�����Բ
  * @param[in]		ͼ�θ���num    ͼ�ε�����name    ͼ�β���operate   ͼ�β���layer
                  ͼ����ɫcolor  ͼ�εĿ�� width  Բ������s_x,s_y   Բ�İ뾶radius  
	* @param[out]		
  * @retval				
*/
void Draw_FullCircle(uint8_t num, uint16_t name, uint8_t operate, uint8_t layer,
     uint8_t color, uint8_t width, uint16_t s_x, uint16_t s_y, uint16_t radius)
{
  unsigned char cliend_tx_buffer[LEN_SEND_CLIENT_SIDE_DATE_MAX];
	static uint8_t data_length;

  //����ͷ֡����
  referee_client.frame_header.SOF = FRAME_HEADER_SOF;//ͷ֡
  referee_client.frame_header.DataLength = LEN_ALL_ID + LEN_SEND_CLIENT_SIDE_BUFFER*LEN_SEND_CLIENT_SIDE_BUFFER_NUM;//���ݳ���
  referee_client.frame_header.Seq = 0;
  //��ͷ֡���ݸ��Ƶ�������������
	memcpy(cliend_tx_buffer, &referee_client.frame_header, LEN_HEADER);
  //���CRC8��ͷ֡��β
  Append_CRC8_Check_Sum(cliend_tx_buffer, LEN_HEADER);
  
  //����CmdID
  referee_client.cmdID = ID_STUDENT_INTERACTIVE_HEADER;
  
  //�����Զ���UI���贫������
  //�������� ID �ٷ��̶�,��ǰΪ2��ͼ�Σ�����idΪ0x0102
  referee_client.student_interactive_header_data.data_cmd_id = ID_CLIENT_CUSTOM_GRAPHIC_DOUBLE;
  //���÷����ߵ� ID
  referee_client.student_interactive_header_data.sender_ID = referee_used.robot_id;
  //���ý����ߵ� ID
  referee_client.student_interactive_header_data.receiver_ID = Get_Receive_Client_Side_ID();
  //���÷��͵�����
  //ͼ�ε����ƣ��������ŵ���name������ÿһλ
  referee_client.graphic_data_struct[num].graphic_name[0] = name%10;
	referee_client.graphic_data_struct[num].graphic_name[1] = (name/10)%10;
	referee_client.graphic_data_struct[num].graphic_name[2] = (name/100)%10;
  //����������ɾ�������ӵȲ���
  referee_client.graphic_data_struct[num].operate_tpye = operate;
  //ͼ�����ࣻ��Բ
  referee_client.graphic_data_struct[num].graphic_tpye = FULL_CIRCLE;
	//ͼ�������൱�ڿ��Բ��ϸ���
  referee_client.graphic_data_struct[num].layer = layer;
  //ͼ����ɫ  
  referee_client.graphic_data_struct[num].color = color;
  //���
  referee_client.graphic_data_struct[num].width = width;
  //Բ�ĵ�x,y����
  referee_client.graphic_data_struct[num].start_x = s_x;
  referee_client.graphic_data_struct[num].start_y = s_y;
	//Բ�İ뾶
  referee_client.graphic_data_struct[num].radius = radius;    
  //�������˽������ݸ��Ƶ�������������
  memcpy(cliend_tx_buffer + LEN_HEADER, (uint8_t *)&referee_client.cmdID, LEN_CMDID + referee_client.frame_header.DataLength);
  //���������ܳ���
  data_length = LEN_HEADER + LEN_CMDID + referee_client.frame_header.DataLength + LEN_TAIL;
  //���CRC16�����ݽ�β
  Append_CRC16_Check_Sum(cliend_tx_buffer, data_length);
  
  for(int i=0; i<data_length; i++)
  {
  	HAL_UART_Transmit(&RD_huart, &cliend_tx_buffer[i], 1, 1);  
  }
}

/**
  * @brief				�Զ���ͼ�Σ���Բ��
  * @param[in]		ͼ�θ���num       ͼ�ε�����name         ͼ�β���operate       ͼ�β���layer       ͼ����ɫcolor  
                  ͼ�εĿ�� width  Բ����ʼ�Ƕ�star_angle Բ����ֹ�Ƕ�end_angle Բ������s_x,s_y xy  �볤�᳤��e_x,e_y

	* @param[out]		
  * @retval				
*/
void Draw_ARC(uint8_t num, uint16_t name, uint8_t operate, uint8_t layer, uint8_t color, uint16_t start_angle, 
     uint16_t end_angle, uint8_t width, uint16_t s_x, uint16_t s_y, uint16_t e_x, uint16_t e_y)
{
  unsigned char cliend_tx_buffer[LEN_SEND_CLIENT_SIDE_DATE_MAX];
	static uint8_t data_length;

  //����ͷ֡����
  referee_client.frame_header.SOF = FRAME_HEADER_SOF;//ͷ֡
  referee_client.frame_header.DataLength = LEN_ALL_ID + LEN_SEND_CLIENT_SIDE_BUFFER*LEN_SEND_CLIENT_SIDE_BUFFER_NUM;//���ݳ���
  referee_client.frame_header.Seq = 0;
  //��ͷ֡���ݸ��Ƶ�������������
	memcpy(cliend_tx_buffer, &referee_client.frame_header, LEN_HEADER);
  //���CRC8��ͷ֡��β
  Append_CRC8_Check_Sum(cliend_tx_buffer, LEN_HEADER);
  
  //����CmdID
  referee_client.cmdID = ID_STUDENT_INTERACTIVE_HEADER;
  
  //�����Զ���UI���贫������
  //�������� ID �ٷ��̶�,��ǰΪ2��ͼ�Σ�����idΪ0x0102
  referee_client.student_interactive_header_data.data_cmd_id = ID_CLIENT_CUSTOM_GRAPHIC_DOUBLE;
  //���÷����ߵ� ID
  referee_client.student_interactive_header_data.sender_ID = referee_used.robot_id;
  //���ý����ߵ� ID
  referee_client.student_interactive_header_data.receiver_ID = Get_Receive_Client_Side_ID();
  //���÷��͵�����
  //ͼ�ε����ƣ��������ŵ���name������ÿһλ
  referee_client.graphic_data_struct[num].graphic_name[0] = name%10;
	referee_client.graphic_data_struct[num].graphic_name[1] = (name/10)%10;
	referee_client.graphic_data_struct[num].graphic_name[2] = (name/100)%10;
  //����������ɾ�������ӵȲ���
  referee_client.graphic_data_struct[num].operate_tpye = operate;
  //ͼ�����ࣻԲ��
  referee_client.graphic_data_struct[num].graphic_tpye = ARC;
	//ͼ�������൱�ڿ��Բ��ϸ���
  referee_client.graphic_data_struct[num].layer = layer;
  //ͼ����ɫ  
  referee_client.graphic_data_struct[num].color = color;
  //Բ������ʼ�Ƕ�
  referee_client.graphic_data_struct[num].start_angle = start_angle;
  //Բ������ֹ�Ƕ�
  referee_client.graphic_data_struct[num].end_angle = end_angle; 
  //���
  referee_client.graphic_data_struct[num].width = width;
  //Բ�ĵ�x,y����
  referee_client.graphic_data_struct[num].start_x = s_x;
  referee_client.graphic_data_struct[num].start_y = s_y;
	//x��y�볤�᳤��
  referee_client.graphic_data_struct[num].end_x = e_x;
  referee_client.graphic_data_struct[num].end_y = e_y;  
  //�������˽������ݸ��Ƶ�������������
  memcpy(cliend_tx_buffer + LEN_HEADER, (uint8_t *)&referee_client.cmdID, LEN_CMDID + referee_client.frame_header.DataLength);
  //���������ܳ���
  data_length = LEN_HEADER + LEN_CMDID + referee_client.frame_header.DataLength + LEN_TAIL;
  //���CRC16�����ݽ�β
  Append_CRC16_Check_Sum(cliend_tx_buffer, data_length);
  
  for(int i=0; i<data_length; i++)
  {
  	HAL_UART_Transmit(&RD_huart, &cliend_tx_buffer[i], 1, 1);  
  }
}

/**
  * @brief				�Զ���ͼ�Σ�����������
  * @param[in]		ͼ�θ���num    ͼ�ε�����name   ͼ�β���operate   ͼ�β���layer  ͼ����ɫcolor  
                  �����Сsize   ������� width   �������s_x,s_y   ����data     
	* @param[out]		
  * @retval				
*/
void Draw_Integral(uint8_t num, uint16_t name, uint8_t operate, uint8_t layer, uint8_t color, uint16_t size,
	   uint8_t width, uint16_t s_x, uint16_t s_y, int32_t data)
{
  unsigned char cliend_tx_buffer[LEN_SEND_CLIENT_SIDE_DATE_MAX];
	static uint8_t data_length;

  //����ͷ֡����
  referee_client.frame_header.SOF = FRAME_HEADER_SOF;//ͷ֡
  referee_client.frame_header.DataLength = LEN_ALL_ID + LEN_SEND_CLIENT_SIDE_BUFFER*LEN_SEND_CLIENT_SIDE_BUFFER_NUM;//���ݳ���
  referee_client.frame_header.Seq = 0;   
  //��ͷ֡���ݸ��Ƶ�������������
	memcpy(cliend_tx_buffer, &referee_client.frame_header, LEN_HEADER);
  //���CRC8��ͷ֡��β
  Append_CRC8_Check_Sum(cliend_tx_buffer, LEN_HEADER);
  
  //����CmdID
  referee_client.cmdID = ID_STUDENT_INTERACTIVE_HEADER;
  
  //�����Զ���UI���贫������
  //�������� ID �ٷ��̶�,��ǰΪ2��ͼ�Σ�����idΪ0x0102
  referee_client.student_interactive_header_data.data_cmd_id = ID_CLIENT_CUSTOM_GRAPHIC_DOUBLE;
  //���÷����ߵ� ID
  referee_client.student_interactive_header_data.sender_ID = referee_used.robot_id;
  //���ý����ߵ� ID
  referee_client.student_interactive_header_data.receiver_ID = Get_Receive_Client_Side_ID();
  //���÷��͵�����
  //ͼ�ε����ƣ��������ŵ���name������ÿһλ
  referee_client.graphic_data_struct[num].graphic_name[0] = name%10;
	referee_client.graphic_data_struct[num].graphic_name[1] = (name/10)%10;
	referee_client.graphic_data_struct[num].graphic_name[2] = (name/100)%10;
  //����������ɾ�������ӵȲ���
  referee_client.graphic_data_struct[num].operate_tpye = operate;
  //ͼ�����ࣻ��������
  referee_client.graphic_data_struct[num].graphic_tpye = INTEGER;
	//ͼ�������൱�ڿ��Բ��ϸ���
  referee_client.graphic_data_struct[num].layer = layer;
  //ͼ����ɫ  
  referee_client.graphic_data_struct[num].color = color;
  //�����С
  referee_client.graphic_data_struct[num].start_angle = size;
  //���
  referee_client.graphic_data_struct[num].width = width;
  //������������x��y
  referee_client.graphic_data_struct[num].start_x = s_x;
  referee_client.graphic_data_struct[num].start_y = s_y;
  //������
	referee_client.graphic_data_struct[num].radius = data >> 22;
	referee_client.graphic_data_struct[num].end_x  = data >> 11;
	referee_client.graphic_data_struct[num].end_y  = data;
  //�������˽������ݸ��Ƶ�������������
  memcpy(cliend_tx_buffer + LEN_HEADER, (uint8_t *)&referee_client.cmdID, LEN_CMDID + referee_client.frame_header.DataLength);
  //���������ܳ���
  data_length = LEN_HEADER + LEN_CMDID + referee_client.frame_header.DataLength + LEN_TAIL;
  //���CRC16�����ݽ�β
  Append_CRC16_Check_Sum(cliend_tx_buffer, data_length);
  
  for(int i=0; i<data_length; i++)
  {
  	HAL_UART_Transmit(&RD_huart, &cliend_tx_buffer[i], 1, 1);  
  }
}

/**
  * @brief				�Զ���ͼ�Σ������ַ���
  * @param[in]		ͼ�ε�����name  ͼ�β���operate    ͼ�β���layer   ͼ����ɫcolor
	* @param[out]		�����Сsize    �ַ�����length     ������� width  �������s_x,s_y   �ַ�data[]     
  * @retval				
*/
void DrawCharacer(uint16_t name, uint8_t operate, uint8_t layer, uint8_t color,uint8_t size, 
     uint8_t length, uint8_t width, uint16_t s_x, uint16_t s_y, char data[30])
{
  unsigned char cliend_tx_buffer[LEN_SEND_CLIENT_SIDE_DATE_MAX];
	static uint8_t data_length;

  //����ͷ֡����
  referee_client.frame_header.SOF = FRAME_HEADER_SOF;//ͷ֡
  referee_client.frame_header.DataLength = LEN_ALL_ID + sizeof(referee_client.client_custom_character);//���ݳ���
  referee_client.frame_header.Seq = 0;
  //��ͷ֡���ݸ��Ƶ�������������
	memcpy(cliend_tx_buffer, &referee_client.frame_header, LEN_HEADER);
  //���CRC8��ͷ֡��β
  Append_CRC8_Check_Sum(cliend_tx_buffer, LEN_HEADER);
  
  //����CmdID
  referee_client.cmdID = ID_STUDENT_INTERACTIVE_HEADER;
  
  //�����Զ���UI���贫������
  //�������� ID �ٷ��̶�,��ǰ�����ַ�������idΪ0x0110
  referee_client.student_interactive_header_data.data_cmd_id = ID_CLIENT_CUSTOM_CHARACTER;
  //���÷����ߵ� ID
  referee_client.student_interactive_header_data.sender_ID = referee_used.robot_id;
  //���ý����ߵ� ID
  referee_client.student_interactive_header_data.receiver_ID = Get_Receive_Client_Side_ID();
  //���÷��͵�����
  //ͼ�ε����ƣ��������ŵ���name������ÿһλ
  referee_client.client_custom_character.grapic_data_struct.graphic_name[0] = name%10;
	referee_client.client_custom_character.grapic_data_struct.graphic_name[1] = (name/10)%10;
	referee_client.client_custom_character.grapic_data_struct.graphic_name[2] = (name/100)%10;
  //����������ɾ�������ӵȲ���
  referee_client.client_custom_character.grapic_data_struct.operate_tpye = operate;
  //ͼ�����ࣻ�ַ���
  referee_client.client_custom_character.grapic_data_struct.graphic_tpye = CHARCTER;
	//ͼ�������൱�ڿ��Բ��ϸ���
  referee_client.client_custom_character.grapic_data_struct.layer = layer;
  //ͼ����ɫ  
  referee_client.client_custom_character.grapic_data_struct.color = color;
  //�����С
  referee_client.client_custom_character.grapic_data_struct.start_angle = size;
  //�ַ�����
  referee_client.client_custom_character.grapic_data_struct.end_angle = length;
  //�������
  referee_client.client_custom_character.grapic_data_struct.width = width;
  //�ַ�������ʼ����x��y
  referee_client.client_custom_character.grapic_data_struct.start_x = s_x;
  referee_client.client_custom_character.grapic_data_struct.start_y = s_y;
  //�ַ�������
  for(int j=0 ; j<30 ; j++)
    referee_client.client_custom_character.data[j] = ' ';
  strcpy((char*)referee_client.client_custom_character.data,data);
  
  //�������˽������ݸ��Ƶ�������������
  memcpy(cliend_tx_buffer + LEN_HEADER, (uint8_t *)&referee_client.cmdID, LEN_CMDID + referee_client.frame_header.DataLength);
  //���������ܳ���
  data_length = LEN_HEADER + LEN_CMDID + referee_client.frame_header.DataLength + LEN_TAIL;
  //���CRC16�����ݽ�β
  Append_CRC16_Check_Sum(cliend_tx_buffer, data_length);
  
  for(int i=0; i<data_length; i++)
  {
  	HAL_UART_Transmit(&RD_huart, &cliend_tx_buffer[i], 1, 1);  
  }

}
//===================================================================================================================//
/*********************************************** ��  �����Զ���UI  �� **************************************************/
//===================================================================================================================//


//===================================================================================================================//
/******************************************* �� �����˼佻�����ݷ��� �� ************************************************/
//===================================================================================================================//
/**
  * @brief				�жϻ����˼佻������ ���շ�ID
  * @param[in]		
	* @param[out]		
  * @retval				
*/
uint16_t Get_Receive_Robot_ID(uint16_t receive_robot_type)
{
  uint16_t receive_robot_id;
  
  if(referee_used.robot_id < 10)
  {
      switch (receive_robot_type)
      {
        case HERO_ROBOT: 
          receive_robot_id = ID_HERO_ROBOT_RED; break;
        
        case ENGINEER_ROBOT: 
          receive_robot_id = ID_ENGINEER_ROBOT_RED; break;
        
        case INFATRY3_ROBOT: 
          receive_robot_id = ID_INFATRY3_ROBOT_RED; break;
        
        case INFATRY4_ROBOT: 
          receive_robot_id = ID_INFATRY4_ROBOT_RED; break;
        
        case INFATRY5_ROBOT: 
          receive_robot_id = ID_INFATRY5_ROBOT_RED; break;
        
        case AERIAL_ROBOT: 
          receive_robot_id = ID_AERIAL_ROBOT_RED; break; 
        
        case SENTRY_ROBOT: 
           receive_robot_id = ID_SENTRY_ROBOT_RED; break;
        
        case RADAR_ROBOT: 
          receive_robot_id = ID_RADAR_ROBOT_RED; break; 
        
        default:break;
      }    
  }
  else
  {
      switch (receive_robot_type)
      {
        case HERO_ROBOT: 
          receive_robot_id = ID_HERO_ROBOT_BLUE; break;
        
        case ENGINEER_ROBOT: 
          receive_robot_id = ID_ENGINEER_ROBOT_BLUE; break;
        
        case INFATRY3_ROBOT: 
          receive_robot_id = ID_INFATRY3_ROBOT_BLUE; break;
        
        case INFATRY4_ROBOT: 
          receive_robot_id = ID_INFATRY4_ROBOT_BLUE; break;
        
        case INFATRY5_ROBOT: 
          receive_robot_id = ID_INFATRY5_ROBOT_BLUE; break;
        
        case AERIAL_ROBOT: 
          receive_robot_id = ID_AERIAL_ROBOT_BLUE; break; 
        
        case SENTRY_ROBOT: 
           receive_robot_id = ID_SENTRY_ROBOT_BLUE; break;
        
        case RADAR_ROBOT: 
          receive_robot_id = ID_RADAR_ROBOT_BLUE; break; 
        
        default:break;
      }        
  }
  return receive_robot_id;
}

/**
  * @brief				������������ͨ��
  * @param[in]		
	* @param[out]		
  * @retval				none
*/
void Robot_Interactive_Date(uint8_t data[], uint16_t robot_interactive_id, uint16_t receive_robot_type)
{
  unsigned char robot_interactive_buffer[LEN_ROBOT_INTERACTIVE_DATE_MAX];//�������ݴ���������
  static uint8_t robot_interactive_data_length,i;//�������ݳ���
  
  //����ͷ֡����
  referee_send.frame_header.SOF = FRAME_HEADER_SOF;//ͷ֡
  referee_send.frame_header.DataLength = LEN_ALL_ID + LEN_ROBOT_INTERACTIVE_DATE;//���ݳ���
  referee_send.frame_header.Seq = 0;
  //��ͷ֡���ݸ��Ƶ�������������
	memcpy(robot_interactive_buffer, &referee_send.frame_header, LEN_HEADER);
  //���CRC8��ͷ֡��β
  Append_CRC8_Check_Sum(robot_interactive_buffer, LEN_HEADER);
  //����CmdID
  referee_send.cmdID = ID_STUDENT_INTERACTIVE_HEADER;
  //���û����˼佻������
  //�������� ID
  referee_send.student_interactive_header_data.data_cmd_id = robot_interactive_id;
  //���÷����ߵ� ID
  referee_send.student_interactive_header_data.sender_ID = referee_used.robot_id;
  //���ý����ߵ� ID
  referee_send.student_interactive_header_data.receiver_ID = Get_Receive_Robot_ID(receive_robot_type);
  //���÷��͵�����
  referee_send.robot_interactive_data.data[0] = data[0];//�ɼ������
  //�������˽������ݸ��Ƶ�������������
  memcpy(robot_interactive_buffer + LEN_HEADER, (uint8_t *)&referee_send.cmdID, LEN_CMDID + referee_send.frame_header.DataLength);
  //���������ܳ���
  robot_interactive_data_length = LEN_HEADER + LEN_CMDID + referee_send.frame_header.DataLength + LEN_TAIL; 
 //���CRC16�����ݽ�β
  Append_CRC16_Check_Sum(robot_interactive_buffer, robot_interactive_data_length);

  for(i=0; i<robot_interactive_data_length; i++)
  {
  	HAL_UART_Transmit(&RD_huart, &robot_interactive_buffer[i], 1, 1);  
  }
  
}
//===================================================================================================================//
/******************************************* �� �����˼佻�����ݷ��� �� ************************************************/
//===================================================================================================================//

