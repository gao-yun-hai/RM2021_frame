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

	ID: 0x0001  Byte:  3    ����״̬����       							1Hz���ڷ���     
	ID: 0x0002  Byte:  1    �����������         						������������      
	ID: 0x0003  Byte:  32   ����������Ѫ������   						1Hz���ڷ���
	ID: 0x0004  Byte:  3    ���ڷ���״̬   									���ڷ���ʱ����
	ID: 0x0005  Byte:  3    �˹�������ս���ӳ���ͷ���״̬  1Hz���ڷ���
	ID: 0x0101  Byte:  4    �����¼�����   									1Hz���ڷ���
	ID: 0x0102  Byte:  4    ���ز���վ������ʶ����    			������������ 
	ID: 0X0104  Byte:  2    ���о�������    							  ���淢������ 
	ID: 0X0105  Byte:  1    ���ڷ���ڵ���ʱ    						1Hz���ڷ��� 
	ID: 0X0201  Byte: 18    ������״̬����        					10Hz���ڷ���
	ID: 0X0202  Byte: 16    ʵʱ������������   							50Hz���ڷ���       
	ID: 0x0203  Byte: 16    ������λ������           				10Hz���ڷ���
	ID: 0x0204  Byte:  1    ��������������           				1Hz���ڷ���
	ID: 0x0205  Byte:  3    ���л���������״̬����    			10Hz���ڷ��ͣ�ֻ�п��л��������ط���
	ID: 0x0206  Byte:  1    �˺�״̬����           					�˺���������
	ID: 0x0207  Byte:  6    ʵʱ�������           					�ӵ��������
	ID: 0x0208  Byte:  2    ����ʣ�෢����          				�����л������Լ��ڱ����������ط��͸����ݣ�1Hz���ڷ���
	ID: 0x0209  Byte:  4    ������ RFID ״̬          			1Hz���ڷ���
	ID: 0x020A  Byte: 12    ���ڻ����˿ͻ���ָ������        1Hz���ڷ���
	ID: 0x0301  Byte:  n    �����˼佻������          			���ͷ���������,10Hz
	
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
	ID_INTERATIVE_DATE							= 0x0301,//�����˼佻������
	
} xCmdID;

//����������Ӧ�������ݶεĳ���,���ݹٷ�Э�������峤��
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


/* �Զ���֡ͷ */
typedef __packed struct
{
	uint8_t  SOF;
	uint16_t DataLength;
	uint8_t  Seq;
	uint8_t  CRC8;
	
} xFrameHeader;

/* ID: 0x0001  Byte:  3    ����״̬���� */
typedef __packed struct 
{ 
	uint8_t game_type : 4;
	uint8_t game_progress : 4;
	uint16_t stage_remain_time;
	
} ext_game_state_t; 


/* ID: 0x0002  Byte:  1    ����������� */
typedef __packed struct 
{ 
	uint8_t winner;
	
} ext_game_result_t; 


/* ID: 0x0003  Byte:  2    ����������Ѫ������ */
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


/* ID: 0x0004  Byte:  3    ���ڷ���״̬���� */
typedef __packed struct
{
	uint8_t dart_belong;						//������ڵĶ��飺1���췽����2����������	
	uint16_t stage_remaining_time;	//����ʱ��ʣ�����ʱ�䣬��λ s
	
} ext_dart_status_t;


/* ID: 0x0005  Byte:  3    �˹�������ս���ӳ���ͷ���״̬���� */
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


/* ID: 0x0101  Byte:  4    �����¼����� */
typedef __packed struct 
{ 
	uint32_t event_type;
	
} ext_event_data_t; 


/* ID: 0x0102  Byte:  4    ����վ������ʶ���� */
typedef __packed struct 
{ 
	uint8_t supply_projectile_id;
	uint8_t supply_robot_id;
	uint8_t supply_projectile_step;
	uint8_t supply_projectile_num;
	
} ext_supply_projectile_action_t;


/* ID: 0x0104  Byte:  2    ���о�����Ϣ���� */
typedef __packed struct
{
uint8_t level;					//����ȼ���
uint8_t foul_robot_id;	//��������� ID��1 ���Լ� 5 ������ʱ�������� ID Ϊ 0�������ļ�����ʱ�������� ID Ϊ��������� ID
	
} ext_referee_warning_t;


/* ID: 0x0105  Byte:  1    ���ڷ���ڵ���ʱ���� */
typedef __packed struct
{
	uint8_t dart_remaining_time;  //15s ����ʱ
	
} ext_dart_remaining_time_t;


/* ID: 0X0201  Byte: 18    ������״̬���� */
typedef __packed struct 
{ 
	uint8_t robot_id;   										//������ID��1�� �췽Ӣ�ۻ����ˣ�2�� �췽���̻����ˣ� 3/4/5�� �췽���������ˣ�	
																					//6�� �췽���л����ˣ�7�� �췽�ڱ������ˣ�8���췽���ڻ����ˣ�9���췽�״�վ��
																					//101�� ����Ӣ�ۻ����ˣ�102�� �������̻����ˣ�103/104/105�� �������������ˣ�
																					//106�� �������л����ˣ�107�� �����ڱ������ˣ�108�� �������ڻ����ˣ�109�� �����״�վ��					
	uint8_t robot_level; 										//1��һ����2��������3������
	uint16_t remain_HP;  										//������ʣ��Ѫ��
	uint16_t max_HP; 												//����������Ѫ��
	uint16_t shooter_heat0_cooling_rate;  	//������ 17mm �ӵ�������ȴ�ٶ� ��λ /s
	uint16_t shooter_heat0_cooling_limit;   //������ 17mm �ӵ���������
	uint16_t shooter_heat1_cooling_rate;    //������ 42mm ǹ��ÿ����ȴֵ ��λ /s
	uint16_t shooter_heat1_cooling_limit;   //������ 42mm ǹ����������
	uint8_t shooter_heat0_speed_limit;			//������ 17mm ǹ�������ٶ� ��λ m/s
	uint8_t shooter_heat1_speed_limit;			//������ 42mm ǹ�������ٶ� ��λ m/s
	uint8_t max_chassis_power;							//�����������̣� ��λ ��/W
	uint8_t mains_power_gimbal_output : 1;  //0 bit�� gimbal �������  1 Ϊ�� 24V ����� 0 Ϊ�� 24v ���
	uint8_t mains_power_chassis_output : 1; //1 bit�� chassis ������� 1 Ϊ�� 24V ����� 0 Ϊ�� 24v ���
	uint8_t mains_power_shooter_output : 1; //2 bit�� shooter ������� 1 Ϊ�� 24V ����� 0 Ϊ�� 24v ���
	
} ext_game_robot_status_t; 


/* ID: 0X0202  Byte: 16    ʵʱ������������ */
typedef __packed struct 
{ 
	uint16_t chassis_volt;   				//���������ѹ ��λ ����/mV
	uint16_t chassis_current;   		//����������� ��λ ����/mA
	float chassis_power;   					//����������� ��λ ��/W
	uint16_t chassis_power_buffer;	//���̹��ʻ��� ��λ ����/J ��ע�����¸��ݹ��������� 250J
	uint16_t shooter_heat0;					//17mm ǹ������
	uint16_t shooter_heat1;					//42mm ǹ������  
	uint16_t mobile_shooter_heat2;	//���� 17 mm ǹ������
	
} ext_power_heat_data_t; 


/* ID: 0x0203  Byte: 16    ������λ������ */
typedef __packed struct 
{   
	float x;   //λ�� x ���꣬��λ ��/m
	float y;   //λ�� Y ���꣬��λ ��/m
	float z;   //λ�� Z ���꣬��λ ��/m
	float yaw; //λ��ǹ�ڣ���λ ��/��
	
} ext_game_robot_pos_t; 


/* ID: 0x0204  Byte:  1    �������������� */
typedef __packed struct 
{ 
	uint8_t power_rune_buff;	//bit 0�� ������Ѫ����Ѫ״̬ bit 1�� ǹ��������ȴ����
														//bit 2�� �����˷����ӳ�		 bit 3�� �����˹����ӳ�
} ext_buff_t; 


/* ID: 0x0205  Byte:  3    ���л���������״̬���� */
typedef __packed struct 
{ 
	uint8_t energy_point;		//���۵�������
	uint8_t attack_time; 		//�ɹ���ʱ�䣬 ��λ s�� 30s �ݼ��� 0
	
} aerial_robot_energy_t; 


/* ID: 0x0206  Byte:  1    �˺�״̬���� */
typedef __packed struct 
{ 
	uint8_t armor_id : 4; 
	uint8_t hurt_type : 4; 
} ext_robot_hurt_t; 


/* ID: 0x0207  Byte:  6    ʵʱ������� */
typedef __packed struct 
{ 
	uint8_t bullet_type;   //�ӵ�����: 1�� 17mm ���� 2�� 42mm ����
	uint8_t bullet_freq;   //�ӵ���Ƶ ��λ Hz
	float bullet_speed;  	 //�ӵ����� ��λ m/s
	
} ext_shoot_data_t; 


/* ID: 0x0208  Byte:  2    �ӵ�ʣ�෢���� */
typedef __packed struct
{
	uint16_t bullet_remaining_num;	//�ӵ�ʣ�෢����Ŀ
	
} ext_bullet_remaining_t;


/* ID: 0x0209  Byte:  4    ������ RFID ״̬ */
typedef __packed struct
{
	uint32_t rfid_status;
	
} ext_rfid_status_t;


/* ID: 0x020A  Byte: 12    ���ڻ����˿ͻ���ָ������ */
typedef __packed struct
{
	uint8_t dart_launch_opening_status;	//��ǰ���ڷ���ڵ�״̬0���رգ�1�����ڿ������߹ر��У�2���Ѿ�������
	uint8_t dart_attack_target;					//���ڵĴ��Ŀ�꣬Ĭ��Ϊǰ��վ��1��ǰ��վ��2�����ء�
	uint16_t target_change_time;				//�л����Ŀ��ʱ�ı���ʣ��ʱ�䣬��λ�룬��δ�л�Ĭ��Ϊ 0��
	uint8_t first_dart_speed;						//��⵽�ĵ�һö�����ٶȣ���λ 0.1m/s/LSB, δ�����Ϊ 0��
	uint8_t second_dart_speed;					//��⵽�ĵڶ�ö�����ٶȣ���λ 0.1m/s/LSB��δ�����Ϊ 0��
	uint8_t third_dart_speed;						//��⵽�ĵ���ö�����ٶȣ���λ 0.1m/s/LSB��δ�����Ϊ 0��
	uint8_t fourth_dart_speed;					//��⵽�ĵ���ö�����ٶȣ���λ 0.1m/s/LSB��δ�����Ϊ 0��
	uint16_t last_dart_launch_time;			//���һ�εķ�����ڵı���ʣ��ʱ�䣬��λ�룬��ʼֵΪ 0��
	uint16_t operate_launch_cmd_time;		//���һ�β�����ȷ������ָ��ʱ�ı���ʣ��ʱ�䣬��λ��, ��ʼֵΪ 0��
	
} ext_dart_client_cmd_t;


/* ID: 0x0301  Byte: 128    �����˼佻������ */
typedef __packed struct
{
	uint16_t data_cmd_id;
	uint16_t sender_ID;
	uint16_t receiver_ID;
	uint8_t	 date[10];
	
}ext_student_interactive_header_data_t;


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
	ext_student_interactive_header_data_t		student_interactive_header_data;	//�����˼佻������	
	//֡β(CRC16У��)
	uint16_t 																frame_tail;
	
}Referee_Struct;


/* ��ģ�����ⲿ�ṩ�ı������� -----------------------------------------------*/

/* ��ģ�����ⲿ�ṩ���Զ����������ͱ������� ---------------------------------*/
extern Referee_Struct  referee;
/* ��ģ�����ⲿ�ṩ�Ľӿں���ԭ������ ---------------------------------------*/
void RefereeDate_Receive_USART_Init(void);
void Referee_UART_IRQHandler(void);
void get_chassis_power_and_buffer(fp32 *power, fp32 *buffer);
void get_shoot_heat0_limit_and_heat0(uint16_t *heat0_limit, uint16_t *heat0);
void get_shoot_heat1_limit_and_heat1(uint16_t *heat1_limit, uint16_t *heat1);





#endif

