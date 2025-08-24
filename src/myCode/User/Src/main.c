#include "main.h"
#include "gpio.h"

#include "../../myCode/myMain.h"
#include "i2c.h"
#include "uart.h"

static void SystemClock_Config(void);
static void NVIC_Configuration(void);

int main(void)
{
    SystemClock_Config();
    NVIC_Configuration();
    MX_GPIO_Init();
    MAX_I2C_Init();
    MAX_UART2_Init();

    setup();
    while (1)
    {
        loop();
    }
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line)
{
}
#endif

static void SystemClock_Config(void)
{
   uint32_t hsi_calibration, hsi_calibration_h, hsi_calibration_l;
   hsi_calibration_l = *(uint8_t *)(0x001007C0);
   hsi_calibration_h = *(uint8_t *)(0x001007C1);
   hsi_calibration = (hsi_calibration_h << 8) | hsi_calibration_l;
   CW_SYSCTRL->HSI_f.TRIM = hsi_calibration; // 校准内部48M频率
   SYSCTRL_HSI_Enable(SYSCTRL_HSIOSC_DIV1);
    SYSCTRL_SysClk_Switch(SYSCTRL_SYSCLKSRC_HSI);
    // SYSCTRL_HSE_Enable( SYSCTRL_HSE_MODE_OSC, 32000000, SYSCTRL_HSE_DRIVER_LEVEL0, 0);
    // SYSCTRL_SysClk_Switch(SYSCTRL_SYSCLKSRC_HSE);
    InitTick(48000000);
    SYSCTRL_SystemCoreClockUpdate(48000000);

   REGBITS_SET(CW_SYSCTRL->AHBEN, (0x5A5A0000 | bv1));
   REGBITS_SET(CW_SYSCTRL->AHBEN, (0x5A5A0000 | bv5));
    
    FLASH_SetLatency(FLASH_Latency_2);                   //频率大于24M需要配置FlashWait=2
//    FLASH_SetLatency(FLASH_Latency_1);                    //频率小于24M需要配置FlashWait=1
    
    SYSCTRL_AHBPeriphClk_Enable(SYSCTRL_AHB_PERIPH_GPIOB, ENABLE);
    SYSCTRL_APBPeriphClk_Enable1(SYSCTRL_APB1_PERIPH_UART2, ENABLE);
}

static void NVIC_Configuration(void)
{
    //优先级，无优先级分组
    NVIC_SetPriority(UART2_IRQn, 0);
    //UARTx中断使能
    NVIC_EnableIRQ(UART2_IRQn);
}

