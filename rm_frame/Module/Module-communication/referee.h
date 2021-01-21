#ifndef __REFEREE_H
#define __REFEREE_H

/* 包含头文件 ---------------------------------------------------------------*/
#include "myinclude.h"

/* 本模块向外部提供的宏定义 -------------------------------------------------*/
#define REFEREE_RX_BUFFER_SIZE  200 //接收裁判系统数据时提供的数组大小

#define    LEN_HEADER    5        //帧头长度
#define    LEN_CMDID     2        //命令码长度
#define    LEN_TAIL      2	      //帧尾CRC16

#define    FRAME_HEADER_SOF         (0xA5)
/* 本模块向外部提供的结构体/枚举定义 ----------------------------------------*/
/* 一帧数据 = frame_header(帧头) + cmd_id(命令码ID) + data(数据) + frame_tail(帧尾) */ 
/***************命令码ID********************/
/* 

	ID: 0x0001  Byte:  3    比赛状态数据       							1Hz周期发送     
	ID: 0x0002  Byte:  1    比赛结果数据         						比赛结束后发送      
	ID: 0x0003  Byte:  32   比赛机器人血量数据   						1Hz周期发送
	ID: 0x0004  Byte:  3    飞镖发射状态   									飞镖发射时发送
	ID: 0x0005  Byte:  3    人工智能挑战赛加成与惩罚区状态  1Hz周期发送
	ID: 0x0101  Byte:  4    场地事件数据   									1Hz周期发送
	ID: 0x0102  Byte:  4    场地补给站动作标识数据    			动作发生后发送 
	ID: 0X0104  Byte:  2    裁判警告数据    							  警告发生后发送 
	ID: 0X0105  Byte:  1    飞镖发射口倒计时    						1Hz周期发送 
	ID: 0X0201  Byte: 18    机器人状态数据        					10Hz周期发送
	ID: 0X0202  Byte: 16    实时功率热量数据   							50Hz周期发送       
	ID: 0x0203  Byte: 16    机器人位置数据           				10Hz周期发送
	ID: 0x0204  Byte:  1    机器人增益数据           				1Hz周期发送
	ID: 0x0205  Byte:  3    空中机器人能量状态数据    			10Hz周期发送，只有空中机器人主控发送
	ID: 0x0206  Byte:  1    伤害状态数据           					伤害发生后发送
	ID: 0x0207  Byte:  6    实时射击数据           					子弹发射后发送
	ID: 0x0208  Byte:  2    弹丸剩余发射数          				仅空中机器人以及哨兵机器人主控发送该数据，1Hz周期发送
	ID: 0x0209  Byte:  4    机器人 RFID 状态          			1Hz周期发送
	ID: 0x020A  Byte: 12    飞镖机器人客户端指令数据        1Hz周期发送
	ID: 0x0301  Byte:  n    机器人间交互数据          			发送方触发发送,10Hz
	
*/
typedef enum
{ 
	ID_GAME_STATE       						= 0x0001,//比赛状态数据
	ID_GAME_RESULT 	   							= 0x0002,//比赛结果数据
	ID_GAME_ROBOT_HP        				= 0x0003,//比赛机器人血量数据
	ID_DART_STATUS									= 0x0004,//飞镖发射状态
	ID_ICRA_BUFF_DEBUFF_ZONE_STATUS = 0x0005,//人工智能挑战赛加成与惩罚区状态
	ID_EVENT_DATE  									= 0x0101,//场地事件数据 
	ID_SUPPLY_PROJRCTILE_ACTION   	= 0x0102,//场地补给站动作标识数据
	ID_REFEREE_WARNING							= 0x0104,//裁判警告数据
	ID_DART_REMAINING_TIME					= 0x0105,//飞镖发射口倒计时
	ID_GAME_ROBOT_STATUS    				= 0x0201,//机器人状态数据
	ID_POWER_HEAT_DATE    					= 0x0202,//实时功率热量数据
	ID_GAME_ROBOT_POS        				= 0x0203,//机器人位置数据
	ID_BUFF													= 0x0204,//机器人增益数据
	ID_AERIAL_ROBOT_ENERGY					= 0x0205,//空中机器人能量状态数据
	ID_ROBOT_HURT										= 0x0206,//伤害状态数据
	ID_SHOOT_DATE										= 0x0207,//实时射击数据
	ID_BULLET_REAMINING							= 0x0208,//弹丸剩余发射数
	ID_RFID_STATUS									= 0x0209,//机器人 RFID 状态
	ID_DART_CLIENT_CMD							= 0x020A,//飞镖机器人客户端指令数据
	ID_INTERATIVE_DATE							= 0x0301,//机器人间交互数据
	
} xCmdID;

//命令码所对应各种数据段的长度,根据官方协议来定义长度
typedef enum
{
	LEN_GAME_STATE       				    =  3,	//0x0001
	LEN_GAME_RESULT       					=  1,	//0x0002
	LEN_GAME_ROBOT_HP				      	= 32,	//0x0003
	LEN_DART_STATUS									=  3, //0x0004
	LEN_ICRA_BUFF_DEBUFF_ZONE_STATUS=  3, //0x0005
	LEN_EVENT_DATE  								=  4,	//0x0101
	LEN_SUPPLY_PROJRCTILE_ACTION    =  4,	//0x0102
	LEN_REFEREE_WARNING							=  2, //0x0104
	LEN_DART_REMAINING_TIME					=  1, //0x0105
	LEN_GAME_ROBOT_STATUS    				= 18,	//0x0201
	LEN_POWER_HEAT_DATE    				  = 16,	//0x0202
	LEN_GAME_ROBOT_POS        			= 16,	//0x0203
	LEN_BUFF											  =  1,	//0x0204
	LEN_AERIAL_ROBOT_ENERGY			 	  =  3,	//0x0205
	LEN_ROBOT_HURT									=  1,	//0x0206
	LEN_SHOOT_DATE								  =  6,	//0x0207
	LEN_BULLET_REAMINING						=  2, //0x0208
	LEN_RFID_STATUS									=  4, //0x0209
	LEN_DART_CLIENT_CMD							= 12, //0x020A
	
} RefereeDataLength;


/* 自定义帧头 */
typedef __packed struct
{
	uint8_t  SOF;
	uint16_t DataLength;
	uint8_t  Seq;
	uint8_t  CRC8;
	
} xFrameHeader;

/* ID: 0x0001  Byte:  3    比赛状态数据 */
typedef __packed struct 
{ 
	uint8_t game_type : 4;
	uint8_t game_progress : 4;
	uint16_t stage_remain_time;
	
} ext_game_state_t; 


/* ID: 0x0002  Byte:  1    比赛结果数据 */
typedef __packed struct 
{ 
	uint8_t winner;
	
} ext_game_result_t; 


/* ID: 0x0003  Byte:  2    比赛机器人血量数据 */
typedef __packed struct
{
	uint16_t red_1_robot_HP;		//红 1 英雄机器人血量，未上场以及罚下血量为 0
	uint16_t red_2_robot_HP;		//红 2 工程机器人血量
	uint16_t red_3_robot_HP;		//红 3 步兵机器人血量
	uint16_t red_4_robot_HP;		//红 4 步兵机器人血量
	uint16_t red_5_robot_HP;		//红 5 步兵机器人血量
	uint16_t red_7_robot_HP;		//红 7 哨兵机器人血量
	uint16_t red_outpost_HP;		//红方前哨站血量
	uint16_t red_base_HP;				//红方基地血量
	uint16_t blue_1_robot_HP;		//蓝 1 英雄机器人血量
	uint16_t blue_2_robot_HP;		//蓝 2 工程机器人血量
	uint16_t blue_3_robot_HP;		//蓝 3 步兵机器人血量
	uint16_t blue_4_robot_HP;		//蓝 4 步兵机器人血量
	uint16_t blue_5_robot_HP;		//蓝 5 步兵机器人血量
	uint16_t blue_7_robot_HP;		//蓝 7 哨兵机器人血量
	uint16_t blue_outpost_HP;		//蓝方前哨站血量
	uint16_t blue_base_HP;			//蓝方基地血量
	
} ext_game_robot_HP_t;


/* ID: 0x0004  Byte:  3    飞镖发射状态数据 */
typedef __packed struct
{
	uint8_t dart_belong;						//发射飞镖的队伍：1：红方飞镖2：蓝方飞镖	
	uint16_t stage_remaining_time;	//发射时的剩余比赛时间，单位 s
	
} ext_dart_status_t;


/* ID: 0x0005  Byte:  3    人工智能挑战赛加成与惩罚区状态数据 */
typedef __packed struct
{
	uint8_t F1_zone_status:1;
	uint8_t F1_zone_buff_debuff_status:3;
	uint8_t F2_zone_status:1;
	uint8_t F2_zone_buff_debuff_status:3;
	uint8_t F3_zone_status:1;
	uint8_t F3_zone_buff_debuff_status:3;
	uint8_t F4_zone_status:1;
	uint8_t F4_zone_buff_debuff_status:3;
	uint8_t F5_zone_status:1;
	uint8_t F5_zone_buff_debuff_status:3;
	uint8_t F6_zone_status:1;
	uint8_t F6_zone_buff_debuff_status:3;
	
} ext_ICRA_buff_debuff_zone_status_t;


/* ID: 0x0101  Byte:  4    场地事件数据 */
typedef __packed struct 
{ 
	uint32_t event_type;
	
} ext_event_data_t; 


/* ID: 0x0102  Byte:  4    补给站动作标识数据 */
typedef __packed struct 
{ 
	uint8_t supply_projectile_id;
	uint8_t supply_robot_id;
	uint8_t supply_projectile_step;
	uint8_t supply_projectile_num;
	
} ext_supply_projectile_action_t;


/* ID: 0x0104  Byte:  2    裁判警告信息数据 */
typedef __packed struct
{
uint8_t level;					//警告等级：
uint8_t foul_robot_id;	//犯规机器人 ID：1 级以及 5 级警告时，机器人 ID 为 0。二三四级警告时，机器人 ID 为犯规机器人 ID
	
} ext_referee_warning_t;


/* ID: 0x0105  Byte:  1    飞镖发射口倒计时数据 */
typedef __packed struct
{
	uint8_t dart_remaining_time;  //15s 倒计时
	
} ext_dart_remaining_time_t;


/* ID: 0X0201  Byte: 18    机器人状态数据 */
typedef __packed struct 
{ 
	uint8_t robot_id;   										//机器人ID，1： 红方英雄机器人；2： 红方工程机器人； 3/4/5： 红方步兵机器人；	
																					//6： 红方空中机器人；7： 红方哨兵机器人；8：红方飞镖机器人；9：红方雷达站。
																					//101： 蓝方英雄机器人；102： 蓝方工程机器人；103/104/105： 蓝方步兵机器人；
																					//106： 蓝方空中机器人；107： 蓝方哨兵机器人；108： 蓝方飞镖机器人；109： 蓝方雷达站。					
	uint8_t robot_level; 										//1：一级，2：二级，3：三级
	uint16_t remain_HP;  										//机器人剩余血量
	uint16_t max_HP; 												//机器人上限血量
	uint16_t shooter_heat0_cooling_rate;  	//机器人 17mm 子弹热量冷却速度 单位 /s
	uint16_t shooter_heat0_cooling_limit;   //机器人 17mm 子弹热量上限
	uint16_t shooter_heat1_cooling_rate;    //机器人 42mm 枪口每秒冷却值 单位 /s
	uint16_t shooter_heat1_cooling_limit;   //机器人 42mm 枪口热量上限
	uint8_t shooter_heat0_speed_limit;			//机器人 17mm 枪口上限速度 单位 m/s
	uint8_t shooter_heat1_speed_limit;			//机器人 42mm 枪口上限速度 单位 m/s
	uint8_t max_chassis_power;							//机器人最大底盘， 单位 瓦/W
	uint8_t mains_power_gimbal_output : 1;  //0 bit： gimbal 口输出：  1 为有 24V 输出， 0 为无 24v 输出
	uint8_t mains_power_chassis_output : 1; //1 bit： chassis 口输出： 1 为有 24V 输出， 0 为无 24v 输出
	uint8_t mains_power_shooter_output : 1; //2 bit： shooter 口输出： 1 为有 24V 输出， 0 为无 24v 输出
	
} ext_game_robot_status_t; 


/* ID: 0X0202  Byte: 16    实时功率热量数据 */
typedef __packed struct 
{ 
	uint16_t chassis_volt;   				//底盘输出电压 单位 毫伏/mV
	uint16_t chassis_current;   		//底盘输出电流 单位 毫安/mA
	float chassis_power;   					//底盘输出功率 单位 瓦/W
	uint16_t chassis_power_buffer;	//底盘功率缓冲 单位 焦耳/J 备注：飞坡根据规则增加至 250J
	uint16_t shooter_heat0;					//17mm 枪口热量
	uint16_t shooter_heat1;					//42mm 枪口热量  
	uint16_t mobile_shooter_heat2;	//机动 17 mm 枪口热量
	
} ext_power_heat_data_t; 


/* ID: 0x0203  Byte: 16    机器人位置数据 */
typedef __packed struct 
{   
	float x;   //位置 x 坐标，单位 米/m
	float y;   //位置 Y 坐标，单位 米/m
	float z;   //位置 Z 坐标，单位 米/m
	float yaw; //位置枪口，单位 度/°
	
} ext_game_robot_pos_t; 


/* ID: 0x0204  Byte:  1    机器人增益数据 */
typedef __packed struct 
{ 
	uint8_t power_rune_buff;	//bit 0： 机器人血量补血状态 bit 1： 枪口热量冷却加速
														//bit 2： 机器人防御加成		 bit 3： 机器人攻击加成
} ext_buff_t; 


/* ID: 0x0205  Byte:  3    空中机器人能量状态数据 */
typedef __packed struct 
{ 
	uint8_t energy_point;		//积累的能量点
	uint8_t attack_time; 		//可攻击时间， 单位 s， 30s 递减至 0
	
} aerial_robot_energy_t; 


/* ID: 0x0206  Byte:  1    伤害状态数据 */
typedef __packed struct 
{ 
	uint8_t armor_id : 4; 
	uint8_t hurt_type : 4; 
} ext_robot_hurt_t; 


/* ID: 0x0207  Byte:  6    实时射击数据 */
typedef __packed struct 
{ 
	uint8_t bullet_type;   //子弹类型: 1： 17mm 弹丸 2： 42mm 弹丸
	uint8_t bullet_freq;   //子弹射频 单位 Hz
	float bullet_speed;  	 //子弹射速 单位 m/s
	
} ext_shoot_data_t; 


/* ID: 0x0208  Byte:  2    子弹剩余发射数 */
typedef __packed struct
{
	uint16_t bullet_remaining_num;	//子弹剩余发射数目
	
} ext_bullet_remaining_t;


/* ID: 0x0209  Byte:  4    机器人 RFID 状态 */
typedef __packed struct
{
	uint32_t rfid_status;
	
} ext_rfid_status_t;


/* ID: 0x020A  Byte: 12    飞镖机器人客户端指令数据 */
typedef __packed struct
{
	uint8_t dart_launch_opening_status;	//当前飞镖发射口的状态0：关闭；1：正在开启或者关闭中；2：已经开启。
	uint8_t dart_attack_target;					//飞镖的打击目标，默认为前哨站；1：前哨站；2：基地。
	uint16_t target_change_time;				//切换打击目标时的比赛剩余时间，单位秒，从未切换默认为 0。
	uint8_t first_dart_speed;						//检测到的第一枚飞镖速度，单位 0.1m/s/LSB, 未检测是为 0。
	uint8_t second_dart_speed;					//检测到的第二枚飞镖速度，单位 0.1m/s/LSB，未检测是为 0。
	uint8_t third_dart_speed;						//检测到的第三枚飞镖速度，单位 0.1m/s/LSB，未检测是为 0。
	uint8_t fourth_dart_speed;					//检测到的第四枚飞镖速度，单位 0.1m/s/LSB，未检测是为 0。
	uint16_t last_dart_launch_time;			//最近一次的发射飞镖的比赛剩余时间，单位秒，初始值为 0。
	uint16_t operate_launch_cmd_time;		//最近一次操作手确定发射指令时的比赛剩余时间，单位秒, 初始值为 0。
	
} ext_dart_client_cmd_t;


/* ID: 0x0301  Byte: 128    机器人间交互数据 */
typedef __packed struct
{
	uint16_t data_cmd_id;
	uint16_t sender_ID;
	uint16_t receiver_ID;
	uint8_t	 date[10];
	
}ext_student_interactive_header_data_t;


/* 裁判系统数据汇总 */
typedef  struct   
{
	//帧头
	xFrameHeader  													frame_header;											
	//命令码ID			
	uint16_t																cmdID;
	//数据段			
	ext_game_state_t 												game_state;                 			//比赛状态数据 
	ext_game_result_t 											game_result;               	 			//比赛结果数据         					
	ext_game_robot_HP_t 										game_robot_HP;             	 			//比赛机器人血量数据   					
	ext_dart_status_t    					  				dart_status;                			//飞镖发射状态   								
	ext_ICRA_buff_debuff_zone_status_t			ICRA_buff_debuff_zone_status;			//人工智能挑战赛加成与惩罚区状态
	ext_event_data_t   											event_data;                 			//场地事件数据   								
	ext_supply_projectile_action_t					supply_projectile_action;  	 			//场地补给站动作标识数据    		
	ext_referee_warning_t										referee_warning;           				//裁判警告数据    							
	ext_dart_remaining_time_t								dart_remaining_time;       				//飞镖发射口倒计时    					
	ext_game_robot_status_t									game_robot_status;         	 			//机器人状态数据        				
	ext_power_heat_data_t										power_heat_data;           	 			//实时功率热量数据   						
	ext_game_robot_pos_t										game_robot_pos;            	 			//机器人位置数据           			
	ext_buff_t															buff;                      	 			//机器人增益数据           			
	aerial_robot_energy_t										aerial_robot_energy;              //空中机器人能量状态数据    		
	ext_robot_hurt_t												robot_hurt;                 			//伤害状态数据           				
	ext_shoot_data_t												shoot_data;                	 			//实时射击数据           				
	ext_bullet_remaining_t									bullet_remaining;          	 			//弹丸剩余发射数          			
	ext_rfid_status_t												RFID_status;               	 			//机器人 RFID 状态          		
	ext_dart_client_cmd_t										dart_client_cmd;           	 			//飞镖机器人客户端指令数据
	ext_student_interactive_header_data_t		student_interactive_header_data;	//机器人间交互数据	
	//帧尾(CRC16校验)
	uint16_t 																frame_tail;
	
}Referee_Struct;


/* 本模块向外部提供的变量声明 -----------------------------------------------*/

/* 本模块向外部提供的自定义数据类型变量声明 ---------------------------------*/
extern Referee_Struct  referee;
/* 本模块向外部提供的接口函数原型声明 ---------------------------------------*/
void RefereeDate_Receive_USART_Init(void);
void Referee_UART_IRQHandler(void);




#endif

