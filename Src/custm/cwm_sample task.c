
#include <stdio.h>
#include <string.h>
#include "main.h"
#include "stm32f4xx_hal.h"
#include "cmsis_os.h"
#include "customIntf.h"
#include "cwmProjectConfig.h"
#include "cwm_sample_task.h"


#define Task_Name "CWMTask"

extern SPI_HandleTypeDef hspi2;

static cwm_sensor_evt mySenEvt[4];

static void cwm_senevt_cb(cwm_sensor_evt senEvt,void *reserved);
static void cwm_logout_cb(const char *msg ,...);
static void cwm_senevt_cb(cwm_sensor_evt senEvt,void *reserved)
{
    switch(senEvt.id){
        case ACCELERATION:
            memcpy(&mySenEvt[0],&senEvt, sizeof(cwm_sensor_evt));
            break;
        case GYRO:
            memcpy(&mySenEvt[1],&senEvt, sizeof(cwm_sensor_evt));
            break;
        case ORIENTATION:
            memcpy(&mySenEvt[2],&senEvt, sizeof(cwm_sensor_evt));
            break;
        case CWM_AIR_MOUSE:
            memcpy(&mySenEvt[3],&senEvt, sizeof(cwm_sensor_evt));
            break;
        default:
            break;
    }

}

static void cwm_logout_cb(const char *msg ,...)
{ 
    /* print the log here!! */
    //static char log[32] = {0};
    //memcpy(log, msg, sizeof(char)*32);
}

static int spiWrite(uint8_t *pBuffer, uint8_t WriteAddr, uint16_t NumByteToWrite )
{
    uint8_t pTxData[255];
    uint8_t pRxData[255];
    int status;
    pTxData[0] = WriteAddr;
    memcpy(&pTxData[1], pBuffer, NumByteToWrite);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
    status = HAL_SPI_TransmitReceive(&hspi2, pTxData, pRxData, NumByteToWrite + 1, 100);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);
    return status;
}
static int spiRead(uint8_t *pBuffer, uint8_t ReadAddr, uint16_t NumByteToRead )
{
    uint8_t pTxData[255];
    uint8_t pRxData[255];
    int status;
    pTxData[0] = ReadAddr;
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
    status = HAL_SPI_TransmitReceive(&hspi2, pTxData, pRxData, NumByteToRead + 1, 100);
    memcpy(pBuffer, &pRxData[1], NumByteToRead);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);
    return status;
}

static void CWM_Task(const void *argument)
{  
    cwm_task_init();
    cwm_sensor_pos_set(ACCELERATION,3);
    cwm_sensor_pos_set(GYRO,3);
    cwm_senevt_cb_register(cwm_senevt_cb);
    cwm_logout_cb_register(cwm_logout_cb);
    cwm_spi_cb_register(spiWrite, spiRead);

    cwm_sensor_enable(ACCELERATION,RT_FASTEST);
    cwm_sensor_enable(GYRO,RT_FASTEST);
    cwm_sensor_enable(CWM_AIR_MOUSE,RT_FASTEST);

    for (;;)
    {
        osDelay(10);
    }
}

void CWM_TASK_INIT(void)
{

    osThreadDef(Task_Name, CWM_Task, osPriorityNormal, 0, configMINIMAL_STACK_SIZE);
    osThreadCreate(osThread(Task_Name), NULL);
}

