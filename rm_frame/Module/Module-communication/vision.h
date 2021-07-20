#ifndef __VISION_H
#define __VISION_H

/* ����ͷ�ļ� ---------------------------------------------------------------*/
#include "myinclude.h"
/* ��ģ�����ⲿ�ṩ�ĺ궨�� -------------------------------------------------*/
#define VISION_RX_BUFFER_SIZE  18 //�����Ӿ�����ʱ�ṩ�������С
#define VISION_TX_BUFFER_SIZE  9  //�����Ӿ�����ʱ�ṩ�������С

#define VISION_DATA_LEN 9					//�Ӿ��������ݵ�ʵ�ʳ���


/* ��ģ�����ⲿ�ṩ�Ľṹ��/ö�ٶ��� ----------------------------------------*/
typedef struct
{
	uint8_t 		frame_header; 		  //֡ͷ0xFF
	int16_t 		angle_yaw;     			//yaw angle
	int16_t 		angle_pit;     			//pitch angle 
	uint8_t 		state_flag;     		//��ǰ״̬����0 δ��׼Ŀ�� ���� 1��ɨ�鵽��δ����Ŀ�꡿��2 ������ɨ�鵽������Ŀ�꡿��3 Զ����ɨ�鵽������Ŀ�꡿
	int16_t 		distance;     			//Ŀ�����
	uint8_t 		frame_tail; 	  	  //֡β0xFE
	
}Minipc_Rx_Struct;

typedef __packed struct
{
	uint8_t 		frame_header; 		  //֡ͷ0xFF
	int16_t 		data1;     			
	int16_t 		data2;     			 
	uint8_t 		data3;     		
	int16_t 		data4;     			
	uint8_t 		frame_tail; 	  	  //֡β0xFE
	
}Minipc_Tx_Struct;
/* ��ģ�����ⲿ�ṩ�ı������� -----------------------------------------------*/

/* ��ģ�����ⲿ�ṩ���Զ����������ͱ������� ---------------------------------*/

/* ��ģ�����ⲿ�ṩ�Ľӿں���ԭ������ ---------------------------------------*/
void VisionDate_Receive_USART_Init(void);
void Vision_UART_IRQHandler(void);
void Send_To_Vision(int16_t data1, int16_t data2, int8_t data3, int16_t data4);



#endif

