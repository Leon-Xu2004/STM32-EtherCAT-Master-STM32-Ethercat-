#ifndef __eth_H
#define __eth_H
#ifdef __cplusplus
 extern "C" {
#endif

/* 包含头文件 ----------------------------------------------------------------*/
#include "stm32h7xx_hal.h"
#include "main.h"

/* 私有类型定义 --------------------------------------------------------------*/

/* 私有宏定义 ----------------------------------------------------------------*/

/* 私有变量 ------------------------------------------------------------------*/

/* 扩展变量 ------------------------------------------------------------------*/

extern ETH_HandleTypeDef heth;

/* 私有函数原形 --------------------------------------------------------------*/

/* 函数体 --------------------------------------------------------------------*/

void MX_ETH_Init(void);
void PHY_Init(void);
void low_level_output(uint8_t *p,uint32_t length);
int bfin_EMAC_send (void *packet, int length);
int bfin_EMAC_recv (uint8_t * packet, size_t size);
void printfBuffer(uint8_t *dat, uint32_t len );


#ifdef __cplusplus
}
#endif
#endif /*__ eth_H */

