#ifndef __eth_H
#define __eth_H
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

extern ETH_HandleTypeDef heth;

/* ˽�к���ԭ�� --------------------------------------------------------------*/

/* ������ --------------------------------------------------------------------*/

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

