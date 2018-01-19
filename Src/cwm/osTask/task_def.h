#ifndef __TASK_DEF_H__
#define __TASK_DEF_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "FreeRTOSConfig.h"

typedef enum {
    TASK_PRIORITY_IDLE = 0,                                 /* lowest, special for idle task */
    TASK_PRIORITY_SYSLOG,                                   /* special for syslog task */

    /* User task priority begin, please define your task priority at this interval */
    TASK_PRIORITY_LOW,                                      /* low */
    TASK_PRIORITY_BELOW_NORMAL,                             /* below normal */
    TASK_PRIORITY_NORMAL,                                   /* normal */
    TASK_PRIORITY_ABOVE_NORMAL,                             /* above normal */
    TASK_PRIORITY_HIGH,                                     /* high */
    TASK_PRIORITY_SOFT_REALTIME,                            /* soft real time */
    TASK_PRIORITY_HARD_REALTIME,                            /* hard real time */
    /* User task priority end */

    /*Be careful, the max-priority number can not be bigger than configMAX_PRIORITIES - 1, or kernel will crash!!! */
    TASK_PRIORITY_TIMER = configMAX_PRIORITIES - 1,         /* highest, special for timer task to keep time accuracy */
} task_priority_type_t;

/* sensor task definition */
#define SENSOR_TASK_NAME "sensor_t"
#define SENSOR_TASK_STACK_SIZE 2048
#define SENSOR_TASK_PRIO TASK_PRIORITY_SOFT_REALTIME

/* sensor command task definition */
#define SENSOR_CMD_TASK_NAME "sensor_cmd_t"
#define SENSOR_CMD_TASK_STACK_SIZE 1024
#define SENSOR_CMD_TASK_PRIO TASK_PRIORITY_NORMAL

#ifdef __cplusplus
}
#endif

#endif /* __TASK_DEF_H__ */

