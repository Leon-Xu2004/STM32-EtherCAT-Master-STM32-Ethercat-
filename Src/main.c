/* 包含头文件 ----------------------------------------------------------------*/
#include "stm32h7xx_hal.h"
#include "string.h"

#include "bsp_debug_usart.h"
#include "bsp_key.h"
#include "bsp_led.h"

#include "ethernetif.h"
#include "bsp_GeneralTIM.h"
#include "ethercat.h"
#include "soem.h"
#include "string.h"

/* 私有类型定义 --------------------------------------------------------------*/

/* 私有宏定义 ----------------------------------------------------------------*/

/* 私有变量 ------------------------------------------------------------------*/

T_CurveMotion AxisParameter[6 + 1];


static void SystemClock_Config(void);
static void CPU_CACHE_Enable(void);
static void MPU_Config(void);

/* 函数体 --------------------------------------------------------------------*/
/**
  * 函数功能: 主函数.
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
  */
int main(void)
{

    MPU_Config();
    CPU_CACHE_Enable();
    /* 复位所有外设，初始化Flash接口和系统滴答定时器(HSI) */
    HAL_Init();
    /* 配置系统时钟 */
    SystemClock_Config();
    /* USART串口初始化 */
    MX_UARTx_Init();
    KEY_GPIO_Init();
    LED_GPIO_Init();
    HAL_GPIO_WritePin(GPIOH, GPIO_PIN_2, GPIO_PIN_RESET);
    HAL_Delay(100);
    /* 高电平，不使能复位 */
    HAL_GPIO_WritePin(GPIOH, GPIO_PIN_2, GPIO_PIN_SET);


    while (1)
    {

        if(KEY1_StateRead()==KEY_DOWN)
        {
            LED1_TOGGLE();
            printf("[KEY1] 按下，返回原点。\r\n");
            // MotorToPulsePosition(轴号, 绝对脉冲位置, 最大速度（pulse/ms）, 加速度（pulse/ms2）)
            MotorToPulsePosition(1, AXIS_1_ORIGIN, 80.0, 1.0);
            HAL_Delay(100);
            MotorToPulsePosition(2, AXIS_2_ORIGIN, 80.0, 1.0);
            HAL_Delay(100);
            MotorToPulsePosition(3, AXIS_3_ORIGIN, 80.0, 1.0);
            HAL_Delay(100);
            MotorToPulsePosition(4, AXIS_4_ORIGIN, 80.0, 1.0);
            HAL_Delay(100);
            MotorToPulsePosition(5, AXIS_5_ORIGIN, 80.0, 1.0);
            HAL_Delay(100);
            MotorToPulsePosition(6, AXIS_6_ORIGIN, 80.0, 1.0);
        }
        else if(KEY2_StateRead()==KEY_DOWN)
        {
            LED2_TOGGLE();
//            printf("[KEY2] 按下，开始发送电机目标位置 = 3524288\r\n");
            // MotorToPulsePosition(轴号, 绝对脉冲位置, 最大速度（pulse/ms）, 加速度（pulse/ms2）)
            MotorToPulsePosition(1, 3309126.0, 80.0, 1.0);
            HAL_Delay(100);
            MotorToPulsePosition(2, 199126.0, 80.0, 1.0);
            HAL_Delay(100);
            MotorToPulsePosition(3, 3150626.0, 80.0, 1.0);
            HAL_Delay(100);
            MotorToPulsePosition(4, 3144126.0, 80.0, 1.0);
            HAL_Delay(100);
            MotorToPulsePosition(5, 3600000.0, 80.0, 1.0);
            HAL_Delay(100);
            MotorToPulsePosition(6, 3600000.0, 80.0, 1.0);


        }
        else if(KEY3_StateRead()==KEY_DOWN)
        {
            LED3_TOGGLE();
//            printf("[KEY3] 按下，电机停止\r\n");
            for (uint8_t axis = 1; axis <= Max_Axis; axis++)
            {
                Motor_SoftStop(axis);
                HAL_Delay(100);
            }
        }
        else if(KEY4_StateRead()==KEY_DOWN)
        {
//			printf("[KEY4] 按下，电机停止\r\n");
            RecoverFromFault(1);

        }
    }
}



/**
  * 函数功能: MPU配置
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
  */
static void MPU_Config(void)
{
    MPU_Region_InitTypeDef MPU_InitStruct;

    /* Disable the MPU */
    HAL_MPU_Disable();

    /* Configure the MPU attributes as Device not cacheable
       for ETH DMA descriptors */
    MPU_InitStruct.Enable = MPU_REGION_ENABLE;
    MPU_InitStruct.BaseAddress = 0x30040000;
    MPU_InitStruct.Size = MPU_REGION_SIZE_256B;
    MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
    MPU_InitStruct.IsBufferable = MPU_ACCESS_BUFFERABLE;
    MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
    MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
    MPU_InitStruct.Number = MPU_REGION_NUMBER0;
    MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
    MPU_InitStruct.SubRegionDisable = 0x00;
    MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;

    HAL_MPU_ConfigRegion(&MPU_InitStruct);

    /* Configure the MPU attributes as Cacheable write through
       for LwIP RAM heap which contains the Tx buffers */
    MPU_InitStruct.Enable = MPU_REGION_ENABLE;
    MPU_InitStruct.BaseAddress = 0x30044000;
    MPU_InitStruct.Size = MPU_REGION_SIZE_16KB;
    MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
    MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;
    MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
    MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
    MPU_InitStruct.Number = MPU_REGION_NUMBER1;
    MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
    MPU_InitStruct.SubRegionDisable = 0x00;
    MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;

    HAL_MPU_ConfigRegion(&MPU_InitStruct);

    /* Enable the MPU */
    HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
}

/**
  * 函数功能: 系统时钟配置
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
  */
static void SystemClock_Config(void)
{

    RCC_OscInitTypeDef RCC_OscInitStruct= {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct= {0};

    /* 使能PWR配置更新 */
    MODIFY_REG(PWR->CR3, PWR_CR3_SCUEN, 0);

    /* 设置调压器输出电压级别1 */
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    /* 等待D3区域VOS 输出准备就绪 */
    while ((PWR->D3CR & (PWR_D3CR_VOSRDY)) != PWR_D3CR_VOSRDY) // 等待内核电源就绪
    {

    }
    __HAL_RCC_PLL_PLLSOURCE_CONFIG(RCC_PLLSOURCE_HSE);

    /* RCC 内部/外部晶振配置 ,用于CPU，AHB，APH总线时钟 */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;// 外部晶振
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;//选择外部时钟晶振 HSE 25MHz
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;//使能PLL
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;//PLL时钟源选择HSE
    RCC_OscInitStruct.PLL.PLLM = 5;  // 外部时钟2分频,得到5MHz 作为PLL时钟源
    RCC_OscInitStruct.PLL.PLLN = 192; // PLL 160倍频,得到800MHz PLLCLK
    RCC_OscInitStruct.PLL.PLLP = 2;  // PLLCLK 2分频 得到400MHz SYSCLK
    RCC_OscInitStruct.PLL.PLLQ = 2;  // PLLCLK 2分频 得到400MHz用于部分外设的时钟
    RCC_OscInitStruct.PLL.PLLR = 2;  // PLLCLK 2分频 得到400MHz用于部分外设的时钟
    RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;// PLL时钟输入范围4-8MHz
    RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;// PLL时钟输出范围192-836MHz
    RCC_OscInitStruct.PLL.PLLFRACN = 0;  // 此参数用于微调 PLL1 VCO 范围0~2^13-1
    HAL_RCC_OscConfig(&RCC_OscInitStruct);

    /* RCC CPU，AHB，APH总线时钟配置*/
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                                  |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                                  |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;//SYSCLK 时钟源400MHz
    RCC_ClkInitStruct.SYSCLKDivider  = RCC_SYSCLK_DIV1;// SYSCLK 1分频 HCLK=400MHz
    RCC_ClkInitStruct.AHBCLKDivider  = RCC_HCLK_DIV2;  // HCLK 2分频，AHB = 200Mhz
    RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2; // APB3 2分频，APB3 = 100MHz
    RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2; // APB1 2分频，APB1 = 100MHz
    RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2; // APB2 2分频，APB2 = 100MHz
    RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2; // APB4 2分频，APB4 = 100MHz
    HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2);


    /* 使能时钟安全机制 */
    HAL_RCC_EnableCSS();

    /* 滴答定时器配置1ms */
    // SystemCoreClock/1000    1ms中断一次
    // SystemCoreClock/100000	 10us中断一次
    // SystemCoreClock/1000000 1us中断一次
    HAL_SYSTICK_Config(SystemCoreClock/(1000));

    /* 系统滴答定时器时钟源 */
    HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

    /* 系统滴答定时器中断优先级配置 */
    HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/**
  * 函数功能: 使能CPU L1-Cache
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
  */
static void CPU_CACHE_Enable(void)
{
    /* 使能 I-Cache */
    SCB_EnableICache();

    /* 使能 D-Cache */
    SCB_EnableDCache();

}

