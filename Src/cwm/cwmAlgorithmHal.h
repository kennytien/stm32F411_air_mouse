/**
  ******************************************************************************
  * @file    CwmAlgorithmHal.h
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


#ifndef __CwmAlgorithmHal_H__
#define __CwmAlgorithmHal_H__

#include <stdint.h>
#include "CwmProjectConfig.h"

#if defined ORIENTATION || defined ROTATION_VECTOR
#define ALGO_USE_FUSION
#define ALGO_USE_6AXIS_FUSION
#define ALGO_USE_GAME_ROTATION_VECTOR
#endif

void algorithmInit( void );
int algorithmEnable( int id );
int algorithmDisable( int id );
void sensorListenAcceleration(uint32_t curTimestamp);
void sensorListenMagnetic(uint32_t curTimestamp);
void sensorListenGyro(uint32_t curTimestamp, uint16_t odr_hz);

#endif
