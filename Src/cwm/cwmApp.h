/**
  ******************************************************************************
  * @file    cwmApp.h
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

#ifndef __CWMAPP_H__
#define __CWMAPP_H__

#include "cwmProjectConfig.h"
#include "FreeRTOS.h"
#include "task.h"

#define SENSOR_IDLE_PERIOD      1000
#define MAX_SENSOR_SUB_EN       3
#define MAX_SENSOR_POS          8

enum {
    SENSOR_READ_TIMER_NO = 1,
};

typedef enum sensor_message_id {
    SENSOR_DEV_POWER_ON_REQ,
    SENSOR_DEV_CONFIG_REQ,
    SENSOR_DEV_READY_TO_READ,
    SENSOR_DEV_READY_TO_WRITE,
} sensor_message_id_enum;

typedef enum sensor_cmd_message_id {
    SENSOR_CMD_DEV_POWER_ON_REQ,
    SENSOR_CMD_DEV_SET_REQ,
} sensor_cmd_message_id_enum;

enum {
    SENSOR_CMD_ENABLE,
    SENSOR_CMD_DISABLE,
    SENSOR_CMD_POSITION,
    SENSOR_CMD_CALIBRATOR,
};

typedef struct sensor_message_struct {
    int message_id;
    int param1;
    void* param2;
} sensor_message_struct_t;

typedef struct sensor_cmd_message_struct {
    int message_id;
    int param1;
    int param2;
    int param3;
} sensor_cmd_message_struct_t;

typedef struct {
    uint8_t id;
    uint8_t reserved[3];
    float data[3];
    union {
        struct {
            float raw_data[3]; /* physical sensor uncalibrated data */
            float hwbias[3]; /* physical sensor calibrated data bias */
        };
        struct {
            float algo_data[6]; /* virtual sensor ouput data */
        };
    };    
    uint32_t timestamp;
} sensor_evt_t,*psensor_evt_t;


typedef struct {
    uint8_t enable;
    uint16_t rate;
    uint32_t timestamp;
}sensor_list_t;

typedef enum {
    SENSOR_REPORT_CONTINUOUS_MODE=0,
    SENSOR_REPORT_FIFO_MODE,
    SENSOR_REPORT_GESTURE_MODE,
    SENSOR_REPORT_ON_CHANGE_MODE,
}SENSOR_REPORT_MODE;

typedef struct {
    uint8_t enable;
    uint16_t rate;
}EnList_t;

typedef struct {
    uint8_t SensorID;
    uint16_t SensorRate;
}EnSetting_t;

typedef struct {
    uint8_t name[32];
    uint8_t report_mode;
    EnSetting_t enSetting[MAX_SENSOR_SUB_EN];
}SensorInfo_t;

typedef enum {
    SENSOR_DIS = 0,
    SENSOR_EN,
} SensorEN_t;

extern sensor_list_t SensorMgr[SENSOR_ID_END];
extern sensor_evt_t SensorData[SENSOR_ID_END];
extern TaskHandle_t sensor_app_create(void);
extern TaskHandle_t sensor_cmd_app_create(void);
extern int cwm_send_log(const char *msg,...);
void sensor_cmd_app_set(int param1, int param2, int param3);
void sensor_cmd_app_set_fromISR(int param1, int param2, int param3);
void sensorInit(void); /* for customized interface */
void sensorMain( void ); /* for customized interface */
void sensorEvtReport( int id, uint32_t curTimestamp );
int sensorSetPosition(uint16_t id, uint16_t position);
int sensorCommandEnable(uint16_t id, uint16_t rate);
int sensorCommandDisable(uint16_t id);
#endif
