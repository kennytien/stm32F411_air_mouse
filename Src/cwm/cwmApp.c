/**
  ******************************************************************************
  * @file    cwmApp.c
  * @author  Cywee Motion - AE
  * @version V1.0
  * @date   14-NOV-2017
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

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "cwmApp.h"
#include "cwmDriverHal.h"
#include "cwmAlgorithmHal.h"
#include "cwmTaskSen.h"
#include "cwmCalibrator.h"
#include "customIntf.h"

//#define SENSOR_PROFILE

uint32_t SysTimestamp = 0;
sensor_list_t SensorCmd[SENSOR_ID_END]={0,};
sensor_list_t SensorMgr[SENSOR_ID_END]={0,};
sensor_evt_t SensorData[SENSOR_ID_END]={0,};
sensor_evt_t SensorRptData[SENSOR_ID_END]={0,};
uint8_t SensorPos[ENTITY_ID_END]={0,};

const SensorInfo_t SensorInfo[SENSOR_ID_END] = {
#ifdef ACCELERATION
    {.name = "Accelerometer",
     .report_mode = SENSOR_REPORT_CONTINUOUS_MODE,
    },
#endif
#ifdef MAGNETIC
    {.name = "Magnetometer",
     .report_mode = SENSOR_REPORT_CONTINUOUS_MODE,
    },
#endif
#ifdef GYRO
    {.name = "Gyroscope",
     .report_mode = SENSOR_REPORT_FIFO_MODE,
    },
#endif
#ifdef MOTION
    {.name = "Motion",
     .report_mode = SENSOR_REPORT_GESTURE_MODE,
    },
#endif
#ifdef ORIENTATION
    {.name = "Orientation",
     .report_mode = SENSOR_REPORT_CONTINUOUS_MODE,
     .enSetting[0] = {.SensorID = ACCELERATION,.SensorRate = FASTEST},
     .enSetting[1] = {.SensorID = GYRO,.SensorRate = FASTEST},
     #ifdef MAGNETIC
     .enSetting[2] = {.SensorID = MAGNETIC,.SensorRate = FASTEST},
     #endif
    },
#endif
#ifdef ROTATION_VECTOR
    {.name = "Rotation Vector",
     .report_mode = SENSOR_REPORT_CONTINUOUS_MODE,
     .enSetting[0] = {.SensorID = ACCELERATION,.SensorRate = FASTEST},
     .enSetting[1] = {.SensorID = GYRO,.SensorRate = FASTEST},
    #ifdef MAGNETIC
     .enSetting[2] = {.SensorID = MAGNETIC,.SensorRate = FASTEST},
    #endif
    },
#endif
#ifdef CWM_AIR_MOUSE
    {.name = "Air Mouse",
     .report_mode = SENSOR_REPORT_CONTINUOUS_MODE,
     .enSetting[0] = {.SensorID = ACCELERATION,.SensorRate = FASTEST},
     .enSetting[1] = {.SensorID = GYRO,.SensorRate = FASTEST},
    },
#endif
};

static void sensorDataInit(void);


uint32_t SensorTimestamp( void )
{
    return xTaskGetTickCount();
}

uint32_t acc_cnt = 0, gyro_cnt = 0;


void sensorInit(void)
{    
    /* sensor driver initialize */
    DriverHalSensorConfig();
    DriverHALSensorHwInit();

    /* algorithm initialize */
    algorithmInit();

    /* sensor manager data initialize */
    sensorDataInit();
    
    /* timer for sensor task start */
    sensor_timer_start();
}

static void sensorDataInit(void)
{
    memset(SensorCmd,0,sizeof(SensorCmd));
    memset(SensorMgr,0,sizeof(SensorMgr));
    memset(SensorData,0,sizeof(SensorData));
    memset(SensorRptData,0,sizeof(SensorRptData));
    for (int i=0;i<SENSOR_ID_END;i++){
        SensorData[i].id = i;
        SensorRptData[i].id = i;
    }

    #ifdef ACCELERATION
    SensorPos[ACCELERATION] = ACC_POSITION;
    #endif
    #ifdef MAGNETIC
    SensorPos[MAGNETIC] = MAG_POSITION;
    #endif
    #ifdef GYRO
    SensorPos[GYRO] = GYR_POSITION;
    #endif

}

static void sensorTimerControl(void)
{
    uint16_t timer = SENSOR_IDLE_PERIOD;
    
    for (int i=0;i<ENTITY_ID_END;i++){
        if (SensorMgr[i].enable == SENSOR_EN){
            if (SensorMgr[i].rate < timer)
                timer = SensorMgr[i].rate;
        }
    }
    sensor_timer_change(timer);
}

static void sensorDataBias(CALIB_TYPE_T CalType, sensor_evt_t *senData)
{
    getCalibratorBias(CalType, senData->hwbias);
    senData->data[0] = senData->raw_data[0] - senData->hwbias[0];
    senData->data[1] = senData->raw_data[1] - senData->hwbias[1];
    senData->data[2] = senData->raw_data[2] - senData->hwbias[2];
}

static void sensorAlignAxis(float *data, uint8_t position)
{
    float data_buff[3];
    
    memcpy(data_buff, data, sizeof(data_buff));
    switch(position){
        case 0:
            data[0] = data_buff[0];
            data[1] = data_buff[1];
            data[2] = data_buff[2];
        break;
        case 1:
            data[0] = data_buff[1];
            data[1] = -data_buff[0];
            data[2] = data_buff[2];
        break;
        case 2:
            data[0] = -data_buff[0];
            data[1] = -data_buff[1];
            data[2] = data_buff[2];
        break;
        case 3:
            data[0] = -data_buff[1];
            data[1] = data_buff[0];
            data[2] = data_buff[2];
        break;
        case 4:
            data[0] = data_buff[1];
            data[1] = data_buff[0];
            data[2] = -data_buff[2];
        break;
        case 5:
            data[0] = data_buff[0];
            data[1] = -data_buff[1];
            data[2] = -data_buff[2];
        break;
        case 6:
            data[0] = -data_buff[1];
            data[1] = -data_buff[0];
            data[2] = -data_buff[2];
        break;
        case 7:
            data[0] = -data_buff[0];
            data[1] = data_buff[1];
            data[2] = -data_buff[2];
        break;
        default:
        break;
    }
}

static int sensorEnable(int id, int rate)
{
    int ret=0;

    if(id >= SENSOR_ID_END){return cwFAIL;}

    if (id < ENTITY_ID_END){ //entity sensor need to set rate
        if (SensorMgr[id].rate != rate){
            ret = DriverAPISetRate(id, rate);
            if (ret){
                //LOGEI("Sensor Set Rate fail! id:%d, rate:%d ret:%d",id,rate,ret);
            }
        }
        if (SensorMgr[id].enable != SENSOR_EN){
            ret = DriverAPIEnable(id);
            if (ret){
                //LOGEI("Sensor Enable fail! id:%d, ret:%d",id,ret);
            }
        }
    }else{ // Algo ID
        if (SensorMgr[id].enable != SENSOR_EN){
            ret = algorithmEnable(id);
            if (ret){
                //LOGEI("Sensor Enable fail! id:%d, ret:%d",id,ret);
            }
        }
    }
    SensorMgr[id].enable = SENSOR_EN;
    SensorMgr[id].rate = rate;
    SensorMgr[id].timestamp = SensorTimestamp();
    
    memset(&SensorRptData[id],0,sizeof(sensor_evt_t));
    SensorRptData[id].id = id;
    
    sensorTimerControl();
    return ret;
}
    
static int sensorDisable(int id)
{
    int ret=0;

    if(id >= SENSOR_ID_END){return cwFAIL;}

    SensorMgr[id].enable = SENSOR_DIS;
    SensorMgr[id].rate = 0;
    SensorMgr[id].timestamp = SensorTimestamp();
    sensorTimerControl();
    
    if (id < ENTITY_ID_END){ //entity sensor need to set rate
        if (SensorMgr[id].enable != SENSOR_DIS){
            ret = DriverAPIDisable(id);
            if (ret){
                //LOGEI("Sensor Disable fail! id:%d, ret:%d",id,ret);
            }
        }
    }else{ // Algo ID
        if (SensorMgr[id].enable != SENSOR_DIS){
            ret = algorithmDisable(id);
            if (ret){
                //LOGEI("Sensor Disable fail! id:%d, ret:%d",id,ret);
            }
        }
    }
    
    return ret;
}

static void sensorDataProcess(int id, SENSORS_MODE mode, sensor_list_t *senMgr, sensor_evt_t *senData, uint32_t curTimestamp)
{
    uint8_t countToRead = 0;
    int ret=0;

    // check enable
    if (senMgr->enable != SENSOR_EN){
        return;
    }
    
    // check data timestamp
    if ((curTimestamp - senData->timestamp) < senMgr->rate ){
        return;
    }

    ret = DriverAPIGetStatus(id,&countToRead);
    if ( ret == SENSOR_DATA_READY){
        for (int i = 0;i < countToRead;i++){
            ret = DriverAPIGetRawData(id, mode,senData->raw_data);
            if (ret){
                //LOGEI("sensor read error, t:%d id:%d ,ret:%d, cnt:%d",curTimestamp,id,ret, countToRead);
            }else{
                #ifdef ACCELERATION
                if (id == ACCELERATION){
                    sensorAlignAxis(senData->raw_data, SensorPos[ACCELERATION]);
                    sensorsCalibratorDataInput(CALIB_TYPE_ACC, senData->raw_data);
                    sensorDataBias(CALIB_TYPE_ACC, senData);
                    sensorListenAcceleration(curTimestamp);
                }
                #endif
                #ifdef MAGNETIC
                if (id == MAGNETIC){
                    sensorAlignAxis(senData->raw_data, SensorPos[MAGNETIC]);
                    sensorListenMagnetic(curTimestamp);
                }
                #endif
                #ifdef GYRO
                if (id == GYRO){
                    uint16_t odr_hz=0;
                    sensorAlignAxis(senData->raw_data, SensorPos[GYRO]);
                    sensorsCalibratorDataInput(CALIB_TYPE_GYRO, senData->raw_data);
                    sensorDataBias(CALIB_TYPE_GYRO, senData);
                    odr_hz = DriverAPIGetODR(GYRO);
                    sensorListenGyro(curTimestamp, odr_hz);
                }
                #endif
                senData->timestamp = curTimestamp;
                sensorEvtReport(id, curTimestamp);
            }
        }
    }
    else{// sensor not ready
        //LOGEI("sensor not ready, t:%d id:%d ,ret:%d",curTimestamp,id,ret);
    }
}

extern int cwm_send_evt(psensor_evt_t algoevt);
void sensorEvtReport( int id, uint32_t curTimestamp )
{
    if(id >= SENSOR_ID_END){return;}
    
    if (SensorCmd[id].enable == SENSOR_EN){
        switch (SensorInfo[id].report_mode){
            case SENSOR_REPORT_CONTINUOUS_MODE:
                if ((curTimestamp - SensorRptData[id].timestamp) >= SensorCmd[id].rate ){
                    memcpy(&SensorRptData[id], &SensorData[id], sizeof(sensor_evt_t));
                    cwm_send_evt(&SensorRptData[id]);
                }
            break;
            case SENSOR_REPORT_FIFO_MODE:
                for (int i=0;i<3;i++){
                    SensorRptData[id].data[i] += SensorData[id].data[i];
                    SensorRptData[id].raw_data[i] += SensorData[id].raw_data[i];
                }
                if ((curTimestamp - SensorRptData[id].timestamp) >= SensorCmd[id].rate ){
                    memcpy(&SensorRptData[id].hwbias, &SensorData[id].hwbias, sizeof(float)*3);
                    SensorRptData[id].timestamp = SensorData[id].timestamp;
                    cwm_send_evt(&SensorRptData[id]);
                    memset(&SensorRptData[id].data,0,sizeof(SensorRptData[id].data));
                    memset(&SensorRptData[id].raw_data,0,sizeof(SensorRptData[id].raw_data));
                }
            break;
            case SENSOR_REPORT_GESTURE_MODE:
                memcpy(&SensorRptData[id], &SensorData[id], sizeof(sensor_evt_t));
                cwm_send_evt(&SensorRptData[id]);
            break;
            case SENSOR_REPORT_ON_CHANGE_MODE:
                if ((SensorRptData[id].data[0] != SensorData[id].data[0]) ||
                    (SensorRptData[id].data[1] != SensorData[id].data[1]) ||
                    (SensorRptData[id].data[2] != SensorData[id].data[2]))
                {
                    memcpy(&SensorRptData[id], &SensorData[id], sizeof(sensor_evt_t));
                    cwm_send_evt(&SensorRptData[id]);
                }
            break;
        }
    }
}

void sensorMain( void )
{
//    static uint32_t lastTimestamp = 0/*, time_diff = 0*/;

    SysTimestamp = SensorTimestamp();

    #ifdef ACCELERATION
    sensorDataProcess(ACCELERATION, MODE_BYPASS,&SensorMgr[ACCELERATION], &SensorData[ACCELERATION], SysTimestamp);
    #endif
    #ifdef MAGNETIC
    sensorDataProcess(MAGNETIC, MODE_BYPASS,&SensorMgr[MAGNETIC], &SensorData[MAGNETIC], SysTimestamp);
    #endif
    #ifdef GYRO
    sensorDataProcess(GYRO, MODE_FIFO_STREAM,&SensorMgr[GYRO], &SensorData[GYRO], SysTimestamp);
    #endif

    //time_diff = SysTimestamp - lastTimestamp;

//    lastTimestamp = SysTimestamp;
}

static void sensorCommandEnableSync(void)
{
    int i=0,j=0;
    EnList_t EnList[SENSOR_ID_END]={0};
    uint8_t subID=0;
    uint16_t subRate=0;

    // check lastest enable list
    for (i=0;i<SENSOR_ID_END;i++){
        if (SensorCmd[i].enable == SENSOR_EN){
            if (EnList[i].enable == SENSOR_DIS){
                EnList[i].enable = SENSOR_EN;
                EnList[i].rate = SensorCmd[i].rate;
            }else if (EnList[i].enable == SENSOR_EN){
                EnList[i].rate = min(EnList[i].rate,SensorCmd[i].rate);
            }else{
                //LOGEI("Enable list check NG!");
            }
            for (j=0;j<MAX_SENSOR_SUB_EN;j++){
                if (SensorInfo[i].enSetting[j].SensorRate){
                    subID = SensorInfo[i].enSetting[j].SensorID;
                    subRate = SensorInfo[i].enSetting[j].SensorRate;
                    if (EnList[subID].enable == SENSOR_DIS){
                        EnList[subID].enable = SENSOR_EN;
                        EnList[subID].rate = subRate;
                    }else if (EnList[subID].enable == SENSOR_EN){
                        EnList[subID].rate = min(EnList[subID].rate,subRate);
                    }else{
                        //LOGEI("Enable list check NG!");
                    }
                }
            }
        }
    }

    // reset enable state
    for (i=0;i<SENSOR_ID_END;i++){
        if (EnList[i].enable == SENSOR_EN){
            if (SensorMgr[i].enable == SENSOR_EN){
                if (EnList[i].rate != SensorMgr[i].rate){
                    sensorEnable(i, EnList[i].rate);
                }
            }else{
                sensorEnable(i, EnList[i].rate);
            }
        }else if (EnList[i].enable == SENSOR_DIS){
            if (SensorMgr[i].enable == SENSOR_EN){
                sensorDisable(i);
            }
        }else{
            //LOGEI("Enable list check NG!");
        }
    }
}

int sensorSetPosition(uint16_t id, uint16_t position)
{
    if (id >= ENTITY_ID_END) return cwFAIL;
    if (position >= MAX_SENSOR_POS) return cwFAIL;
    
    SensorPos[id] = position;

    return NO_ERROR;
}

int sensorCommandEnable(uint16_t id, uint16_t rate)
{
    if (id >= SENSOR_ID_END) return cwFAIL;

    if (!rate)                  {rate = NORMAL;}
    else if (rate > RATE_1HZ)   {rate = RATE_1HZ;}
    else if (rate < FASTEST)    {rate = FASTEST;}
    
    SensorCmd[id].timestamp = SensorTimestamp();
    SensorCmd[id].enable = SENSOR_EN;
    SensorCmd[id].rate = rate;
    sensorCommandEnableSync();
    return NO_ERROR;
}

int sensorCommandDisable(uint16_t id)
{
    if (id >= SENSOR_ID_END) return cwFAIL;

    SensorCmd[id].timestamp = SensorTimestamp();
    SensorCmd[id].enable = SENSOR_DIS;
    SensorCmd[id].rate = 0;
    sensorCommandEnableSync();
    return NO_ERROR;
}

