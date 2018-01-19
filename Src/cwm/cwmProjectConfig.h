/**
  ******************************************************************************
  * @file    cwmProjectConfig.h
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

#ifndef __CwmProjectConfig_H__
#define __CwmProjectConfig_H__

#define SPI_MASTER_SUPPORT
#define DRIVER_USE_BMI160_SPI

// 7-bit slave addr
#define BMI160_SLV_ADDRESS  0x68
#define NON_SLV_ADDRESS     0x00

#define ACC_SLAVE_ADDR	    BMI160_SLV_ADDRESS
#define MAG_SLAVE_ADDR      NON_SLV_ADDRESS
#define GYRO_SLAVE_ADDR	    BMI160_SLV_ADDRESS
#define MOTION_SLAVE_ADDR   BMI160_SLV_ADDRESS

// HW_ID
typedef enum {
    BMI160,
    HW_ID_NON,
    HW_ID_END
} HW_ID;

#define ACC_HW_ID           BMI160
#define MAG_HW_ID           HW_ID_NON
#define GYRO_HW_ID          BMI160
#define MOTION_HW_ID        BMI160

// Sensor position
#define ACC_POSITION    0
#define MAG_POSITION    0
#define GYR_POSITION    0

// Sensor support
#define CWM_SUPPORT                 (1)
#define CWM_NOT_SUPPORT             (0)

#define SP_ACCELEROMETER            (CWM_SUPPORT)
#define SP_MAGNETIC_FIELD_SENSOR    (CWM_NOT_SUPPORT)
#define SP_GYROSCOPE                (CWM_SUPPORT)
#define SP_MOTION                   (CWM_NOT_SUPPORT)
#define SP_ORIENTATION              (CWM_SUPPORT)
#define SP_ROTATION_VECTOR          (CWM_SUPPORT)
#define SP_CWM_AIR_MOUSE                  (CWM_SUPPORT)

// Sensor support list
typedef enum {
#if SP_ACCELEROMETER
    emACCELEROMETER = 0,
#endif
#if SP_MAGNETIC_FIELD_SENSOR
    emMAGNETIC_FIELD_SENSOR,
#endif
#if SP_GYROSCOPE
    emGYROSCOPE,
#endif
#if SP_MOTION
    emMOTION,
#endif
    ENTITY_ID_END
} ENTITY_ID;

typedef enum {
    ALGO_ID_START = (ENTITY_ID_END-1),
#if SP_ORIENTATION
    emORIENTATION,
#endif
#if SP_ROTATION_VECTOR
    emROTATION_VECTOR,
#endif
#if SP_CWM_AIR_MOUSE
    emCWM_AIR_MOUSE,
#endif
    SENSOR_ID_END
} SENSOR_ID;

#if SP_ACCELEROMETER
#define ACCELERATION                        (emACCELEROMETER)
#endif
#if SP_MAGNETIC_FIELD_SENSOR
#define MAGNETIC                            (emMAGNETIC_FIELD_SENSOR)
#endif
#if SP_GYROSCOPE
#define GYRO                                (emGYROSCOPE)
#endif
#if SP_MOTION
#define MOTION                              (emMOTION)
#endif
#if SP_ORIENTATION
#define ORIENTATION                         (emORIENTATION)
#endif
#if SP_ROTATION_VECTOR
#define ROTATION_VECTOR                     (emROTATION_VECTOR)
#endif
#if SP_CWM_AIR_MOUSE
#define CWM_AIR_MOUSE                             (emCWM_AIR_MOUSE)
#endif

#endif
