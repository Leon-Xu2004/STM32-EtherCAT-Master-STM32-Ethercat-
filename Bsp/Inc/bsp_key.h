/**
  ******************************************************************************
  * 文件名程: bsp_key.h 
  * 作    者: 硬石嵌入式开发团队
  * 版    本: V1.0
  * 编写日期: 2018-09-01
  * 功    能: KEY-API
  ******************************************************************************
  * 说明：
  * 本例程配套硬石stm32开发板YS-H7Multi使用。
  * 
  * 淘宝：
  * 论坛：http://www.ing10bbs.com
  * 版权归硬石嵌入式开发团队所有，请勿商用。
  ******************************************************************************
  */

#ifndef __BSP_KEY_H_
#define __BSP_KEY_H_

/* 包含头文件 ----------------------------------------------------------------*/
#include "stm32h7xx_hal.h"

/* 类型定义 ------------------------------------------------------------------*/

typedef enum
{
  KEY_UP   = 0,
  KEY_DOWN = 1,
}KEYState_TypeDef;

/* 宏定义 --------------------------------------------------------------------*/

/* GPIO初始化定义 */
#define KEY1_RCC_CLK_ENABLE           __HAL_RCC_GPIOI_CLK_ENABLE
#define KEY1_GPIO_PIN                 GPIO_PIN_8
#define KEY1_GPIO                     GPIOI
#define KEY1_DOWN_LEVEL               GPIO_PIN_RESET /* 根据原理图，KEY1按下时引脚为低电平，
                                                        所以这里设置为0 */
#define KEY2_RCC_CLK_ENABLE           __HAL_RCC_GPIOC_CLK_ENABLE
#define KEY2_GPIO_PIN                 GPIO_PIN_13
#define KEY2_GPIO                     GPIOC
#define KEY2_DOWN_LEVEL               GPIO_PIN_RESET /* 根据原理图，KEY2按下时引脚为低电平，
                                                        所以这里设置为0 */
#define KEY3_RCC_CLK_ENABLE           __HAL_RCC_GPIOI_CLK_ENABLE
#define KEY3_GPIO_PIN                 GPIO_PIN_10
#define KEY3_GPIO                     GPIOI
#define KEY3_DOWN_LEVEL               GPIO_PIN_RESET /* 根据原理图，KEY3按下时引脚为低电平，
                                                        所以这里设置为0 */
#define KEY4_RCC_CLK_ENABLE           __HAL_RCC_GPIOI_CLK_ENABLE
#define KEY4_GPIO_PIN                 GPIO_PIN_11
#define KEY4_GPIO                     GPIOI
#define KEY4_DOWN_LEVEL               GPIO_PIN_RESET /* 根据原理图，KEY4按下时引脚为低电平，
                                                        所以这里设置为0 */
/* 扩展变量 ------------------------------------------------------------------*/

/* 函数声明 ------------------------------------------------------------------*/
void KEY_GPIO_Init(void);
KEYState_TypeDef KEY1_StateRead(void);
KEYState_TypeDef KEY2_StateRead(void);
KEYState_TypeDef KEY3_StateRead(void);
KEYState_TypeDef KEY4_StateRead(void);

#endif /* __BSP_KEY_H_ */

/********** (C) COPYRIGHT 2019-2030 硬石嵌入式开发团队 *******END OF FILE******/
