#include "FreeRTOS.h"
#include "timers.h"
#include "queue.h"
#include "task_def.h"
#include "cwmTaskSen.h"
#include "cwmApp.h"

#define SENSOR_QUEUE_SIZE 100

static struct {
    QueueHandle_t sensor_task_queue;
} sensor_task_cntx;

static TimerHandle_t g_sensor_timer;
static TaskHandle_t sensor_task;
static void sensor_timer_callback( TimerHandle_t timer_handle );

static uint16_t sensor_timer_period = SENSOR_IDLE_PERIOD;

void sensor_timer_init(void)
{
    g_sensor_timer = xTimerCreate("sensor reading Timer",
                    SENSOR_IDLE_PERIOD / portTICK_PERIOD_MS,
                    pdTRUE,
                    (void *) SENSOR_READ_TIMER_NO,
                    sensor_timer_callback
                    );
}
void sensor_timer_start(void)
{
    xTimerStart(g_sensor_timer, portMAX_DELAY);
}

void sensor_timer_stop(void)
{
    xTimerStop(g_sensor_timer, portMAX_DELAY);
}

void sensor_timer_change(uint16_t peroid)
{
    if (!peroid){
        return;
    }
    
    if (peroid != sensor_timer_period){
        sensor_timer_period = peroid;
        //LOGEI("Sensor Timer period %d",peroid);
        xTimerStop(g_sensor_timer, portMAX_DELAY);
        xTimerChangePeriod(g_sensor_timer, sensor_timer_period / portTICK_PERIOD_MS, portMAX_DELAY);
        xTimerStart(g_sensor_timer, portMAX_DELAY);
    }
}

void sensor_app_start(void)
{
    //LOGEI("sensor_app_start\n");
    sensor_message_struct_t sensor_message;
    sensor_message.message_id = SENSOR_DEV_POWER_ON_REQ;
    xQueueSend(sensor_task_cntx.sensor_task_queue, &sensor_message, 0);
}

void sensor_app_config(int param1)
{
    //LOGEI("sensor_app_config\n");
    sensor_message_struct_t sensor_message;
    sensor_message.message_id = SENSOR_DEV_CONFIG_REQ;
    sensor_message.param1 = param1;
    xQueueSend(sensor_task_cntx.sensor_task_queue, &sensor_message, 0);
    xTaskResumeFromISR(sensor_task);
}

static void sensor_task_msg_handler(sensor_message_struct_t *message)
{
    switch (message->message_id) {
        case SENSOR_DEV_POWER_ON_REQ:
            sensorInit();
            break;
        case SENSOR_DEV_CONFIG_REQ:
            break;
        case SENSOR_DEV_READY_TO_READ:
            {
            #ifdef SENSOR_PROFILE
            uint32_t elapse,tick_start,tick_end;
            tick_start = (uint32_t)xTaskGetTickCount();
            #endif
            sensorMain();
            #ifdef SENSOR_PROFILE
            tick_end = (uint32_t)xTaskGetTickCount();
            elapse = tick_end - tick_start;
            #endif
            }
            break;
        case SENSOR_DEV_READY_TO_WRITE:
            break;
        default:
            break;
    }
}

static void sensor_task_init(void)
{
    sensor_timer_init();
}

static void sensor_task_main(void)
{
    sensor_message_struct_t queue_item;
    //LOGEI("sensor_task_main\n");

    sensor_app_start();
    while (1) {
        while (xQueueReceive(sensor_task_cntx.sensor_task_queue, &queue_item, portMAX_DELAY)) {
            sensor_task_msg_handler(&queue_item);
        }
        vTaskSuspend(sensor_task);
    }
}

TaskHandle_t sensor_app_create(void){
    BaseType_t ret;

    sensor_task_init();

    sensor_task_cntx.sensor_task_queue = xQueueCreate( SENSOR_QUEUE_SIZE, sizeof( sensor_message_struct_t ) );

    ret = xTaskCreate((TaskFunction_t) sensor_task_main, 
        SENSOR_TASK_NAME, 
        SENSOR_TASK_STACK_SIZE/(( uint32_t )sizeof( StackType_t )), 
        NULL, 
        SENSOR_TASK_PRIO, 
        &sensor_task );

    if(ret != pdPASS){
        //LOGEI("can not create sensor task!\n");
    }
    return sensor_task;
}

static void sensor_timer_callback( TimerHandle_t timer_handle )
{
    uint32_t id;

    id = ( uint32_t ) pvTimerGetTimerID( timer_handle );

    if (id == SENSOR_READ_TIMER_NO) {
        sensor_message_struct_t sensor_message;
        sensor_message.message_id = SENSOR_DEV_READY_TO_READ;
        xQueueSendFromISR(sensor_task_cntx.sensor_task_queue, &sensor_message, 0);
        xTaskResumeFromISR(sensor_task);
    }
}

