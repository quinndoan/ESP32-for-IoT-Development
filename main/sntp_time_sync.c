#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "lwip/apps/sntp.h"
#include "task_common.h"
#include "http_server.h"
#include "sntp_time_sync.h"
#include "wifi_app.h"

static bool sntp_op_mode_set = false;

static const char TAG[] = "sntp_time_sync";

// initialize sntp service using SNTP_OPMODE_POOL mode
static void sntp_time_sync_init_sntp(void){
    ESP_LOGI(TAG, "Initializing the SNTP service");
    if (!sntp_op_mode_set){
        // set the operating mode
        sntp_setoperatingmode(SNTP_OPMODE_POLL);
        sntp_op_mode_set = true;
    }
    sntp_setservername(0, "pool.ntp.org");
    // initialize the servers
    sntp_init();
    // let the http_server know service is initiliazed
    http_server_monitor_send_message(HTTP_MSG_TIME_SERVICE_HTTP_INITIALIZED);
}

static void sntp_time_sync_obtain_time(void){
    time_t now =0;
    struct tm time_info = {0};
    time(&now);
    localtime_r(&now, &time_info);
    // chek the time, see if need to initialize
    if (time_info.tm_year < (2016- 1900)){
        sntp_time_sync_init_sntp();
        // set time
        setenv("TZ", "WIB-7", 1);   // need to check again, timezone in VN: UTC+7
        tzset();
    }
}

// the sntp time sync task
static void sntp_time_sync(void *pvParam){
    while(1){
        sntp_time_sync_obtain_time();
        vTaskDelay(1000);
    }
    vTaskDelete(NULL);
}

void sntp_time_sync_task_start(void){
    xTaskCreatePinnedToCore(&sntp_time_sync, "sntp_time_sync", SNTP_TIME_SYNC_TASK_STACK_SIZE, NULL, SNTP_TIME_SYNC_TASK_PRIORITY, NULL, STNP_TIME_SYNC_CORE_ID);
}

// return local time if set in buffer type
char* sntp_time_sync_get_time(void){
    static char time_buffer[100] = {0};
    time_t now = 0;
    struct tm time_info = {0};

    time(&now);
    localtime_r(&now, &time_info);

    if (time_info.tm_year < (2016-1900)){
        ESP_LOGI(TAG, "Time is not set yet");
    }
    else{
        stftime(time_buffer, sizeof(time_buffer), "%d.%m.%Y %H:%M:%S", &time_info);
        ESP_LOGI(TAG, "Current time: %s", time_buffer);

    }
    return time_buffer;
}