#ifndef __CWM_ALGO_CALLBACK_H
#define __CWM_ALGO_CALLBACK_H

typedef enum {
    SensorEnReq = 0,
    SensorOffReq,
    AlgoInToLowPowerState,
    AlgoInToNormalPowerState,
}uAlgoReq;

/*
typedef enum {
    ACC = 0,
    MAG,
    GYR,
    HEART_RATE
}uSensorId;
*/
typedef enum {
    CWM_MOTION = 0,
    CWM_NON_MOTION,
}uSystemStatus;

typedef int (*cwmAlgoRequestCb)(int algoReq, int sensorId, void *reserved);
typedef int (*cwmAlgoLogCb)(const char *format ,...);

typedef struct {
    cwmAlgoLogCb algoLogCb; /*LOG MSG info callback function */
    cwmAlgoRequestCb algoRequestCb; /* ALGO Request callback function */
} CWM_ALGO_CB_Handle_t, *pCWM_ALGO_CB_Handle_t;

void cwm_algo_set_status(uSystemStatus status);
int cwm_algo_cb_register(pCWM_ALGO_CB_Handle_t cwm_algo_cb);
int cwm_algo_main_process(float *acc, float *out, float dt_us);

#endif /* __CWM_ALGO_CALLBACK_H */

