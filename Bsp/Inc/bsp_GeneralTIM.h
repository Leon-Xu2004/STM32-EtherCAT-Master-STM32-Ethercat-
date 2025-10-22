#ifndef __tim_H
#define __tim_H
#ifdef __cplusplus
 extern "C" {
#endif

/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "stm32h7xx_hal.h"
#include "main.h"

/* ˽�����Ͷ��� --------------------------------------------------------------*/

/* ˽�к궨�� ----------------------------------------------------------------*/

/* ˽�б��� ------------------------------------------------------------------*/

/* ��չ���� ------------------------------------------------------------------*/

extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;

extern uint16_t tim4_arr;

/* ˽�к���ԭ�� --------------------------------------------------------------*/

void MX_TIM2_Init(void);
void MX_TIM3_Init(void);
void MX_TIM4_Init(void);


uint32_t GetSec(void);
uint32_t GetUSec(void);


#ifdef __cplusplus
}
#endif
#endif /*__ tim_H */

