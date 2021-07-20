#ifndef __VISION_H
#define __VISION_H

/* 包含头文件 ---------------------------------------------------------------*/
#include "myinclude.h"
/* 本模块向外部提供的宏定义 -------------------------------------------------*/
#define VISION_RX_BUFFER_SIZE  18 //接收视觉数据时提供的数组大小
#define VISION_TX_BUFFER_SIZE  9  //发送视觉数据时提供的数组大小

#define VISION_DATA_LEN 9					//视觉传输数据的实际长度


/* 本模块向外部提供的结构体/枚举定义 ----------------------------------------*/
typedef struct
{
	uint8_t 		frame_header; 		  //帧头0xFF
	int16_t 		angle_yaw;     			//yaw angle
	int16_t 		angle_pit;     			//pitch angle 
	uint8_t 		state_flag;     		//当前状态：【0 未瞄准目标 】【 1已扫瞄到但未锁定目标】【2 近距离扫瞄到并锁定目标】【3 远距离扫瞄到并锁定目标】
	int16_t 		distance;     			//目标距离
	uint8_t 		frame_tail; 	  	  //帧尾0xFE
	
}Minipc_Rx_Struct;

typedef __packed struct
{
	uint8_t 		frame_header; 		  //帧头0xFF
	int16_t 		data1;     			
	int16_t 		data2;     			 
	uint8_t 		data3;     		
	int16_t 		data4;     			
	uint8_t 		frame_tail; 	  	  //帧尾0xFE
	
}Minipc_Tx_Struct;
/* 本模块向外部提供的变量声明 -----------------------------------------------*/

/* 本模块向外部提供的自定义数据类型变量声明 ---------------------------------*/

/* 本模块向外部提供的接口函数原型声明 ---------------------------------------*/
void VisionDate_Receive_USART_Init(void);
void Vision_UART_IRQHandler(void);
void Send_To_Vision(int16_t data1, int16_t data2, int8_t data3, int16_t data4);



#endif

