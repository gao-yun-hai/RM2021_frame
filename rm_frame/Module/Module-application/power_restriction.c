/*******************************************************************************
                      版权所有 (C), 2017-,NCUROBOT
 *******************************************************************************
  文 件 名   : power_restriction.c
  版 本 号   : 初稿
  作    者   : NCUERM
  生成日期   : 2018年7月
  最近修改   :
  功能描述   : 底盘功率限制
  函数列表   :void power_limit(float  Current_get)


*******************************************************************************/
/*
* 电压输出与电流输入的关系:
*			VOUT = VOUT(Q) + Vsens*I;
*	VOUT(Q)为静态输出电压，即VCC/2，也就是无输入时，输出的电压。Vsens是传感器灵敏度
*（使用的型号的系数是40MV/A）I的方向是从IP+流向IP-的电流
*	eg:
*			VCC为5V，I电流为10A，那么输出即为5V / 2 + 40MV/A * 10A = 2.9V
*			VCC为3.3V，I电流为10A，那么输出即为3.3V / 2 + 40MV/A*10A = 2.05V
* 结果：
*			I = (VOUT - VOUT(Q))/Vsens
*			I = (V_get - 2.5)/0.04; 	  //接5V电压
*			I = (V_get - 1.65)/0.04;		//接3.3V电压
*/
/* 包含头文件 ----------------------------------------------------------------*/
#include "power_restriction.h"
#include "referee.h"
#include "user_lib.h"
/* 内部自定义数据类型 --------------------------------------------------------*/

/* 内部宏定义 ----------------------------------------------------------------*/
#define mylimit 20
#define mylimitpower 200
#define MyAbs(x) ((x > 0) ? (x) : (-x))
#define VAL_LIMIT(val, min, max)\
if(val<=min)\
{\
	val = min;\
}\
else if(val>=max)\
{\
	val = max;\
}\

//#define Predict_Time  0.05fb
#define Predict_RemainPower 20
/* 任务相关信息定义-----------------------------------------------------------*/

/* 内部常量定义---------------------------------------------------------------*/

/* 外部变量声明 --------------------------------------------------------------*/

/* 外部函数原型声明 ----------------------------------------------------------*/

/* 内部变量 ------------------------------------------------------------------*/
/*
*
*	数据说明:ADC缓存数据 , 乘以系数0.8058608{3300/4095}
*  			  得到实际电压值
*	采样时间:采集一次数据的时间是(转换周期+采样周期)/时钟(12位采集是15个周期)
*				  所以配置的采样时间是(12+3)/(84/4)M = 0.7142us
*	   Note:实际调用数据的间隔最小5ms，所以创建一个长度为10的数组，每次使用数据的时候
*		   		对10个数据取平均值，然后再创建一个10位的数组存储历史数据，进行窗口滑动滤
*					波 (因为现在还没有硬件设计，暂时用这种方法简单滤波),数据有效时长延长到0.7ms
*/
uint32_t  uhADCxConvertedValue[10] = {0};  
Limit  limit = {.PowerRemain_Calculat = mylimitpower};
Current_GET  current_get = {0};
MyTimeTick  time_for_limit = {0}; //限制时间计时
MyTimeTick  time_for_RP = {0};  //remain power 缓存能量计算计时
//int16_t angle1[4];

//测试变量
int limit_flag = 0;
/* 函数原型声明 ----------------------------------------------------------*/

/*
** Descriptions: 粗略测量任务时间(s)
** Input: 时间结构体的指针
** Output: NULL
*/
void	MyTime_statistics(MyTimeTick *time_tick)
{

	time_tick->time_now = HAL_GetTick();
	if(time_tick->time_last == 0)//避免第一次计数的时候last为零
	{
		time_tick->time = time_tick->time_now - time_tick->time_now;
	}else
	{
		time_tick->time = time_tick->time_now - time_tick->time_last;		
	}
	time_tick->time_last = time_tick->time_now;
	//切记清零总时间
	time_tick->total_time_ms += time_tick->time;
	time_tick->total_time_s = time_tick->total_time_ms * 0.001f;
}
/*
** Descriptions: 清空时间结构体
** Input: 
**				MyTimeTick *: 		时间结构体的指针
**				flag:  选择清空的内容
**								flag = 1  清空全部
**								flag = 2	清空总时间(防止连续调用时的计时溢出错误)
** Output: NULL
*/
void MyTime_memset(MyTimeTick *time_tick ,char flag)
{
	if(flag == 1)
	{
		memset(time_tick,0,sizeof(MyTimeTick));
	}else if(flag == 2)
	{
		time_tick->total_time_ms = 0;
		time_tick->total_time_s = 0;
	}
	
}

/*
** Descriptions: ADC数据采集并滤波
** Input: NULL
** Output: 数据采集值
*/
void Get_ADC_Value(void)
{
		uint32_t *buff = uhADCxConvertedValue;
		//第一个滤波要关闭ADC的dma
		int getbuff = 0;
		for(uint8_t i = 0;i < 10;i++)
		{
			getbuff += buff[i];
		}
		/*current_get.CurrentBuff1_get =  Median_value_fliter(buff,10);
			current_get.CurrentBuff1_get =  Median_average_fliter(buff,10);*/
		current_get.CurrentBuff1_get =  getbuff * 0.1f;
		current_get.CurrentBuff2_fliter[10] = current_get.CurrentBuff1_get;
		current_get.CurrentBuff2_get = Window_sliding_filter(current_get.CurrentBuff2_fliter)*0.1f;
	
		if(current_get.Current_Offset_num > 200)
		{
			
			current_get.CurrentCalculat = (current_get.CurrentBuff2_get * (0.00080566f) - 2.50f) * 25.0f -
																		 current_get.Current_Offset;
		}
		else
		{
			current_get.Current_Offset_num++;
			current_get.CurrentCalculat = (current_get.CurrentBuff2_get * (0.00080566f) - 2.50f) * 25.0f;
			
			if(current_get.Current_Offset_num > 50)
			{		
				current_get.Current_Offset += current_get.CurrentCalculat - current_get.Current_Referee;
			}
			if(current_get.Current_Offset_num > 200)
			{				
				current_get.Current_Offset = current_get.Current_Offset/150.0f;
			}
		}

}

/*
** Descriptions: 缓存能量计算
** Input: NULL
** Output: NULL
*/

void Remain_Power_Calculate(void)
{
		/*采集时间*/
		MyTime_statistics(&time_for_RP);
	
		/*能量缓存计算*/
		if(limit.Power_Calculat >80)
		{
			limit.PowerRemain_Calculat -= (limit.Power_Calculat - 80) * time_for_RP.time * 0.01f;
		}
		if(limit.Power_Calculat <80)
		{
			limit.PowerRemain_Calculat = 60-(limit.Power_Calculat - 80) * time_for_RP.time * 0.01f;
		}
		
		/*清空总时间*/
		MyTime_memset(&time_for_RP,2);		
		
		/*恢复能量缓存*/
		VAL_LIMIT(limit.PowerRemain_Calculat, 0.0f, 60.0f);

}

/*
** Descriptions: 功率限制
** Input: NULL
** Output: 功率限制值
*/

/*
*	如何充分利用裁判系统传回的功率和能量缓存
*
*/
void power_limit(float  Current_get[2])
{
		
//		float total_current = 0;
	

////		total_current = MyAbs(Current_get[0]) + MyAbs(Current_get[1]); 
//		total_current = MyAbs(Current_get[0]); 
		
//		/*电流采集*/
//		Get_ADC_Value();

		/*功率计算*/
//		if(limit.Volt_Referee != 0)//防止裁判系统失效
//		{
//			limit.Power_Calculat = current_get.CurrentCalculat * limit.Volt_Referee;

//		}else
//		{
//  			limit.Power_Calculat = current_get.CurrentCalculat * 23.0f;		
//		}
		
		/*能量缓存计算*/
//		Remain_Power_Calculate();
		
		/*功率限制*/
		//	limit.PowerRemain_Calculat_Last=limit.PowerRemain_Calculat;
//		limit.PowerRemain_Calculat_Last = referee.power_heat_data.chassis_power_buffer;
//		limit.Power_Calculat = referee.power_heat_data.chassis_power;
		
				

//			if(limit.Power_Calculat - 20 <=0)
//			{
//				limit.Power_Calculat  = 20;
//			}
//			
//			if(((limit.PowerRemain_Calculat_Last - (limit.Power_Calculat - 20) * 0.2 ) < Predict_RemainPower)||(limit_flag==1))
//			{
//				limit_flag = 1;
//				limit.PowerLimit = 600 + 0.102 * (limit.PowerRemain_Calculat_Last * limit.PowerRemain_Calculat_Last);//0.1
//				
//			/*电流限制*/
//				Current_get[0] = (Current_get[0]/(total_current + 1.0f)) * limit.PowerLimit; 
//		//		Current_get[1] = (Current_get[1]/(total_current + 1.0f)) * limit.PowerLimit; 

//			}
//		 if(limit.PowerRemain_Calculat_Last >= 190)
//			{
//				limit_flag = 0;	  				
//			}
//			else limit.PowerLimit = 0;
}

