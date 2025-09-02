#include "../inc/register.h"

uint8_t sys_reg_buf[5] = {0};
uint8_t ultr_reg_buf[4] = {
    [3] = 1,
};

Register_t sys_register = {
    .reg = sys_reg_buf,
    .size = 5,
};

Register_t ultr_register = {
    .reg = ultr_reg_buf,
    .size = 4,
};
