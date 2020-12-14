#ifndef __PID_WIRELESS_DEBUGGING_H
#define __PID_WIRELESS_DEBUGGING_H
/* 包含头文件 ---------------------------------------------------------------*/
#include "myinclude.h"
#include "pid.h"

/* 本模块向外部提供的宏定义 -------------------------------------------------*/
#define USART_Waiting   0
#define USART_Receiving 1
#define USART_Success   2 
#define USART_Failed    3

#define QUERY_CMD_ID 					0X10
#define SET_CMD_ID 						0X01
#define UPLOAD_PARAMS_CMD_ID 	0x10
#define DATA_LENGTH_UPLOAD_PID_PATAMS 30

#define USART_RX_LEN  			200  	//定义最大接收字节数 200
#define USART_TX_LEN        200

/* 本模块向外部提供的结构体/枚举定义 ----------------------------------------*/
typedef struct{
    unsigned char motor_ID;
    unsigned char PID_Mode;//增量式：1；位置式：0
    union{
        unsigned char tempChar[4];
        float tempFloat;
    }Kp_value;
    union{
        unsigned char tempChar[4];
        float tempFloat;
    }Ki_value;
    union{
        unsigned char tempChar[4];
        float tempFloat;
    }Kd_value;
    union{
        unsigned char tempChar[4];
        float tempFloat;
    }P_out_max;
    union{
        unsigned char tempChar[4];
        float tempFloat;
    }I_out_max;
    union{
        unsigned char tempChar[4];
        float tempFloat;
    }D_out_max;
    union{
        unsigned char tempChar[4];
        float tempFloat;
    }PID_out_max;
}PID_struct;

typedef struct{
    unsigned char SOF;
    unsigned char Data_Length;
    unsigned char seq;
    unsigned char crc8;
    unsigned char cmd_ID;
    PID_struct PID;
    union{
        unsigned char tempChar[2];
        int tempInt;
    }crc16;
}MsgsFrame_struct;

/* 本模块向外部提供的变量声明 -----------------------------------------------*/

/* 本模块向外部提供的自定义数据类型变量声明 ---------------------------------*/

/* 本模块向外部提供的接口函数原型声明 ---------------------------------------*/
void PW_IRQHandler(void); 
void Wireless_Debug_USART_Init(void);

#endif



