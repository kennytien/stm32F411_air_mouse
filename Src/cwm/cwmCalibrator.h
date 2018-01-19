/**
  ******************************************************************************
  * @file    CwmCalibrator.h
  * @author  Cywee Motion - Leon Lin
  * @version V1.0
  * @date    25-May-2017
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

#ifndef __cwmcalibrator_H__
#define __cwmcalibrator_H__

#include <stdint.h>

typedef enum {
    CALIB_TYPE_ACC = 0,
    CALIB_TYPE_GYRO,
    CALIB_TYPE_END,
} CALIB_TYPE_T;

typedef enum {
    CALIB_STATUS_DATA_OUT_OF_RANGE= -3,
    CALIB_STATUS_OUT_OF_RANGE= -2,
    CALIB_STATUS_FAIL= -1,
    CALIB_STATUS_NON = 0,
    CALIB_STATUS_INPROCESS = 1,
    CALIB_STATUS_PASS = 2,
} CALIBRATOR_STATUS;

typedef struct {
    int8_t  status;
    uint8_t count;
    float   data[3];
}SensorsCalibratorDataEvent_T;

void sensorsCalibratorInitial(CALIB_TYPE_T CalibType);
void sensorsCalibratorEnable(CALIB_TYPE_T CalibType);
void sensorsCalibratorDataInput(CALIB_TYPE_T CalibType, float *data);
int saveCalibratorData(CALIB_TYPE_T CalibType);
int loadCalibratorData(void);
void getCalibratorBias(CALIB_TYPE_T CalibType, float *Bias);
#endif
