#include <stdint.h>
#include "cwmProjectConfig.h"

#ifdef SPI_MASTER_SUPPORT // TODO: to be implemented!
void GPIO_ACC_Chip_Select(uint8_t uSelect)
{
    if(uSelect)
    {
        //GPIO_ResetBits( GPIOB, GPIO_Pin_12 );
    }
    else
    {
        //GPIO_SetBits( GPIOB, GPIO_Pin_12 );
    }
}

void GPIO_Motion_Chip_Select(uint8_t uSelect)
{
    if(uSelect)
    {
        //GPIO_ResetBits( GPIOB, GPIO_Pin_12 );
    }
    else
    {
        //GPIO_SetBits( GPIOB, GPIO_Pin_12 );
    }
}

void GPIO_Gyro_Chip_Select(uint8_t uSelect)
{
    if(uSelect)
    {
        //GPIO_ResetBits( GPIOB, GPIO_Pin_12 );
    }
    else
    {
        //GPIO_SetBits( GPIOB, GPIO_Pin_12 );
    }
}
#endif

