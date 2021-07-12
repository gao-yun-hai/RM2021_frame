/*******************************************************************************
                      版权所有 (C), 2020-,NCUROBOT
 *******************************************************************************
  文 件 名   : referee.c 
  版 本 号   : V1.0 → V1.1
  作    者   : 高云海
  生成日期   : 2020.12.2
  最近修改   : 2021.7.9
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
Referee_Struct  referee_receive;          //裁判系统接收数据
Referee_Used_Struct referee_used;         //裁判系统接收数据-外部使用
Referee_Struct  referee_send;             //裁判系统数据发送-机器人交互
Referee_Struct  referee_client;           //裁判系统数据发送-自定义UI

/* 内部变量 ------------------------------------------------------------------*/
static uint8_t referee_rx_buf[REFEREE_RX_BUFFER_SIZE];//接收裁判系统原始数据
uint16_t referee_rx_date_len;//实际接收到裁判系统数据的长度
/* 内部函数原型声明 ----------------------------------------------------------*/
static void SBUS_To_Referee(uint8_t *buff, Referee_Struct  *Referee);
void Referee_To_Use(void);

/* 函数主体部分 --------------------------------------------------------------*/
//===================================================================================================================//
/******************************************* ↓ 裁判系统数据接收部分 ↓ ************************************************/
//===================================================================================================================//
/**
  * @brief				裁判系统数据接收USART初始化函数（开启DMA+开启USART空闲中断）
  * @param[in]		
	* @param[out]		
  * @retval				none
*/
void RefereeDate_Receive_USART_Init(void)
{
    HAL_UART_IT_Init(&RD_huart,referee_rx_buf,REFEREE_RX_BUFFER_SIZE);
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
			SBUS_To_Referee(referee_rx_buf,&referee_receive);
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
                  
									case ID_STUDENT_INTERACTIVE_HEADER:				 //0x0301 机器人间交互数据
                    //空中机器人发送给哨兵
                    if(Referee->student_interactive_header_data.data_cmd_id ==  ID_AERIAL_ROBOT_TO_SENTRY_ROBOT)
                    {
                      memcpy(&Referee->aerial_to_sentry_data, (buff + LEN_HEADER + LEN_CMDID + LEN_ALL_ID + i),LEN_AERIAL_ROBOT_TO_SENTRY_ROBOT);                      
                    }
                    //空中机器人发送给飞镖
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
    
   Referee_To_Use();//将外部需要使用的裁判系统数据幅值给referee_used结构体
   
}
/**
  * @brief				外部需要使用的裁判系统数据（如需其他数据请先在referee_used结构体中添加后进行幅值）
  * @param[in]		
	* @param[out]		
  * @retval				
*/
void Referee_To_Use(void)
{
  referee_used.robot_id =  referee_receive.game_robot_status.robot_id;//机器人ID

  referee_used.robot_remain_hp = referee_receive.game_robot_status.remain_HP;//机器人剩余血量

  referee_used.chassis_power = referee_receive.power_heat_data.chassis_power;//底盘实时功率
  referee_used.chassis_power_buffer = referee_receive.power_heat_data.chassis_power_buffer;//底盘缓冲能量
  referee_used.chassis_power_limit = referee_receive.game_robot_status.chassis_power_limit;//底盘功率上限

  referee_used.shooter1_17mm_heat_limit = referee_receive.game_robot_status.shooter_id1_17mm_cooling_limit;//17mm 发射机构1 枪口热量上限
  referee_used.shooter2_17mm_heat_limit = referee_receive.game_robot_status.shooter_id2_17mm_cooling_limit;//17mm 发射机构2 枪口热量上限
  referee_used.shooter1_42mm_heat_limit = referee_receive.game_robot_status.shooter_id1_42mm_cooling_limit;//42mm 发射机构1 枪口热量上限
  
  referee_used.shooter1_17mm_speed_limit = referee_receive.game_robot_status.shooter_id1_17mm_speed_limit;//17mm 发射机构1 枪口射速上限
  referee_used.shooter2_17mm_speed_limit = referee_receive.game_robot_status.shooter_id2_17mm_speed_limit;//17mm 发射机构2 枪口射速上限
  referee_used.shooter1_42mm_speed_limit = referee_receive.game_robot_status.shooter_id1_42mm_speed_limit;//42mm 发射机构1 枪口射速上限

  referee_used.shooter1_17mm_heat = referee_receive.power_heat_data.shooter_id1_17mm_cooling_heat;//17mm 发射机构1 枪口热量
  referee_used.shooter2_17mm_heat = referee_receive.power_heat_data.shooter_id2_17mm_cooling_heat;//17mm 发射机构2 枪口热量
  referee_used.shooter1_42mm_heat = referee_receive.power_heat_data.shooter_id1_42mm_cooling_heat;//42mm 发射机构1 枪口热量

  if(referee_receive.shoot_data.bullet_type == BULLET_17MM)
  {
    if(referee_receive.shoot_data.shooter_id == SHOOTER1_17MM)
    {
       referee_used.shooter1_17mm_freq  = referee_receive.shoot_data.bullet_freq; //17mm 发射机构1 射频
       referee_used.shooter1_17mm_speed = referee_receive.shoot_data.bullet_speed;//17mm 发射机构1 射速
    }

    else if(referee_receive.shoot_data.shooter_id == SHOOTER2_17MM)
    {    
       referee_used.shooter2_17mm_freq  = referee_receive.shoot_data.bullet_freq; //17mm 发射机构2 射频
       referee_used.shooter2_17mm_speed = referee_receive.shoot_data.bullet_speed;//17mm 发射机构2 射速
    }
  }
  else if(referee_receive.shoot_data.bullet_type == BULLET_42MM && referee_receive.shoot_data.shooter_id == SHOOTER2_17MM)
  {
     referee_used.shooter1_42mm_freq  = referee_receive.shoot_data.bullet_freq; //42mm 发射机构1 射频
     referee_used.shooter1_42mm_speed = referee_receive.shoot_data.bullet_speed;//42mm 发射机构1 射速
  }
  
  referee_used.bullet_17mm_remaining_num = referee_receive.bullet_remaining.bullet_remaining_num_17mm;//17mm 子弹剩余发射数目（对抗赛仅空中机器人与哨兵有用）
  referee_used.coin_remaining_num = referee_receive.bullet_remaining.coin_remaining_num;//剩余金币数量
  
  referee_used.chassis_output = referee_receive.game_robot_status.mains_power_chassis_output;//云台口输出
  referee_used.gimbal_output  = referee_receive.game_robot_status.mains_power_gimbal_output; //底盘口输出
  referee_used.shooter_output = referee_receive.game_robot_status.mains_power_shooter_output;//发射口输出  
}
//===================================================================================================================//
/******************************************* ↑ 裁判系统数据接收部分 ↑ ************************************************/
//===================================================================================================================//


//===================================================================================================================//
/*********************************************** ↓  绘制自定义UI  ↓ **************************************************/
//===================================================================================================================//
/**
  * @brief				获取绘制自定义UI 客户端ID
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
  * @brief				自定义图形，画直线或者矩形或者椭圆
  * @param[in]		图形个数num    图形的名称name    图形操作operate   图形的类型type   图形层数layer
                  图形颜色color  图形的宽度 width  起始坐标s_x,s_y   终点坐标e_x,e_y  
	* @param[out]		
  * @retval				
*/
void Draw_StraightLine_or_Rectangle_or_Ellipse(uint8_t num, uint8_t name, uint8_t operate, uint8_t type, 
     uint8_t layer, uint8_t color, uint8_t width , uint16_t s_x, uint16_t s_y, uint16_t e_x, uint16_t e_y)
{
  unsigned char cliend_tx_buffer[LEN_SEND_CLIENT_SIDE_DATE_MAX];
	static uint8_t data_length;

  //设置头帧数据
  referee_client.frame_header.SOF = FRAME_HEADER_SOF;//头帧
  referee_client.frame_header.DataLength = LEN_ALL_ID + LEN_SEND_CLIENT_SIDE_BUFFER*LEN_SEND_CLIENT_SIDE_BUFFER_NUM;//数据长度
  referee_client.frame_header.Seq = 0;
  //将头帧数据复制到待发送数组中
	memcpy(cliend_tx_buffer, &referee_client.frame_header, LEN_HEADER);
  //添加CRC8到头帧结尾
  Append_CRC8_Check_Sum(cliend_tx_buffer, LEN_HEADER);
  
  //设置CmdID
  referee_client.cmdID = ID_STUDENT_INTERACTIVE_HEADER;
  
  //设置自定义UI所需传输数据
  //设置内容 ID 官方固定,当前为2个图形，所以id为0x0102
  referee_client.student_interactive_header_data.data_cmd_id = ID_CLIENT_CUSTOM_GRAPHIC_DOUBLE;
  //设置发送者的 ID
  referee_client.student_interactive_header_data.sender_ID = referee_used.robot_id;
  //设置接收者的 ID
  referee_client.student_interactive_header_data.receiver_ID = Get_Receive_Client_Side_ID();
  //设置发送的数据
  //图形的名称；数组里存放的是name变量的每一位
  referee_client.graphic_data_struct[num].graphic_name[0] = name%10;
	referee_client.graphic_data_struct[num].graphic_name[1] = (name/10)%10;
	referee_client.graphic_data_struct[num].graphic_name[2] = (name/100)%10;
  //操作；可以删除，增加等操作
  referee_client.graphic_data_struct[num].operate_tpye = operate;
  //图形种类；直线、矩形或者椭圆
  referee_client.graphic_data_struct[num].graphic_tpye = type;
	//图层数；相当于可以不断覆盖
  referee_client.graphic_data_struct[num].layer = layer;
  //图形颜色
  referee_client.graphic_data_struct[num].color = color;
  //宽度
  referee_client.graphic_data_struct[num].width = width;
  //起始坐标；直线或者矩形的起始位置x,y，椭圆圆心的x,y坐标
  referee_client.graphic_data_struct[num].start_x = s_x;
  referee_client.graphic_data_struct[num].start_y = s_y;
	//直线的末尾位置；矩形对顶角x,y的坐标，椭圆x半轴长度和y半轴长度
  referee_client.graphic_data_struct[num].end_x = e_x;
  referee_client.graphic_data_struct[num].end_y = e_y;    
  //将机器人交互数据复制到待发送数组中
  memcpy(cliend_tx_buffer + LEN_HEADER, (uint8_t *)&referee_client.cmdID, LEN_CMDID + referee_client.frame_header.DataLength);
  //计算数据总长度
  data_length = LEN_HEADER + LEN_CMDID + referee_client.frame_header.DataLength + LEN_TAIL;
  //添加CRC16到数据结尾
  Append_CRC16_Check_Sum(cliend_tx_buffer, data_length);
  
  for(int i=0; i<data_length; i++)
  {
  	HAL_UART_Transmit(&RD_huart, &cliend_tx_buffer[i], 1, 1);  
  }
  
}

/**
  * @brief				自定义图形，画整圆
  * @param[in]		图形个数num    图形的名称name    图形操作operate   图形层数layer
                  图形颜色color  图形的宽度 width  圆心坐标s_x,s_y   圆的半径radius  
	* @param[out]		
  * @retval				
*/
void Draw_FullCircle(uint8_t num, uint16_t name, uint8_t operate, uint8_t layer,
     uint8_t color, uint8_t width, uint16_t s_x, uint16_t s_y, uint16_t radius)
{
  unsigned char cliend_tx_buffer[LEN_SEND_CLIENT_SIDE_DATE_MAX];
	static uint8_t data_length;

  //设置头帧数据
  referee_client.frame_header.SOF = FRAME_HEADER_SOF;//头帧
  referee_client.frame_header.DataLength = LEN_ALL_ID + LEN_SEND_CLIENT_SIDE_BUFFER*LEN_SEND_CLIENT_SIDE_BUFFER_NUM;//数据长度
  referee_client.frame_header.Seq = 0;
  //将头帧数据复制到待发送数组中
	memcpy(cliend_tx_buffer, &referee_client.frame_header, LEN_HEADER);
  //添加CRC8到头帧结尾
  Append_CRC8_Check_Sum(cliend_tx_buffer, LEN_HEADER);
  
  //设置CmdID
  referee_client.cmdID = ID_STUDENT_INTERACTIVE_HEADER;
  
  //设置自定义UI所需传输数据
  //设置内容 ID 官方固定,当前为2个图形，所以id为0x0102
  referee_client.student_interactive_header_data.data_cmd_id = ID_CLIENT_CUSTOM_GRAPHIC_DOUBLE;
  //设置发送者的 ID
  referee_client.student_interactive_header_data.sender_ID = referee_used.robot_id;
  //设置接收者的 ID
  referee_client.student_interactive_header_data.receiver_ID = Get_Receive_Client_Side_ID();
  //设置发送的数据
  //图形的名称；数组里存放的是name变量的每一位
  referee_client.graphic_data_struct[num].graphic_name[0] = name%10;
	referee_client.graphic_data_struct[num].graphic_name[1] = (name/10)%10;
	referee_client.graphic_data_struct[num].graphic_name[2] = (name/100)%10;
  //操作；可以删除，增加等操作
  referee_client.graphic_data_struct[num].operate_tpye = operate;
  //图形种类；整圆
  referee_client.graphic_data_struct[num].graphic_tpye = FULL_CIRCLE;
	//图层数；相当于可以不断覆盖
  referee_client.graphic_data_struct[num].layer = layer;
  //图形颜色  
  referee_client.graphic_data_struct[num].color = color;
  //宽度
  referee_client.graphic_data_struct[num].width = width;
  //圆心的x,y坐标
  referee_client.graphic_data_struct[num].start_x = s_x;
  referee_client.graphic_data_struct[num].start_y = s_y;
	//圆的半径
  referee_client.graphic_data_struct[num].radius = radius;    
  //将机器人交互数据复制到待发送数组中
  memcpy(cliend_tx_buffer + LEN_HEADER, (uint8_t *)&referee_client.cmdID, LEN_CMDID + referee_client.frame_header.DataLength);
  //计算数据总长度
  data_length = LEN_HEADER + LEN_CMDID + referee_client.frame_header.DataLength + LEN_TAIL;
  //添加CRC16到数据结尾
  Append_CRC16_Check_Sum(cliend_tx_buffer, data_length);
  
  for(int i=0; i<data_length; i++)
  {
  	HAL_UART_Transmit(&RD_huart, &cliend_tx_buffer[i], 1, 1);  
  }
}

/**
  * @brief				自定义图形，画圆弧
  * @param[in]		图形个数num       图形的名称name         图形操作operate       图形层数layer       图形颜色color  
                  图形的宽度 width  圆弧起始角度star_angle 圆弧终止角度end_angle 圆心坐标s_x,s_y xy  半长轴长度e_x,e_y

	* @param[out]		
  * @retval				
*/
void Draw_ARC(uint8_t num, uint16_t name, uint8_t operate, uint8_t layer, uint8_t color, uint16_t start_angle, 
     uint16_t end_angle, uint8_t width, uint16_t s_x, uint16_t s_y, uint16_t e_x, uint16_t e_y)
{
  unsigned char cliend_tx_buffer[LEN_SEND_CLIENT_SIDE_DATE_MAX];
	static uint8_t data_length;

  //设置头帧数据
  referee_client.frame_header.SOF = FRAME_HEADER_SOF;//头帧
  referee_client.frame_header.DataLength = LEN_ALL_ID + LEN_SEND_CLIENT_SIDE_BUFFER*LEN_SEND_CLIENT_SIDE_BUFFER_NUM;//数据长度
  referee_client.frame_header.Seq = 0;
  //将头帧数据复制到待发送数组中
	memcpy(cliend_tx_buffer, &referee_client.frame_header, LEN_HEADER);
  //添加CRC8到头帧结尾
  Append_CRC8_Check_Sum(cliend_tx_buffer, LEN_HEADER);
  
  //设置CmdID
  referee_client.cmdID = ID_STUDENT_INTERACTIVE_HEADER;
  
  //设置自定义UI所需传输数据
  //设置内容 ID 官方固定,当前为2个图形，所以id为0x0102
  referee_client.student_interactive_header_data.data_cmd_id = ID_CLIENT_CUSTOM_GRAPHIC_DOUBLE;
  //设置发送者的 ID
  referee_client.student_interactive_header_data.sender_ID = referee_used.robot_id;
  //设置接收者的 ID
  referee_client.student_interactive_header_data.receiver_ID = Get_Receive_Client_Side_ID();
  //设置发送的数据
  //图形的名称；数组里存放的是name变量的每一位
  referee_client.graphic_data_struct[num].graphic_name[0] = name%10;
	referee_client.graphic_data_struct[num].graphic_name[1] = (name/10)%10;
	referee_client.graphic_data_struct[num].graphic_name[2] = (name/100)%10;
  //操作；可以删除，增加等操作
  referee_client.graphic_data_struct[num].operate_tpye = operate;
  //图形种类；圆弧
  referee_client.graphic_data_struct[num].graphic_tpye = ARC;
	//图层数；相当于可以不断覆盖
  referee_client.graphic_data_struct[num].layer = layer;
  //图形颜色  
  referee_client.graphic_data_struct[num].color = color;
  //圆弧的起始角度
  referee_client.graphic_data_struct[num].start_angle = start_angle;
  //圆弧的终止角度
  referee_client.graphic_data_struct[num].end_angle = end_angle; 
  //宽度
  referee_client.graphic_data_struct[num].width = width;
  //圆心的x,y坐标
  referee_client.graphic_data_struct[num].start_x = s_x;
  referee_client.graphic_data_struct[num].start_y = s_y;
	//x，y半长轴长度
  referee_client.graphic_data_struct[num].end_x = e_x;
  referee_client.graphic_data_struct[num].end_y = e_y;  
  //将机器人交互数据复制到待发送数组中
  memcpy(cliend_tx_buffer + LEN_HEADER, (uint8_t *)&referee_client.cmdID, LEN_CMDID + referee_client.frame_header.DataLength);
  //计算数据总长度
  data_length = LEN_HEADER + LEN_CMDID + referee_client.frame_header.DataLength + LEN_TAIL;
  //添加CRC16到数据结尾
  Append_CRC16_Check_Sum(cliend_tx_buffer, data_length);
  
  for(int i=0; i<data_length; i++)
  {
  	HAL_UART_Transmit(&RD_huart, &cliend_tx_buffer[i], 1, 1);  
  }
}

/**
  * @brief				自定义图形，绘制整型数
  * @param[in]		图形个数num    图形的名称name   图形操作operate   图形层数layer  图形颜色color  
                  字体大小size   线条宽度 width   起点坐标s_x,s_y   数据data     
	* @param[out]		
  * @retval				
*/
void Draw_Integral(uint8_t num, uint16_t name, uint8_t operate, uint8_t layer, uint8_t color, uint16_t size,
	   uint8_t width, uint16_t s_x, uint16_t s_y, int32_t data)
{
  unsigned char cliend_tx_buffer[LEN_SEND_CLIENT_SIDE_DATE_MAX];
	static uint8_t data_length;

  //设置头帧数据
  referee_client.frame_header.SOF = FRAME_HEADER_SOF;//头帧
  referee_client.frame_header.DataLength = LEN_ALL_ID + LEN_SEND_CLIENT_SIDE_BUFFER*LEN_SEND_CLIENT_SIDE_BUFFER_NUM;//数据长度
  referee_client.frame_header.Seq = 0;   
  //将头帧数据复制到待发送数组中
	memcpy(cliend_tx_buffer, &referee_client.frame_header, LEN_HEADER);
  //添加CRC8到头帧结尾
  Append_CRC8_Check_Sum(cliend_tx_buffer, LEN_HEADER);
  
  //设置CmdID
  referee_client.cmdID = ID_STUDENT_INTERACTIVE_HEADER;
  
  //设置自定义UI所需传输数据
  //设置内容 ID 官方固定,当前为2个图形，所以id为0x0102
  referee_client.student_interactive_header_data.data_cmd_id = ID_CLIENT_CUSTOM_GRAPHIC_DOUBLE;
  //设置发送者的 ID
  referee_client.student_interactive_header_data.sender_ID = referee_used.robot_id;
  //设置接收者的 ID
  referee_client.student_interactive_header_data.receiver_ID = Get_Receive_Client_Side_ID();
  //设置发送的数据
  //图形的名称；数组里存放的是name变量的每一位
  referee_client.graphic_data_struct[num].graphic_name[0] = name%10;
	referee_client.graphic_data_struct[num].graphic_name[1] = (name/10)%10;
	referee_client.graphic_data_struct[num].graphic_name[2] = (name/100)%10;
  //操作；可以删除，增加等操作
  referee_client.graphic_data_struct[num].operate_tpye = operate;
  //图形种类；整形数据
  referee_client.graphic_data_struct[num].graphic_tpye = INTEGER;
	//图层数；相当于可以不断覆盖
  referee_client.graphic_data_struct[num].layer = layer;
  //图形颜色  
  referee_client.graphic_data_struct[num].color = color;
  //字体大小
  referee_client.graphic_data_struct[num].start_angle = size;
  //宽度
  referee_client.graphic_data_struct[num].width = width;
  //整型数的坐标x，y
  referee_client.graphic_data_struct[num].start_x = s_x;
  referee_client.graphic_data_struct[num].start_y = s_y;
  //整型数
	referee_client.graphic_data_struct[num].radius = data >> 22;
	referee_client.graphic_data_struct[num].end_x  = data >> 11;
	referee_client.graphic_data_struct[num].end_y  = data;
  //将机器人交互数据复制到待发送数组中
  memcpy(cliend_tx_buffer + LEN_HEADER, (uint8_t *)&referee_client.cmdID, LEN_CMDID + referee_client.frame_header.DataLength);
  //计算数据总长度
  data_length = LEN_HEADER + LEN_CMDID + referee_client.frame_header.DataLength + LEN_TAIL;
  //添加CRC16到数据结尾
  Append_CRC16_Check_Sum(cliend_tx_buffer, data_length);
  
  for(int i=0; i<data_length; i++)
  {
  	HAL_UART_Transmit(&RD_huart, &cliend_tx_buffer[i], 1, 1);  
  }
}

/**
  * @brief				自定义图形，绘制字符串
  * @param[in]		图形的名称name  图形操作operate    图形层数layer   图形颜色color
	* @param[out]		字体大小size    字符长度length     线条宽度 width  起点坐标s_x,s_y   字符data[]     
  * @retval				
*/
void DrawCharacer(uint16_t name, uint8_t operate, uint8_t layer, uint8_t color,uint8_t size, 
     uint8_t length, uint8_t width, uint16_t s_x, uint16_t s_y, char data[30])
{
  unsigned char cliend_tx_buffer[LEN_SEND_CLIENT_SIDE_DATE_MAX];
	static uint8_t data_length;

  //设置头帧数据
  referee_client.frame_header.SOF = FRAME_HEADER_SOF;//头帧
  referee_client.frame_header.DataLength = LEN_ALL_ID + sizeof(referee_client.client_custom_character);//数据长度
  referee_client.frame_header.Seq = 0;
  //将头帧数据复制到待发送数组中
	memcpy(cliend_tx_buffer, &referee_client.frame_header, LEN_HEADER);
  //添加CRC8到头帧结尾
  Append_CRC8_Check_Sum(cliend_tx_buffer, LEN_HEADER);
  
  //设置CmdID
  referee_client.cmdID = ID_STUDENT_INTERACTIVE_HEADER;
  
  //设置自定义UI所需传输数据
  //设置内容 ID 官方固定,当前绘制字符，所以id为0x0110
  referee_client.student_interactive_header_data.data_cmd_id = ID_CLIENT_CUSTOM_CHARACTER;
  //设置发送者的 ID
  referee_client.student_interactive_header_data.sender_ID = referee_used.robot_id;
  //设置接收者的 ID
  referee_client.student_interactive_header_data.receiver_ID = Get_Receive_Client_Side_ID();
  //设置发送的数据
  //图形的名称；数组里存放的是name变量的每一位
  referee_client.client_custom_character.grapic_data_struct.graphic_name[0] = name%10;
	referee_client.client_custom_character.grapic_data_struct.graphic_name[1] = (name/10)%10;
	referee_client.client_custom_character.grapic_data_struct.graphic_name[2] = (name/100)%10;
  //操作；可以删除，增加等操作
  referee_client.client_custom_character.grapic_data_struct.operate_tpye = operate;
  //图形种类；字符串
  referee_client.client_custom_character.grapic_data_struct.graphic_tpye = CHARCTER;
	//图层数；相当于可以不断覆盖
  referee_client.client_custom_character.grapic_data_struct.layer = layer;
  //图形颜色  
  referee_client.client_custom_character.grapic_data_struct.color = color;
  //字体大小
  referee_client.client_custom_character.grapic_data_struct.start_angle = size;
  //字符长度
  referee_client.client_custom_character.grapic_data_struct.end_angle = length;
  //线条宽度
  referee_client.client_custom_character.grapic_data_struct.width = width;
  //字符串的起始坐标x，y
  referee_client.client_custom_character.grapic_data_struct.start_x = s_x;
  referee_client.client_custom_character.grapic_data_struct.start_y = s_y;
  //字符型数据
  for(int j=0 ; j<30 ; j++)
    referee_client.client_custom_character.data[j] = ' ';
  strcpy((char*)referee_client.client_custom_character.data,data);
  
  //将机器人交互数据复制到待发送数组中
  memcpy(cliend_tx_buffer + LEN_HEADER, (uint8_t *)&referee_client.cmdID, LEN_CMDID + referee_client.frame_header.DataLength);
  //计算数据总长度
  data_length = LEN_HEADER + LEN_CMDID + referee_client.frame_header.DataLength + LEN_TAIL;
  //添加CRC16到数据结尾
  Append_CRC16_Check_Sum(cliend_tx_buffer, data_length);
  
  for(int i=0; i<data_length; i++)
  {
  	HAL_UART_Transmit(&RD_huart, &cliend_tx_buffer[i], 1, 1);  
  }

}
//===================================================================================================================//
/*********************************************** ↑  绘制自定义UI  ↑ **************************************************/
//===================================================================================================================//


//===================================================================================================================//
/******************************************* ↓ 机器人间交互数据发送 ↓ ************************************************/
//===================================================================================================================//
/**
  * @brief				判断机器人间交互数据 接收方ID
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
  * @brief				与其他机器人通信
  * @param[in]		
	* @param[out]		
  * @retval				none
*/
void Robot_Interactive_Date(uint8_t data[], uint16_t robot_interactive_id, uint16_t receive_robot_type)
{
  unsigned char robot_interactive_buffer[LEN_ROBOT_INTERACTIVE_DATE_MAX];//交互数据待发送数组
  static uint8_t robot_interactive_data_length,i;//交互数据长度
  
  //设置头帧数据
  referee_send.frame_header.SOF = FRAME_HEADER_SOF;//头帧
  referee_send.frame_header.DataLength = LEN_ALL_ID + LEN_ROBOT_INTERACTIVE_DATE;//数据长度
  referee_send.frame_header.Seq = 0;
  //将头帧数据复制到待发送数组中
	memcpy(robot_interactive_buffer, &referee_send.frame_header, LEN_HEADER);
  //添加CRC8到头帧结尾
  Append_CRC8_Check_Sum(robot_interactive_buffer, LEN_HEADER);
  //设置CmdID
  referee_send.cmdID = ID_STUDENT_INTERACTIVE_HEADER;
  //设置机器人间交互数据
  //设置内容 ID
  referee_send.student_interactive_header_data.data_cmd_id = robot_interactive_id;
  //设置发送者的 ID
  referee_send.student_interactive_header_data.sender_ID = referee_used.robot_id;
  //设置接收者的 ID
  referee_send.student_interactive_header_data.receiver_ID = Get_Receive_Robot_ID(receive_robot_type);
  //设置发送的数据
  referee_send.robot_interactive_data.data[0] = data[0];//可继续添加
  //将机器人交互数据复制到待发送数组中
  memcpy(robot_interactive_buffer + LEN_HEADER, (uint8_t *)&referee_send.cmdID, LEN_CMDID + referee_send.frame_header.DataLength);
  //计算数据总长度
  robot_interactive_data_length = LEN_HEADER + LEN_CMDID + referee_send.frame_header.DataLength + LEN_TAIL; 
 //添加CRC16到数据结尾
  Append_CRC16_Check_Sum(robot_interactive_buffer, robot_interactive_data_length);

  for(i=0; i<robot_interactive_data_length; i++)
  {
  	HAL_UART_Transmit(&RD_huart, &robot_interactive_buffer[i], 1, 1);  
  }
  
}
//===================================================================================================================//
/******************************************* ↑ 机器人间交互数据发送 ↑ ************************************************/
//===================================================================================================================//

