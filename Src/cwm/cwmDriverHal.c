/**
  ******************************************************************************
  * @file    cwmDriverHal.c
  * @author  Cywee Motion - AE
  * @version V1.0
  * @date    14-NOV-2017
  * @brief   First create
  *           
  ******************************************************************************
  * @attention
  *
  * COPYRIGHT 2017 Cywee Motion
  *
  * Licensed under Cywee Motion License Agreement
  * You may not use this file except in compliance with the License.
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************  
  */
#include "CwmDriverHal.h"
#include "FreeRTOS.h"
#include "task.h"
#include "customIntf.h"
#ifdef DRIVER_USE_BMI160_SPI
#include "DriverBMI160_SPI.h"
#endif

volatile uint32_t   g_DriverEnableList;
DriverSensorMem_t   g_DriverHalMem[ENTITY_ID_END];

extern cwm_spiCb_t cb_SPIWrite;
extern cwm_spiCb_t cb_SPIRead;

void osTaskDelay(int ms)
{
    /* user-written implementation-specific source code */
    vTaskDelay((ms*1000 + 999) / 1000 / portTICK_RATE_MS); // sleep in 1 ms unit
    // LOGI("\nCalled mt_device_usleep=%dusec)", usec);
}

#if 0 // TODO:
static void mt_device_usleep(int usec)
{
    /* user-written implementation-specific source code */
    vTaskDelay((usec + 999) / 1000 / portTICK_RATE_MS); // sleep in 1 ms unit
    // LOGI("\nCalled mt_device_usleep=%dusec)", usec);
}

static int sensor_I2C_write(uint8_t slave_addr, uint8_t addr, uint8_t value)
{
    int32_t ret;
    uint8_t send[2];

    send[0] = addr;
    send[1] = value;

    //ret = mems_i2c_write(slave_addr, send, 2U);

    mt_device_usleep(2); // 2 us acoording to datasheet

    return ret;
}

static int sensor_I2C_read(uint8_t slave_addr, uint8_t addr, uint8_t *result)
{
    uint8_t send;
    //int32_t ret = HAL_I2C_STATUS_OK;

    send = (addr) | 0x00;

    uint8_t receive[2] = {0};

    //ret = mems_i2c_write_read(slave_addr, send, receive, 1);
    *result = receive[0];


    return ret;
}

static int sensor_I2C_multipe_read(uint8_t slave_addr, uint8_t addr, uint8_t *result, uint16_t len)
{
    uint8_t send;
    //int32_t ret = HAL_I2C_STATUS_OK;

    send = (addr) | 0x00;

    //ret = mems_i2c_write_read(slave_addr, send, result, len);

    return ret;
}
#endif

int DriverAPIEnable( int id)
{
    int rt = SENSORS_NO_INITIAL;
    
    if(g_DriverHalMem[id].pdrvCallBack != NULL){
        rt = g_DriverHalMem[id].pdrvCallBack->Enable();
        BitSetDataToX(g_DriverEnableList,id);
    }
    return rt;
}

int DriverAPIDisable( int id )
{
    int rt = SENSORS_NO_INITIAL;

    if(g_DriverHalMem[id].pdrvCallBack !=NULL){
        rt = g_DriverHalMem[id].pdrvCallBack->Disable();
        BitCleanDataX(g_DriverEnableList,id);
    }
    return rt;
}

int DriverAPISetRate( int id, int rate )
{
    int rt = cwFAIL;
    
    if(g_DriverHalMem[id].pdrvCallBack !=NULL){
        rt = g_DriverHalMem[id].pdrvCallBack->SetRate(rate);
        g_DriverHalMem[id].rate = rate;
    }
    return rt;
}

int DriverAPIGetStatus( int id, uint8_t *count)
{
    int rt = SENSORS_NO_INITIAL;

    *count = 0;
    if(g_DriverHalMem[id].pdrvCallBack != NULL){
        rt = g_DriverHalMem[id].pdrvCallBack->GetStatus(count);
    }
    return rt;
}

int DriverAPIGetRawData( int id, SENSORS_MODE mode, float *raw_data)
{
    int rt = SENSORS_NO_INITIAL;

    raw_data[0] = 0.0;
    raw_data[1] = 0.0;
    raw_data[2] = 0.0;
    if(g_DriverHalMem[id].pdrvCallBack != NULL){
        if (mode == MODE_BYPASS){
            rt = g_DriverHalMem[id].pdrvCallBack->GetBypassData(raw_data);
        }else if (mode == MODE_FIFO_STREAM){
            rt = g_DriverHalMem[id].pdrvCallBack->GetFIFOData(raw_data);
        }
    }
    return rt;
}

uint16_t DriverAPIGetODR(int id)
{
    return g_DriverHalMem[id].odr_Hz;
}

void DriverHalSensorConfig(void)
{
    #ifdef ACCELERATION
    DriverHALSelectDefaultSensor(ACCELERATION, ACC_HW_ID, ACC_SLAVE_ADDR);
    #endif
    #ifdef MAGNETIC
    DriverHALSelectDefaultSensor(MAGNETIC, MAG_HW_ID, MAG_SLAVE_ADDR);
    #endif
    #ifdef GYRO
    DriverHALSelectDefaultSensor(GYRO, GYRO_HW_ID, GYRO_SLAVE_ADDR);
    #endif
    #ifdef MOTION
    DriverHALSelectDefaultSensor(MOTION, MOTION_HW_ID,MOTION_SLAVE_ADDR);
    #endif
}

int DriverHALSensorHwInit( void )
{
    int i = 0;
    for(i=0;i<ENTITY_ID_END;i++){
        if(g_DriverHalMem[i].pdrvCallBack !=NULL)
            g_DriverHalMem[i].pdrvCallBack->HwInit();
    }
    return 0;
}

int DriverHALInterfaceRead(
    pDriverSensorMem_t  pSensorMem,
    uint8_t             RegAddr,
    uint8_t*            pBuffer,
    int                 NumByteToRead
    )
    {
        int uretry = 0;
        uint8_t udelay = 0;
    
        emPlatformSensorInterfaceType  eInterType = emSPI2_WL_RH;
    
#ifdef SPI_MASTER_SUPPORT
        if(pSensorMem->pdrvInterafceSetting)
        {
            eInterType = pSensorMem->pdrvInterafceSetting->eInterfaceType;
        }
#endif
    
        switch( eInterType )
        {
#ifdef SPI_MASTER_SUPPORT
            case emSPI1_WH_RL:
            case emSPI1_WL_RH:
            case emSPI2_WH_RL:
            case emSPI2_WL_RH:
                // step 1: asserted chip select pin for which device
                pSensorMem->pdrvCallBack->SensorChipSelect( SELECT );
    
                // step 2: read with MSB high check
                if( (eInterType == emSPI1_WL_RH) ||
                    (eInterType == emSPI2_WL_RH)
                   )
                {
                    RegAddr = RegAddr | 0x80;
                }
    
                // step 3:
                while( uretry++ < DRIVER_SPI_MAX_RETRY )
                {
                    if(cb_SPIRead !=NULL){
                        if(cb_SPIRead(pBuffer, RegAddr, NumByteToRead) == 0)
                            break;
                    }
                    if( uretry >= DRIVER_SPI_MAX_RETRY )
                        return I2C_FAIL;
                }
    
                // step 4: de-asserted chip select pin for which device
                pSensorMem->pdrvCallBack->SensorChipSelect( DESELECT );
    
                // step 5: delay for timing constraint
                for( udelay=0 ; udelay<100 ; udelay++ );
    
                break;
#endif
        }
    
        return NO_ERROR;
    }


int DriverHALInterfaceWrite(
    pDriverSensorMem_t  pSensorMem,
    uint8_t             RegAddr,
    uint8_t*            pBuffer,
    int                 NumByteToWrite
    )
    {
        uint8_t uretry = 0;
        uint8_t udelay = 0;
    
        emPlatformSensorInterfaceType  eInterType = emSPI2_WL_RH;
    
#ifdef SPI_MASTER_SUPPORT
        if(pSensorMem->pdrvInterafceSetting)
        {
            eInterType = pSensorMem->pdrvInterafceSetting->eInterfaceType;
        }
#endif
    
        switch( eInterType )
        {
    
#ifdef SPI_MASTER_SUPPORT
            case emSPI1_WH_RL:
            case emSPI1_WL_RH:
            case emSPI2_WH_RL:
            case emSPI2_WL_RH:
                // step 1: asserted chip select pin for which device
                pSensorMem->pdrvCallBack->SensorChipSelect( SELECT );
    
                // step 2: write with MSB high check
                if( (eInterType == emSPI1_WH_RL) ||
                    (eInterType == emSPI2_WH_RL)
                   )
                {
                    RegAddr = RegAddr | 0x80;
                }
    
                // step 3:
                while( uretry++ < DRIVER_SPI_MAX_RETRY )
                {
                    if(cb_SPIWrite !=NULL){
                        if(cb_SPIWrite(pBuffer, RegAddr, NumByteToWrite) == 0)
                            break;
                    }    
                    if( uretry == DRIVER_SPI_MAX_RETRY )
                        return I2C_FAIL;
                }
    
                // step 4: de-asserted chip select pin for which device
                pSensorMem->pdrvCallBack->SensorChipSelect( DESELECT );
    
                // step 5: delay for timing constraint
                for( udelay=0 ; udelay<100 ; udelay++ );
    
                break;
#endif
        }
    
        return NO_ERROR;
    }


int DriverHALSelectDefaultSensor( ENTITY_ID  sensor_id, HW_ID hw_id, uint8_t uSlaveAddress )
{
    int Ret = DRIVER_NO_USE;
    
    switch(sensor_id)
    {
    #ifdef ACCELERATION
        case ACCELERATION:
            switch(hw_id)
            {
                #ifdef DRIVER_USE_BMI160_SPI
                case BMI160:    Ret = DriverBMI160SPIAccInit(&g_DriverHalMem[sensor_id]);  break;
                #endif
                default:    
                    //LOGEI(LOG_ERROR, "DH:%d,ERROR:%d\n",__LINE__,sensor_id);  
                    break;
            }
            break;
    #endif
    #ifdef MAGNETIC
        case MAGNETIC:
            switch(hw_id)
            {
                default:
                    //LOGEI(LOG_ERROR, "DH:%d,ERROR:%d\n",__LINE__,sensor_id);
                    break;
            }
            break;
    #endif
    #ifdef GYRO
        case GYRO:
            switch(hw_id)
            {
                #ifdef DRIVER_USE_BMI160_SPI
                case BMI160:    Ret = DriverBMI160SPIGyroInit(&g_DriverHalMem[sensor_id]); break;
                #endif
                default:
                    //LOGEI(LOG_ERROR, "DH:%d,ERROR:%d\n",__LINE__,sensor_id);
                    break;
            }
            break;
    #endif
    #ifdef MOTION
        case MOTION:
            switch(hw_id)
            {
                #ifdef DRIVER_USE_BMI160_SPI
                case BMI160:    Ret = DriverBMI160SPIMotionInit(&g_DriverHalMem[sensor_id]); break;
                #endif
                default:
                    //LOGEI(LOG_ERROR, "DH:%d,ERROR:%d\n",__LINE__,sensor_id);
                    break;
            }
            break;
    #endif
        default:
            break;
    }

    /* Indicate the HW sensor SlaveAddress */
    g_DriverHalMem[sensor_id].uHWSensorSlaveAddr = uSlaveAddress;

    return Ret;
}



