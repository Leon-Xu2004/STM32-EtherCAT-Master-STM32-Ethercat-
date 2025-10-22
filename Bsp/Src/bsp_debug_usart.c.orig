/**
  ******************************************************************************
  * 文件名程: main.c 
  * 作    者: 硬石嵌入式开发团队
  * 版    本: V1.0
  * 编写日期: 2018-09-01
  * 功    能: 串口通信功能实现
  ******************************************************************************
  * 说明：
  * 本例程配套硬石stm32开发板YS-H7Multi使用。
  * 
  * 淘宝：
  * 论坛：http://www.ing10bbs.com
  * 版权归硬石嵌入式开发团队所有，请勿商用。
  ******************************************************************************
  */

/* 包含头文件 ----------------------------------------------------------------*/
#include "bsp_debug_usart.h"
#include <stdio.h>

/* 私有类型定义 --------------------------------------------------------------*/

/* 私有宏定义 ----------------------------------------------------------------*/

/* 私有变量 ------------------------------------------------------------------*/
UART_HandleTypeDef husart_debug={0};

/* 扩展变量 ------------------------------------------------------------------*/

/* 私有函数原形 --------------------------------------------------------------*/

/* 函数体 --------------------------------------------------------------------*/

/**
  * 函数功能: 板载串口引脚初始化
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 初始化串口引脚，本函数有HAL库调用
  */
static void YS_UARTx_MspInit(void)
{

  GPIO_InitTypeDef GPIO_InitStruct;

  /* USART 时钟使能 */
  DEBUG_USART_RCC_CLK_ENABLE();
  /* 引脚 时钟使能 */
  DEBUG_USARTx_GPIO_ClK_ENABLE();
  
  GPIO_InitStruct.Pin = DEBUG_USARTx_Tx_GPIO_PIN |\
                        DEBUG_USARTx_Rx_GPIO_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = DEBUG_USARTx_AFx;
  HAL_GPIO_Init(DEBUG_USARTx_TxRx_GPIO, &GPIO_InitStruct);

  /* USART 中断优先级配置*/
  HAL_NVIC_SetPriority(DEBUG_USART_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DEBUG_USART_IRQn);

}

/**
  * 函数功能: 板载串口通信初始化
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 初始化串口通信功能
  */
void MX_DEBUG_UART_Init(void)
{
  /* 串口通信GPIO初始化 */
  YS_UARTx_MspInit();
  /* 串口通信功能配置 */
  husart_debug.Instance = DEBUG_USARTx;
  husart_debug.Init.BaudRate   = DEBUG_USARTx_BAUDRATE;
  husart_debug.Init.WordLength = UART_WORDLENGTH_8B;
  husart_debug.Init.StopBits   = UART_STOPBITS_1;
  husart_debug.Init.Parity     = UART_PARITY_NONE;
  husart_debug.Init.Mode       = UART_MODE_TX_RX;
  husart_debug.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
  husart_debug.Init.OverSampling    = UART_OVERSAMPLING_16;
  husart_debug.Init.OneBitSampling  = UART_ONE_BIT_SAMPLE_DISABLE;
  husart_debug.Init.Prescaler       = UART_PRESCALER_DIV1;
  husart_debug.Init.FIFOMode        = UART_FIFOMODE_DISABLE;
  husart_debug.Init.TXFIFOThreshold = UART_TXFIFO_THRESHOLD_1_8;
  husart_debug.Init.RXFIFOThreshold = UART_RXFIFO_THRESHOLD_1_8;
  husart_debug.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  HAL_UART_Init(&husart_debug);

}

/**
  * 函数功能: 重定向c库函数printf到DEBUG_USARTx
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
int fputc(int ch, FILE *f)
{
  HAL_UART_Transmit(&husart_debug, (uint8_t *)&ch, 1, 0xffff);
  return ch;
}

/**
  * 函数功能: 重定向c库函数getchar,scanf到DEBUG_USARTx
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
int fgetc(FILE * f)
{
  uint8_t ch = 0;
  HAL_UART_Receive(&husart_debug,&ch, 1, 0xffff);
  return ch;
}


/********** (C) COPYRIGHT 2019-2030 硬石嵌入式开发团队 *******END OF FILE******/
