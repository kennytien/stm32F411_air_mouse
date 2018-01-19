/*
 *  @brief   customized interface
 *  @date    2017-11-10
 *  @author  CyweeMotion AE team
 *  @history v1.0: new created  2017-11-10
 *                
*/

#ifndef __CUSTOMINTF_H__
#define __CUSTOMINTF_H__

#include <stdint.h>

typedef enum {
    ERR_INVALID = -1,
    ERR_NON = 0,
} cwm_status_e;

typedef enum {
    RT_FASTEST = 10,
    RT_GAME = 20,
    RT_RATE_33 = 30,
    RT_RATE_25 = 40,
    RT_UI = 60,
    RT_RATE_10 = 100,
    RT_NORMAL = 200
}cwm_rate_e;

typedef enum {
    LVL_L = 0,
    LVL_M = 1,
    LVL_H = 2,
} cwm_algo_set_e;


typedef struct {
    uint8_t id;
    uint8_t reserved[3];
    union {
        struct {
            float raw_data[3]; /* physical sensor uncalibrated data */
            float data[3]; /* physical sensor calibrated data */
        };
        struct {
            float algo_data[6]; /* virtual sensor ouput data */
        };
    };    
    uint32_t timestamp;
} cwm_sensor_evt,*pcwm_sensor_evt;

typedef void (*cwm_evtCb_t)(cwm_sensor_evt evt, void *reserved);
typedef void (*cwm_logCb_t)(const char *format ,...); 
typedef int (*cwm_spiCb_t)(uint8_t *, uint8_t , uint16_t); 

cwm_status_e cwm_senevt_cb_register(cwm_evtCb_t cwmCb); /* register it to get the cwm sensor event */
cwm_status_e cwm_logout_cb_register(cwm_logCb_t cwmCb); /* register it to get the cwm log */
cwm_status_e cwm_spi_cb_register(cwm_spiCb_t spiWrite, cwm_spiCb_t spiRead);

cwm_status_e cwm_task_init(void);
cwm_status_e cwm_sensor_pos_set(uint16_t id, uint16_t position);/* accel:0, gyro:1 ,position: 0 ~ 7 */
cwm_status_e cwm_sensor_enable(int id,cwm_rate_e rate);
cwm_status_e cwm_sensor_main(void);
cwm_status_e cwm_sensor_disable(int id);
cwm_status_e cwm_sensor_algo_set(int id, cwm_algo_set_e level);
#endif
