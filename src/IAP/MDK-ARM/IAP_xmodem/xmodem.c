#include <stdint.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "xmodem.h"
#include "common.h"
#include "light.h"
// ----------------- Flash 参数配置 ------------------
// #define def_FLASH_BASE ((uint32_t)0x08000000)
// #define def_FLASH_PAGESIZE ((uint32_t)2048) // 页面大小（你要根据芯片实际配置）
// #define def_FLASH_PAGECOUNT ((uint32_t)256) // 总页数
// #define def_USERAPP_START (def_FLASH_BASE + def_FLASH_PAGESIZE * 20)
// #define def_USERAPP_BOTTOM (def_FLASH_BASE + def_FLASH_PAGESIZE * (def_FLASH_PAGECOUNT - 2))

// ----------------- XMODEM 控制码 -------------------
#define XMODEM_NUL 0x00
#define XMODEM_SOH 0x01
#define XMODEM_EOT 0x04
#define XMODEM_ACK 0x06
#define XMODEM_NAK 0x15
#define XMODEM_CAN 0x18
#define XMODEM_EOF 0x1A

#define XMODEM_WAIT_CHAR XMODEM_NAK

#define dST_WAIT_START 0x00
#define dST_BLOCK_OK   0x01
#define dST_BLOCK_FAIL 0x02
#define dST_OK 0x03

// ----------------- XMODEM 接收数据结构 ---------------
typedef struct
{
    uint8_t SOH;
    uint8_t BlockNo;
    uint8_t nBlockNo;
    uint8_t Xdata[128];
    uint8_t CheckSum;
} XMODEM_Frame_t;

static XMODEM_Frame_t strXMODEM;
static uint8_t gXM_BlockCount = 1;
static uint8_t gXM_STATUS = dST_WAIT_START;
static uint32_t gFlash_User_Address = 0;
static uint32_t gFlash_User_Count = 0;
static uint32_t gFlash_Count = 0;

static uint8_t Block_512[512] = {0};

// ======= 用户实现接口 ==========
extern void uart_send_byte(uint8_t byte);
extern int uart_receive_bytes(uint8_t *buf, uint16_t len, uint32_t timeout_ms);
extern void SysTickDelay(uint32_t ms);


// ======= 工具函数 ==============
static void uUART_PutChar(uint8_t Data)
{
    uart_send_byte(Data);
}

static uint8_t get_data(uint8_t *ptr, uint16_t len, uint32_t timeout_ms)
{
    return uart_receive_bytes(ptr, len, timeout_ms) ;
}

static uint8_t uCheckSum(const uint8_t *ptr, uint8_t len)
{
    uint8_t sum = 0;
    while (len--)
        sum += *ptr++;
    return sum;
}

// ========== XMODEM 主处理函数 ===========
int uXMODEM_Process(uint32_t timeout_s)
{
    uint16_t i;
    uint8_t checksum;

    // 初始化参数
    gXM_BlockCount = 1;
    uint16_t error_count = 0;
		uint8_t time = 0;
    // 主循环：接收数据包
    
    while (gXM_STATUS != dST_OK)
    {
        // 发送 NAK 提示继续发送
        if (gXM_STATUS == dST_WAIT_START)
        {
            uUART_PutChar(XMODEM_WAIT_CHAR);
        }

        i = get_data((uint8_t *)&strXMODEM, sizeof(XMODEM_Frame_t), 1000);
        if (i == 0)
        {
            if (++error_count > timeout_s) // 失败重试次数
            {
                return -1;
            }
            else
            {
                continue;
            }
        }
        error_count = 0;

        switch (strXMODEM.SOH)
        {
        case XMODEM_SOH:
            if (i >= sizeof(XMODEM_Frame_t))
            {
                gXM_STATUS = dST_BLOCK_OK;
            }
            else
            {
                gXM_STATUS = dST_BLOCK_FAIL;
                uUART_PutChar(XMODEM_NAK);
            }
            break;

        case XMODEM_EOT:
            uUART_PutChar(XMODEM_ACK);
            gXM_STATUS = dST_OK;
            break;

        case XMODEM_CAN:
            uUART_PutChar(XMODEM_ACK);
            gXM_STATUS = dST_OK;
            break;

        default:
            uUART_PutChar(XMODEM_NAK);
            gXM_STATUS = dST_BLOCK_FAIL;
            break;
        }

        if (gXM_STATUS == dST_BLOCK_OK)
        {
//						if(time ++ == 10)
//						{
//								time = 0;
//								GPIO_TogglePin(CW_GPIOA,GPIO_PIN_3|GPIO_PIN_4);
//								GPIO_TogglePin(CW_GPIOB,GPIO_PIN_0|GPIO_PIN_1);
//						}
            // 校验编号与反码
            if ((uint8_t)(strXMODEM.BlockNo + strXMODEM.nBlockNo) != 0xFF)
            {
                uUART_PutChar(XMODEM_NAK);
                continue;
            }

            // 处理重传包
            if (strXMODEM.BlockNo == (uint8_t)(gXM_BlockCount - 1))
            {
                uUART_PutChar(XMODEM_ACK); // 已处理，直接 ACK
                continue;
            }

            // 检查是否正确的包号
            if (strXMODEM.BlockNo != gXM_BlockCount)
            {
                uUART_PutChar(XMODEM_NAK);
                continue;
            }

            // 校验 CheckSum
            checksum = uCheckSum(strXMODEM.Xdata, 128);
            if (checksum != strXMODEM.CheckSum)
            {
                uUART_PutChar(XMODEM_NAK);
                continue;
            }
            // 校验通过，写入 Flash
            memcpy(Block_512+ (gFlash_User_Count%4)*128, strXMODEM.Xdata, 128);
            gFlash_User_Address += 128;
            gFlash_User_Count++;
            if (gFlash_User_Count%4 == 0)
            {
                gFlash_Count++;
                uint32_t target_addr = FLASH_SAVE_ADDR + (gFlash_Count-1)*512;

                FLASH_UnlockPages(target_addr, target_addr);
                FLASH_ErasePages(target_addr, target_addr);
                FLASH_LockAllPages();

                SysTickDelay(1);

                FLASH_UnlockPages(target_addr, target_addr);
                FLASH_WriteBytes(target_addr, Block_512, 512);
                memset(Block_512, 0xff, 512);
                FLASH_LockAllPages();
            }
            // 包计数+1，ACK应答
            gXM_BlockCount++;
            uUART_PutChar(XMODEM_ACK);
        }
    }
    // 收尾：最后不足 512 字节的数据写入 Flash
    if (gFlash_Count % 4 != 0)
    {
        gFlash_Count++;
        uint32_t target_addr = FLASH_SAVE_ADDR + (gFlash_Count-1)*512;
        FLASH_UnlockPages(target_addr, target_addr);
        FLASH_ErasePages(target_addr, target_addr);
        FLASH_WriteBytes(target_addr, Block_512, 512);
        FLASH_LockAllPages();
    }

    return 0;
}
