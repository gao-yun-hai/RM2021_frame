#ifndef __MYINCLUDE_H
#define __MYINCLUDE_H

#include "stm32f4xx_hal.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdio.h>

#include "can.h"
#include "dma.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "cmsis_os.h"

#include "user_lib.h"

#define GREEN_ON()    HAL_GPIO_WritePin(GREEN_LED_GPIO_Port,GREEN_LED_Pin,GPIO_PIN_RESET)
#define GREEN_OFF()   HAL_GPIO_WritePin(GREEN_LED_GPIO_Port,GREEN_LED_Pin,GPIO_PIN_SET)
#define	GREEN_TOG() 	HAL_GPIO_TogglePin(GREEN_LED_GPIO_Port,GREEN_LED_Pin)

#define LED1_ON()     HAL_GPIO_WritePin(LED1_GPIO_Port,LED1_Pin,GPIO_PIN_RESET)
#define LED1_OFF()    HAL_GPIO_WritePin(LED1_GPIO_Port,LED1_Pin,GPIO_PIN_SET)
#define	LED1_TOG() 	  HAL_GPIO_TogglePin(LED1_GPIO_Port,LED1_Pin)

#define LED2_ON()     HAL_GPIO_WritePin(LED2_GPIO_Port,LED2_Pin,GPIO_PIN_RESET)
#define LED2_OFF()    HAL_GPIO_WritePin(LED2_GPIO_Port,LED2_Pin,GPIO_PIN_SET)
#define	LED2_TOG() 	  HAL_GPIO_TogglePin(LED2_GPIO_Port,LED2_Pin)

#define LED3_ON()     HAL_GPIO_WritePin(LED3_GPIO_Port,LED3_Pin,GPIO_PIN_RESET)
#define LED3_OFF()    HAL_GPIO_WritePin(LED3_GPIO_Port,LED3_Pin,GPIO_PIN_SET)
#define	LED3_TOG() 	  HAL_GPIO_TogglePin(LED3_GPIO_Port,LED3_Pin)

#define LED4_ON()     HAL_GPIO_WritePin(LED4_GPIO_Port,LED4_Pin,GPIO_PIN_RESET)
#define LED4_OFF()    HAL_GPIO_WritePin(LED4_GPIO_Port,LED4_Pin,GPIO_PIN_SET)
#define	LED4_TOG() 	  HAL_GPIO_TogglePin(LED4_GPIO_Port,LED4_Pin)

#define LED5_ON()     HAL_GPIO_WritePin(LED5_GPIO_Port,LED5_Pin,GPIO_PIN_RESET)
#define LED5_OFF()    HAL_GPIO_WritePin(LED5_GPIO_Port,LED5_Pin,GPIO_PIN_SET)
#define	LED5_TOG() 	  HAL_GPIO_TogglePin(LED5_GPIO_Port,LED5_Pin)

#define LED6_ON()     HAL_GPIO_WritePin(LED6_GPIO_Port,LED6_Pin,GPIO_PIN_RESET)
#define LED6_OFF()    HAL_GPIO_WritePin(LED6_GPIO_Port,LED6_Pin,GPIO_PIN_SET)
#define	LED6_TOG() 	  HAL_GPIO_TogglePin(LED6_GPIO_Port,LED6_Pin)

#define LED7_ON()     HAL_GPIO_WritePin(LED7_GPIO_Port,LED7_Pin,GPIO_PIN_RESET)
#define LED7_OFF()    HAL_GPIO_WritePin(LED7_GPIO_Port,LED7_Pin,GPIO_PIN_SET)
#define	LED7_TOG() 	  HAL_GPIO_TogglePin(LED7_GPIO_Port,LED7_Pin)

#define LED8_ON()     HAL_GPIO_WritePin(LED8_GPIO_Port,LED8_Pin,GPIO_PIN_RESET)
#define LED8_OFF()    HAL_GPIO_WritePin(LED8_GPIO_Port,LED8_Pin,GPIO_PIN_SET)
#define	LED8_TOG() 	  HAL_GPIO_TogglePin(LED8_GPIO_Port,LED8_Pin)

typedef enum
{
	REMOTE_PERIOD = 7,
	VISION_PERIOD = 1,
	REFEREE_PERIOD = 10,
	GIMBAL_PERIOD = 5,
	CHASSIS_PERIOD = 2,
	FRICTION_PERIOD = 5,
	TRIGGER_PERIOD = 2,
	OFFLINE_PERIOD = 10,
  PRINTF_PERIOD = 5,
	
}Delay_Period;

extern osThreadId RemoteDataTaskHandle;
extern osThreadId VisionDataTaskHandle;
extern osThreadId RefereeDataTaskHandle;
extern osThreadId TriggerDriveTaskHandle;
extern osThreadId FrictionDriveTaskHandle;
extern osThreadId GimbalControlTaskHandle;		
extern osThreadId ChassisControlTaskHandle;	

#endif


