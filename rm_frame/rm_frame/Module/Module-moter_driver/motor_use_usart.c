/*******************************************************************************
                      版权所有 (C), 2020-,NCUROBOT
 *******************************************************************************
  文 件 名   : motor_use_usart.c
  版 本 号   : V1.0
  作    者   : NCURM
  生成日期   : 2018/10/8
  最近修改   : 2020/12/1
  功能描述   : AX-12A数字总线舵机驱动程序
  函数列表   : 
*******************************************************************************/
/* 包含头文件 ----------------------------------------------------------------*/
#include "motor_use_usart.h"

/* 内部宏定义 ----------------------------------------------------------------*/

/* 内部自定义数据类型的变量 --------------------------------------------------*/

/* 内部变量 ------------------------------------------------------------------*/

/* 内部函数原型声明 ----------------------------------------------------------*/
void uart_t(uint8_t data);

/* 函数主体部分 --------------------------------------------------------------*/

void AX_12A_Init(uint8_t id)
{
      uart_t(0xff);
      uart_t(0xff);
      uart_t(0xfe);
      uart_t(0x09);
      uart_t(0x83);
      uart_t(0x06);
      uart_t(0x04);
      uart_t(id);
      uart_t(0x00);
      uart_t(0x00);  
      uart_t(0xff);
      uart_t(0x03);
      uart_t(~((0x94 + 0x03 + 0xff + (id)) & 0xff));
      /*设置波特率*/
      uart_t(0xff);
      uart_t(0xff);
      uart_t(0xfe);
      uart_t(0x06);
      uart_t(0x83);
      uart_t(0x04);
      uart_t(0x01);
      uart_t(id);
      uart_t(0x10);//115200
      uart_t(~((0x9c + (id)) & 0xff));
}

void uart_t(uint8_t data)
{  
  HAL_UART_Transmit(&huart2,&data,1,10);  
}

void AX_12A_Set(uint8_t id,uint16_t angle,uint16_t speed)
{
  uint8_t low = angle;
  uint8_t high = angle >> 8;
  uint8_t low2 = speed;
  uint8_t high2 = speed >> 8;

  uart_t(0xff);
  uart_t(0xff);
  uart_t(0xfe);
  uart_t(0x0b);
  uart_t(0x83);
  uart_t(0x1c);
  uart_t(0x06);
  uart_t(id);
  uart_t(0x20);
  uart_t(0x20);  
  uart_t(low);
  uart_t(high);
  uart_t(low2);
  uart_t(high2);
  uart_t(~((0xee + (id) + (high) + (low) + (high2) + (low2)) & 0xff));
}

