/*******************************************************************************
                      版权所有 (C), 2020-,NCUROBOT
 *******************************************************************************
  文 件 名   : pid_wireless_debug.c
  版 本 号   : V1.0
  作    者   : 高云海
  生成日期   : 2020.11.27
  最近修改   :
  功能描述   : PID无线调参【USART初始化、数据发送、数据接收、USART中断处理】
  函数列表   : 1) Wireless_Debug_Init() 	【外部调用：bsp.c】
							 2) PID_Params_Set()				【内部调用：PID_Regulator_Decode()】
							 3) PID_Regulator_Decode()  【内部调用：PW_IRQHandler()】
							 4) PID_Params_Upload()			【内部调用：PW_IRQHandler()】
							 5) PW_IRQHandler()					【外部调用：外部调用：stm32f4xx_it.c的USART2中断服务函数】
							 //使用CubeMX修改工程并生成程序时需要注释掉函数stm32f4xx_it.c文件中
								 void USART2_IRQHandler(void)中的HAL_UART_IRQHandler(&huart2)。

*******************************************************************************/
/* 包含头文件 ----------------------------------------------------------------*/
#include "pid_wireless_debug.h"
#include "crc.h"
#include "motor_use_can.h"

/* 内部宏定义 ----------------------------------------------------------------*/
#define PW_huart   huart2           //pid无线调参使用串口

/* 内部自定义数据类型的变量 --------------------------------------------------*/
MsgsFrame_struct pid_regulator_msg;

/* 内部变量 ------------------------------------------------------------------*/
uint8_t usart_state = USART_Waiting;//接收状态标记	
uint8_t usart_rx_index=0;       
uint8_t usart_tx_index=0;
unsigned char usart_rx_Info[USART_RX_LEN];
unsigned char usart_tx_Info[USART_TX_LEN];
uint16_t rxLength = 8;

/* 内部函数原型声明 ----------------------------------------------------------*/
void PID_Params_Set(MsgsFrame_struct *pidSet);
void PID_Params_Upload(char motor_ID);
void PID_Regulator_Decode(void);

/* 函数主体部分 --------------------------------------------------------------*/
/**
  * @brief				PID无线调参开启串口数据接收中断
  * @param[out]		
  * @param[in]		
  * @retval				
*/
void Wireless_Debug_USART_Init(void)
{
	__HAL_UART_ENABLE_IT(&PW_huart,UART_IT_RXNE);
}

/**
  * @brief				将上位机接收到的数据设置进PID结构体中及PID参数设置
  * @param[out]		
  * @param[in]		
  * @retval				
*/
void PID_Params_Set(MsgsFrame_struct *pidSet)
{
    unsigned char index = pidSet->PID.motor_ID;
    motor_pid[index].pid_mode = pidSet->PID.PID_Mode;
    motor_pid[index].Kp = pidSet->PID.Kp_value.tempFloat;
    motor_pid[index].Ki = pidSet->PID.Ki_value.tempFloat;
    motor_pid[index].Kd = pidSet->PID.Kd_value.tempFloat;
    
    motor_pid[index].max_pout = pidSet->PID.P_out_max.tempFloat;
    motor_pid[index].max_iout = pidSet->PID.I_out_max.tempFloat;
    motor_pid[index].max_dout = pidSet->PID.D_out_max.tempFloat;
//    motor_pid[index].pidout_max = pidSet->PID.PID_out_max.tempFloat;
    
    motor_pid[index].max_out = pidSet->PID.PID_out_max.tempFloat;
}

/**
  * @brief				将从上位机接收到的数据进行解码
  * @param[out]		
  * @param[in]		
  * @retval				
*/
void PID_Regulator_Decode(void)
{
    int i = 0,j=0;    
    pid_regulator_msg.SOF = usart_rx_Info[i++];
    pid_regulator_msg.Data_Length = usart_rx_Info[i++];
    pid_regulator_msg.seq = usart_rx_Info[i++];
    pid_regulator_msg.crc8 = usart_rx_Info[i++];
    pid_regulator_msg.cmd_ID = usart_rx_Info[i++];
    if ( pid_regulator_msg.cmd_ID == 0x00)//上位机->下位机，查询板载PID参数
    {
        pid_regulator_msg.PID.motor_ID = usart_rx_Info[i++];
        
        for(j=0;j<2;j++) pid_regulator_msg.crc16.tempChar[j] = usart_rx_Info[i++];        
    }
    else if ( pid_regulator_msg.cmd_ID == 0x01)//上位机->下位机，PID参数设置
    {
        pid_regulator_msg.PID.motor_ID = usart_rx_Info[i++];
        pid_regulator_msg.PID.PID_Mode = usart_rx_Info[i++];
        for(j=0;j<4;j++) pid_regulator_msg.PID.Kp_value.tempChar[j] = usart_rx_Info[i++];
        for(j=0;j<4;j++) pid_regulator_msg.PID.Ki_value.tempChar[j] = usart_rx_Info[i++];
        for(j=0;j<4;j++) pid_regulator_msg.PID.Kd_value.tempChar[j] = usart_rx_Info[i++];
        for(j=0;j<4;j++) pid_regulator_msg.PID.P_out_max.tempChar[j] = usart_rx_Info[i++];
        for(j=0;j<4;j++) pid_regulator_msg.PID.I_out_max.tempChar[j] = usart_rx_Info[i++];
        for(j=0;j<4;j++) pid_regulator_msg.PID.D_out_max.tempChar[j] = usart_rx_Info[i++];
        for(j=0;j<4;j++) pid_regulator_msg.PID.PID_out_max.tempChar[j] = usart_rx_Info[i++];
        
        for(j=0;j<2;j++) pid_regulator_msg.crc16.tempChar[j] = usart_rx_Info[i++];
        
        //参数赋值、存储操作
        PID_Params_Set(&pid_regulator_msg);        
    }    
}

/**
  * @brief				将PID参数回传给上位机即参数上传
  * @param[out]		
  * @param[in]		
  * @retval				
*/
void PID_Params_Upload(char motor_ID)
{
    unsigned char i=0,j=0;
    static unsigned char seq = 0;
    uint8_t usart_tx[200];
    uint8_t crc8;
    uint16_t crc16;
    MsgsFrame_struct tempMsgs;
    
    tempMsgs.SOF = 0xA5;
    tempMsgs.Data_Length = DATA_LENGTH_UPLOAD_PID_PATAMS;
    tempMsgs.seq = seq;
    seq++;
    //tempMsgs.crc8 = 0;//
    tempMsgs.cmd_ID = UPLOAD_PARAMS_CMD_ID;
    tempMsgs.PID.motor_ID = motor_ID;
    tempMsgs.PID.PID_Mode = motor_pid[motor_ID].pid_mode;
    tempMsgs.PID.Kp_value.tempFloat = motor_pid[motor_ID].Kp;
    tempMsgs.PID.Ki_value.tempFloat = motor_pid[motor_ID].Ki;
    tempMsgs.PID.Kd_value.tempFloat = motor_pid[motor_ID].Kd;
    tempMsgs.PID.P_out_max.tempFloat = motor_pid[motor_ID].max_pout;//TODO
    tempMsgs.PID.I_out_max.tempFloat = motor_pid[motor_ID].max_iout;//TODO
    tempMsgs.PID.D_out_max.tempFloat = motor_pid[motor_ID].max_dout;//TODO
//    tempMsgs.PID.PID_out_max.tempFloat = motor_pid[motor_ID].pidout_max;
    tempMsgs.PID.PID_out_max.tempFloat = motor_pid[motor_ID].max_out;
    
    usart_tx[i++] = tempMsgs.SOF;
    usart_tx[i++] = tempMsgs.Data_Length;
    usart_tx[i++] = tempMsgs.seq;
    
    crc8 = Get_CRC8_Check_Sum(usart_tx,i,0xff);
    usart_tx[i++] = crc8;
    usart_tx[i++] = tempMsgs.cmd_ID;
    usart_tx[i++] = tempMsgs.PID.motor_ID;
    usart_tx[i++] = tempMsgs.PID.PID_Mode;
    for(j=0;j<4;j++) usart_tx[i++] = tempMsgs.PID.Kp_value.tempChar[j];
    for(j=0;j<4;j++) usart_tx[i++] = tempMsgs.PID.Ki_value.tempChar[j];
    for(j=0;j<4;j++) usart_tx[i++] = tempMsgs.PID.Kd_value.tempChar[j];
    for(j=0;j<4;j++) usart_tx[i++] = tempMsgs.PID.P_out_max.tempChar[j];
    for(j=0;j<4;j++) usart_tx[i++] = tempMsgs.PID.I_out_max.tempChar[j];
    for(j=0;j<4;j++) usart_tx[i++] = tempMsgs.PID.D_out_max.tempChar[j];
    for(j=0;j<4;j++) usart_tx[i++] = tempMsgs.PID.PID_out_max.tempChar[j];
    
    crc16 = Get_CRC16_Check_Sum(usart_tx,i,0xffff);
    usart_tx[i++] = crc16&0xff;
    usart_tx[i++] = (crc16>>8)&0xff;    
    
    for (j=0;j<i;j++)
    {
			while((PW_huart.Instance->SR & UART_FLAG_TC) == RESET);//未发送完成
			HAL_UART_Transmit(&PW_huart,&usart_tx[j],1,10);
    }    
}

/**
  * @brief				串口中断接收上位机信息
  * @param[out]		
  * @param[in]		
  * @retval				
*/
void PW_IRQHandler(void)    
{
		static uint8_t upload_flag=0;
		uint8_t Res = 0;
    if(PW_huart.Instance->SR & UART_FLAG_RXNE || PW_huart.Instance->SR &UART_FLAG_ORE)
	{
		Res =(uint8_t)(PW_huart.Instance->DR & (uint8_t)0x00FF);

      if( (usart_state == USART_Waiting)&&(Res == 0xA5) ) 
      {
          usart_state = USART_Receiving;
          usart_rx_index = 0;                                      //接收数组的计数器
          usart_rx_Info[usart_rx_index] = Res;
          usart_rx_index++;
      }
      else if( usart_state == USART_Receiving )
      {       
          if( usart_rx_index == 1 )//Data Length
          {
              usart_rx_Info[usart_rx_index] = Res;
              usart_rx_index++;
              rxLength = Res+7;
          }
          else if( usart_rx_index == 2 )//Seq
          {
              usart_rx_Info[usart_rx_index] = Res;
              usart_rx_index++;
          }
          else if( usart_rx_index == 3 )
          {
              usart_rx_Info[usart_rx_index] = Res;
              usart_rx_index++;					
						  if( Verify_CRC8_Check_Sum(usart_rx_Info,4)==0) //CRC校验错误，重新等待读取
              {
                rxLength=8 ;
                usart_rx_index = 0;
                usart_state = USART_Waiting;	
              }                  

          }
          else if (usart_rx_index < rxLength)
          { 				
              usart_rx_Info[usart_rx_index]=Res ;
              usart_rx_index++;
              if (usart_rx_index == rxLength) //接收完成 ，结束了
              {               
                  //首先进行crc校验应该                   -
                  if (Verify_CRC16_Check_Sum(usart_rx_Info, rxLength ))  
                  {
                      PID_Regulator_Decode();
                      upload_flag = 1;
                  }
                  rxLength=8 ;
                  usart_rx_index = 0;
                  usart_state = USART_Waiting;	                		   
              }
          }          
      }
      __HAL_UART_CLEAR_FLAG(&PW_huart,UART_FLAG_RXNE);
			__HAL_UART_CLEAR_OREFLAG(&PW_huart);
    }  
    
    if(upload_flag)
    {
        PID_Params_Upload(pid_regulator_msg.PID.motor_ID);
        upload_flag=0;
    }
}



