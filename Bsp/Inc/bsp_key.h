/**
  ******************************************************************************
  * �ļ�����: bsp_key.h 
  * ��    ��: ӲʯǶ��ʽ�����Ŷ�
  * ��    ��: V1.0
  * ��д����: 2018-09-01
  * ��    ��: KEY-API
  ******************************************************************************
  * ˵����
  * ����������Ӳʯstm32������YS-H7Multiʹ�á�
  * 
  * �Ա���
  * ��̳��http://www.ing10bbs.com
  * ��Ȩ��ӲʯǶ��ʽ�����Ŷ����У��������á�
  ******************************************************************************
  */

#ifndef __BSP_KEY_H_
#define __BSP_KEY_H_

/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "stm32h7xx_hal.h"

/* ���Ͷ��� ------------------------------------------------------------------*/

typedef enum
{
  KEY_UP   = 0,
  KEY_DOWN = 1,
}KEYState_TypeDef;

/* �궨�� --------------------------------------------------------------------*/

/* GPIO��ʼ������ */
#define KEY1_RCC_CLK_ENABLE           __HAL_RCC_GPIOI_CLK_ENABLE
#define KEY1_GPIO_PIN                 GPIO_PIN_8
#define KEY1_GPIO                     GPIOI
#define KEY1_DOWN_LEVEL               GPIO_PIN_RESET /* ����ԭ��ͼ��KEY1����ʱ����Ϊ�͵�ƽ��
                                                        ������������Ϊ0 */
#define KEY2_RCC_CLK_ENABLE           __HAL_RCC_GPIOC_CLK_ENABLE
#define KEY2_GPIO_PIN                 GPIO_PIN_13
#define KEY2_GPIO                     GPIOC
#define KEY2_DOWN_LEVEL               GPIO_PIN_RESET /* ����ԭ��ͼ��KEY2����ʱ����Ϊ�͵�ƽ��
                                                        ������������Ϊ0 */
#define KEY3_RCC_CLK_ENABLE           __HAL_RCC_GPIOI_CLK_ENABLE
#define KEY3_GPIO_PIN                 GPIO_PIN_10
#define KEY3_GPIO                     GPIOI
#define KEY3_DOWN_LEVEL               GPIO_PIN_RESET /* ����ԭ��ͼ��KEY3����ʱ����Ϊ�͵�ƽ��
                                                        ������������Ϊ0 */
#define KEY4_RCC_CLK_ENABLE           __HAL_RCC_GPIOI_CLK_ENABLE
#define KEY4_GPIO_PIN                 GPIO_PIN_11
#define KEY4_GPIO                     GPIOI
#define KEY4_DOWN_LEVEL               GPIO_PIN_RESET /* ����ԭ��ͼ��KEY4����ʱ����Ϊ�͵�ƽ��
                                                        ������������Ϊ0 */
/* ��չ���� ------------------------------------------------------------------*/

/* �������� ------------------------------------------------------------------*/
void KEY_GPIO_Init(void);
KEYState_TypeDef KEY1_StateRead(void);
KEYState_TypeDef KEY2_StateRead(void);
KEYState_TypeDef KEY3_StateRead(void);
KEYState_TypeDef KEY4_StateRead(void);

#endif /* __BSP_KEY_H_ */

/********** (C) COPYRIGHT 2019-2030 ӲʯǶ��ʽ�����Ŷ� *******END OF FILE******/
