#ifndef __CWM_TASK_SEN_H__
#define __CWM_TASK_SEN_H__

TaskHandle_t sensor_app_create(void);
void sensor_timer_start(void);
void sensor_timer_change(uint16_t peroid);

#endif

