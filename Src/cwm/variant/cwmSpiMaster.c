#include <stdint.h>

#define DUMMY_BYTE  ((uint8_t)0x00)

#define SPI_TIMEOUT ((uint32_t)5000)

#define SPI_PORT            SPI2
#define SPI_PORT_CLK        RCC_APB1Periph_SPI2
#define SPI_CLK_INIT        RCC_APB1PeriphClockCmd

#define SPI_GPIO_AF         GPIO_AF_SPI2

#define PXI_CSN_PORT        GPIOC
#define PXI_CSN_PORT_CLK    RCC_AHB1Periph_GPIOC
#define PXI_CSN_PIN         GPIO_Pin_13

#define BMI_CSN_PORT        GPIOB
#define BMI_CSN_PORT_CLK    RCC_AHB1Periph_GPIOB
#define BMI_CSN_PIN         GPIO_Pin_12

#define SCK_PORT            GPIOB
#define SCK_PORT_CLK        RCC_AHB1Periph_GPIOB
#define SCK_PIN             GPIO_Pin_13
#define SCK_PIN_SOURCE      GPIO_PinSource13

#define MISO_PORT           GPIOB
#define MISO_PORT_CLK       RCC_AHB1Periph_GPIOB
#define MISO_PIN            GPIO_Pin_14
#define MISO_PIN_SOURCE     GPIO_PinSource14

#define MOSI_PORT           GPIOB
#define MOSI_PORT_CLK       RCC_AHB1Periph_GPIOB
#define MOSI_PIN            GPIO_Pin_15
#define MOSI_PIN_SOURCE     GPIO_PinSource15

#define PXI_CW_CS_LOW()     GPIO_ResetBits(PXI_CSN_PORT, PXI_CSN_PIN)
#define PXI_CW_CS_HIGH()    GPIO_SetBits(PXI_CSN_PORT, PXI_CSN_PIN)

#define BMI_CW_CS_LOW()     GPIO_ResetBits(BMI_CSN_PORT, BMI_CSN_PIN)
#define BMI_CW_CS_HIGH()    GPIO_SetBits(BMI_CSN_PORT, BMI_CSN_PIN)

volatile uint8_t flag_cw_spi_reset = 0;

void cwm_spi_master_init( void );
int cwm_spi_write( uint8_t *pBuffer, uint8_t WriteAddr, uint16_t NumByteToWrite );
int cwm_spi_read( uint8_t *pBuffer, uint8_t ReadAddr, uint16_t NumByteToRead );
static uint8_t cwmSpiSendByte( uint8_t byte );

uint32_t cwm_timeout_userCb(void);

#pragma optimize=none
void cwm_spi_master_init( void )
{
    SPI_InitTypeDef  SPI_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    
    /* Enable GPIO Port CLK for CSN/SCK/MISO/MOSI */
    RCC_AHB1PeriphClockCmd(PXI_CSN_PORT_CLK, ENABLE);
    RCC_AHB1PeriphClockCmd(BMI_CSN_PORT_CLK, ENABLE);
    RCC_AHB1PeriphClockCmd(SCK_PORT_CLK,     ENABLE);
    RCC_AHB1PeriphClockCmd(MISO_PORT_CLK,    ENABLE);
    RCC_AHB1PeriphClockCmd(MOSI_PORT_CLK,    ENABLE);
    
    /* Enable SPI1 CLK */
    SPI_CLK_INIT(SPI_PORT_CLK, ENABLE);
    
    /* Connect SPI pins (SCK, MISO, MOSI) to AFx */
    GPIO_PinAFConfig(SCK_PORT,  SCK_PIN_SOURCE,  SPI_GPIO_AF);
    GPIO_PinAFConfig(MISO_PORT, MISO_PIN_SOURCE, SPI_GPIO_AF);
    GPIO_PinAFConfig(MOSI_PORT, MOSI_PIN_SOURCE, SPI_GPIO_AF);
    
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
    
    GPIO_InitStructure.GPIO_Pin = SCK_PIN;
    GPIO_Init(SCK_PORT, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = MISO_PIN;
    GPIO_Init(MISO_PORT, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = MOSI_PIN;
    GPIO_Init(MOSI_PORT, &GPIO_InitStructure);
    
    /* Connect CSN Pin to GPIO */
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
    
//    GPIO_InitStructure.GPIO_Pin = PXI_CSN_PIN;
//    GPIO_Init(PXI_CSN_PORT, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = BMI_CSN_PIN;
    GPIO_Init(BMI_CSN_PORT, &GPIO_InitStructure);
    
//    GPIO_SetBits(PXI_CSN_PORT, PXI_CSN_PIN);
    GPIO_SetBits(BMI_CSN_PORT, BMI_CSN_PIN);
    
    /* SPI configuration */
    SPI_Cmd(SPI_PORT, DISABLE);
    SPI_I2S_DeInit(SPI_PORT);
    SPI_InitStructure.SPI_Direction         = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode              = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize          = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL              = SPI_CPOL_High;
    SPI_InitStructure.SPI_CPHA              = SPI_CPHA_2Edge;
    SPI_InitStructure.SPI_NSS               = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
    SPI_InitStructure.SPI_FirstBit          = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial     = 7;
    SPI_Init(SPI_PORT, &SPI_InitStructure);
    
    /* Enable SPI1  */
    SPI_Cmd(SPI_PORT, ENABLE);
}

#pragma optimize=none
int cwm_spi_write(uint8_t *pBuffer, uint8_t WriteAddr, uint16_t NumByteToWrite )
{
    uint8_t delay = 0;
    
    int retval;
    
    /* Send the Address of the indexed register */
    cwmSpiSendByte(WriteAddr);

    for( delay=0 ; delay<25 ; delay++ );
    
    /* Send the data that will be written into the device (MSB First) */
    while( (NumByteToWrite >= 0x01) && (flag_cw_spi_reset == 0) )
    {
        cwmSpiSendByte(*pBuffer);
        NumByteToWrite--;
        pBuffer++;
        
        for( delay=0 ; delay<25 ; delay++ );
    }
    
    if( flag_cw_spi_reset == 1 )
        retval = -1;
    else
        retval = 0;
    
    flag_cw_spi_reset = 0;
    
    return retval;
}

#pragma optimize=none
int cwm_spi_read(uint8_t *pBuffer, uint8_t ReadAddr, uint16_t NumByteToRead )
{  
    uint8_t delay = 0;
    
    int retval;
    
    /* Send the Address of the indexed register */
    cwmSpiSendByte(ReadAddr);
  
    for( delay=0 ; delay<25 ; delay++ );
    
    /* Receive the data that will be read from the device (MSB First) */
    while( (NumByteToRead > 0x00) && (flag_cw_spi_reset == 0) )
    {
        /* Send dummy byte (0x00) to generate the SPI clock to L3GD20 (Slave device) */
        *pBuffer = cwmSpiSendByte(DUMMY_BYTE);
        NumByteToRead--;
        pBuffer++;
        
        for( delay=0 ; delay<25 ; delay++ );
    }
    
    if( flag_cw_spi_reset == 1 )
        retval = -1;
    else
        retval = 0;
    
    flag_cw_spi_reset = 0;
    
    return retval;
}

#pragma optimize=none
static uint8_t cwmSpiSendByte(uint8_t byte)
{
    uint32_t Timeout;
    
    /* Loop while DR register in not empty */
    Timeout = SPI_TIMEOUT;

    while (SPI_I2S_GetFlagStatus(SPI_PORT, SPI_I2S_FLAG_TXE) == RESET)
    {
        if((Timeout--) == 0) 
            return cwm_timeout_userCb();
    }
  
    /* Send a Byte through the SPI peripheral */
    SPI_I2S_SendData(SPI_PORT, (uint16_t)byte);
  
    /* Wait to receive a Byte */
    Timeout = SPI_TIMEOUT;
    while (SPI_I2S_GetFlagStatus(SPI_PORT, SPI_I2S_FLAG_RXNE) == RESET)
    {
        if((Timeout--) == 0) 
            return cwm_timeout_userCb();
    }
  
    /* Return the Byte read from the SPI bus */
    return (uint8_t)SPI_I2S_ReceiveData(SPI_PORT);
}

uint32_t cwm_timeout_userCb(void)
{
    /* Block communication and all processes */
    CwSPI_Master_Init();
    
    flag_cw_spi_reset = 1;
    
    return 0;
}
