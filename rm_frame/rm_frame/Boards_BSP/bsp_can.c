/*******************************************************************************
                      版权所有 (C), 2020-,NCUROBOT
 *******************************************************************************
  文 件 名   : bsp_can.c
  版 本 号   : V1.0
  作    者   : 高云海
  生成日期   : 2020.11.18
  最近修改   :
  功能描述   : CAN通信初始化【开启CAN、开启CAN接收中断、CAN通信滤波器初始化】
  函数列表   : 1) BSP_CAN_Init() 			【外部调用：bsp.c】   
*******************************************************************************/
/* 包含头文件 ----------------------------------------------------------------*/
#include "bsp_can.h"

/* 内部宏定义 ----------------------------------------------------------------*/

/* 内部自定义数据类型的变量 --------------------------------------------------*/

/* 内部变量 ------------------------------------------------------------------*/

/* 内部函数原型声明 ----------------------------------------------------------*/

/* 函数主体部分 --------------------------------------------------------------*/
/**
  * @brief				CAN初始化函数
  * @param[out]		
  * @param[in]		
  * @retval				
*/
uint8_t BSP_CAN_Init(CAN_HandleTypeDef* hcan)    
{
	uint8_t status=0;
	CAN_FilterTypeDef canFilter;
	
	/*can过滤器初始化*/	
	canFilter.FilterIdHigh=0;
	canFilter.FilterIdLow=0;
	canFilter.FilterMaskIdHigh=0;
	canFilter.FilterMaskIdLow=0;
	canFilter.FilterMode=CAN_FILTERMODE_IDMASK;  							//掩码模式
	canFilter.FilterActivation=CAN_FILTER_ENABLE;							//开启
	canFilter.FilterScale=CAN_FILTERSCALE_32BIT; 							//32位模式
	canFilter.FilterFIFOAssignment=CAN_FILTER_FIFO0; 					//链接到fifo0
	canFilter.SlaveStartFilterBank=14;												//can2筛选组起始编号		
	
	if(hcan == &hcan1)
  {
    canFilter.FilterBank = 0;    														//筛选器组0
  }
  if(hcan == &hcan2)
  {
    canFilter.FilterBank = 14;															//筛选器组14
  }
	
	status|=HAL_CAN_ConfigFilter(hcan,&canFilter);					//配置过滤器
	
	/*开启CAN及CAN中断*/
	HAL_CAN_Start(hcan);					
	HAL_CAN_ActivateNotification(hcan, CAN_IT_RX_FIFO0_MSG_PENDING);       //can1 接收fifo 0不为空中断	
	
	return status;
}

