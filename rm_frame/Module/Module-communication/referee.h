#ifndef __REFEREE_H
#define __REFEREE_H

/* ����ͷ�ļ� ---------------------------------------------------------------*/
#include "myinclude.h"

/* ��ģ�����ⲿ�ṩ�ĺ궨�� -------------------------------------------------*/
#define REFEREE_RX_BUFFER_SIZE  200 //���ղ���ϵͳ����ʱ�ṩ�������С

#define    LEN_HEADER    5        //֡ͷ����
#define    LEN_CMDID     2        //�����볤��
#define    LEN_TAIL      2	      //֡βCRC16

#define    FRAME_HEADER_SOF         (0xA5)
/* ��ģ�����ⲿ�ṩ�Ľṹ��/ö�ٶ��� ----------------------------------------*/
/* һ֡���� = frame_header(֡ͷ) + cmd_id(������ID) + data(����) + frame_tail(֡β) */ 
/***************������ID********************/
/* 

	CMD_ID: 0x0001  Byte:  11   ����״̬����       							1Hz���ڷ���     
	CMD_ID: 0x0002  Byte:  1    �����������         						������������      
	CMD_ID: 0x0003  Byte:  28   ����������Ѫ������   						1Hz���ڷ���
	CMD_ID: 0x0004  Byte:  3    ���ڷ���״̬   									���ڷ���ʱ���͡�2021 V2.1��Э����ɾ�� ��
	CMD_ID: 0x0005  Byte:  11   �˹�������ս���ӳ���ͷ���״̬  1Hz���ڷ���
	CMD_ID: 0x0101  Byte:  4    �����¼�����   									1Hz���ڷ���
	CMD_ID: 0x0102  Byte:  3    ���ز���վ������ʶ����    			������������
  CMD_ID��0x0103  Byte:  2    ���󲹸�վ��������    			    �ɲ����ӷ��ͣ����� 10Hz��RM �Կ�����δ���ţ�
	CMD_ID: 0X0104  Byte:  2    ���о�������    							  ���淢������ 
	CMD_ID: 0X0105  Byte:  1    ���ڷ���ڵ���ʱ    						1Hz���ڷ��� 
	CMD_ID: 0X0201  Byte:  27   ������״̬����        					10Hz���ڷ���
	CMD_ID: 0X0202  Byte:  14   ʵʱ������������   							50Hz���ڷ���       
	CMD_ID: 0x0203  Byte:  16   ������λ������           				10Hz���ڷ���
	CMD_ID: 0x0204  Byte:  1    ��������������           				1Hz���ڷ���
	CMD_ID: 0x0205  Byte:  3    ���л���������״̬����    			10Hz���ڷ��ͣ�ֻ�п��л��������ط���
	CMD_ID: 0x0206  Byte:  1    �˺�״̬����           					�˺���������
	CMD_ID: 0x0207  Byte:  6    ʵʱ�������           					�ӵ��������
	CMD_ID: 0x0208  Byte:  2    ����ʣ�෢����          				�����л������Լ��ڱ����������ط��͸����ݣ�1Hz���ڷ���
	CMD_ID: 0x0209  Byte:  4    ������ RFID ״̬          			1Hz���ڷ���
	CMD_ID: 0x020A  Byte:  6   ���ڻ����˿ͻ���ָ������        1Hz���ڷ���
	CMD_ID: 0x0301  Byte:  n    �����˼佻������          			���ͷ���������,10Hz
	CMD_ID: 0x0302  Byte:  n    �Զ���������������ݽӿ�        ͨ���ͻ��˴������ͣ����� 30Hz
	CMD_ID: 0x0303  Byte:  15   �ͻ���С��ͼ��������          	��������
	CMD_ID: 0x0304  Byte:  12   ���̡������Ϣ��          			ͨ��ͼ�����ڷ��ͣ�30Hz
	CMD_ID: 0x0305  Byte:  10   �ͻ���С��ͼ������Ϣ��          �״�վ���͵�������Ϣ���Ա����м����������ڵ�һ�ӽ�С��ͼ������������Ƶ�ʣ� 10Hz�� 
  	
*/
typedef enum
{ 
	ID_GAME_STATE       						= 0x0001,//����״̬����
	ID_GAME_RESULT 	   							= 0x0002,//�����������
	ID_GAME_ROBOT_HP        				= 0x0003,//����������Ѫ������
	ID_DART_STATUS									= 0x0004,//���ڷ���״̬
	ID_ICRA_BUFF_DEBUFF_ZONE_STATUS = 0x0005,//�˹�������ս���ӳ���ͷ���״̬
	ID_EVENT_DATE  									= 0x0101,//�����¼����� 
	ID_SUPPLY_PROJRCTILE_ACTION   	= 0x0102,//���ز���վ������ʶ����
	ID_REFEREE_WARNING							= 0x0104,//���о�������
	ID_DART_REMAINING_TIME					= 0x0105,//���ڷ���ڵ���ʱ
	ID_GAME_ROBOT_STATUS    				= 0x0201,//������״̬����
	ID_POWER_HEAT_DATE    					= 0x0202,//ʵʱ������������
	ID_GAME_ROBOT_POS        				= 0x0203,//������λ������
	ID_BUFF													= 0x0204,//��������������
	ID_AERIAL_ROBOT_ENERGY					= 0x0205,//���л���������״̬����
	ID_ROBOT_HURT										= 0x0206,//�˺�״̬����
	ID_SHOOT_DATE										= 0x0207,//ʵʱ�������
	ID_BULLET_REAMINING							= 0x0208,//����ʣ�෢����
	ID_RFID_STATUS									= 0x0209,//������ RFID ״̬
	ID_DART_CLIENT_CMD							= 0x020A,//���ڻ����˿ͻ���ָ������
	ID_STUDENT_INTERACTIVE_HEADER		= 0x0301,//�����˼佻������
	ID_CUSTOMCONTROLLER_DATE        = 0x0302,//�Զ���������������ݽӿڣ�ͨ���ͻ��˴������ͣ�����30HZ
	ID_MAP_DATE                     = 0x0303,//�ͻ���С��ͼ��������
	ID_KEYBOARD_AND_MOUSE_DATE      = 0x0304,//���̡������Ϣ��ͨ��ͼ�����ڷ���     
	
} xCmdID;

//����������Ӧ�������ݶεĳ���,���ݹٷ�Э�������峤��
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

//�ӵ�����
typedef enum
{
  BULLET_17MM = 1,
  BULLET_42MM = 2,
  
} bullet_type_t;

//�������ID
typedef enum
{
  SHOOTER1_17MM = 1,
  SHOOTER2_17MM  = 2,
  SHOOTER1_42MM  = 3,
  
} shooter_id_t;


/* ֡ͷ */
typedef __packed struct
{
	uint8_t  SOF;
	uint16_t DataLength;
	uint8_t  Seq;
	uint8_t  CRC8;
	
} xFrameHeader;

/* CMD_ID: 0x0001  Byte:  11    ����״̬���� */
typedef __packed struct 
{ 
	uint8_t game_type : 4;
	uint8_t game_progress : 4;
	uint16_t stage_remain_time;
	
  uint64_t SyncTimeStamp;  //�����˽��յ���ָ��ľ�ȷ Unix ʱ�䣬�����ض��յ���Ч�� NTP �������� ʱ����Ч 

} ext_game_state_t; 


/* CMD_ID: 0x0002  Byte:  1    ����������� */
typedef __packed struct 
{ 
	uint8_t winner;
	
} ext_game_result_t; 


/* CMD_ID: 0x0003  Byte:  32    ����������Ѫ������ */
typedef __packed struct
{
	uint16_t red_1_robot_HP;		//�� 1 Ӣ�ۻ�����Ѫ����δ�ϳ��Լ�����Ѫ��Ϊ 0
	uint16_t red_2_robot_HP;		//�� 2 ���̻�����Ѫ��
	uint16_t red_3_robot_HP;		//�� 3 ����������Ѫ��
	uint16_t red_4_robot_HP;		//�� 4 ����������Ѫ��
	uint16_t red_5_robot_HP;		//�� 5 ����������Ѫ��
	uint16_t red_7_robot_HP;		//�� 7 �ڱ�������Ѫ��
	uint16_t red_outpost_HP;		//�췽ǰ��վѪ��
	uint16_t red_base_HP;				//�췽����Ѫ��
	uint16_t blue_1_robot_HP;		//�� 1 Ӣ�ۻ�����Ѫ��
	uint16_t blue_2_robot_HP;		//�� 2 ���̻�����Ѫ��
	uint16_t blue_3_robot_HP;		//�� 3 ����������Ѫ��
	uint16_t blue_4_robot_HP;		//�� 4 ����������Ѫ��
	uint16_t blue_5_robot_HP;		//�� 5 ����������Ѫ��
	uint16_t blue_7_robot_HP;		//�� 7 �ڱ�������Ѫ��
	uint16_t blue_outpost_HP;		//����ǰ��վѪ��
	uint16_t blue_base_HP;			//��������Ѫ��
	
} ext_game_robot_HP_t;


/* CMD_ID: 0x0004  Byte:  3    ���ڷ���״̬���� */
typedef __packed struct
{
	uint8_t dart_belong;						//������ڵĶ��飺1���췽����2����������	
	uint16_t stage_remaining_time;	//����ʱ��ʣ�����ʱ�䣬��λ s
	
} ext_dart_status_t;


/* CMD_ID: 0x0005  Byte:  11    �˹�������ս���ӳ���ͷ���״̬���� */
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


/* CMD_ID: 0x0101  Byte:  4    �����¼����� */
typedef __packed struct 
{ 
	uint32_t event_type;
	
} ext_event_data_t; 


/* CMD_ID: 0x0102  Byte:  4    ����վ������ʶ���� */
typedef __packed struct 
{ 
	uint8_t supply_projectile_id;
	uint8_t supply_robot_id;
	uint8_t supply_projectile_step;
	uint8_t supply_projectile_num;
	
} ext_supply_projectile_action_t;


/* CMD_ID: 0x0104  Byte:  2    ���о�����Ϣ���� */
typedef __packed struct
{
uint8_t level;					//����ȼ���
uint8_t foul_robot_id;	//��������� ID��1 ���Լ� 5 ������ʱ�������� ID Ϊ 0�������ļ�����ʱ�������� ID Ϊ��������� ID
	
} ext_referee_warning_t;


/* CMD_ID: 0x0105  Byte:  1    ���ڷ���ڵ���ʱ���� */
typedef __packed struct
{
	uint8_t dart_remaining_time;  //15s ����ʱ
	
} ext_dart_remaining_time_t;


/* CMD_ID: 0X0201  Byte: 27    ������״̬���� */
typedef __packed struct 
{ 
	uint8_t robot_id;   										//������ID��1�� �췽Ӣ�ۻ����ˣ�2�� �췽���̻����ˣ� 3/4/5�� �췽���������ˣ�	
																					//6�� �췽���л����ˣ�7�� �췽�ڱ������ˣ�8���췽���ڻ����ˣ�9���췽�״�վ��
																					//101�� ����Ӣ�ۻ����ˣ�102�� �������̻����ˣ�103/104/105�� �������������ˣ�
																					//106�� �������л����ˣ�107�� �����ڱ������ˣ�108�� �������ڻ����ˣ�109�� �����״�վ��					
	uint8_t robot_level; 										//1��һ����2��������3������
	uint16_t remain_HP;  										//������ʣ��Ѫ��
	uint16_t max_HP; 												//����������Ѫ��
  
  uint16_t shooter_id1_17mm_cooling_rate; //������ 1 �� 17mm ǹ��ÿ����ȴֵ
  uint16_t shooter_id1_17mm_cooling_limit;//������ 1 �� 17mm ǹ����������
  uint16_t shooter_id1_17mm_speed_limit;  //������ 1 �� 17mm ǹ�������ٶ� ��λ m/s
  
  uint16_t shooter_id2_17mm_cooling_rate; //������ 2 �� 17mm ǹ��ÿ����ȴֵ
  uint16_t shooter_id2_17mm_cooling_limit;//������ 2 �� 17mm ǹ����������
  uint16_t shooter_id2_17mm_speed_limit;  //������ 2 �� 17mm ǹ�������ٶ� ��λ m/s
  
  uint16_t shooter_id1_42mm_cooling_rate; //������ 42mm ǹ��ÿ����ȴֵ
  uint16_t shooter_id1_42mm_cooling_limit;//������ 42mm ǹ����������
  uint16_t shooter_id1_42mm_speed_limit;  //������ 42mm ǹ�������ٶ� ��λ m/s
  
	uint16_t chassis_power_limit;						//�����˵��̹����������ޣ� ��λ ��/W
  
	uint8_t mains_power_gimbal_output : 1;  //0 bit�� gimbal �������  1 Ϊ�� 24V ����� 0 Ϊ�� 24v ���
	uint8_t mains_power_chassis_output : 1; //1 bit�� chassis ������� 1 Ϊ�� 24V ����� 0 Ϊ�� 24v ���
	uint8_t mains_power_shooter_output : 1; //2 bit�� shooter ������� 1 Ϊ�� 24V ����� 0 Ϊ�� 24v ���
	
} ext_game_robot_status_t; 


/* CMD_ID: 0X0202  Byte: 16    ʵʱ������������ */
typedef __packed struct 
{ 
	uint16_t chassis_volt;   				        //���������ѹ ��λ ����/mV
	uint16_t chassis_current;   		        //����������� ��λ ����/mA
	float chassis_power;   					        //����������� ��λ ��/W
	uint16_t chassis_power_buffer;	        //���̹��ʻ��� ��λ ����/J ��ע�����¸��ݹ��������� 250J
	uint16_t shooter_id1_17mm_cooling_heat;	//1 �� 17mm ǹ������
	uint16_t shooter_id2_17mm_cooling_heat;	//2 �� 17mm ǹ������
  uint16_t shooter_id1_42mm_cooling_heat; //42mm ǹ������
	
} ext_power_heat_data_t; 


/* CMD_ID: 0x0203  Byte: 16    ������λ������ */
typedef __packed struct 
{   
	float x;   //λ�� x ���꣬��λ ��/m
	float y;   //λ�� Y ���꣬��λ ��/m
	float z;   //λ�� Z ���꣬��λ ��/m
	float yaw; //λ��ǹ�ڣ���λ ��/��
	
} ext_game_robot_pos_t; 


/* CMD_ID: 0x0204  Byte:  1    �������������� */
typedef __packed struct 
{ 
	uint8_t power_rune_buff;	//bit 0�� ������Ѫ����Ѫ״̬ bit 1�� ǹ��������ȴ����
														//bit 2�� �����˷����ӳ�		 bit 3�� �����˹����ӳ�
} ext_buff_t; 


/* CMD_ID: 0x0205  Byte:  2    ���л���������״̬���� */
typedef __packed struct 
{ 
	uint8_t attack_time; 		//�ɹ���ʱ�䣬 ��λ s�� 30s �ݼ��� 0
  uint8_t energy_point;		//���۵�������
	
} aerial_robot_energy_t; 


/* CMD_ID: 0x0206  Byte:  1    �˺�״̬���� */
typedef __packed struct 
{ 
	uint8_t armor_id : 4; 
	uint8_t hurt_type : 4; 
} ext_robot_hurt_t; 


/* CMD_ID: 0x0207  Byte:  7    ʵʱ������� */
typedef __packed struct 
{ 
	uint8_t bullet_type;   //�ӵ�����: 1�� 17mm ���� 2�� 42mm ����
  uint8_t shooter_id;    //������� ID��1�� 1 �� 17mm �������
                                      //2�� 2 �� 17mm �������
                                      //3�� 42mm �������
	uint8_t bullet_freq;   //�ӵ���Ƶ ��λ Hz
	float bullet_speed;  	 //�ӵ����� ��λ m/s
	
} ext_shoot_data_t; 


/* CMD_ID: 0x0208  Byte:  6    �ӵ�ʣ�෢���� */
typedef __packed struct
{
  uint16_t bullet_remaining_num_17mm;   //17mm �ӵ�ʣ�෢����Ŀ
  uint16_t bullet_remaining_num_42mm;   //42mm �ӵ�ʣ�෢����Ŀ
  uint16_t coin_remaining_num;	        //ʣ��������
  
} ext_bullet_remaining_t;


/* CMD_ID: 0x0209  Byte:  4    ������ RFID ״̬ */
typedef __packed struct
{
	uint32_t rfid_status;
	
} ext_rfid_status_t;


/* CMD_ID: 0x020A  Byte: 6    ���ڻ����˿ͻ���ָ������ */
typedef __packed struct
{
	uint8_t dart_launch_opening_status;	//��ǰ���ڷ���ڵ�״̬0���رգ�1�����ڿ������߹ر��У�2���Ѿ�������
	uint8_t dart_attack_target;					//���ڵĴ��Ŀ�꣬Ĭ��Ϊǰ��վ��1��ǰ��վ��2�����ء�
	uint16_t target_change_time;				//�л����Ŀ��ʱ�ı���ʣ��ʱ�䣬��λ�룬��δ�л�Ĭ��Ϊ 0��
	uint16_t operate_launch_cmd_time;		//���һ�β�����ȷ������ָ��ʱ�ı���ʣ��ʱ�䣬��λ��, ��ʼֵΪ 0��
	
} ext_dart_client_cmd_t;

//===================================================================================================================//
/***************************  ��   CMD_ID: 0x0301  Byte: 128    �����˼佻������  ��  **********************************/
//===================================================================================================================//
#define  LEN_ALL_ID  6        //��ID���ȣ�����ID���ȣ�2�� + �����ߵ�ID���ȣ�2�� + �����ߵ�ID���ȣ�2��

/* һ֡���� = frame_header(֡ͷ) + cmd_id(������ID��0x0301) + �����˼佻������ + frame_tail(֡β)*/ 
/* �����˼佻������ = data_cmd_id(���� ID) + sender_ID(�����ߵ� ID) + receiver_ID(�����ߵ� ID) + data(���ݶ�) */

/***************����ID********************/
/* 
	ID: 0x0200~0x02FF  Byte:  6+n    ���������˼�ͨ�� 
	ID: 0x0100         Byte:  6+2    �ͻ���ɾ��ͼ�� 
	ID: 0x0101         Byte:  6+15   �ͻ��˻���һ��ͼ�� 
	ID: 0x0102         Byte:  6+30   �ͻ��˻��ƶ���ͼ�� 
	ID: 0x0103         Byte:  6+75   �ͻ��˻������ͼ�� 
	ID: 0x0104         Byte:  6+105  �ͻ��˻����߸�ͼ�� 
	ID: 0x0110         Byte:  6+45   �ͻ��˻����ַ�ͼ��  
*/

/***************������ID********************/
/*
  1  ��Ӣ��(��)�� 2  ������(��)�� 3  /4  /5  ������(��)�� 6  ������(��)�� 7  ���ڱ�(��)�� 9  ���״�վ���죩��
  101��Ӣ��(��)�� 102������(��)�� 103/104/105������(��)�� 106������(��)�� 107���ڱ�(��)�� 109���״�վ��������
*/

/***************�ͻ���ID********************/
/*
  0x0101��Ӣ�۲����ֿͻ���(��)��0x0102�����̲����ֿͻ���(��)��0x0103/0x0104/0x0105�����������ֿͻ���(��)�� 0x0106�����в����ֿͻ���(��)�� 
  0x0165��Ӣ�۲����ֿͻ���(��)��0x0166�����̲����ֿͻ���(��)��0x0167/0x0168/0x0169�����������ֿͻ���(��)�� 0x016A�����в����ֿͻ���(��)��
*/

typedef enum
{   
  ID_CLIENT_CUSTOM_GRAPHIC_DELETE = 0x0100,//�ͻ���ɾ��ͼ��
  ID_CLIENT_CUSTOM_GRAPHIC_SINGLE = 0x0101,//�ͻ��˻���һ��ͼ��
  ID_CLIENT_CUSTOM_GRAPHIC_DOUBLE = 0x0102,//�ͻ��˻��ƶ���ͼ��
  ID_CLIENT_CUSTOM_GRAPHIC_FIVE   = 0x0103,//�ͻ��˻������ͼ�� 
  ID_CLIENT_CUSTOM_GRAPHIC_SEVEN  = 0x0104,//�ͻ��˻����߸�ͼ��
  ID_CLIENT_CUSTOM_CHARACTER      = 0x0110,//�ͻ��˻����ַ�
  ID_AERIAL_ROBOT_TO_SENTRY_ROBOT = 0x0201,//���л����˷������ݸ��ڱ�������
  ID_AERIAL_ROBOT_TO_DART_ROBOT   = 0x0202,//���л����˷������ݸ����ڻ�����
  //��������ʵ��ʹ������������ӣ�0x0200~0x02FF��  
} xdateCmdID;//���� ID

typedef enum
{ 
  LEN_AERIAL_ROBOT_TO_SENTRY_ROBOT = 1,  //���л����˷��͸��ڱ����ݳ���
  LEN_AERIAL_ROBOT_TO_DART_ROBOT   = 1,  //���л����˷��͸��������ݳ���
  LEN_ROBOT_INTERACTIVE_DATE = 10,       //�����˼����ݽ����������趨����
  LEN_ROBOT_INTERACTIVE_DATE_MAX  = 113, //�����˼����ݽ���������󳤶�
  LEN_SEND_CLIENT_SIDE_DATE_MAX   = 105, //�����˷��͸��ͻ���������󳤶�
  LEN_SEND_CLIENT_SIDE_BUFFER     = 15,  //�����˷��͸��ͻ���ͼ�����ݵĳ��ȣ�����Э��ȷ����
  LEN_SEND_CLIENT_SIDE_BUFFER_NUM = 2,   //�����˷��͸��ͻ��������趨������������и�����Ҫ����ͼ������ȷ����
  
} xrobot_interactive_data;//���ݳ���  ʵ�ʳ������е�


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
  
} xrobotType;//������ID


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
  
} xrobotID;//������ID



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

} xclientID;//�ͻ���ID

typedef enum
{
	NULL_OPERATION =0,//�ղ���
	ADD  =1,          //����
	MODIFY  =2,       //�޸�
	DELETE  =3,       //ɾ��
}Operate;

typedef enum
{
	RED_AND_BULE  =0,  //������ɫ
	YELLOW =1,         //��ɫ
	GREEN  =2,         //��ɫ
	ORANGE =3,         //��ɫ
	PURPLISH_RED =4,   //�Ϻ�ɫ
	PINK  =5,          //��ɫ
	CYAN  =6,          //��ɫ
	BLACK =7,          //��ɫ
	WRITE =8,          //��ɫ
}Color;

typedef enum
{
  STRAIGHT_LINE =0,  //ֱ��
	RECTANGLE     =1,  //����
	FULL_CIRCLE   =2,  //��Բ
	ELLIPSE       =3,  //��Բ
	ARC           =4,  //Բ��
	FLOATING_POINT=5,  //������
  INTEGER       =6,  //������ 
	CHARCTER      =7,  //�ַ�
}Type;


//�����˼佻������֡ͷ
typedef __packed struct
{
	uint16_t data_cmd_id; //���ݶε����� ID
	uint16_t sender_ID;   //�����ߵ� ID
	uint16_t receiver_ID; //�����ߵ� ID

}ext_student_interactive_header_data_t;

/****************************************   ��  �����˼佻������  ��  ************************************/
/* ���ݶ�ID: 0x0200~0x02FF  Byte: 6+n(n < 113)  �������� */
typedef __packed struct
{
   uint8_t data[LEN_ROBOT_INTERACTIVE_DATE];    //�������ݶΣ����113���ֽڣ�����ʵ����������趨
} robot_interactive_data_t;

typedef __packed struct
{
  uint8_t sentry_mode;
}ext_aerial_to_sentry_data_t;//���л����˷��͸��ڱ�����

typedef __packed struct
{
  uint8_t launch_num;
}ext_aerial_to_dart_data_t;//���л����˷��͸���������
/****************************************   ��  �����˼佻������  ��  ************************************/

/****************************************   ��  �ͻ�����ͼ�λ���  ��  ************************************/
/* ���ݶ�ID: 0x0100  Byte: 6+2  �ͻ���ɾ��ͼ�� */
typedef __packed struct
{
  uint8_t operate_tpye; //ͼ�β���  0���ղ�����1��ɾ��ͼ�㣻2��ɾ�����У�
  uint8_t layer;        //ͼ����    ͼ������0~9
} ext_client_custom_graphic_delete_t;

//ͼ�����ݣ�ͼ�λ���ʱ�������������ο�RM2021����ϵͳ����Э�鸽¼V1.2
typedef __packed struct
{
	uint8_t graphic_name[3]; //��ɾ�����޸ĵȲ����У���Ϊ�ͻ��˵����� 
	uint32_t operate_tpye:3; //ͼ�β�����0���ղ�����1�����ӣ�2���޸ģ�3��ɾ����
	uint32_t graphic_tpye:3; //ͼ�����ͣ�0��ֱ�ߣ�1�����Σ�2����Բ��3����Բ��4��Բ����5����������6����������7���ַ���
	uint32_t layer:4;        //ͼ������ 0~9
	uint32_t color:4;        //��ɫ��0��������ɫ��1����ɫ��2����ɫ��3����ɫ��4���Ϻ�ɫ��5����ɫ��6����ɫ��7����ɫ��8����ɫ��
	uint32_t start_angle:9;  //��ʼ�Ƕȣ���λ�� �㣬��Χ[0,360]��
	uint32_t end_angle:9;    //��ֹ�Ƕȣ���λ�� �㣬��Χ[0,360]��
	uint32_t width:10;       //�߿�
	uint32_t start_x:11;     //��� x ���ꣻ
	uint32_t start_y:11;     //��� y ����
	uint32_t radius:10;      //�����С���߰뾶��
	uint32_t end_x:11;       //�յ� x ���ꣻ
	uint32_t end_y:11;       //�յ� y ���ꣻ
}graphic_data_struct_t;
 
/* ���ݶ�ID: 0x0101  Byte: 6+15  �ͻ��˻���һ��ͼ�� */
typedef __packed struct
{
  graphic_data_struct_t grapic_data_struct;//ͼ������  ���ͼ�����ݽ���
} ext_client_custom_graphic_single_t;

/* ���ݶ�ID: 0x0102  Byte: 6+30  �ͻ��˻��ƶ���ͼ�� */
typedef __packed struct
{
  graphic_data_struct_t grapic_data_struct[2];//ͼ������  ���ͼ�����ݽ���
} ext_client_custom_graphic_double_t;

/* ���ݶ�ID: 0x0103  Byte: 6+75  �ͻ��˻������ͼ�� */
typedef __packed struct
{
graphic_data_struct_t grapic_data_struct[5];//ͼ������  ���ͼ�����ݽ���
} ext_client_custom_graphic_five_t;

/* ���ݶ�ID: 0x0104  Byte: 6+105  �ͻ��˻����߸�ͼ�� */
typedef __packed struct
{
  graphic_data_struct_t grapic_data_struct[7];//ͼ������  ���ͼ�����ݽ���
} ext_client_custom_graphic_seven_t;

/* ���ݶ�ID: 0x0110  Byte: 6+45  �ͻ��˻����ַ� */
typedef __packed struct
{
  graphic_data_struct_t grapic_data_struct;//�ַ�����  ���ͼ�����ݽ���
  uint8_t data[30];                        //�ַ�
} ext_client_custom_character_t;
/****************************************   ��  �ͻ�����ͼ�λ���  ��  ************************************/
//===================================================================================================================//
/***************************  ��   CMD_ID: 0x0301  Byte: 128    �����˼佻������  ��  **********************************/
//===================================================================================================================//


/* CMD_ID: 0x0302  Byte: x(x <= 30)   �Զ������������ */
typedef __packed struct
{
  uint8_t data[20];//���ݶ�����,С��30�ֽ�
} robot_interactive_custom_data_t;


/* CMD_ID: 0x0303  Byte: 15   �ͻ���С��ͼ�·���Ϣ */
typedef __packed struct 
	{ 
	float target_position_x;  //Ŀ�� x λ�����꣬��λ m
	float target_position_y;  //Ŀ�� y λ�����꣬��λ m
	float target_position_z;  //Ŀ�� z λ�����꣬��λ m
	uint8_t commd_keyboard;   //����ָ��ʱ����̨�ְ��µļ�����Ϣ
	uint16_t target_robot_ID; //Ҫ���õ�Ŀ������� ID
} ext_robot_command_t_map;    


/* CMD_ID: 0x0305  Byte: 10   �ͻ��˽�����Ϣ *///�״�վ���͵�������Ϣ���Ա����м����������ڵ�һ�ӽ�С��ͼ����
typedef __packed struct
{ 
  uint16_t target_robot_ID;//Ŀ������� ID
  float target_position_x; //Ŀ�� x λ�����꣬��λ m
  float target_position_y; //Ŀ�� y λ�����꣬��λ m
} ext_client_map_command_t;


/* CMD_ID: 0x0304  Byte: 12   ͼ��ң����Ϣ *///�ͻ���ͨ��ͼ��ģ�齫������Ϣ����������ϵͳ
typedef __packed struct 
{ 
	int16_t mouse_x;           //��� X ����Ϣ 
	int16_t mouse_y;           //��� Y ����Ϣ
	int16_t mouse_z;           //��������Ϣ 
	int8_t left_button_down;   //����������
	int8_t right_button_down;  //����Ҽ�����
	uint16_t keyboard_value;   //���̸������Ƿ���
	uint16_t reserved;         //����λ
}ext_robot_command_t_keyboard;  


/* ����ϵͳ���ݻ��� */
typedef  struct   
{
	//֡ͷ
	xFrameHeader  													frame_header;											
	//������ID			
	uint16_t																cmdID;
	//���ݶ�			
	ext_game_state_t 												game_state;                 			//����״̬���� 
	ext_game_result_t 											game_result;               	 			//�����������         					
	ext_game_robot_HP_t 										game_robot_HP;             	 			//����������Ѫ������   					
	ext_dart_status_t    					  				dart_status;                			//���ڷ���״̬   								
	ext_ICRA_buff_debuff_zone_status_t			ICRA_buff_debuff_zone_status;			//�˹�������ս���ӳ���ͷ���״̬
	ext_event_data_t   											event_data;                 			//�����¼�����   								
	ext_supply_projectile_action_t					supply_projectile_action;  	 			//���ز���վ������ʶ����    		
	ext_referee_warning_t										referee_warning;           				//���о�������    							
	ext_dart_remaining_time_t								dart_remaining_time;       				//���ڷ���ڵ���ʱ    					
	ext_game_robot_status_t									game_robot_status;         	 			//������״̬����        				
	ext_power_heat_data_t										power_heat_data;           	 			//ʵʱ������������   						
	ext_game_robot_pos_t										game_robot_pos;            	 			//������λ������           			
	ext_buff_t															buff;                      	 			//��������������           			
	aerial_robot_energy_t										aerial_robot_energy;              //���л���������״̬����    		
	ext_robot_hurt_t												robot_hurt;                 			//�˺�״̬����           				
	ext_shoot_data_t												shoot_data;                	 			//ʵʱ�������           				
	ext_bullet_remaining_t									bullet_remaining;          	 			//����ʣ�෢����          			
	ext_rfid_status_t												RFID_status;               	 			//������ RFID ״̬          		
	ext_dart_client_cmd_t										dart_client_cmd;           	 			//���ڻ����˿ͻ���ָ������

  ext_client_custom_graphic_delete_t      client_custom_graphic_delete;     //�ͻ���ɾ��ͼ��
  /*ע�⣺����Э�飬������7��ͼ�Σ��������Զ���Ϊcustom_data[7]������Զ���ͼ���������ݵ��������Ը���
    ������ģ��������ݵ�cmd_ID��Ҫ����ͼ�ε��������ģ������������Э�飩�����統ǰ����2��ͼ�Σ����ݵ���
    ��IDΪ0x0102 ���Ǿ������Ժ�ó�*/
  /*�����һ�������е�ͼ�����Ʋ�ͬ������Գ����µ�ͼ�Σ�֮ǰ��ͬһ�������е�ͼ�β�������ʧ�����һ������
    �Ϳ��Ի��ƶ����ͬ��ͼ�Σ�û��Ҫ����7������*/
  graphic_data_struct_t                   graphic_data_struct[LEN_SEND_CLIENT_SIDE_BUFFER_NUM]; //�Զ���ͼ����������          
  ext_client_custom_character_t           client_custom_character;//�Զ����ַ�����������
  
	ext_student_interactive_header_data_t		student_interactive_header_data;	//�����˼佻������ ID��
  robot_interactive_data_t	              robot_interactive_data;           //�����˼佻������ ���ݶ�
  ext_aerial_to_sentry_data_t             aerial_to_sentry_data;            //���˻����ڱ���������
  ext_aerial_to_dart_data_t               aerial_to_dart_data;              //���˻�����ڽ�������
  
  robot_interactive_custom_data_t         robot_interactive_custom_data;    //�Զ������������
	//֡β(CRC16У��)
	uint16_t 																frame_tail;
	
}Referee_Struct;

/* ʹ�õ��Ĳ���ϵͳ���ݻ���(�ɸ���ʵ������������) */
typedef  struct   
{
  uint8_t robot_id;               //������ID
  
  uint16_t robot_remain_hp;       //������ʣ��Ѫ��
  
  fp32 chassis_power;             //����ʵʱ����
  uint16_t chassis_power_buffer;  //���̻�������
  uint16_t chassis_power_limit;   //���̹�������
  
  uint16_t shooter1_17mm_heat;        //17mm �������1 ǹ������
  uint16_t shooter2_17mm_heat;        //17mm �������2 ǹ������
  uint16_t shooter1_42mm_heat;        //42mm �������1 ǹ������

  uint16_t shooter1_17mm_freq;        //17mm �������1 ��Ƶ
  uint16_t shooter2_17mm_freq;        //17mm �������2 ��Ƶ
  uint16_t shooter1_42mm_freq;        //42mm �������1 ��Ƶ

  uint16_t shooter1_17mm_speed;       //17mm �������1 ����
  uint16_t shooter2_17mm_speed;       //17mm �������2 ����
  uint16_t shooter1_42mm_speed;       //42mm �������1 ����
  
  uint16_t shooter1_17mm_heat_limit;  //17mm �������1 ǹ����������
  uint16_t shooter2_17mm_heat_limit;  //17mm �������2 ǹ����������
  uint16_t shooter1_42mm_heat_limit;  //42mm �������1 ǹ����������
  
  uint16_t shooter1_17mm_speed_limit; //17mm �������1 ǹ����������
  uint16_t shooter2_17mm_speed_limit; //17mm �������2 ǹ����������
  uint16_t shooter1_42mm_speed_limit; //42mm �������1 ǹ����������
  
  uint16_t bullet_17mm_remaining_num; //17mm �ӵ�ʣ�෢����Ŀ  �Կ��������л��������ڱ�����
  uint16_t coin_remaining_num;        //ʣ��������
  
  uint8_t gimbal_output;                //��̨�����
  uint8_t chassis_output;               //���̿����
  uint8_t shooter_output;               //��������
  

}Referee_Used_Struct;
/* ��ģ�����ⲿ�ṩ�ı������� -----------------------------------------------*/

/* ��ģ�����ⲿ�ṩ���Զ����������ͱ������� ---------------------------------*/
extern Referee_Used_Struct referee_used;

/* ��ģ�����ⲿ�ṩ�Ľӿں���ԭ������ ---------------------------------------*/
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

