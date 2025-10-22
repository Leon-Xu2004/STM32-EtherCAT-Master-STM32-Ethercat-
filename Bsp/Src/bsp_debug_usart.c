#include "bsp_debug_usart.h"
#include "bsp_led.h"
#include "System_Command.h"

// 全局变量定义：UART 和 DMA 句柄
UART_HandleTypeDef huartx;
DMA_HandleTypeDef hdma_uartx_rx;

// DMA 接收缓冲区，使用 .dma_buffer 段防止数据缓存失效问题

/**
 * @brief 初始化 USART3 + DMA + 空闲中断，用于不定长串口数据接收
 */
void MX_UARTx_Init(void)
{
    // ① 打开外设时钟：GPIOB、USART3、DMA1
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_USART3_CLK_ENABLE();
    __HAL_RCC_DMA1_CLK_ENABLE();

    // ② 初始化 GPIOB10(TX) 和 GPIOB11(RX)
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Pin = GPIO_PIN_10;  // USART3_TX
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;  // 复用推挽输出
    GPIO_InitStruct.Pull = GPIO_NOPULL;      // 无上拉/下拉
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART3;  // 选择复用功能 AF7
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_11;  // USART3_RX
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    // ③ 初始化 DMA：串口接收 USART3_RX -> 内存 uart_rx_buffer
    hdma_uartx_rx.Instance = DMA1_Stream1;
    hdma_uartx_rx.Init.Request = DMA_REQUEST_USART3_RX;
    hdma_uartx_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;  // 外设 -> 内存
    hdma_uartx_rx.Init.PeriphInc = DMA_PINC_DISABLE;      // 外设地址不递增
    hdma_uartx_rx.Init.MemInc = DMA_MINC_ENABLE;          // 内存地址递增
    hdma_uartx_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_uartx_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_uartx_rx.Init.Mode = DMA_NORMAL;  // 接收一次中断一次
    hdma_uartx_rx.Init.Priority = DMA_PRIORITY_HIGH;
    hdma_uartx_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    HAL_DMA_Init(&hdma_uartx_rx);

    // ④ 将 DMA 和 UART 绑定
    __HAL_LINKDMA(&huartx, hdmarx, hdma_uartx_rx);

    // ⑤ 初始化串口 USART3 参数
    huartx.Instance = USART3;
    huartx.Init.BaudRate = 115200;
    huartx.Init.WordLength = UART_WORDLENGTH_8B;
    huartx.Init.StopBits = UART_STOPBITS_1;
    huartx.Init.Parity = UART_PARITY_NONE;
    huartx.Init.Mode = UART_MODE_TX_RX;
    huartx.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huartx.Init.OverSampling = UART_OVERSAMPLING_16;
    huartx.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
    huartx.Init.Prescaler = UART_PRESCALER_DIV1;
    huartx.Init.FIFOMode = UART_FIFOMODE_DISABLE;
    huartx.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
    HAL_UART_Init(&huartx);

    // ⑥ 启动 DMA 接收和空闲中断（IDLE）
    HAL_UART_Receive_DMA(&huartx, uart_rx_buffer, UART_BUFFER_SIZE);
    __HAL_UART_ENABLE_IT(&huartx, UART_IT_IDLE);

    // ⑦ 启用 USART3 和 DMA 中断
    HAL_NVIC_SetPriority(USART3_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(USART3_IRQn);
    HAL_NVIC_SetPriority(DMA1_Stream1_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(DMA1_Stream1_IRQn);
}

/**
 * @brief 重定向 printf 到串口输出
 */
int fputc(int ch, FILE *f)
{
    HAL_UART_Transmit(&huartx, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
    return ch;
}
