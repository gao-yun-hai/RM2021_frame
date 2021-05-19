/*******************************************************************************
                      版权所有 (C), 2020-,NCUROBOT
 *******************************************************************************
  文 件 名   : referee.c 
  版 本 号   : V1.0
  作    者   : 高云海
  生成日期   : 2020.12.2
  最近修改   : 
  功能描述   : 裁判系统数据接收与解码及发送【USART初始化、UASRT中断处理、数据解码、数据发送】
							 根据2020.5发布的裁判系统串口协议V1.1版本
  函数列表   : 1) RefereeDate_Receive_Init()【外部调用：bsp.c】
							 2) Referee_UART_IRQHandler() 【外部调用：stm32f4xx_it.c的USART6中断服务函数】
							 3) SBUS_To_Referee()					【内部调用：Referee_UART_IRQHandler()】
*******************************************************************************/
/* 包含头文件 ----------------------------------------------------------------*/
#include "referee.h"
#include "bsp_usart.h"
#include "crc.h"
#include "offline_check.h"
/* 内部宏定义 ----------------------------------------------------------------*/
#define RD_huart  huart6	//与裁判系统通信所用串口
/* 内部自定义数据类型的变量 --------------------------------------------------*/
Referee_Struct  referee;

/* 内部变量 ------------------------------------------------------------------*/
static uint8_t referee_rx_buf[REFEREE_RX_BUFFER_SIZE];//接收裁判系统原始数据
uint16_t referee_rx_date_len;//实际接收到裁判系统数据的长度
/* 内部函数原型声明 ----------------------------------------------------------*/
static void SBUS_To_Referee(uint8_t *buff, Referee_Struct  *Referee);


/* 函数主体部分 --------------------------------------------------------------*/
/**
  * @brief				裁判系统数据接收USART初始化函数（开启DMA+开启USART空闲中断）
  * @param[in]		
	* @param[out]		
  * @retval				none
*/
void RefereeDate_Receive_USART_Init(void)
{
    UART_IT_Init(&RD_huart,referee_rx_buf,REFEREE_RX_BUFFER_SIZE);
}


/**
  * @brief				裁判系统USART中断处理函数（重新设置DMA参数+数据解码+发送任务通知）
  * @param[in]		
	* @param[out]		
  * @retval				none
*/
void Referee_UART_IRQHandler(void)
{
	static  BaseType_t  pxHigherPriorityTaskWoken;
	/* 判断是否为空闲中断 */
  if (__HAL_UART_GET_FLAG(&RD_huart, UART_FLAG_IDLE))
	{
			/* 清除空闲标志，避免一直处于空闲状态的中断 */
			__HAL_UART_CLEAR_IDLEFLAG(&RD_huart);

			/* 关闭DMA传输 */
			__HAL_DMA_DISABLE(RD_huart.hdmarx);

			/* 计算接收裁判系统数据的长度 */
			referee_rx_date_len = (REFEREE_RX_BUFFER_SIZE - RD_huart.hdmarx->Instance->NDTR);
			/* 数据解码 */
			SBUS_To_Referee(referee_rx_buf,&referee);
			/* 断线检测刷新时间 */
			Refresh_Device_OffLine_Time(Referee_TOE);		
			/* 任务通知 */
			vTaskNotifyGiveFromISR(RefereeDataTaskHandle,&pxHigherPriorityTaskWoken);
			portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);	
			/* 重新启动DMA传输 */
			/* 设置DMA可传输最大数据长度 */
			__HAL_DMA_SET_COUNTER(RD_huart.hdmarx, REFEREE_RX_BUFFER_SIZE);
			/* 开启DMA传输 */
			__HAL_DMA_ENABLE(RD_huart.hdmarx);				
			
	}
}


/**
  * @brief				裁判系统数据解码
  * @param[in]		buff：指向数据接收数组的指针
	* @param[out]		Referee：指向存储裁判系统数据结构体的指针
  * @retval				none
*/
static void SBUS_To_Referee(uint8_t *buff, Referee_Struct  *Referee)
{		
		uint16_t  referee_length = 0;//一帧数据总长度
	
		//无数据包，则不作任何处理
		if (buff == NULL)
		{
			return ;
		}
		
		for(uint8_t i=0;i<referee_rx_date_len;i++)
		{
			//判断帧头数据是否为0xA5(接收到的一组数据中包含多帧数据，故对于一组数据需要循环判断)
			if(buff[i] == FRAME_HEADER_SOF)
			{
					//写入帧头数据,用于判断是否开始存储裁判数据
					memcpy(&Referee->frame_header, buff + i, LEN_HEADER);
					//写入命令码ID数据,用于判断是哪个类型数据
				 	memcpy(&Referee->cmdID, buff + LEN_HEADER + i, LEN_CMDID);
				  //一帧数据总长度：帧头长度 + 命令码ID长度 + 数据长度 + 帧尾长度
					referee_length = LEN_HEADER + LEN_CMDID + Referee->frame_header.DataLength + LEN_TAIL; 
					//帧头CRC8校验及帧尾CRC16校验
					if(Verify_CRC16_Check_Sum(&buff[0+i], referee_length) && Verify_CRC8_Check_Sum(&buff[0+i],LEN_HEADER))
					{
							switch(Referee->cmdID)
							{
									case ID_GAME_STATE:       								 //0x0001 比赛状态数据
										memcpy(&Referee->game_state, (buff + LEN_HEADER + LEN_CMDID + i), LEN_GAME_STATE);
									break;
									
									case ID_GAME_RESULT:      								 //0x0002 比赛结果数据
										memcpy(&Referee->game_result, (buff + LEN_HEADER + LEN_CMDID + i), LEN_GAME_RESULT);
									break;

									case ID_GAME_ROBOT_HP:   									 //0x0003 比赛机器人血量数据
										memcpy(&Referee->game_robot_HP, (buff + LEN_HEADER + LEN_CMDID + i), LEN_GAME_ROBOT_HP);
									break;
									
									case ID_DART_STATUS:      								 //0x0004 飞镖发射状态
										memcpy(&Referee->dart_status, (buff + LEN_HEADER + LEN_CMDID + i), LEN_DART_STATUS);
									break;	

									case ID_ICRA_BUFF_DEBUFF_ZONE_STATUS:      //0x0005 人工智能挑战赛加成与惩罚区状态
										memcpy(&Referee->ICRA_buff_debuff_zone_status, (buff + LEN_HEADER + LEN_CMDID + i), LEN_ICRA_BUFF_DEBUFF_ZONE_STATUS);
									break;								
									
									case ID_EVENT_DATE:      									 //0x0101 场地事件数据
										memcpy(&Referee->event_data, (buff + LEN_HEADER + LEN_CMDID + i), LEN_EVENT_DATE);
									break;	
									
									case ID_SUPPLY_PROJRCTILE_ACTION:      		 //0x0102 场地补给站动作标识数据
										memcpy(&Referee->supply_projectile_action, (buff + LEN_HEADER + LEN_CMDID+i), LEN_SUPPLY_PROJRCTILE_ACTION);
									break;		

									case ID_REFEREE_WARNING:      		 				 //0x0104 裁判警告数据
										memcpy(&Referee->referee_warning, (buff + LEN_HEADER + LEN_CMDID + i), LEN_REFEREE_WARNING);
									break;	

									case ID_DART_REMAINING_TIME:      		 		 //0x0105 飞镖发射口倒计时
										memcpy(&Referee->dart_remaining_time, (buff + LEN_HEADER + LEN_CMDID + i), LEN_DART_REMAINING_TIME);
									break;									
								
									case ID_GAME_ROBOT_STATUS:      		 		 	 //0x0201 机器人状态数据
										memcpy(&Referee->game_robot_status, (buff + LEN_HEADER + LEN_CMDID + i), LEN_GAME_ROBOT_STATUS);
									break;	

									case ID_POWER_HEAT_DATE:      		 		 	   //0x0202 实时功率热量数据
										memcpy(&Referee->power_heat_data, (buff + LEN_HEADER + LEN_CMDID + i), LEN_POWER_HEAT_DATE);
									break;

									case ID_GAME_ROBOT_POS:      		 		 	     //0x0203 机器人位置数据
										memcpy(&Referee->game_robot_pos, (buff + LEN_HEADER + LEN_CMDID + i), LEN_GAME_ROBOT_POS);
									break;

									case ID_BUFF:      		 		 	    					 //0x0204 机器人增益数据
										memcpy(&Referee->buff, (buff + LEN_HEADER + LEN_CMDID + i), LEN_BUFF);
									break;									
									
									case ID_AERIAL_ROBOT_ENERGY:      		 	   //0x0205 机器人增益数据
										memcpy(&Referee->aerial_robot_energy, (buff + LEN_HEADER + LEN_CMDID + i), LEN_AERIAL_ROBOT_ENERGY);
									break;

									case ID_ROBOT_HURT:      		 	  				   //0x0206 伤害状态数据
										memcpy(&Referee->robot_hurt, (buff + LEN_HEADER + LEN_CMDID + i), LEN_ROBOT_HURT);
									break;

									case ID_SHOOT_DATE:      		 	  				   //0x0207 实时射击数据
										memcpy(&Referee->shoot_data, (buff + LEN_HEADER + LEN_CMDID + i), LEN_SHOOT_DATE);
									break;	

									case ID_BULLET_REAMINING:      		 	  		 //0x0208 弹丸剩余发射数
										memcpy(&Referee->bullet_remaining, (buff + LEN_HEADER + LEN_CMDID + i), LEN_BULLET_REAMINING);
									break;	

									case ID_RFID_STATUS:      		 	  		 		 //0x0209 机器人 RFID 状态
										memcpy(&Referee->RFID_status, (buff + LEN_HEADER + LEN_CMDID + i), LEN_RFID_STATUS);
									break;

									case ID_DART_CLIENT_CMD:      		 	  		 //0x20A 飞镖机器人客户端指令数据
										memcpy(&Referee->dart_client_cmd, (buff + LEN_HEADER + LEN_CMDID + i), LEN_DART_CLIENT_CMD);
									break;								
							}		
							
						i=i+referee_length;	
					}
			}
	 }			
}
/**
  * @brief				获取机器人剩余血量
  * @param[in]		
	* @param[out]		
  * @retval				
*/
void get_robot_remain_hp(uint16_t *remain_hp)
{
   *remain_hp = referee.game_robot_status.remain_HP;
}

/**
  * @brief				获取裁判系统供电接口状态
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
  * @brief				获取底盘功率与剩余缓存能量
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
  * @brief				获取1号17mm枪口热量及上限
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
  * @brief				获取枪口射频及射速
  * @param[in]		type：枪管类型
                  id：枪管ID
                  freq：射频
                  speed：射速
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

