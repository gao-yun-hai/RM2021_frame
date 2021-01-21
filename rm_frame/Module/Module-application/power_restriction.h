#ifndef __POWER_RESTRICTION_H
#define __POWER_RESTRICTION_H
/* 包含头文件 ----------------------------------------------------------------*/
#include "myinclude.h"



/* 本模块向外部提供的宏定义 --------------------------------------------------*/

/* 本模块向外部提供的数据类型定义 --------------------------------------------*/
typedef struct{
	
 float  CurrentBuff1_get; 			 		//第一层滤波结果
 float  CurrentBuff2_fliter[11];		//第二层滤波缓存
 float  CurrentBuff2_get;					  //第二层滤波结果
 float  CurrentCalculat;			 			//电流计算结果
 float  Current_Offset;						  //电流偏置量(和裁判系统比较)
 float  Current_Referee;						//来自裁判系统的电流值
int16_t  Current_Offset_num;

}Current_GET;   //电流采集

typedef struct{
		
 float  Volt_Referee;								//来自裁判系统的电压值
 float  Power_Referee;							//来自裁判系统的功率
 float  Power_Referee_Last;         
 float  Power_Calculat;							//自行计算的功率值
 float  PowerRemain_Referee;				//来自裁判系统的剩余缓存能量
 float  PowerRemain_Referee_last;		//来自裁判系统的上一次剩余缓存能量
 float  PowerRemain_Calculat;				//自行计算的裁判系统的剩余缓存能量
 float  PowerRemain_Calculat_Last;  //自行计算的上一次裁判系统的剩余缓存能量
 float  PowerRemain_Calculat_Next;
 float  PowerLimit;									//最终限制值
 
}Limit;				 //电流限制




typedef struct{

	uint32_t time_now;
	uint32_t time_last;
	int32_t time;
	int32_t total_time_ms;
	double total_time_s;
	
}MyTimeTick;
/* 本模块向外部提供的接口常量声明 --------------------------------------------*/
extern  uint32_t  uhADCxConvertedValue[10];  //ADC缓存数据
extern  Current_GET  current_get;
extern  Limit  limit;
extern  MyTimeTick  time_for_limit;
/* 本模块向外部提供的接口函数原型声明 ----------------------------------------*/
void power_limit(float  Current_get[2]);
float Limit_filter(float oldData,float newData,float val);

#endif

