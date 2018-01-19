/**
  ******************************************************************************
  * @file    CwmAlgorithmHal.c
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
#include <string.h>
#include <stdlib.h>  
#include "cwmAlgorithmHal.h"
#include "cwmApp.h"

#ifdef CWM_AIR_MOUSE
#include "cwm_air_mouse.h"
#endif
#ifdef ALGO_USE_FUSION
#include "cwm_fusion_ExMem.h"
#endif

#define MAX_TIME_TIME (int)(3600*200)

void *g_hAirMouseMem = NULL;
void *rot_vec_mem = NULL;
void *game_rot_vec_mem = NULL;
void *geo_rot_vec_mem = NULL;

static void _assert();

void algorithmInit( void )
{
#ifdef ALGO_USE_FUSION
    rot_vec_mem = pvPortMalloc(memsize_cwm_rot_vec());
    if(rot_vec_mem == NULL)
        _assert();
    memset(rot_vec_mem, 0x00, memsize_cwm_rot_vec());
    init_cwm_fusion(rot_vec_mem);
    char cFusionCfgParmData[5] = {2, 15, 3, 3, 1};
    set_config_cwm_fusion(rot_vec_mem, (void*)cFusionCfgParmData, sizeof(cFusionCfgParmData));
    enable_cwm_fusion(rot_vec_mem);
#endif
#ifdef ALGO_USE_GAME_ROTATION_VECTOR
    game_rot_vec_mem = pvPortMalloc(memsize_cwm_game_rot_vec());

    if(game_rot_vec_mem == NULL)
        _assert();

    memset(game_rot_vec_mem, 0x00, memsize_cwm_game_rot_vec());
    init_cwm_game_rot_vec(game_rot_vec_mem);
    char cGameRotCfgParmData[5] = {2, 15, 3, 3, 1};
    set_config_cwm_game_rot_vec(game_rot_vec_mem, (void*)cGameRotCfgParmData, sizeof(cGameRotCfgParmData));
    enable_cwm_game_rot_vec(game_rot_vec_mem);
#endif
// TODO: one unknown memory issue had happened here, need to look out
#ifdef CWM_AIR_MOUSE
    g_hAirMouseMem = pvPortMalloc(memsize_cwm_air_mouse());
    if(g_hAirMouseMem == NULL)
        _assert();
    memset(g_hAirMouseMem, 0x00, memsize_cwm_air_mouse());
    int nAirMouseCfgParmData[3] = {1920, 1080, 60};
    init_cwm_air_mouse(g_hAirMouseMem);
    set_config_cwm_air_mouse(g_hAirMouseMem,(void*)nAirMouseCfgParmData,sizeof(nAirMouseCfgParmData));
#endif

}

int algorithmEnable( int id )
{
    int ret = 0;

    switch (id)
    {
    #ifdef CWM_AIR_MOUSE
        case CWM_AIR_MOUSE:
            enable_cwm_air_mouse(g_hAirMouseMem);/* no break here!!don't insert code here!! */
    #endif
    #ifdef ALGO_USE_FUSION
        case ORIENTATION:
        case ROTATION_VECTOR:
            #ifdef ALGO_USE_6AXIS_FUSION
            enable_cwm_game_rot_vec(geo_rot_vec_mem);
            #else
            enable_cwm_fusion(rot_vec_mem);
            #endif
            break;
    #endif

        default:
            ret = -1;
            break;
    }
	return ret;
}

int algorithmDisable( int id )
{
    int ret = 0;

    switch (id)
    {
    #ifdef CWM_AIR_MOUSE
        case CWM_AIR_MOUSE:
            disable_cwm_air_mouse(g_hAirMouseMem);/* no break here!!don't insert code here!! */
    #endif
    #ifdef ALGO_USE_FUSION
        case ORIENTATION:
        case ROTATION_VECTOR:
            #ifdef ALGO_USE_6AXIS_FUSION
            disable_cwm_game_rot_vec(geo_rot_vec_mem);
            #else
            disable_cwm_fusion(rot_vec_mem);
            #endif
            break;
    #endif
        default:
            ret = -1;
            break;
    }
	return ret;
}


#ifdef ACCELERATION
void sensorListenAcceleration(uint32_t curTimestamp)
{
    //int ret = -1;
    //float time_diff=0;

    //time_diff = (float)(curTimestamp - SensorData[ACCELERATION].timestamp)*1000;
}
#endif

#ifdef MAGNETIC
void sensorListenMagnetic(uint32_t curTimestamp)
{
    //int ret = -1;
    //float time_diff=0;

    //time_diff = (float)(curTimestamp - SensorData[MAGNETIC].timestamp)*1000;
}
#endif

#ifdef GYRO
int algo_ret = -1;
void sensorListenGyro(uint32_t curTimestamp, uint16_t odr_hz)
{
    int ret = -1;
    #ifndef ALGO_USE_6AXIS_FUSION
    static float tmp_mag[4] = {0};
    #endif
    static float time_diff = 0;
    float rot_mat[9] = {0};
    float rotationVector[5] = {0};
    static float gamerotationVector[5];
    float orientation[3] = {0};
    static int timeoutCount = MAX_TIME_TIME;
    
    if(timeoutCount-- < 0)
        return;

    if (!odr_hz){
        //LOGEI("ODR zero");
        return;
    }
    time_diff = ((1.0f/(float)odr_hz) * 1000000.0f);

    #ifdef ALGO_USE_FUSION
    if (
         #ifdef ROTATION_VECTOR
         SensorMgr[ROTATION_VECTOR].enable == SENSOR_EN || 
         #endif
         #ifdef ORIENTATION
         SensorMgr[ORIENTATION].enable == SENSOR_EN ||
         #endif
         #ifdef CWM_AIR_MOUSE
         SensorMgr[CWM_AIR_MOUSE].enable == SENSOR_EN 
         #endif
         )
       {
        
        #ifdef ALGO_USE_6AXIS_FUSION
        ret = cwm_game_rot_vec( 
            game_rot_vec_mem,
            SensorData[ACCELERATION].data, 
            SensorData[GYRO].data, 
            gamerotationVector, 
            time_diff);
        #else
        ret = cwm_fusion(rot_vec_mem,
            SensorData[ACCELERATION].data,
            SensorData[GYRO].data,
            tmp_mag,
            rotationVector,
            time_diff
            );
        #endif
        
        if(ret > 0) {
            #ifdef ALGO_USE_6AXIS_FUSION
            cwm_get_rot_mat(gamerotationVector, rot_mat);
            #else
            cwm_get_rot_mat(rotationVector, rot_mat);
            #endif
            cwm_get_orien(rot_mat, orientation);
            memcpy(SensorData[ROTATION_VECTOR].algo_data, rotationVector, sizeof(rotationVector));
            SensorData[ROTATION_VECTOR].timestamp = curTimestamp;
            sensorEvtReport(ROTATION_VECTOR, curTimestamp);
            
            memcpy(SensorData[ORIENTATION].algo_data, orientation, sizeof(orientation));
            SensorData[ORIENTATION].timestamp = curTimestamp;
            sensorEvtReport(ORIENTATION, curTimestamp);
        }
    }
    #endif

    #ifdef CWM_AIR_MOUSE
    if (SensorMgr[CWM_AIR_MOUSE].enable == SENSOR_EN){
        float airMouseOut[3] = {0,};
        static float algoIn[5] = {0,};

        #ifdef ALGO_USE_6AXIS_FUSION
        memcpy(algoIn, gamerotationVector,sizeof(float)*5);
        #else
        memcpy(algoIn, rotationVector,sizeof(float)*5);
        #endif

        /* air mouse main process here */
        algo_ret = ret = cwm_air_mouse(g_hAirMouseMem, algoIn, airMouseOut, time_diff);

        if(ret > 0){
            SensorData[CWM_AIR_MOUSE].algo_data[0] = (float)airMouseOut[0];
            SensorData[CWM_AIR_MOUSE].algo_data[1] = (float)airMouseOut[1];
            SensorData[CWM_AIR_MOUSE].timestamp = curTimestamp;
            sensorEvtReport(CWM_AIR_MOUSE, curTimestamp);
        }
    }
    #endif
}

static void _assert() {
    while(1){
    }
}

#endif
