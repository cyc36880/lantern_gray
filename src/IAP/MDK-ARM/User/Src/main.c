#include "main.h"
#include "gpio.h"
#include "uart.h"
#include "stdio.h"
#include "control.h"
#include "xmodem.h"
#include "flash.h"
#include "sys_config_and_flash.h"

static void SystemClock_Config(void);

static uint8_t Check_jump_addr(uint32_t app_addr)
{
	return !( ( (*(__IO uint32_t*)app_addr) & 0x2FFE0000 ) == 0x20000000  );
}

int main(void)
{
    __enable_irq(); 
    SystemClock_Config(); 
    sys_config_info_init(); 
    uint8_t need_xmodem = 0;
    get_sys_config_data(SYSCONF_MEM_LIST_NEED_XMODEM, &need_xmodem);
  if (1 == need_xmodem)
    {
//				setup();
				MAX_UART1_Init();
				need_xmodem = 0;
				set_sys_config_info(SYSCONF_MEM_LIST_NEED_XMODEM,&need_xmodem,1);
				sync_sys_config_info();
        int ret = uXMODEM_Process(30000); // 等待30s
        if (0 == ret)
        {
            uint8_t copy_state = 1;
            set_sys_config_info(SYSCONF_MEM_LIST_FLASH_COPY_STATE, &copy_state, 1);
            sync_sys_config_info();
        }
        else 
				{
            while (1)
						{
						}
        }
    }
    
    if (0 != Check_jump_addr(FLASH_APP_ADDR))
    {
        uint8_t copy_state = 1;
        set_sys_config_info(SYSCONF_MEM_LIST_FLASH_COPY_STATE, &copy_state, 1);
        sync_sys_config_info();
    }
    
    uint8_t copy_state = 0;
    get_sys_config_data(SYSCONF_MEM_LIST_FLASH_COPY_STATE, &copy_state);
    if (1 == copy_state)
    {
        int copy_ret = Flash_copy(FLASH_APP_ADDR, FLASH_SAVE_ADDR, FLASH_APP_SIZE);
        if (0 == copy_ret)
        {
            uint8_t copy_state = 0;
            uint8_t need_xmodem = 0;

            set_sys_config_info(SYSCONF_MEM_LIST_FLASH_COPY_STATE, &copy_state, 1);
            set_sys_config_info(SYSCONF_MEM_LIST_NEED_XMODEM, &need_xmodem, 1);
            sync_sys_config_info();
        }
        else
        {
            while (1) // 😱拷贝失败 ！！！！！！！！！！！
						{
						};
        }
    }
    
    // 跳转
    jump(FLASH_APP_ADDR);
    
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
    
    SYSCTRL_HSI_Enable(SYSCTRL_HSIOSC_DIV2);
//    SYSCTRL_HSE_Enable(SYSCTRL_HSE_MODE_OSC, 32000000, SYSCTRL_HSE_DRIVER_LEVEL5, 0); // 开外部高速时钟，实际频率需要根据实际晶体频率重新配置
    SYSCTRL_SysClk_Switch(SYSCTRL_SYSCLKSRC_HSE);
    InitTick(24000000);
    SYSCTRL_SystemCoreClockUpdate(24000000);

    REGBITS_SET(CW_SYSCTRL->AHBEN, (0x5A5A0000 | bv1));
    REGBITS_SET(CW_SYSCTRL->AHBEN, (0x5A5A0000 | bv5));
    
//    FLASH_SetLatency(FLASH_Latency_2);                   //频率大于24M需要配置FlashWait=2
    FLASH_SetLatency(FLASH_Latency_1);                    //频率小于24M需要配置FlashWait=1
}
