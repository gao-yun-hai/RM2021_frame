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

	CMD_ID: 0x0001  Byte:  11   比赛状态数据       							1Hz周期发送     
	CMD_ID: 0x0002  Byte:  1    比赛结果数据         						比赛结束后发送      
	CMD_ID: 0x0003  Byte:  28   比赛机器人血量数据   						1Hz周期发送
	CMD_ID: 0x0004  Byte:  3    飞镖发射状态   									飞镖发射时发送【2021 V2.1版协议中删除 】
	CMD_ID: 0x0005  Byte:  11   人工智能挑战赛加成与惩罚区状态  1Hz周期发送
	CMD_ID: 0x0101  Byte:  4    场地事件数据   									1Hz周期发送
	CMD_ID: 0x0102  Byte:  3    场地补给站动作标识数据    			动作发生后发送
  CMD_ID：0x0103  Byte:  2    请求补给站补弹数据    			    由参赛队发送，上限 10Hz（RM 对抗赛尚未开放）
	CMD_ID: 0X0104  Byte:  2    裁判警告数据    							  警告发生后发送 
	CMD_ID: 0X0105  Byte:  1    飞镖发射口倒计时    						1Hz周期发送 
	CMD_ID: 0X0201  Byte:  27   机器人状态数据        					10Hz周期发送
	CMD_ID: 0X0202  Byte:  14   实时功率热量数据   							50Hz周期发送       
	CMD_ID: 0x0203  Byte:  16   机器人位置数据           				10Hz周期发送
	CMD_ID: 0x0204  Byte:  1    机器人增益数据           				1Hz周期发送
	CMD_ID: 0x0205  Byte:  3    空中机器人能量状态数据    			10Hz周期发送，只有空中机器人主控发送
	CMD_ID: 0x0206  Byte:  1    伤害状态数据           					伤害发生后发送
	CMD_ID: 0x0207  Byte:  6    实时射击数据           					子弹发射后发送
	CMD_ID: 0x0208  Byte:  2    弹丸剩余发射数          				仅空中机器人以及哨兵机器人主控发送该数据，1Hz周期发送
	CMD_ID: 0x0209  Byte:  4    机器人 RFID 状态          			1Hz周期发送
	CMD_ID: 0x020A  Byte:  6   飞镖机器人客户端指令数据        1Hz周期发送
	CMD_ID: 0x0301  Byte:  n    机器人间交互数据          			发送方触发发送,10Hz
	CMD_ID: 0x0302  Byte:  n    自定义控制器交互数据接口        通过客户端触发发送，上限 30Hz
	CMD_ID: 0x0303  Byte:  15   客户端小地图交互数据          	触发发送
	CMD_ID: 0x0304  Byte:  12   键盘、鼠标信息，          			通过图传串口发送，30Hz
	CMD_ID: 0x0305  Byte:  10   客户端小地图接收信息，          雷达站发送的坐标信息可以被所有己方操作手在第一视角小地图看到。最大接收频率： 10Hz。 
  	
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
	ID_STUDENT_INTERACTIVE_HEADER		= 0x0301,//机器人间交互数据
	ID_CUSTOMCONTROLLER_DATE        = 0x0302,//自定义控制器交互数据接口，通过客户端触发发送，上限30HZ
	ID_MAP_DATE                     = 0x0303,//客户端小地图交互数据
	ID_KEYBOARD_AND_MOUSE_DATE      = 0x0304,//键盘、鼠标信息，通过图传串口发送     
	
} xCmdID;

//命令码所对应各种数据段的长度,根据官方协议来定义长度
typedef enum
{
	LEN_GAME_STATE       				    = 11,	//0x0001
	LEN_GAME_RESULT       					=  1,	//0x0002
	LEN_GAME_ROBOT_HP				      	= 32,	//0x0003
	LEN_DART_STATUS									=  3, //0x0004
	LEN_ICRA_BUFF_DEBUFF_ZONE_STATUS= 11, //0x0005
	LEN_EVENT_DATE  								=  4,	//0x0101
	LEN_SUPPLY_PROJRCTILE_ACTION    =  4,	//0x0102
	LEN_REFEREE_WARNING							=  2, //0x0104
	LEN_DART_REMAINING_TIME					=  1, //0x0105
	LEN_GAME_ROBOT_STATUS    				= 27,	//0x0201
	LEN_POWER_HEAT_DATE    				  = 16,	//0x0202
	LEN_GAME_ROBOT_POS        			= 16,	//0x0203
	LEN_BUFF											  =  1,	//0x0204
	LEN_AERIAL_ROBOT_ENERGY			 	  =  2,	//0x0205
	LEN_ROBOT_HURT									=  1,	//0x0206
	LEN_SHOOT_DATE								  =  7,	//0x0207
	LEN_BULLET_REAMINING						=  6, //0x0208
	LEN_RFID_STATUS									=  4, //0x0209
	LEN_DART_CLIENT_CMD							=  6, //0x020A
  LEN_STUDENT_INTERACTIVE_HEADER  =  1, //0x0301
  LEN_MAP_DATE                    = 15, //0x0303
	LEN_KEYBOARD_AND_MOUSE_DATE     = 12, //0x0304
	
} RefereeDataLength;

//子弹类型
typedef enum
{
  BULLET_17MM = 1,
  BULLET_42MM = 2,
  
} bullet_type_t;

//发射机构ID
typedef enum
{
  SHOOTER1_17MM = 1,
  SHOOTER2_17MM  = 2,
  SHOOTER1_42MM  = 3,
  
} shooter_id_t;


/* 帧头 */
typedef __packed struct
{
	uint8_t  SOF;
	uint16_t DataLength;
	uint8_t  Seq;
	uint8_t  CRC8;
	
} xFrameHeader;

/* CMD_ID: 0x0001  Byte:  11    比赛状态数据 */
typedef __packed struct 
{ 
	uint8_t game_type : 4;
	uint8_t game_progress : 4;
	uint16_t stage_remain_time;
	
  uint64_t SyncTimeStamp;  //机器人接收到该指令的精确 Unix 时间，当机载端收到有效的 NTP 服务器授 时后生效 

} ext_game_state_t; 


/* CMD_ID: 0x0002  Byte:  1    比赛结果数据 */
typedef __packed struct 
{ 
	uint8_t winner;
	
} ext_game_result_t; 


/* CMD_ID: 0x0003  Byte:  32    比赛机器人血量数据 */
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


/* CMD_ID: 0x0004  Byte:  3    飞镖发射状态数据 */
typedef __packed struct
{
	uint8_t dart_belong;						//发射飞镖的队伍：1：红方飞镖2：蓝方飞镖	
	uint16_t stage_remaining_time;	//发射时的剩余比赛时间，单位 s
	
} ext_dart_status_t;


/* CMD_ID: 0x0005  Byte:  11    人工智能挑战赛加成与惩罚区状态数据 */
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
  
  uint16_t red1_bullet_left;
  uint16_t red2_bullet_left;
  uint16_t blue1_bullet_left;
  uint16_t blue2_bullet_left;
	
} ext_ICRA_buff_debuff_zone_status_t;


/* CMD_ID: 0x0101  Byte:  4    场地事件数据 */
typedef __packed struct 
{ 
	uint32_t event_type;
	
} ext_event_data_t; 


/* CMD_ID: 0x0102  Byte:  4    补给站动作标识数据 */
typedef __packed struct 
{ 
	uint8_t supply_projectile_id;
	uint8_t supply_robot_id;
	uint8_t supply_projectile_step;
	uint8_t supply_projectile_num;
	
} ext_supply_projectile_action_t;


/* CMD_ID: 0x0104  Byte:  2    裁判警告信息数据 */
typedef __packed struct
{
uint8_t level;					//警告等级：
uint8_t foul_robot_id;	//犯规机器人 ID：1 级以及 5 级警告时，机器人 ID 为 0。二三四级警告时，机器人 ID 为犯规机器人 ID
	
} ext_referee_warning_t;


/* CMD_ID: 0x0105  Byte:  1    飞镖发射口倒计时数据 */
typedef __packed struct
{
	uint8_t dart_remaining_time;  //15s 倒计时
	
} ext_dart_remaining_time_t;


/* CMD_ID: 0X0201  Byte: 27    机器人状态数据 */
typedef __packed struct 
{ 
	uint8_t robot_id;   										//机器人ID，1： 红方英雄机器人；2： 红方工程机器人； 3/4/5： 红方步兵机器人；	
																					//6： 红方空中机器人；7： 红方哨兵机器人；8：红方飞镖机器人；9：红方雷达站。
																					//101： 蓝方英雄机器人；102： 蓝方工程机器人；103/104/105： 蓝方步兵机器人；
																					//106： 蓝方空中机器人；107： 蓝方哨兵机器人；108： 蓝方飞镖机器人；109： 蓝方雷达站。					
	uint8_t robot_level; 										//1：一级，2：二级，3：三级
	uint16_t remain_HP;  										//机器人剩余血量
	uint16_t max_HP; 												//机器人上限血量
  
  uint16_t shooter_id1_17mm_cooling_rate; //机器人 1 号 17mm 枪口每秒冷却值
  uint16_t shooter_id1_17mm_cooling_limit;//机器人 1 号 17mm 枪口热量上限
  uint16_t shooter_id1_17mm_speed_limit;  //机器人 1 号 17mm 枪口上限速度 单位 m/s
  
  uint16_t shooter_id2_17mm_cooling_rate; //机器人 2 号 17mm 枪口每秒冷却值
  uint16_t shooter_id2_17mm_cooling_limit;//机器人 2 号 17mm 枪口热量上限
  uint16_t shooter_id2_17mm_speed_limit;  //机器人 2 号 17mm 枪口上限速度 单位 m/s
  
  uint16_t shooter_id1_42mm_cooling_rate; //机器人 42mm 枪口每秒冷却值
  uint16_t shooter_id1_42mm_cooling_limit;//机器人 42mm 枪口热量上限
  uint16_t shooter_id1_42mm_speed_limit;  //机器人 42mm 枪口上限速度 单位 m/s
  
	uint16_t chassis_power_limit;						//机器人底盘功率限制上限， 单位 瓦/W
  
	uint8_t mains_power_gimbal_output : 1;  //0 bit： gimbal 口输出：  1 为有 24V 输出， 0 为无 24v 输出
	uint8_t mains_power_chassis_output : 1; //1 bit： chassis 口输出： 1 为有 24V 输出， 0 为无 24v 输出
	uint8_t mains_power_shooter_output : 1; //2 bit： shooter 口输出： 1 为有 24V 输出， 0 为无 24v 输出
	
} ext_game_robot_status_t; 


/* CMD_ID: 0X0202  Byte: 16    实时功率热量数据 */
typedef __packed struct 
{ 
	uint16_t chassis_volt;   				        //底盘输出电压 单位 毫伏/mV
	uint16_t chassis_current;   		        //底盘输出电流 单位 毫安/mA
	float chassis_power;   					        //底盘输出功率 单位 瓦/W
	uint16_t chassis_power_buffer;	        //底盘功率缓冲 单位 焦耳/J 备注：飞坡根据规则增加至 250J
	uint16_t shooter_id1_17mm_cooling_heat;	//1 号 17mm 枪口热量
	uint16_t shooter_id2_17mm_cooling_heat;	//2 号 17mm 枪口热量
  uint16_t shooter_id1_42mm_cooling_heat; //42mm 枪口热量
	
} ext_power_heat_data_t; 


/* CMD_ID: 0x0203  Byte: 16    机器人位置数据 */
typedef __packed struct 
{   
	float x;   //位置 x 坐标，单位 米/m
	float y;   //位置 Y 坐标，单位 米/m
	float z;   //位置 Z 坐标，单位 米/m
	float yaw; //位置枪口，单位 度/°
	
} ext_game_robot_pos_t; 


/* CMD_ID: 0x0204  Byte:  1    机器人增益数据 */
typedef __packed struct 
{ 
	uint8_t power_rune_buff;	//bit 0： 机器人血量补血状态 bit 1： 枪口热量冷却加速
														//bit 2： 机器人防御加成		 bit 3： 机器人攻击加成
} ext_buff_t; 


/* CMD_ID: 0x0205  Byte:  2    空中机器人能量状态数据 */
typedef __packed struct 
{ 
	uint8_t attack_time; 		//可攻击时间， 单位 s， 30s 递减至 0
  uint8_t energy_point;		//积累的能量点
	
} aerial_robot_energy_t; 


/* CMD_ID: 0x0206  Byte:  1    伤害状态数据 */
typedef __packed struct 
{ 
	uint8_t armor_id : 4; 
	uint8_t hurt_type : 4; 
} ext_robot_hurt_t; 


/* CMD_ID: 0x0207  Byte:  7    实时射击数据 */
typedef __packed struct 
{ 
	uint8_t bullet_type;   //子弹类型: 1： 17mm 弹丸 2： 42mm 弹丸
  uint8_t shooter_id;    //发射机构 ID：1： 1 号 17mm 发射机构
                                      //2： 2 号 17mm 发射机构
                                      //3： 42mm 发射机构
	uint8_t bullet_freq;   //子弹射频 单位 Hz
	float bullet_speed;  	 //子弹射速 单位 m/s
	
} ext_shoot_data_t; 


/* CMD_ID: 0x0208  Byte:  6    子弹剩余发射数 */
typedef __packed struct
{
  uint16_t bullet_remaining_num_17mm;   //17mm 子弹剩余发射数目
  uint16_t bullet_remaining_num_42mm;   //42mm 子弹剩余发射数目
  uint16_t coin_remaining_num;	        //剩余金币数量
  
} ext_bullet_remaining_t;


/* CMD_ID: 0x0209  Byte:  4    机器人 RFID 状态 */
typedef __packed struct
{
	uint32_t rfid_status;
	
} ext_rfid_status_t;


/* CMD_ID: 0x020A  Byte: 6    飞镖机器人客户端指令数据 */
typedef __packed struct
{
	uint8_t dart_launch_opening_status;	//当前飞镖发射口的状态0：关闭；1：正在开启或者关闭中；2：已经开启。
	uint8_t dart_attack_target;					//飞镖的打击目标，默认为前哨站；1：前哨站；2：基地。
	uint16_t target_change_time;				//切换打击目标时的比赛剩余时间，单位秒，从未切换默认为 0。
	uint16_t operate_launch_cmd_time;		//最近一次操作手确定发射指令时的比赛剩余时间，单位秒, 初始值为 0。
	
} ext_dart_client_cmd_t;

//===================================================================================================================//
/***************************  ↓   CMD_ID: 0x0301  Byte: 128    机器人间交互数据  ↓  **********************************/
//===================================================================================================================//
#define  LEN_ALL_ID  6        //总ID长度：内容ID长度（2） + 发送者的ID长度（2） + 接收者的ID长度（2）

/* 一帧数据 = frame_header(帧头) + cmd_id(命令码ID：0x0301) + 机器人间交互数据 + frame_tail(帧尾)*/ 
/* 机器人间交互数据 = data_cmd_id(内容 ID) + sender_ID(发送者的 ID) + receiver_ID(接收者的 ID) + data(数据段) */

/***************内容ID********************/
/* 
	ID: 0x0200~0x02FF  Byte:  6+n    己防机器人间通信 
	ID: 0x0100         Byte:  6+2    客户端删除图形 
	ID: 0x0101         Byte:  6+15   客户端绘制一个图形 
	ID: 0x0102         Byte:  6+30   客户端绘制二个图形 
	ID: 0x0103         Byte:  6+75   客户端绘制五个图形 
	ID: 0x0104         Byte:  6+105  客户端绘制七个图形 
	ID: 0x0110         Byte:  6+45   客户端绘制字符图形  
*/

/***************机器人ID********************/
/*
  1  ，英雄(红)； 2  ，工程(红)； 3  /4  /5  ，步兵(红)； 6  ，空中(红)； 7  ，哨兵(红)； 9  ，雷达站（红）；
  101，英雄(蓝)； 102，工程(蓝)； 103/104/105，步兵(蓝)； 106，空中(蓝)； 107，哨兵(蓝)； 109，雷达站（蓝）。
*/

/***************客户端ID********************/
/*
  0x0101，英雄操作手客户端(红)；0x0102，工程操作手客户端(红)；0x0103/0x0104/0x0105，步兵操作手客户端(红)； 0x0106，空中操作手客户端(红)； 
  0x0165，英雄操作手客户端(蓝)；0x0166，工程操作手客户端(蓝)；0x0167/0x0168/0x0169，步兵操作手客户端(蓝)； 0x016A，空中操作手客户端(蓝)。
*/

typedef enum
{   
  ID_CLIENT_CUSTOM_GRAPHIC_DELETE = 0x0100,//客户端删除图形
  ID_CLIENT_CUSTOM_GRAPHIC_SINGLE = 0x0101,//客户端绘制一个图形
  ID_CLIENT_CUSTOM_GRAPHIC_DOUBLE = 0x0102,//客户端绘制二个图形
  ID_CLIENT_CUSTOM_GRAPHIC_FIVE   = 0x0103,//客户端绘制五个图形 
  ID_CLIENT_CUSTOM_GRAPHIC_SEVEN  = 0x0104,//客户端绘制七个图形
  ID_CLIENT_CUSTOM_CHARACTER      = 0x0110,//客户端绘制字符
  ID_AERIAL_ROBOT_TO_SENTRY_ROBOT = 0x0201,//空中机器人发送数据给哨兵机器人
  ID_AERIAL_ROBOT_TO_DART_ROBOT   = 0x0202,//空中机器人发送数据给飞镖机器人
  //后续根据实际使用情况自行增加（0x0200~0x02FF）  
} xdateCmdID;//内容 ID

typedef enum
{ 
  LEN_AERIAL_ROBOT_TO_SENTRY_ROBOT = 1,  //空中机器人发送给哨兵数据长度
  LEN_AERIAL_ROBOT_TO_DART_ROBOT   = 1,  //空中机器人发送给飞镖数据长度
  LEN_ROBOT_INTERACTIVE_DATE = 10,       //机器人间数据交互数据总设定长度
  LEN_ROBOT_INTERACTIVE_DATE_MAX  = 113, //机器人间数据交互数据最大长度
  LEN_SEND_CLIENT_SIDE_DATE_MAX   = 105, //机器人发送给客户端数据最大长度
  LEN_SEND_CLIENT_SIDE_BUFFER     = 15,  //机器人发送给客户端图形数据的长度（根据协议确定）
  LEN_SEND_CLIENT_SIDE_BUFFER_NUM = 2,   //机器人发送给客户端数据设定数组个数（自行根据需要画的图形数量确定）
  
} xrobot_interactive_data;//内容长度  实际长度自行调


typedef enum
{ 
  HERO_ROBOT      = 1,
  ENGINEER_ROBOT  = 2,
  INFATRY3_ROBOT  = 3,
  INFATRY4_ROBOT  = 4,
  INFATRY5_ROBOT  = 5,
  AERIAL_ROBOT    = 6,
  SENTRY_ROBOT    = 7,
  RADAR_ROBOT     = 9, 
  
} xrobotType;//机器人ID


typedef enum
{ 
  ID_HERO_ROBOT_RED      = 1,
  ID_ENGINEER_ROBOT_RED  = 2,
  ID_INFATRY3_ROBOT_RED  = 3,
  ID_INFATRY4_ROBOT_RED  = 4,
  ID_INFATRY5_ROBOT_RED  = 5,
  ID_AERIAL_ROBOT_RED    = 6,
  ID_SENTRY_ROBOT_RED    = 7,
  ID_RADAR_ROBOT_RED     = 9,
  ID_HERO_ROBOT_BLUE     = 101,
  ID_ENGINEER_ROBOT_BLUE = 102,
  ID_INFATRY3_ROBOT_BLUE = 103,
  ID_INFATRY4_ROBOT_BLUE = 104,
  ID_INFATRY5_ROBOT_BLUE = 105,
  ID_AERIAL_ROBOT_BLUE   = 106,
  ID_SENTRY_ROBOT_BLUE   = 107,
  ID_RADAR_ROBOT_BLUE    = 109,  
  
} xrobotID;//机器人ID



typedef enum
{ 
  ID_HERO_CLIENT_RED      = 0x101,
  ID_ENGINEER_CLIENT_RED  = 0x102,
  ID_INFATRY3_CLIENT_RED  = 0x103,
  ID_INFATRY4_CLIENT_RED  = 0x104,
  ID_INFATRY5_CLIENT_RED  = 0x105,
  ID_AERIAL_CLIENT_RED    = 0x106,
  ID_HERO_CLIENT_BLUE     = 0x165,
  ID_ENGINEER_CLIENT_BLUE = 0x166,
  ID_INFATRY3_CLIENT_BLUE = 0x167,
  ID_INFATRY4_CLIENT_BLUE = 0x168,
  ID_INFATRY5_CLIENT_BLUE = 0x169,
  ID_AERIAL_CLIENT_BLUE   = 0x16A,

} xclientID;//客户端ID

typedef enum
{
	NULL_OPERATION =0,//空操作
	ADD  =1,          //增加
	MODIFY  =2,       //修改
	DELETE  =3,       //删除
}Operate;

typedef enum
{
	RED_AND_BULE  =0,  //红蓝主色
	YELLOW =1,         //黄色
	GREEN  =2,         //绿色
	ORANGE =3,         //橙色
	PURPLISH_RED =4,   //紫红色
	PINK  =5,          //粉色
	CYAN  =6,          //青色
	BLACK =7,          //黑色
	WRITE =8,          //白色
}Color;

typedef enum
{
  STRAIGHT_LINE =0,  //直线
	RECTANGLE     =1,  //矩形
	FULL_CIRCLE   =2,  //整圆
	ELLIPSE       =3,  //椭圆
	ARC           =4,  //圆弧
	FLOATING_POINT=5,  //浮点数
  INTEGER       =6,  //整型数 
	CHARCTER      =7,  //字符
}Type;


//机器人间交互数据帧头
typedef __packed struct
{
	uint16_t data_cmd_id; //数据段的内容 ID
	uint16_t sender_ID;   //发送者的 ID
	uint16_t receiver_ID; //接收者的 ID

}ext_student_interactive_header_data_t;

/****************************************   ↓  机器人间交互数据  ↓  ************************************/
/* 内容段ID: 0x0200~0x02FF  Byte: 6+n(n < 113)  交互数据 */
typedef __packed struct
{
   uint8_t data[LEN_ROBOT_INTERACTIVE_DATE];    //内容数据段，最大113个字节，根据实际需求进行设定
} robot_interactive_data_t;

typedef __packed struct
{
  uint8_t sentry_mode;
}ext_aerial_to_sentry_data_t;//空中机器人发送给哨兵数据

typedef __packed struct
{
  uint8_t launch_num;
}ext_aerial_to_dart_data_t;//空中机器人发送给飞镖数据
/****************************************   ↑  机器人间交互数据  ↑  ************************************/

/****************************************   ↓  客户端上图形绘制  ↓  ************************************/
/* 内容段ID: 0x0100  Byte: 6+2  客户端删除图形 */
typedef __packed struct
{
  uint8_t operate_tpye; //图形操作  0：空操作；1：删除图层；2：删除所有；
  uint8_t layer;        //图层数    图层数：0~9
} ext_client_custom_graphic_delete_t;

//图形数据：图形绘制时所需参数，具体参考RM2021裁判系统串口协议附录V1.2
typedef __packed struct
{
	uint8_t graphic_name[3]; //在删除，修改等操作中，作为客户端的索引 
	uint32_t operate_tpye:3; //图形操作：0：空操作；1：增加；2：修改；3：删除；
	uint32_t graphic_tpye:3; //图形类型：0：直线；1：矩形；2：整圆；3：椭圆；4：圆弧；5：浮点数；6：整型数；7：字符；
	uint32_t layer:4;        //图层数， 0~9
	uint32_t color:4;        //颜色：0：红蓝主色；1：黄色；2：绿色；3：橙色；4：紫红色；5：粉色；6：青色；7：黑色；8：白色；
	uint32_t start_angle:9;  //起始角度，单位： °，范围[0,360]；
	uint32_t end_angle:9;    //终止角度，单位： °，范围[0,360]。
	uint32_t width:10;       //线宽；
	uint32_t start_x:11;     //起点 x 坐标；
	uint32_t start_y:11;     //起点 y 坐标
	uint32_t radius:10;      //字体大小或者半径；
	uint32_t end_x:11;       //终点 x 坐标；
	uint32_t end_y:11;       //终点 y 坐标；
}graphic_data_struct_t;
 
/* 内容段ID: 0x0101  Byte: 6+15  客户端绘制一个图形 */
typedef __packed struct
{
  graphic_data_struct_t grapic_data_struct;//图形配置  详见图形数据介绍
} ext_client_custom_graphic_single_t;

/* 内容段ID: 0x0102  Byte: 6+30  客户端绘制二个图形 */
typedef __packed struct
{
  graphic_data_struct_t grapic_data_struct[2];//图形配置  详见图形数据介绍
} ext_client_custom_graphic_double_t;

/* 内容段ID: 0x0103  Byte: 6+75  客户端绘制五个图形 */
typedef __packed struct
{
graphic_data_struct_t grapic_data_struct[5];//图形配置  详见图形数据介绍
} ext_client_custom_graphic_five_t;

/* 内容段ID: 0x0104  Byte: 6+105  客户端绘制七个图形 */
typedef __packed struct
{
  graphic_data_struct_t grapic_data_struct[7];//图形配置  详见图形数据介绍
} ext_client_custom_graphic_seven_t;

/* 内容段ID: 0x0110  Byte: 6+45  客户端绘制字符 */
typedef __packed struct
{
  graphic_data_struct_t grapic_data_struct;//字符配置  详见图形数据介绍
  uint8_t data[30];                        //字符
} ext_client_custom_character_t;
/****************************************   ↑  客户端上图形绘制  ↑  ************************************/
//===================================================================================================================//
/***************************  ↑   CMD_ID: 0x0301  Byte: 128    机器人间交互数据  ↑  **********************************/
//===================================================================================================================//


/* CMD_ID: 0x0302  Byte: x(x <= 30)   自定义控制器数据 */
typedef __packed struct
{
  uint8_t data[20];//内容段数据,小于30字节
} robot_interactive_custom_data_t;


/* CMD_ID: 0x0303  Byte: 15   客户端小地图下发信息 */
typedef __packed struct 
	{ 
	float target_position_x;  //目标 x 位置坐标，单位 m
	float target_position_y;  //目标 y 位置坐标，单位 m
	float target_position_z;  //目标 z 位置坐标，单位 m
	uint8_t commd_keyboard;   //发送指令时，云台手按下的键盘信息
	uint16_t target_robot_ID; //要作用的目标机器人 ID
} ext_robot_command_t_map;    


/* CMD_ID: 0x0305  Byte: 10   客户端接收信息 *///雷达站发送的坐标信息可以被所有己方操作手在第一视角小地图看到
typedef __packed struct
{ 
  uint16_t target_robot_ID;//目标机器人 ID
  float target_position_x; //目标 x 位置坐标，单位 m
  float target_position_y; //目标 y 位置坐标，单位 m
} ext_client_map_command_t;


/* CMD_ID: 0x0304  Byte: 12   图传遥控信息 *///客户端通过图传模块将键盘信息发送至裁判系统
typedef __packed struct 
{ 
	int16_t mouse_x;           //鼠标 X 轴信息 
	int16_t mouse_y;           //鼠标 Y 轴信息
	int16_t mouse_z;           //鼠标滚轮信息 
	int8_t left_button_down;   //鼠标左键按下
	int8_t right_button_down;  //鼠标右键按下
	uint16_t keyboard_value;   //键盘各按键是否按下
	uint16_t reserved;         //保留位
}ext_robot_command_t_keyboard;  


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

  ext_client_custom_graphic_delete_t      client_custom_graphic_delete;     //客户端删除图形
  /*注意：根据协议，可以有7个图形，故最多可以定义为custom_data[7]，因此自定义图形数据内容的数量可以根据
    需求更改，并且数据的cmd_ID需要根据图形的数量更改（具体内容详见协议），比如当前绘制2个图形，数据的内
    容ID为0x0102 但是经过测试后得出*/
  /*如果在一个数组中的图形名称不同，则可以出现新的图形，之前在同一个数组中的图形并不会消失，因此一个数组
    就可以绘制多个不同的图形，没必要设置7个数组*/
  graphic_data_struct_t                   graphic_data_struct[LEN_SEND_CLIENT_SIDE_BUFFER_NUM]; //自定义图形数据内容          
  ext_client_custom_character_t           client_custom_character;//自定义字符串数据内容
  
	ext_student_interactive_header_data_t		student_interactive_header_data;	//机器人间交互数据 ID段
  robot_interactive_data_t	              robot_interactive_data;           //机器人间交互数据 数据段
  ext_aerial_to_sentry_data_t             aerial_to_sentry_data;            //无人机与哨兵交互数据
  ext_aerial_to_dart_data_t               aerial_to_dart_data;              //无人机与飞镖交互数据
  
  robot_interactive_custom_data_t         robot_interactive_custom_data;    //自定义控制器数据
	//帧尾(CRC16校验)
	uint16_t 																frame_tail;
	
}Referee_Struct;

/* 使用到的裁判系统数据汇总(可根据实际需求进行添加) */
typedef  struct   
{
  uint8_t robot_id;               //机器人ID
  
  uint16_t robot_remain_hp;       //机器人剩余血量
  
  fp32 chassis_power;             //底盘实时功率
  uint16_t chassis_power_buffer;  //底盘缓冲能量
  uint16_t chassis_power_limit;   //底盘功率上限
  
  uint16_t shooter1_17mm_heat;        //17mm 发射机构1 枪口热量
  uint16_t shooter2_17mm_heat;        //17mm 发射机构2 枪口热量
  uint16_t shooter1_42mm_heat;        //42mm 发射机构1 枪口热量

  uint16_t shooter1_17mm_freq;        //17mm 发射机构1 射频
  uint16_t shooter2_17mm_freq;        //17mm 发射机构2 射频
  uint16_t shooter1_42mm_freq;        //42mm 发射机构1 射频

  uint16_t shooter1_17mm_speed;       //17mm 发射机构1 射速
  uint16_t shooter2_17mm_speed;       //17mm 发射机构2 射速
  uint16_t shooter1_42mm_speed;       //42mm 发射机构1 射速
  
  uint16_t shooter1_17mm_heat_limit;  //17mm 发射机构1 枪口热量上限
  uint16_t shooter2_17mm_heat_limit;  //17mm 发射机构2 枪口热量上限
  uint16_t shooter1_42mm_heat_limit;  //42mm 发射机构1 枪口热量上限
  
  uint16_t shooter1_17mm_speed_limit; //17mm 发射机构1 枪口射速上限
  uint16_t shooter2_17mm_speed_limit; //17mm 发射机构2 枪口射速上限
  uint16_t shooter1_42mm_speed_limit; //42mm 发射机构1 枪口射速上限
  
  uint16_t bullet_17mm_remaining_num; //17mm 子弹剩余发射数目  对抗赛仅空中机器人与哨兵有用
  uint16_t coin_remaining_num;        //剩余金币数量
  
  uint8_t gimbal_output;                //云台口输出
  uint8_t chassis_output;               //底盘口输出
  uint8_t shooter_output;               //发射口输出
  

}Referee_Used_Struct;
/* 本模块向外部提供的变量声明 -----------------------------------------------*/

/* 本模块向外部提供的自定义数据类型变量声明 ---------------------------------*/
extern Referee_Used_Struct referee_used;

/* 本模块向外部提供的接口函数原型声明 ---------------------------------------*/
void RefereeDate_Receive_USART_Init(void);
void Referee_UART_IRQHandler(void);

void Robot_Interactive_Date(uint8_t data[], uint16_t robot_interactive_id, uint16_t receive_robot_type);

void Draw_StraightLine_or_Rectangle_or_Ellipse(uint8_t num, uint8_t name, uint8_t operate, uint8_t type, 
     uint8_t layer, uint8_t color, uint8_t width , uint16_t s_x, uint16_t s_y, uint16_t e_x, uint16_t e_y);
void Draw_FullCircle(uint8_t num, uint16_t name, uint8_t operate, uint8_t layer,uint8_t color, uint8_t width, 
     uint16_t s_x, uint16_t s_y, uint16_t radius);
void Draw_ARC(uint8_t num, uint16_t name, uint8_t operate, uint8_t layer, uint8_t color, uint16_t start_angle, 
     uint16_t end_angle, uint8_t width, uint16_t s_x, uint16_t s_y, uint16_t e_x, uint16_t e_y);
void Draw_Integral(uint8_t num, uint16_t name, uint8_t operate, uint8_t layer, uint8_t color, uint16_t size,
	   uint8_t width, uint16_t s_x, uint16_t s_y, int32_t data);
void DrawCharacer(uint16_t name, uint8_t operate, uint8_t layer, uint8_t color,uint8_t size, uint8_t length, 
     uint8_t width, uint16_t s_x, uint16_t s_y, char data[30]);



#endif

