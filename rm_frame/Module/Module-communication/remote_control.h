#ifndef __RC_H
#define __RC_H
/* 包含头文件 ---------------------------------------------------------------*/
#include "myinclude.h"

/* 本模块向外部提供的宏定义 -------------------------------------------------*/
#define DR16_RX_BUFFER_SIZE 36u
#define DR16_DATA_LEN 18u

/* 本模块向外部提供的结构体/枚举定义 ----------------------------------------*/
//遥控器数据结构体
typedef __packed struct
{
	__packed struct
	{
			int16_t ch0;
			int16_t ch1;
			int16_t ch2;
			int16_t ch3;
			uint8_t s1;
		  uint8_t s2;
	} rc;
	__packed struct
	{
			int16_t x;
			int16_t y;
			int16_t z;
			uint8_t press_l;
			uint8_t press_r;
	} mouse;
	__packed struct
	{
			uint16_t v;
	} key;

} RC_ctrl_t;

/* 本模块向外部提供的变量声明 -----------------------------------------------*/

/* 本模块向外部提供的自定义数据类型变量声明 ---------------------------------*/
extern RC_ctrl_t rc_ctrl;//遥控器数据结构体


/* 本模块向外部提供的接口函数原型声明 ---------------------------------------*/
void Remote_Control_USART_Init(void);
void DR16_UART_IRQHandler(void);

#endif
