/* ����ͷ�ļ� ----------------------------------------------------------------*/
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

/* ˽�����Ͷ��� --------------------------------------------------------------*/

/* ˽�к궨�� ----------------------------------------------------------------*/

/* ˽�б��� ------------------------------------------------------------------*/

T_CurveMotion AxisParameter[6 + 1];


static void SystemClock_Config(void);
static void CPU_CACHE_Enable(void);
static void MPU_Config(void);

/* ������ --------------------------------------------------------------------*/
/**
  * ��������: ������.
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ��: ��
  */
int main(void)
{

    MPU_Config();
    CPU_CACHE_Enable();
    /* ��λ�������裬��ʼ��Flash�ӿں�ϵͳ�δ�ʱ��(HSI) */
    HAL_Init();
    /* ����ϵͳʱ�� */
    SystemClock_Config();
    /* USART���ڳ�ʼ�� */
    MX_UARTx_Init();
    KEY_GPIO_Init();
    LED_GPIO_Init();
    HAL_GPIO_WritePin(GPIOH, GPIO_PIN_2, GPIO_PIN_RESET);
    HAL_Delay(100);
    /* �ߵ�ƽ����ʹ�ܸ�λ */
    HAL_GPIO_WritePin(GPIOH, GPIO_PIN_2, GPIO_PIN_SET);


    while (1)
    {

        if(KEY1_StateRead()==KEY_DOWN)
        {
            LED1_TOGGLE();
            printf("[KEY1] ���£�����ԭ�㡣\r\n");
            // MotorToPulsePosition(���, ��������λ��, ����ٶȣ�pulse/ms��, ���ٶȣ�pulse/ms2��)
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
//            printf("[KEY2] ���£���ʼ���͵��Ŀ��λ�� = 3524288\r\n");
            // MotorToPulsePosition(���, ��������λ��, ����ٶȣ�pulse/ms��, ���ٶȣ�pulse/ms2��)
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
//            printf("[KEY3] ���£����ֹͣ\r\n");
            for (uint8_t axis = 1; axis <= Max_Axis; axis++)
            {
                Motor_SoftStop(axis);
                HAL_Delay(100);
            }
        }
        else if(KEY4_StateRead()==KEY_DOWN)
        {
//			printf("[KEY4] ���£����ֹͣ\r\n");
            RecoverFromFault(1);

        }
    }
}



/**
  * ��������: MPU����
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ��: ��
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
  * ��������: ϵͳʱ������
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ��: ��
  */
static void SystemClock_Config(void)
{

    RCC_OscInitTypeDef RCC_OscInitStruct= {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct= {0};

    /* ʹ��PWR���ø��� */
    MODIFY_REG(PWR->CR3, PWR_CR3_SCUEN, 0);

    /* ���õ�ѹ�������ѹ����1 */
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    /* �ȴ�D3����VOS ���׼������ */
    while ((PWR->D3CR & (PWR_D3CR_VOSRDY)) != PWR_D3CR_VOSRDY) // �ȴ��ں˵�Դ����
    {

    }
    __HAL_RCC_PLL_PLLSOURCE_CONFIG(RCC_PLLSOURCE_HSE);

    /* RCC �ڲ�/�ⲿ�������� ,����CPU��AHB��APH����ʱ�� */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;// �ⲿ����
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;//ѡ���ⲿʱ�Ӿ��� HSE 25MHz
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;//ʹ��PLL
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;//PLLʱ��Դѡ��HSE
    RCC_OscInitStruct.PLL.PLLM = 5;  // �ⲿʱ��2��Ƶ,�õ�5MHz ��ΪPLLʱ��Դ
    RCC_OscInitStruct.PLL.PLLN = 192; // PLL 160��Ƶ,�õ�800MHz PLLCLK
    RCC_OscInitStruct.PLL.PLLP = 2;  // PLLCLK 2��Ƶ �õ�400MHz SYSCLK
    RCC_OscInitStruct.PLL.PLLQ = 2;  // PLLCLK 2��Ƶ �õ�400MHz���ڲ��������ʱ��
    RCC_OscInitStruct.PLL.PLLR = 2;  // PLLCLK 2��Ƶ �õ�400MHz���ڲ��������ʱ��
    RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;// PLLʱ�����뷶Χ4-8MHz
    RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;// PLLʱ�������Χ192-836MHz
    RCC_OscInitStruct.PLL.PLLFRACN = 0;  // �˲�������΢�� PLL1 VCO ��Χ0~2^13-1
    HAL_RCC_OscConfig(&RCC_OscInitStruct);

    /* RCC CPU��AHB��APH����ʱ������*/
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                                  |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                                  |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;//SYSCLK ʱ��Դ400MHz
    RCC_ClkInitStruct.SYSCLKDivider  = RCC_SYSCLK_DIV1;// SYSCLK 1��Ƶ HCLK=400MHz
    RCC_ClkInitStruct.AHBCLKDivider  = RCC_HCLK_DIV2;  // HCLK 2��Ƶ��AHB = 200Mhz
    RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2; // APB3 2��Ƶ��APB3 = 100MHz
    RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2; // APB1 2��Ƶ��APB1 = 100MHz
    RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2; // APB2 2��Ƶ��APB2 = 100MHz
    RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2; // APB4 2��Ƶ��APB4 = 100MHz
    HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2);


    /* ʹ��ʱ�Ӱ�ȫ���� */
    HAL_RCC_EnableCSS();

    /* �δ�ʱ������1ms */
    // SystemCoreClock/1000    1ms�ж�һ��
    // SystemCoreClock/100000	 10us�ж�һ��
    // SystemCoreClock/1000000 1us�ж�һ��
    HAL_SYSTICK_Config(SystemCoreClock/(1000));

    /* ϵͳ�δ�ʱ��ʱ��Դ */
    HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

    /* ϵͳ�δ�ʱ���ж����ȼ����� */
    HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/**
  * ��������: ʹ��CPU L1-Cache
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ��: ��
  */
static void CPU_CACHE_Enable(void)
{
    /* ʹ�� I-Cache */
    SCB_EnableICache();

    /* ʹ�� D-Cache */
    SCB_EnableDCache();

}

