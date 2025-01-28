#include <portmacro.h>
#ifndef MAIN_HTTP_SEVER_H_
#define MAIN_HTTP_SERVER_H_

#define CONFIG_LOG_MAXIMUM_LEVEL 1
// messages for HTTP monitor
typedef enum http_server_message
{
    HTTP_MSG_WIFI_CONNECT_INIT =0,
    HTTP_MSG_WIFI_CONNECT_SUCCESS,
    HTTP_MSG_WIFI_CONNECT_FAIL,
    HTTP_MSG_OTA_UPDATE_SUCCESSFUL,
    HTTP_MSG_OTA_UPDATE_FAILED,
    HTTP_MSG_OTA_UPDATE_INITIALIZED,
} http_server_message_e;

// structure for message queue
typedef struct http_server_queue_message
{
    http_server_message_e msgID;
} http_server_queue_message_t;

BaseType_t http_server_monitor_send_message(http_server_message_e msgID);

void http_server_start(void);
void http_server_stop(void);

// timer callback calls esp_restart upon successful


#endif