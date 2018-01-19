/*
 *  @brief   customized interface
 *  @date    2017-11-10
 *  @author  CyweeMotion AE team
 *  @history v1.0: new created  2017-11-10
 *                
 *
*/
#include <stdio.h>
#include <string.h>
#include "cwmApp.h"
#include "customIntf.h"

int cwm_send_evt(psensor_evt_t algoevt);
int cwm_send_log(const char *msg,...);
static void evt_adapt(pcwm_sensor_evt evt, psensor_evt_t algoevt);

static cwm_evtCb_t cwm_sensor_evt_ptr = NULL;
static cwm_logCb_t cwm_logout_ptr = NULL;
cwm_spiCb_t cb_SPIWrite = NULL;
cwm_spiCb_t cb_SPIRead = NULL;

//////////////////////////////////////////////////////////////////////////////
/////
/////                API function open to customer 
/////
//////////////////////////////////////////////////////////////////////////////

/* 
** register this callback to get the sensor evt data
*/
cwm_status_e cwm_senevt_cb_register(cwm_evtCb_t cwmCb)
{
    if(cwmCb != NULL){
        cwm_sensor_evt_ptr = cwmCb;
    }
    else {
        /* callback register fail */
        return ERR_INVALID;
    }

    return ERR_NON;
}

/*
** register this callback to get the cwm log data 
*/
cwm_status_e cwm_logout_cb_register(cwm_logCb_t cwmCb)
{
    if(cwmCb != NULL){
        cwm_logout_ptr = cwmCb;
    }
    else {
        /* callback register fail */
        return ERR_INVALID;
    }

    return ERR_NON;
}

cwm_status_e cwm_spi_cb_register(cwm_spiCb_t spiWrite, cwm_spiCb_t spiRead)
{
    if(spiWrite != NULL){
        cb_SPIWrite = spiWrite;
    }
    else {
        /* callback register fail */
        return ERR_INVALID;
    }
    
    if(spiRead != NULL){
        cb_SPIRead = spiRead;
    }
    else {
        /* callback register fail */
        return ERR_INVALID;
    }
    return ERR_NON;
}

/*
** enable cwm sensor 
*/
cwm_status_e cwm_sensor_enable(int id,cwm_rate_e rate) {
    int ret = ERR_NON;

    sensor_cmd_app_set(SENSOR_CMD_ENABLE, id, (int)rate);

    if(ret == 0)
        return ERR_NON;
    else
        return ERR_INVALID;
}

/*
** disable cwm sensor 
*/
cwm_status_e cwm_sensor_disable(int id) {
    int ret = ERR_NON;

    sensor_cmd_app_set(SENSOR_CMD_DISABLE, id, NULL);

    if(ret == 0)
        return ERR_NON;
    else
        return ERR_INVALID;
}

cwm_status_e cwm_sensor_algo_set(int id, cwm_algo_set_e level) {
    int ret = ERR_INVALID;

    switch(id) {
        case CWM_AIR_MOUSE:
            // TODO: to be implemented!
            break;
        default:
            break;
    }

    if(ret == 0)
        return ERR_NON;
    else
        return ERR_INVALID;
}

cwm_status_e cwm_sensor_pos_set(uint16_t id, uint16_t position) {
    int ret = ERR_INVALID;
    
    ret = sensorSetPosition(id, position);

    if(ret == 0)
        return ERR_NON;
    else
        return ERR_INVALID;
}


cwm_status_e cwm_task_init(void) {
    sensor_cmd_app_create();
    sensor_app_create();
    return ERR_NON;
}


cwm_status_e cwm_sensor_init(void) {
    sensorInit();
    return ERR_NON;
}

cwm_status_e cwm_sensor_main(void) {
    sensorMain();
    return ERR_NON;
}

#if 0 /* callback functon sample code to customer */
static void cwm_senevt_cb(cwm_sensor_evt senEvt,void *reserved)
{
    static cwm_sensor_evt mySenEvt;
    
    memset(&mySenEvt, 0, sizeof(cwm_sensor_evt));
    memcpy(&mySenEvt,&senEvt, sizeof(cwm_sensor_evt));

    switch(mySenEvt.id){
        case ACCELERATION:
            break;
        case GYRO:
            break;
        case ORIENTATION:
            break;
        case CYW_AIR_MOUSE:
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

#endif

//////////////////////////////////////////////////////////////////////////////
/////
/////                internal proprietary function
/////
//////////////////////////////////////////////////////////////////////////////

int cwm_send_evt(psensor_evt_t algoevt) {
    cwm_sensor_evt evt;

    /* adapt the sensor evt to the format for customer */
    evt_adapt(&evt, algoevt);
    
    if(cwm_sensor_evt_ptr != NULL) {
        cwm_sensor_evt_ptr(evt, NULL);
    }
    else {
        return ERR_INVALID;
    }
  
  return ERR_NON;
}

int cwm_send_log(const char *msg,...) {
    
    if(cwm_logout_ptr != NULL) {
        cwm_logout_ptr(msg, NULL);
    }
    else {
        return ERR_INVALID;
    }
  
  return ERR_NON;
}

static void evt_adapt(pcwm_sensor_evt evt,psensor_evt_t algoevt) {
    evt->id = algoevt->id;
    evt->timestamp = algoevt->timestamp;
    
    if(evt->id < ENTITY_ID_END) {
        memcpy(evt->raw_data, algoevt->raw_data, sizeof(float)*3);
        memcpy(evt->data, algoevt->data, sizeof(float)*3);
    }else {
        memcpy(evt->algo_data, algoevt->algo_data, sizeof(float)*6);
    }
}
