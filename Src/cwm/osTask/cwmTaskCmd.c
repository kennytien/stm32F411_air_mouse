#include "FreeRTOS.h"
#include "timers.h"
#include "queue.h"

#include "task_def.h"
#include "cwmTaskCmd.h"
#include "cwmApp.h"
#include "cwmCalibrator.h"
#include "customIntf.h"

#define SENSOR_CMD_QUEUE_SIZE 10

struct {
    QueueHandle_t sensor_cmd_task_queue;
} sensor_cmd_task_cntx;

static TaskHandle_t sensor_cmd_task;

void sensor_cmd_app_start(void)
{
    //LOGEI("sensor_cmd_app_start\n");
    sensor_cmd_message_struct_t sensor_cmd_message;
    sensor_cmd_message.message_id = SENSOR_CMD_DEV_POWER_ON_REQ;
    xQueueSend(sensor_cmd_task_cntx.sensor_cmd_task_queue, &sensor_cmd_message, 0);
}

void sensor_cmd_app_set_fromISR(int param1, int param2, int param3)
{
    BaseType_t xHigherPriorityTaskWoken;

    //LOGEI("sensor_cmd_app_config\n");
    sensor_cmd_message_struct_t sensor_cmd_message;
    sensor_cmd_message.message_id = SENSOR_CMD_DEV_SET_REQ;
    sensor_cmd_message.param1 = param1;
    sensor_cmd_message.param2 = param2;
    sensor_cmd_message.param3 = param3;
    xQueueSendFromISR(sensor_cmd_task_cntx.sensor_cmd_task_queue, &sensor_cmd_message, &xHigherPriorityTaskWoken);
    xTaskResumeFromISR(sensor_cmd_task);
}
void sensor_cmd_app_set(int param1, int param2, int param3)
{
    //LOGEI("sensor_cmd_app_config\n");
    sensor_cmd_message_struct_t sensor_cmd_message;
    sensor_cmd_message.message_id = SENSOR_CMD_DEV_SET_REQ;
    sensor_cmd_message.param1 = param1;
    sensor_cmd_message.param2 = param2;
    sensor_cmd_message.param3 = param3;
    xQueueSend(sensor_cmd_task_cntx.sensor_cmd_task_queue, &sensor_cmd_message, portMAX_DELAY);
    vTaskResume(sensor_cmd_task);
}

static void sensor_cmd_task_msg_handler(sensor_cmd_message_struct_t *message)
{
   int sensor_cmd = message->param1;
   uint16_t sensor_id = (uint16_t)message->param2;
   uint16_t sensor_param = (uint16_t)message->param3;
   
    switch (message->message_id) {
        case SENSOR_CMD_DEV_POWER_ON_REQ:
            break;
        case SENSOR_CMD_DEV_SET_REQ:
            switch (sensor_cmd){
                case SENSOR_CMD_ENABLE:
                    sensorCommandEnable(sensor_id, sensor_param);
                    //LOGEI("sensor cmd enable id:%d rate:%d",sensor_id,sensor_param);
                    break;
                case SENSOR_CMD_DISABLE:
                    sensorCommandDisable(sensor_id);
                    //LOGEI("sensor cmd disable id:%d",sensor_id);
                    break;
                case SENSOR_CMD_POSITION:
                    sensorSetPosition(sensor_id, sensor_param);
                    //LOGEI("sensor cmd position id:%d position:%d",sensor_id,sensor_param);
                    break;
                case SENSOR_CMD_CALIBRATOR:
                    #ifdef ACCELERATION
                    if (sensor_id == ACCELERATION){
                        sensorsCalibratorEnable(CALIB_TYPE_ACC);
                    }
                    #endif
                    #ifdef GYRO
                    if(sensor_id == GYRO){
                        sensorsCalibratorEnable(CALIB_TYPE_GYRO);
                    }
                    #endif
                    //LOGEI("sensor cmd calib id:%d",sensor_id);
                    break;
                default:
                    //LOGEI("Unknown sensor cmd param:%d",message->param1);
                    break;
            }
            break;
        default:
            //LOGEI("Unknown sensor cmd id:%d",message->message_id);
            break;
    }
}

static void sensor_cmd_task_main(void)
{
    sensor_cmd_message_struct_t queue_item;
    //LOGEI("sensor_cmd_task_main\n");

    sensor_cmd_app_start();
    while (1) {
        while (xQueueReceive(sensor_cmd_task_cntx.sensor_cmd_task_queue, &queue_item, portMAX_DELAY)) {
            sensor_cmd_task_msg_handler(&queue_item);
        }
        vTaskSuspend(sensor_cmd_task);
    }
}

TaskHandle_t sensor_cmd_app_create(void)
{
    BaseType_t ret;

    sensor_cmd_task_cntx.sensor_cmd_task_queue = xQueueCreate( SENSOR_CMD_QUEUE_SIZE, sizeof( sensor_cmd_message_struct_t ) );

    ret = xTaskCreate((TaskFunction_t) sensor_cmd_task_main, 
        SENSOR_CMD_TASK_NAME, 
        SENSOR_CMD_TASK_STACK_SIZE/(( uint32_t )sizeof( StackType_t )), 
        NULL, 
        SENSOR_CMD_TASK_PRIO, 
        &sensor_cmd_task );

    if(ret != pdPASS){
        //LOGEI("can not create sensor task!\n");
    }

    return sensor_cmd_task;
}

