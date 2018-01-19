/**
  ******************************************************************************
  * @file    CwmCalibrator.c
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

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "cwmCalibrator.h"
#include "cwmApp.h"
#include "customIntf.h"

/* typedef -----------------------------------------------------------*/
/* define ------------------------------------------------------------*/
#define CALIB_GROUP_NAME    "calib"
#define CALIB_TYPE_STR_ACC  "type_acc"
#define CALIB_TYPE_STR_GYRO "type_gyro"

#define UNIT_NEWTON             9.80665f

#define CalibratorMaxSample     100
#define CalibratorGyroFilter    30

#define CalibratorAccRangeX         1.96f
#define CalibratorAccRangeY         1.96f
#define CalibratorAccMaxRangeZ      (UNIT_NEWTON+2.49f)//12.29f
#define CalibratorAccMinRangeZ      (UNIT_NEWTON-2.49f)//7.31f
#define CalibratorAccRangeCalibX    0.39f
#define CalibratorAccRangeCalibY    0.39f
#define CalibratorAccMaxRangeCalibZ (UNIT_NEWTON+0.4f)//10.2f
#define CalibratorAccMinRangeCalibZ (UNIT_NEWTON-0.4f)//9.4f

#define CalibratorGyroRange         0.4f
/* macro -------------------------------------------------------------*/
/* variables ---------------------------------------------------------*/
float AccCalData[3]={0.0f,0.0f,0.0f};
float GyroCalData[3]={0.0f,0.0f,0.0f};
SensorsCalibratorDataEvent_T calibrator[CALIB_TYPE_END];
/* functions ---------------------------------------------------------*/
void sensorsCalibratorInitial(CALIB_TYPE_T CalibType)
{
    if (CalibType <= CALIB_TYPE_END){
    	if (CalibType == CALIB_TYPE_END){
            memset(calibrator,0,sizeof(calibrator));
    	}else{
            memset(&calibrator[CalibType],0,sizeof(SensorsCalibratorDataEvent_T));
    	}
    }
}

void sensorsCalibratorEnable(CALIB_TYPE_T CalibType)
{
    if (CalibType < CALIB_TYPE_END){
        memset(&calibrator[CalibType],0,sizeof(SensorsCalibratorDataEvent_T));
        calibrator[CalibType].status = CALIB_STATUS_INPROCESS;
    }
}

static int CalibratorCheckUserFactor(CALIB_TYPE_T CalibType, float *data)
{
    switch(CalibType){
        case    CALIB_TYPE_ACC:
            if( fabs(data[0])>CalibratorAccRangeX || 
                fabs(data[1])>CalibratorAccRangeY ||
                fabs(data[2])>CalibratorAccMaxRangeZ || 
                fabs(data[2])<CalibratorAccMinRangeZ )
            {
                return CALIB_STATUS_DATA_OUT_OF_RANGE;
            }
            break;
        case    CALIB_TYPE_GYRO:
            if( fabs(data[0])>CalibratorGyroRange ||
                fabs(data[1])>CalibratorGyroRange ||
                fabs(data[2])>CalibratorGyroRange )
            {
                return CALIB_STATUS_DATA_OUT_OF_RANGE;
            }
            break;
        default:
            return CALIB_STATUS_FAIL;
    }
    return CALIB_STATUS_NON;
}

static int CalibratorCheckCriterion(CALIB_TYPE_T CalibType, float *data)
{
    switch(CalibType){
        case    CALIB_TYPE_ACC:
            if( fabs(data[0]) > CalibratorAccRangeCalibX || 
                fabs(data[1]) > CalibratorAccRangeCalibY ||
                fabs(data[2]) > CalibratorAccMaxRangeCalibZ ||
                fabs(data[2]) < CalibratorAccMinRangeCalibZ)
            {
                return CALIB_STATUS_OUT_OF_RANGE;
            }
            if (data[2] > 0){
                data[2] -= UNIT_NEWTON;
            }else{
                data[2] += UNIT_NEWTON;
            }
            break;
        case    CALIB_TYPE_GYRO:
            if( fabs(data[0])>CalibratorGyroRange ||
                fabs(data[1])>CalibratorGyroRange ||
                fabs(data[2])>CalibratorGyroRange)
            {
                return CALIB_STATUS_OUT_OF_RANGE;
            }
            break;
        default:
            return CALIB_STATUS_FAIL;
    }
    return CALIB_STATUS_PASS;
}

static void SensorsCalibratorAcceleration(float *data)
{
    int res = 0;

    res = CalibratorCheckUserFactor(CALIB_TYPE_ACC,data);
    if(res){
        sensorsCalibratorInitial(CALIB_TYPE_ACC);
        calibrator[CALIB_TYPE_ACC].status = res;
        cwm_send_log("Index:[%d][%d]:%2.4f %2.4f %2.4f\n",calibrator[CALIB_TYPE_ACC].count,CALIB_STATUS_FAIL,data[0],data[1],data[2]);
        return;
    }

    if(calibrator[CALIB_TYPE_ACC].count < CalibratorMaxSample){
        calibrator[CALIB_TYPE_ACC].data[0] += data[0];
        calibrator[CALIB_TYPE_ACC].data[1] += data[1];
        calibrator[CALIB_TYPE_ACC].data[2] += data[2];
        calibrator[CALIB_TYPE_ACC].count++;
    }else{
        calibrator[CALIB_TYPE_ACC].data[0] /= calibrator[CALIB_TYPE_ACC].count;
        calibrator[CALIB_TYPE_ACC].data[1] /= calibrator[CALIB_TYPE_ACC].count;
        calibrator[CALIB_TYPE_ACC].data[2] /= calibrator[CALIB_TYPE_ACC].count;
        res = CalibratorCheckCriterion(CALIB_TYPE_ACC, calibrator[CALIB_TYPE_ACC].data);
        if(res == CALIB_STATUS_PASS){
            memcpy(AccCalData, calibrator[CALIB_TYPE_ACC].data, sizeof(float)*3);
            saveCalibratorData(CALIB_TYPE_ACC);
        }
        calibrator[CALIB_TYPE_ACC].status = res;
    }
}

static void SensorsCalibratorGyro(float *data)
{
    int res = 0;

    res = CalibratorCheckUserFactor(CALIB_TYPE_GYRO,data);
    if(res){
        sensorsCalibratorInitial(CALIB_TYPE_GYRO);
        calibrator[CALIB_TYPE_GYRO].status = res;
        cwm_send_log( "Index:[%d][%d]:%2.4f %2.4f %2.4f\n",calibrator[CALIB_TYPE_GYRO].count,CALIB_STATUS_FAIL,data[0],data[1],data[2]);
        return;
    }

    if(calibrator[CALIB_TYPE_GYRO].count < (CalibratorMaxSample+CalibratorGyroFilter)){
        if(calibrator[CALIB_TYPE_GYRO].count >=CalibratorGyroFilter){
            calibrator[CALIB_TYPE_GYRO].data[0] += data[0];
            calibrator[CALIB_TYPE_GYRO].data[1] += data[1];
            calibrator[CALIB_TYPE_GYRO].data[2] += data[2];
        }
        calibrator[CALIB_TYPE_GYRO].count++;
    }else{
        calibrator[CALIB_TYPE_GYRO].data[0] /= (calibrator[CALIB_TYPE_GYRO].count-CalibratorGyroFilter);
        calibrator[CALIB_TYPE_GYRO].data[1] /= (calibrator[CALIB_TYPE_GYRO].count-CalibratorGyroFilter);
        calibrator[CALIB_TYPE_GYRO].data[2] /= (calibrator[CALIB_TYPE_GYRO].count-CalibratorGyroFilter);
        res = CalibratorCheckCriterion(CALIB_TYPE_GYRO,calibrator[CALIB_TYPE_GYRO].data);
        if(res == CALIB_STATUS_PASS){
            memcpy(GyroCalData, calibrator[CALIB_TYPE_GYRO].data, sizeof(float)*3);
            saveCalibratorData(CALIB_TYPE_GYRO);
        }
        calibrator[CALIB_TYPE_GYRO].status = res;
    }
}

void sensorsCalibratorDataInput(CALIB_TYPE_T CalibType, float *data)
{
    if(calibrator[CalibType].status == CALIB_STATUS_INPROCESS){
        switch(CalibType){
            case    CALIB_TYPE_ACC:
                SensorsCalibratorAcceleration(data);
                break;
            case    CALIB_TYPE_GYRO:
                SensorsCalibratorGyro(data);
                break;
        }
    }
}

int saveCalibratorData(CALIB_TYPE_T CalibType)
{
    // TODO: to be implemented!    
    return 0;
}

int loadCalibratorData(void)
{
    // TODO: to be implemented!
    return 0;
}

void getCalibratorBias(CALIB_TYPE_T CalibType, float *Bias)
{
    memset(Bias, 0, sizeof(float)*3);

    switch(CalibType){
        case    CALIB_TYPE_ACC:
            if (calibrator[CALIB_TYPE_ACC].status == CALIB_STATUS_PASS){
                memcpy(Bias, AccCalData, sizeof(float)*3);
            }
            break;    
        case    CALIB_TYPE_GYRO:
            if (calibrator[CALIB_TYPE_GYRO].status == CALIB_STATUS_PASS){
                memcpy(Bias, GyroCalData, sizeof(float)*3);
            }
            break;
        default:
            break;
    }

}

