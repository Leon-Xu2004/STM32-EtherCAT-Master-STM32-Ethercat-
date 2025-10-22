#ifndef __BSP_UART_H
#define __BSP_UART_H

#include "stm32h7xx_hal.h"

extern UART_HandleTypeDef huartx;
extern DMA_HandleTypeDef hdma_uartx_rx;

void MX_UARTx_Init(void);

#endif
