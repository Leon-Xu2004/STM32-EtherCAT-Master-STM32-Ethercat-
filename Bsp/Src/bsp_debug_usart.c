#include "bsp_debug_usart.h"
#include "bsp_led.h"
#include "System_Command.h"

// ȫ�ֱ������壺UART �� DMA ���
UART_HandleTypeDef huartx;
DMA_HandleTypeDef hdma_uartx_rx;

// DMA ���ջ�������ʹ�� .dma_buffer �η�ֹ���ݻ���ʧЧ����

/**
 * @brief ��ʼ�� USART3 + DMA + �����жϣ����ڲ������������ݽ���
 */
void MX_UARTx_Init(void)
{
    // �� ������ʱ�ӣ�GPIOB��USART3��DMA1
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_USART3_CLK_ENABLE();
    __HAL_RCC_DMA1_CLK_ENABLE();

    // �� ��ʼ�� GPIOB10(TX) �� GPIOB11(RX)
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Pin = GPIO_PIN_10;  // USART3_TX
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;  // �����������
    GPIO_InitStruct.Pull = GPIO_NOPULL;      // ������/����
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART3;  // ѡ���ù��� AF7
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_11;  // USART3_RX
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    // �� ��ʼ�� DMA�����ڽ��� USART3_RX -> �ڴ� uart_rx_buffer
    hdma_uartx_rx.Instance = DMA1_Stream1;
    hdma_uartx_rx.Init.Request = DMA_REQUEST_USART3_RX;
    hdma_uartx_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;  // ���� -> �ڴ�
    hdma_uartx_rx.Init.PeriphInc = DMA_PINC_DISABLE;      // �����ַ������
    hdma_uartx_rx.Init.MemInc = DMA_MINC_ENABLE;          // �ڴ��ַ����
    hdma_uartx_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_uartx_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_uartx_rx.Init.Mode = DMA_NORMAL;  // ����һ���ж�һ��
    hdma_uartx_rx.Init.Priority = DMA_PRIORITY_HIGH;
    hdma_uartx_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    HAL_DMA_Init(&hdma_uartx_rx);

    // �� �� DMA �� UART ��
    __HAL_LINKDMA(&huartx, hdmarx, hdma_uartx_rx);

    // �� ��ʼ������ USART3 ����
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

    // �� ���� DMA ���պͿ����жϣ�IDLE��
    HAL_UART_Receive_DMA(&huartx, uart_rx_buffer, UART_BUFFER_SIZE);
    __HAL_UART_ENABLE_IT(&huartx, UART_IT_IDLE);

    // �� ���� USART3 �� DMA �ж�
    HAL_NVIC_SetPriority(USART3_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(USART3_IRQn);
    HAL_NVIC_SetPriority(DMA1_Stream1_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(DMA1_Stream1_IRQn);
}

/**
 * @brief �ض��� printf ���������
 */
int fputc(int ch, FILE *f)
{
    HAL_UART_Transmit(&huartx, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
    return ch;
}
