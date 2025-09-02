#include "binary.h"

#include "gray.h"

uint8_t binaryVal[SENSORE_NUM] = {0};


void binary_init(void)
{
    
}


void binary_study(void)
{
    gary_study();
}

void binary_identify(void)
{
    gary_identify();
    for (uint8_t i = 0; i < SENSORE_NUM; i++)
    {
        binaryVal[i] = grayVal[i] > 170 ? 1 : 0;
    }
}
