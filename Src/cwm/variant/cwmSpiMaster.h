#ifndef __CWM_SPI_MASTER_H__
#define __CWM_SPI_MASTER_H__
extern void cwm_spi_master_init( void );
extern int cwm_spi_write( uint8_t *pBuffer, uint8_t WriteAddr, uint16_t NumByteToWrite );
extern int cwm_spi_read( uint8_t *pBuffer, uint8_t ReadAddr, uint16_t NumByteToRead );
#endif