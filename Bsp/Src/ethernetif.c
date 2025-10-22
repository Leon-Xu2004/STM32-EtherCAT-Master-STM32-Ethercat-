/* 包含头文件 ----------------------------------------------------------------*/
#include "ethernetif.h"
#include "lan8720.h"
#include "string.h"

/* 私有类型定义 --------------------------------------------------------------*/

/* 私有宏定义 ----------------------------------------------------------------*/

#if defined ( __ICCARM__ ) /*!< IAR Compiler */

#pragma location=0x30040000
ETH_DMADescTypeDef  DMARxDscrTab[ETH_RX_DESC_CNT]; /* Ethernet Rx DMA Descriptors */
#pragma location=0x30040060
ETH_DMADescTypeDef  DMATxDscrTab[ETH_TX_DESC_CNT]; /* Ethernet Tx DMA Descriptors */
#pragma location=0x30040200
uint8_t Rx_Buff[ETH_RX_DESC_CNT][ETH_MAX_PACKET_SIZE]; /* Ethernet Receive Buffers */
#pragma location=0x30044000
uint8_t Tx_Buff[ETH_TX_DESC_CNT][ETH_MAX_PACKET_SIZE]; /* Ethernet Receive Buffers */

#elif defined ( __CC_ARM )  /* MDK ARM Compiler */

__attribute__((at(0x30040000))) ETH_DMADescTypeDef  DMARxDscrTab[ETH_RX_DESC_CNT]; /* Ethernet Rx DMA Descriptors */
__attribute__((at(0x30040060))) ETH_DMADescTypeDef  DMATxDscrTab[ETH_TX_DESC_CNT]; /* Ethernet Tx DMA Descriptors */
__attribute__((at(0x30040200))) uint8_t Rx_Buff[ETH_RX_DESC_CNT][ETH_MAX_PACKET_SIZE]; /* Ethernet Receive Buffer */
__attribute__((at(0x30044000))) uint8_t Tx_Buff[ETH_RX_DESC_CNT][ETH_MAX_PACKET_SIZE]; /* Ethernet Tranform Buffer */

#elif defined ( __GNUC__ ) /* GNU Compiler */

ETH_DMADescTypeDef DMARxDscrTab[ETH_RX_DESC_CNT] __attribute__((section(".RxDecripSection"))); /* Ethernet Rx DMA Descriptors */
ETH_DMADescTypeDef DMATxDscrTab[ETH_TX_DESC_CNT] __attribute__((section(".TxDecripSection")));   /* Ethernet Tx DMA Descriptors */
uint8_t Rx_Buff[ETH_RX_DESC_CNT][ETH_MAX_PACKET_SIZE] __attribute__((section(".RxArraySection"))); /* Ethernet Receive Buffers */

#endif
/* 私有变量 ------------------------------------------------------------------*/
ETH_TxPacketConfig TxConfig;
ETH_HandleTypeDef heth;

uint8_t RecvLength=0;
uint32_t current_pbuf_idx =0;

/* 扩展变量 ------------------------------------------------------------------*/

/* 私有函数原形 --------------------------------------------------------------*/

/* 函数体 --------------------------------------------------------------------*/
/**
  * 函数功能: ETH初始化
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
  */
void MX_ETH_Init(void)
{
    uint8_t MACAddr[6];
    uint32_t idx = 0;

    // 设置以太网控制器的外设基地址，ETH 是 STM32 内部的以太网模块。
    heth.Instance = ETH;

    // 配置设备 MAC 地址，为网络通信中的唯一标识。
    MACAddr[0] = 0x01;
    MACAddr[1] = 0x00;
    MACAddr[2] = 0x45;
    MACAddr[3] = 0x19;
    MACAddr[4] = 0x03;
    MACAddr[5] = 0x00;
    heth.Init.MACAddr = &MACAddr[0];

    // 配置媒体接口类型为 RMII（简化型以太网接口），适用于本项目使用的 LAN8720 芯片。
    heth.Init.MediaInterface = HAL_ETH_RMII_MODE;

    // 设置以太网 DMA 发送描述符表的地址，用于管理发送数据。
    heth.Init.TxDesc = DMATxDscrTab;

    // 设置以太网 DMA 接收描述符表的地址，用于管理接收数据。
    heth.Init.RxDesc = DMARxDscrTab;

    // 设置接收缓冲区的最大长度（通常为 1524 字节），足以接收完整的以太网帧。
    heth.Init.RxBuffLen = ETH_MAX_PACKET_SIZE;

    /* USER CODE BEGIN MACADDRESS */
    /* 可在此区域插入用户自定义 MAC 地址逻辑 */
    /* USER CODE END MACADDRESS */

    // 调用 HAL 库初始化函数，应用上述配置并启动 ETH 外设。
    HAL_ETH_Init(&heth);
}


void printfBuffer(uint8_t *dat, uint32_t len )
{
    //printf("\r\n");
    for( int i=0; i<len; i++ )
    {
        if( (i % 32) == 0 && i != 0 )
            printf("\r\n");
        else if( (i % 8) == 0 && i != 0 )
            printf("  ");

        printf("%0.2X ",*dat++);
    }
    printf("\r\n\r\n");
}

/**
  * 函数功能: ETH引脚初始化
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
  */
void HAL_ETH_MspInit(ETH_HandleTypeDef* ethHandle)
{

    GPIO_InitTypeDef GPIO_InitStruct;
    if(ethHandle->Instance==ETH)
    {
        /* ETH GPIO端口时钟使能 */
        __HAL_RCC_GPIOC_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();
        __HAL_RCC_GPIOH_CLK_ENABLE();
        __HAL_RCC_GPIOG_CLK_ENABLE();
        /* E使能ETH外设时钟 */
        __HAL_RCC_ETH1MAC_CLK_ENABLE();
        __HAL_RCC_ETH1TX_CLK_ENABLE();
        __HAL_RCC_ETH1RX_CLK_ENABLE();


        /**ETH GPIO Configuration
        PC1     ------> ETH_MDC
        PA1     ------> ETH_REF_CLK
        PA2     ------> ETH_MDIO
        PA7     ------> ETH_CRS_DV
        PC4     ------> ETH_RXD0
        PC5     ------> ETH_RXD1
        PG11     ------> ETH_TX_EN
        PG12     ------> ETH_TXD1
        PG13     ------> ETH_TXD0
        */
        GPIO_InitStruct.Pin = GPIO_PIN_1|GPIO_PIN_4|GPIO_PIN_5;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
        HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_7;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
        HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

        /* 高电平，不使能复位 */
        HAL_GPIO_WritePin(GPIOH, GPIO_PIN_2, GPIO_PIN_SET);

        /* 复位引脚配置 */
        GPIO_InitStruct.Pin = GPIO_PIN_2;
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);

        /* ETH interrupt Init */
//    HAL_NVIC_SetPriority(ETH_IRQn, 1, 0);
//    HAL_NVIC_EnableIRQ(ETH_IRQn);
        /* USER CODE BEGIN ETH_MspInit 1 */

        /* USER CODE END ETH_MspInit 1 */
    }
}


/**
  * 函数功能: PHY初始化
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
  */
void PHY_Init(void)
{
    uint32_t idx, duplex, speed = 0;
	int retry_count = 0;

    int32_t PHYLinkState;
    ETH_MACConfigTypeDef MACConf;

    ETH_MACFilterConfigTypeDef filterDef;

    HAL_ETH_GetMACFilterConfig(&heth,&filterDef);
    filterDef.PromiscuousMode = ENABLE;
    HAL_ETH_SetMACFilterConfig(&heth,&filterDef);


    for(idx = 0; idx < ETH_RX_DESC_CNT; idx ++)
    {
        HAL_ETH_DescAssignMemory(&heth, idx, Rx_Buff[idx], NULL);
    }

    LAN8720_Init();

	do
	{
		HAL_Delay(100);
		PHYLinkState = LAN8720_GetLinkState();
		retry_count++;
	}
	while(PHYLinkState <= LAN8720_STATUS_LINK_DOWN && retry_count < 20);

	// 判断是否失败
	if (PHYLinkState <= LAN8720_STATUS_LINK_DOWN)
	{
		printf("LAN8720 link failed after %d attempts.\r\n", 20);
		return; // 或者走降级逻辑
	}

    switch (PHYLinkState)
    {
    case LAN8720_STATUS_100MBITS_FULLDUPLEX:
        duplex = ETH_FULLDUPLEX_MODE;
        speed = ETH_SPEED_100M;

        //printf("LAN8720_STATUS_100MBITS_FULLDUPLEX \r\n");
        break;
    case LAN8720_STATUS_100MBITS_HALFDUPLEX:
        duplex = ETH_HALFDUPLEX_MODE;
        speed = ETH_SPEED_100M;

        //printf("LAN8720_STATUS_100MBITS_HALFDUPLEX \r\n");
        break;
    case LAN8720_STATUS_10MBITS_FULLDUPLEX:
        duplex = ETH_FULLDUPLEX_MODE;
        speed = ETH_SPEED_10M;

        //printf("LAN8720_STATUS_10MBITS_FULLDUPLEX \r\n");
        break;
    case LAN8720_STATUS_10MBITS_HALFDUPLEX:
        duplex = ETH_HALFDUPLEX_MODE;
        speed = ETH_SPEED_10M;

        //printf("LAN8720_STATUS_10MBITS_HALFDUPLEX \r\n");
        break;
    default:
        duplex = ETH_FULLDUPLEX_MODE;
        speed = ETH_SPEED_100M;

        //printf("ETH_FULLDUPLEX_MODE ETH_SPEED_100M\r\n");
        break;
    }

    /* Get MAC Config MAC */
    HAL_ETH_GetMACConfig(&heth, &MACConf);
    MACConf.DuplexMode = duplex;
    MACConf.Speed = speed;
    HAL_ETH_SetMACConfig(&heth, &MACConf);
    HAL_ETH_Start_IT(&heth);

    HAL_ETH_BuildRxDescriptors(&heth);
}


uint32_t sendfinishflag=0;
/**
  * 函数功能: 数据输出
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
  */
void low_level_output(uint8_t *p,uint32_t length)
{
    uint32_t framelen = 0;

    ETH_BufferTypeDef Txbuffer[ETH_TX_DESC_CNT];

    memset(Txbuffer, 0, ETH_TX_DESC_CNT*sizeof(ETH_BufferTypeDef));

    Txbuffer[0].buffer = p;
    Txbuffer[0].len = length;
    framelen += length;

    TxConfig.Length = framelen;
    TxConfig.TxBuffer = Txbuffer;

    SCB_CleanInvalidateDCache();

    //while( sendfinishflag == 1 );
    HAL_ETH_Transmit(&heth, &TxConfig, 5);
    sendfinishflag = 1;
}

void low_level_input()
{

}


void HAL_ETH_RxCpltCallback(ETH_HandleTypeDef *heth)
{
    low_level_input();
    //printf("rx isr\r\n");

}



void HAL_ETH_TxCpltCallback(ETH_HandleTypeDef *heth)
{
    sendfinishflag = 0;
    //printf("tx isr\r\n");
}

int bfin_EMAC_send (void *packet, int length)
{
    memcpy(&Tx_Buff[0][0],packet,length);

    low_level_output(Tx_Buff[0],length);

    return 0;
}


int bfin_EMAC_recv (uint8_t * packet, size_t size)
{
    ETH_BufferTypeDef RxBuff;
    uint32_t framelength = 0;
    SCB_CleanInvalidateDCache();

    HAL_StatusTypeDef status = HAL_ETH_GetRxDataBuffer(&heth, &RxBuff);

    if( status == HAL_OK)
    {
        HAL_ETH_GetRxDataLength(&heth, &framelength);

        SCB_InvalidateDCache_by_Addr((uint32_t *)Rx_Buff, (ETH_RX_DESC_CNT*ETH_MAX_PACKET_SIZE));

        //printf("Recv = %d current_pbuf_idx=%d\r\n",framelength,current_pbuf_idx);
        //printfBuffer(&Rx_Buff[current_pbuf_idx][0],framelength);

        memcpy(packet, Rx_Buff[current_pbuf_idx], framelength);

        if(current_pbuf_idx < (ETH_RX_DESC_CNT -1))
        {
            current_pbuf_idx++;
        }
        else
        {
            current_pbuf_idx = 0;
        }

        /* Invalidate data cache for ETH Rx Buffers */
        HAL_ETH_BuildRxDescriptors(&heth);

        return framelength;
    }

    return -1;
}

