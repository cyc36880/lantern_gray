#include "i2c.h"

#include "register.h"

#define I2C_SLAVEADDRESS (0x57 << 1)

static void HAL_I2c_MspInit(void);

void MAX_I2C_Init(void)
{
    I2C_InitTypeDef I2C_InitStruct = {0};

    HAL_I2c_MspInit();

    // I2C初始化
    I2C_InitStruct.PCLK_Freq = 48000000;
    I2C_InitStruct.I2C_SCL_Freq = 1000000;
    I2C_InitStruct.I2C_OwnSlaveAddr0 = I2C_SLAVEADDRESS; // TEST ADDRESS0
    I2C_InitStruct.I2C_OwnSlaveAddr1 = I2C_SLAVEADDRESS; // TEST ADDRESS1
    // I2C_InitStruct.I2C_OwnSlaveAddr2 = I2C_SLAVEADDRESS_OTHER2;  //TEST ADDRESS2

    I2C_InitStruct.I2C_OwnGc = DISABLE;
    I2C1_DeInit();
    I2C_Slave_Init(CW_I2C1, &I2C_InitStruct); // 初始化模块
    CW_I2C1->CR_f.FLT = 1;
    I2C_Cmd(CW_I2C1, ENABLE);               // 模块使能
    I2C_AcknowledgeConfig(CW_I2C1, ENABLE); // ACK打开

    __disable_irq();
    NVIC_EnableIRQ(I2C1_IRQn);
    __enable_irq();
}

static void HAL_I2c_MspInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    __SYSCTRL_GPIOA_CLK_ENABLE();
    __SYSCTRL_I2C_CLK_ENABLE();

    PA05_AFx_I2C1SDA();
    PA06_AFx_I2C1SCL();
    GPIO_InitStructure.Pins = GPIO_PIN_6 | GPIO_PIN_5;
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_Init(CW_GPIOA, &GPIO_InitStructure);

    CW_GPIOA->PUR_f.PIN5 = 1; // 上拉
    CW_GPIOA->PUR_f.PIN6 = 1;
}

void I2C1_IRQHandlerCallback(void)
{
    static uint8_t rev_flag = 0;
    static uint8_t i2c_comm_count = 0;

    switch (CW_I2C1->STAT)
    {
    // 从机收到写入的请求
    case 0x60: // 接收到SLA+W，ACK已回应
    case 0x68: // 主机丢失仲裁后被SLA+W寻址，ACK已回应
    case 0x70: // 接收到SLA（广播地址）+W，ACK已回应
    case 0x78: // 主机丢失仲裁后被SLA（广播地址）+W寻址，ACK已回应
        i2c_comm_count = 0;
        break;
    case 0x80: // 从机已接收到1字节数据，ACK已回复
    case 0x90: // 从机已接收到1字节广播数据，ACK已回复
        if (sys_register.reg)
        {
            sys_register.reg[i2c_comm_count] = CW_I2C1->DR;
            if (i2c_comm_count < (sys_register.size - 1))
                i2c_comm_count++;
            rev_flag = 1;
        }
        break;
    case 0x88: // 从机已接收到1字节数据，NACK已回应
    case 0x98: // 从机已接收到1字节广播数据，NACK已回应
        // data = CW_I2C1->DR;
        CW_I2C1->CR_f.STO = 1;
        CW_I2C1->CR_f.SI = 0;
        return;
    case 0xA0: // 重复START信号或者STOP信号已收到
        if (rev_flag && sys_register.reg[0]==0x02)
        {
            rev_flag = 0;
            sys_register.changle_flag = 1;
        }
        i2c_comm_count = 0;
        break;

    // 从机收到读出的请求
    case 0xA8: // 接收到SLA+R,ACK已回应
    case 0xB0: // 主机丢失仲裁后被转从机并被SLA+R寻址，ACK已回应
        i2c_comm_count = 0;
        if (ultr_register.reg)
            CW_I2C1->DR = ultr_register.reg[i2c_comm_count++];
        else
            CW_I2C1->DR = 0x00;
        break;
    case 0xB8: // 从机已发送1字节数据，ACK已收到
        if (i2c_comm_count < ultr_register.size)
        {
            if (ultr_register.reg)
                CW_I2C1->DR = ultr_register.reg[i2c_comm_count++];
            else
                CW_I2C1->DR = 0x00;
        }
        else
            CW_I2C1->DR = 0x00; // 停止响应ACK
        break;
    case 0xC8:                // 从机最后一个数据字节已被发送(发送前自身AA被设置为0)，并已接收到ACK
        CW_I2C1->CR_f.AA = 1; // 响应下一次的SLA+R
        break;
    case 0xC0: // 已发送数据字节，已接收 NACK
    default:
        break;
    }
    CW_I2C1->CR_f.SI = 0;
}
