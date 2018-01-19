/**
  ******************************************************************************
  * @file    CwmDriverHal.h
  * @author  Cywee Motion AE
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

#include <stdint.h>

#ifndef __CWM_DRIVER_HAL_H__
#define __CWM_DRIVER_HAL_H__

#include "cwmProjectConfig.h"

#define SENSOR_PRVIATE_SETTING_LEN  6
#define DRIVER_SPI_MAX_RETRY    (3)


#define min(x,y)         (x < y?x:y)
#define max(x,y)         (x > y?x:y)

#define BitSet(x)           ((uint64_t)1 << x)
#define BitSetData(x,y)     (x | ((uint64_t)1 << y))
#define BitCheck(x,y)       (x & ((uint64_t)1 << y))
#define BitCheckAND(x,y)    ((x & ((uint64_t)1 << y)) && ((uint64_t)1 << y))
#define BitClean(x,y)       (x & (~((uint64_t)1 << y)))
#define Compare(x,y)        ((x==y)?1:0)
#define BitSetDataToX(x,y)     x = (x | ((uint64_t)1 << y))
#define BitCleanDataX(x,y)     x = (x & (~((uint64_t)1 << y)))

typedef enum {
    SELECT      = 1,
    DESELECT    = 0
} MEMS_CHIP_SEL;

typedef enum {
    FASTEST = 5,
    RATE_100 = 10,
    GAME = 20,
    RATE_33 = 30,
    RATE_25 = 40,
    UI = 60,
    RATE_10 = 100,
    NORMAL = 200,
    RATE_1HZ = 1000
} RATE;

typedef enum _emPlatformSensorInterfaceType {
    emI2C1 = 0,
    emI2C2,
    emSPI1_WH_RL,
    emSPI1_WL_RH,
    emSPI2_WH_RL,
    emSPI2_WL_RH
} emPlatformSensorInterfaceType;

typedef enum {
    SENSOR_DATA_NOT_READY = 0,
    SENSOR_DATA_READY,
    SENSOR_DATA_INTERRUPT,
    SENSOR_COMBO_INTERRUPT
} SENSOR_STATUS;

typedef enum {
    MODE_BYPASS = 0,
    MODE_FIFO_STREAM,
    MODE_INT,
    MODE_LOWPOWER,
    MODE_SPECIAL,
    MODE_BASIC = 0x07,
} SENSORS_MODE;

typedef enum {
    INTERRUPT_OFF = 0,
    INTERRUPT_USE_INT1 = 1,
    INTERRUPT_USE_INT2 = 2,
} INTERRUPT_SETTING;

typedef enum {
    COMM_FAIL                   = -18,
    ERR_I2cComandBlock          = -17,
    ERR_I2cSlave                = -16,
    ERR_I2cMaster               = -15,
    ERR_UsageFault              = -14,
    ERR_BusFault                = -13,
    ERR_MemManage               = -12,
    ERR_FLASH                   = -11,
    ERR_TASK_BLOCK              = -10,
    SEND_QUEUE_FULL             = -9,
    DRIVER_CHECK_CHIP_ID_FAIL   = -8,
    DRIVER_ENABLE_FAIL          = -7,
    DRIVER_DISABLE_FAIL         = -6,
    DRIVER_GETDATA_FAIL         = -5,
    I2C_FAIL                    = -4,
    DRIVER_NO_USE               = -3,
    SENSORS_NO_INITIAL          = -2,
    cwFAIL                      = -1,
    NO_ERROR                    =  0,
    NO_DATA                     =  1
} ERR_MSG;

typedef enum {
    SELFTEST_STATUS_FAIL= -1,
    SELFTEST_STATUS_NON = 0,
    SELFTEST_STATUS_INPROCESS = 1,
    SELFTEST_STATUS_PASS = 2,    
}SELFTEST_STATUS;

typedef int(*FUNCTION_PTR_I2C)(uint8_t ,uint8_t ,uint8_t *,int);
typedef int(*FUNCTION_PTR_INTERFACE_ACCESS)(void *, uint8_t ,uint8_t *,int);
typedef uint32_t(*FUNCTION_PTR_UINT32_VOID)(void);
typedef int(*FUNCTION_PTR_INT_INT)(int);
typedef int(*FUNCTION_PTR_INT_VOID)(void);
typedef int(*FUNCTION_PTR_INT_SENEVENT_PTR)(float *);
typedef int(*FUNCTION_PTR_INT_UINT8T_PTR)(uint8_t *);
typedef int(*FUNCTION_PTR_INT_UINT8T)(uint8_t);
typedef uint8_t(*FUNCTION_PTR_UINT8_UINT8T_PTR)(uint8_t *);

typedef struct {
    emPlatformSensorInterfaceType   eInterfaceType;
    FUNCTION_PTR_INT_VOID           HwInit;
    FUNCTION_PTR_INT_VOID           Enable;
    FUNCTION_PTR_INT_VOID           Disable;
    FUNCTION_PTR_INT_SENEVENT_PTR   GetBypassData;
    FUNCTION_PTR_INT_SENEVENT_PTR   GetFIFOData;
    FUNCTION_PTR_INT_UINT8T_PTR     GetStatus;
    FUNCTION_PTR_INT_UINT8T         SetStatus;
    FUNCTION_PTR_INT_VOID           SelftTest;
    FUNCTION_PTR_INT_VOID           SetOffset;
#ifdef SPI_MASTER_SUPPORT
    FUNCTION_PTR_INT_UINT8T         SensorChipSelect;
#endif
    FUNCTION_PTR_INT_INT            SetRate;
    FUNCTION_PTR_INT_INT            SetMode;
    FUNCTION_PTR_INT_UINT8T         SetSetting;
    FUNCTION_PTR_INT_UINT8T         CheckChipId;
    FUNCTION_PTR_INT_VOID           ScanSlaveAddr;
    uint8_t                         uHWSensorType;
    uint8_t                         uHWSensorID;
}DriverServiceCallback_t, *pDriverServiceCallback_t;

typedef struct {
    uint8_t                         uSensorID;
    emPlatformSensorInterfaceType   eInterfaceType;
}DriverInterfaceSetting_t, *pDriverInterfaceSetting_t;

typedef struct {

    const DriverServiceCallback_t *pdrvCallBack;
#ifdef SPI_MASTER_SUPPORT
    const DriverInterfaceSetting_t *pdrvInterafceSetting;
#endif
    uint8_t             uHWSensorSlaveAddr;
    uint8_t             rate;
    volatile uint8_t    ustatus;
    /*** Don't change this order, we do memory copy in somecall ***/
    uint8_t             mode;
    uint8_t             position;
    union {
        struct  {
            uint8_t     uSensorPeriod;
            uint8_t     uSensorThreshold;
            uint8_t     uSensorIntrMode;
            uint8_t     uSensorReserved;
            };
        struct  {
            uint8_t     PrviateReserved[SENSOR_PRVIATE_SETTING_LEN];
            };
    };
    /*** Don't change this order, we do memory copy in somecall ***/
    uint8_t            *private_data;  //byte 0 : id,byte 1 : accuracy,  byte 2 ~ byte n data
    uint8_t            *private_log;   //byte 0 : size  byte 1 ~ byte n data
    float               hwbias[3];
    uint16_t            odr_Hz;
    uint32_t            timestamp;
    uint8_t             uHWChipId[2];

}DriverSensorMem_t, *pDriverSensorMem_t;

int DriverAPIEnable( int id);
int DriverAPIDisable( int id );
int DriverAPISetRate( int id, int rate );
int DriverAPIGetStatus( int id, uint8_t *count);
int DriverAPIGetRawData( int id, SENSORS_MODE mode, float *raw_data);
uint16_t DriverAPIGetODR(int id);
void DriverHalSensorConfig(void);
int DriverHALSensorHwInit( void );
int DriverHALInterfaceRead(
    pDriverSensorMem_t  pSensorMem,
    uint8_t             RegAddr,
    uint8_t*            pBuffer,
    int                 NumByteToRead
    );
int DriverHALInterfaceWrite(
    pDriverSensorMem_t  pSensorMem,
    uint8_t             RegAddr,
    uint8_t*            pBuffer,
    int                 NumByteToWrite
    );
void osTaskDelay(int ms);
int DriverHALSelectDefaultSensor( ENTITY_ID  sensor_id, HW_ID hw_id, uint8_t uSlaveAddress );


#endif
