#ifndef __BSP_LED_H_
#define __BSP_LED_H_

/* 包含头文件 ----------------------------------------------------------------*/
#include "stm32h7xx_hal.h"

/* 类型定义 ------------------------------------------------------------------*/
typedef enum
{
  LED_OFF = 0,
  LED_ON  = 1,
  LED_TOGGLE = 2,
}LEDState_TypeDef;
#define IS_LED_STATE(STATE)           (((STATE) == LED_OFF) || ((STATE) == LED_ON) || ((STATE) == LED_TOGGLE))

/* 宏定义 --------------------------------------------------------------------*/
#define LED1                        (uint8_t)0x01
#define LED2                        (uint8_t)0x02
#define LED3                        (uint8_t)0x04
#define IS_LED_TYPEDEF(LED)         (((LED) == LED1) || ((LED) == LED2) || ((LED) == LED3))

/* GPIO初始化定义 */
#define LED1_RCC_GPIO_CLK_ENABLE()  __HAL_RCC_GPIOE_CLK_ENABLE()
#define LED1_PIN                    GPIO_PIN_5
#define LED1_GPIO_PORT              GPIOE

#define LED2_RCC_GPIO_CLK_ENABLE()  __HAL_RCC_GPIOE_CLK_ENABLE()
#define LED2_PIN                    GPIO_PIN_6
#define LED2_GPIO_PORT              GPIOE

#define LED3_RCC_GPIO_CLK_ENABLE()  __HAL_RCC_GPIOB_CLK_ENABLE()
#define LED3_PIN                    GPIO_PIN_12
#define LED3_GPIO_PORT              GPIOB

/* 宏定义函数 */
#define LED1_ON()                   HAL_GPIO_WritePin(LED1_GPIO_PORT,LED1_PIN, GPIO_PIN_SET);
#define LED1_OFF()                  HAL_GPIO_WritePin(LED1_GPIO_PORT,LED1_PIN, GPIO_PIN_RESET);
#define LED1_TOGGLE()               HAL_GPIO_TogglePin(LED1_GPIO_PORT,LED1_PIN);

#define LED2_ON()                   HAL_GPIO_WritePin(LED2_GPIO_PORT,LED2_PIN, GPIO_PIN_SET);
#define LED2_OFF()                  HAL_GPIO_WritePin(LED2_GPIO_PORT,LED2_PIN, GPIO_PIN_RESET);
#define LED2_TOGGLE()               HAL_GPIO_TogglePin(LED2_GPIO_PORT,LED2_PIN);

#define LED3_ON()                   HAL_GPIO_WritePin(LED3_GPIO_PORT,LED3_PIN, GPIO_PIN_SET);
#define LED3_OFF()                  HAL_GPIO_WritePin(LED3_GPIO_PORT,LED3_PIN, GPIO_PIN_RESET);
#define LED3_TOGGLE()               HAL_GPIO_TogglePin(LED3_GPIO_PORT,LED3_PIN);

/* 扩展变量 ------------------------------------------------------------------*/

/* 函数声明 ------------------------------------------------------------------*/

void LED_GPIO_Init(void);
void LEDx_StateSet(uint8_t LEDx,LEDState_TypeDef state);

#endif /* __BSP_LED_H_ */

