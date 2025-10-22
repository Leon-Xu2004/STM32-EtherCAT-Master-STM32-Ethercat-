/* 包含头文件 ----------------------------------------------------------------*/

#include "lan8720.h"

#include "stm32h7xx_hal.h"
#include "stm32h7xx_hal_eth.h"
/* 私有类型定义 --------------------------------------------------------------*/

/* 私有宏定义 ----------------------------------------------------------------*/

/* 私有变量 ------------------------------------------------------------------*/
uint32_t lan8720addr;

/* 扩展变量 ------------------------------------------------------------------*/
extern ETH_HandleTypeDef heth;

/* 私有函数原形 --------------------------------------------------------------*/

/* 函数体 --------------------------------------------------------------------*/
/**
  * 函数功能: 读取PHY寄存器值
  * 输入参数: reg：要读取的寄存器地址
  * 返 回 值: 0：读取成功，-1：读取失败
  * 说    明: 无
  */
int32_t ETH_PHY_ReadReg(uint16_t reg,uint32_t *regval)
{
    if(HAL_ETH_ReadPHYRegister(&heth,0,reg,regval)!=HAL_OK)
        return -1;
    return 0;
}

/**
  * 函数功能: 向LAN8720A指定寄存器写入值
  * 输入参数: reg：要读取的寄存器地址 ，value:要写入的值
  * 返 回 值: 0：写入正常，-1：写入失败
  * 说    明: 无
  */
int32_t ETH_PHY_WriteReg(uint16_t reg,uint16_t value)
{
    uint32_t temp=value;
    if(HAL_ETH_WritePHYRegister(&heth,0,reg,temp)!=HAL_OK)
        return -1;
    return 0;
}
#define LAN8720A_BCR_AUTONEGO_EN        ((uint16_t)0x1000U)

/**
  * 函数功能: 开启LAN8720A的自协商功能
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
  */
void LAN8720A_StartAutoNego(void)
{
    uint32_t readval=0;
    ETH_PHY_ReadReg(0,&readval);
    readval|=LAN8720A_BCR_AUTONEGO_EN;
    ETH_PHY_WriteReg(0,readval);
}

/**
  * 函数功能: LAN872初始化
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明:
  */
int LAN8720_Init(void)
{
    uint32_t regvalue = 0, addr = 0;
    HAL_StatusTypeDef rtn;

    //HAL_ETH_SetMDIOClockRange(&heth);
    lan8720addr = 32;
    for(addr = 0; addr <= lan8720addr; addr ++)
    {
        rtn = HAL_ETH_ReadPHYRegister(&heth, addr, LAN8720_SMR, &regvalue);
        if( rtn != HAL_OK)
        {
            printf("Read Address %d error \r\n",addr);
            continue;
        }
        if((regvalue & LAN8720_SMR_PHY_ADDR) == addr)
        {
            lan8720addr = addr;
            break;
        }
    }

    printf("lan8720addr = %d\r\n",addr);
    if( lan8720addr > 31 )
        return -1;
    rtn = HAL_ETH_WritePHYRegister(&heth,lan8720addr,LAN8720_BCR,LAN8720_BCR_SOFT_RESET);
    if( rtn != HAL_OK )
    {
        return -1;
    }
    do
    {
        rtn = HAL_ETH_ReadPHYRegister(&heth, lan8720addr, LAN8720_BCR, &regvalue);
        if( rtn != HAL_OK )
        {
            printf("HAL_ETH_ReadPHYRegister LAN8720_BCR Error \r\n");
            return -1;
        }
    }
    while( regvalue & LAN8720_BCR_SOFT_RESET );

    LAN8720A_StartAutoNego();                /*开启自动协商功能 */

    return 0;
}

/**
  * 函数功能: LAN8720获取链接状态
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
  */
int LAN8720_GetLinkState(void)
{
    uint32_t readval = 0;
    int32_t rtn;

    //读状态寄存器
    rtn = HAL_ETH_ReadPHYRegister(&heth, lan8720addr, LAN8720_BSR, &readval);
    if( rtn != HAL_OK )
    {
        //   printf("HAL_ETH_ReadPHYRegister LAN8720_BSR Error \r\n");
        return LAN8720_STATUS_READ_ERROR;
    }

    //再读一遍状态寄存器
    rtn = HAL_ETH_ReadPHYRegister(&heth, lan8720addr, LAN8720_BSR, &readval);
    if( rtn != HAL_OK )
    {
        //    printf("HAL_ETH_ReadPHYRegister LAN8720_BSR Error \r\n");
        return LAN8720_STATUS_READ_ERROR;
    }

    if((readval & LAN8720_BSR_LINK_STATUS) == 0)
    {
        return LAN8720_STATUS_LINK_DOWN;
    }

    /* Check Auto negotiaition */

    rtn = HAL_ETH_ReadPHYRegister(&heth, lan8720addr, LAN8720_BCR, &readval);
    if( rtn != HAL_OK )
    {
        return LAN8720_STATUS_READ_ERROR;
    }

    if((readval & LAN8720_BCR_AUTONEGO_EN) != LAN8720_BCR_AUTONEGO_EN)
    {
        if(((readval & LAN8720_BCR_SPEED_SELECT) == LAN8720_BCR_SPEED_SELECT) && ((readval & LAN8720_BCR_DUPLEX_MODE) == LAN8720_BCR_DUPLEX_MODE))
        {
            return LAN8720_STATUS_100MBITS_FULLDUPLEX;
        }
        else if ((readval & LAN8720_BCR_SPEED_SELECT) == LAN8720_BCR_SPEED_SELECT)
        {
            return LAN8720_STATUS_100MBITS_HALFDUPLEX;
        }
        else if ((readval & LAN8720_BCR_DUPLEX_MODE) == LAN8720_BCR_DUPLEX_MODE)
        {
            return LAN8720_STATUS_10MBITS_FULLDUPLEX;
        }
        else
        {
            return LAN8720_STATUS_10MBITS_HALFDUPLEX;
        }
    }
    else /* Auto Nego enabled */
    {
        rtn = HAL_ETH_ReadPHYRegister(&heth, lan8720addr, LAN8720_PHYSCSR, &readval);
        if( rtn != HAL_OK )
        {
            return LAN8720_STATUS_READ_ERROR;
        }

        /* Check if auto nego not done */
        if((readval & LAN8720_PHYSCSR_AUTONEGO_DONE) == 0)
        {
            return LAN8720_STATUS_AUTONEGO_NOTDONE;
        }

        if((readval & LAN8720_PHYSCSR_HCDSPEEDMASK) == LAN8720_PHYSCSR_100BTX_FD)
        {
            return LAN8720_STATUS_100MBITS_FULLDUPLEX;
        }
        else if ((readval & LAN8720_PHYSCSR_HCDSPEEDMASK) == LAN8720_PHYSCSR_100BTX_HD)
        {
            return LAN8720_STATUS_100MBITS_HALFDUPLEX;
        }
        else if ((readval & LAN8720_PHYSCSR_HCDSPEEDMASK) == LAN8720_PHYSCSR_10BT_FD)
        {
            return LAN8720_STATUS_10MBITS_FULLDUPLEX;
        }
        else
        {
            return LAN8720_STATUS_10MBITS_HALFDUPLEX;
        }
    }
}


