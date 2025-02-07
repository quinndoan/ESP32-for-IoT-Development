#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>
// file nay khong dung de build project, chi dung de hoc
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_vfs_fat.h"
#include "driver/sdmmc_host.h"

#include "aws_iot_core.h"
#include "DHT22.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "task_common.h"
#include "wifi_app.h"

// add later after add the folder cloned from github, clone version cũ
#include "aws_iot_config.h"
#include "aws_iot_log.h"
#include "aws_iot_version.h"
#include "aws_iot_mqtt_client_interface.h"
static const char *TAG = "aws_iot";

// task handle AWS IOT
static TaskHandle_t task_aws_iot = NULL;

// embedded the CA root cer, Thing cer, device private key
extern const uint8_t aws_root_ca_pem_start[] asm("_binary_aws_root_ca_pem_start");
extern const uint8_t certificate_pem_crt_start[] asm("_binary_certificate_pem_crt_start");
extern const uint8_t private_pem_key_start[] asm("_binary_private_pem_key_start");

// default MQTT host url 
char HostAddress[255] = AWS_IOT_MQTT_HOST;
// default port
uint32_t port = AWS_IOT_MQTT_PORT;

void iot_subcribe_callback_handler();   // can bo sung them 2 function nay
void disconnectCallbackHandler(AWS_IoT_Client *pClient, void *data);

void aws_iot_task(void *param){
    char cPayLoad[100];
    int32_t i =0;
    
    IoT_Error_t rc = FAILURE;

    AWS_IoT_Client client;
    IoT_Client_Init_Params mqttInitParams = iotClientInitParamsDefault;
    IoT_Client_Connect_Params connectParams = iotClientConnectParamsDefault;

    IoT_Publish_Message_Params paramsQ0S0;
    IoT_Publish_Message_Params paramsQ0S1;

    ESP_LOGI(TAG, "AWS IoT SDK Version %d.%d.%d-%s", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH, VERSION_TAG);
    mqttInitParams.enableAutoReconnect = false;
    mqttInitParams.pHostURL = HostAddress;
    mqttInitParams.port = port;

    mqttInitParams.pRootCALocation = (const char *)aws_root_ca_pem_start;
    mqttInitParams.pDeviceCertLocation = (const char *)certificate_pem_crt_start;
    mqttInitParams.pDevicePrivateKeyLocation = (const char *)private_pem_key_start;

    mqttInitParams.mqttCommandTimeout_ms = 20000;
    mqttInitParams.tlsHandshakeTimeout_ms = 5000;
    mqttInitParams.isSSLHostnameVerify = true;
    mqttInitParams.disconnectHandler = disconnectCallbackHandler;
    mqttInitParams.disconnectHandlerData = NULL;

    rc = aws_iot_mqtt_init(&client, &mqttInitParams);
    if(rc!= SUCCESS){
        ESP_LOGE(TAG, "aws_iot_mqtt_init returned error: %d", rc);
        abort();
    }
    connectParams.keepAliveIntervalInSec = 10;
    connectParams.isCleanSession = true;
    connectParams.MQTTVersion = MQTT_3_1_1;
    connectParams.pClientID = CONFIG_AWS_EXAMPLE_CLIENT_ID;
    connectParams.clientIDLen = (uint16_t) strlen(CONFIG_AWS_EXAMPLE_CLIENT_ID);
    connectParams.isWillMsgPresent = false;

     ESP_LOGI(TAG, "Connecting to AWS...");
    do {
        rc = aws_iot_mqtt_connect(&client, &connectParams);
        if(SUCCESS != rc) {
            ESP_LOGE(TAG, "Error(%d) connecting to %s:%d", rc, mqttInitParams.pHostURL, mqttInitParams.port);
            vTaskDelay(1000 / portTICK_RATE_MS);
        }
    } while(SUCCESS != rc);

    // auto reconnect
    rc = aws_iot_mqtt_autoreconnect_set_status(&client, true);
    if (rc != SUCCESS){
        ESP_LOGE(TAG, "Unable to set the auto reconnect to true- %d", rc);
        abort();
    }
    const char *TOPIC = "test_iot_esp32";
    const int TOPIC_LEN = strlen(TOPIC);

    ESP_LOGI(TAG, "Subcribing...");
    rc = aws_iot_mqtt_subcrible(&client, TOPIC, TOPIC_LEN, Q0S0, iot_subcrible_callback_handler, NULL);
    if (rc != SUCCESS){
        ESP_LOGE(TAG, "Error subcribing: %d", rc);
        abort();
    }
    sprintf(cPayLoad, "%s: %d ", "hello from SDK", i);

    paramsQOS0.qos = QOS0;
    paramsQOS0.payload = (void *) cPayload;
    paramsQOS0.isRetained = 0;

    paramsQOS1.qos = QOS1;
    paramsQOS1.payload = (void *) cPayload;
    paramsQOS1.isRetained = 0;

    while((NETWORK_ATTEMPTING_RECONNECT == rc || NETWORK_RECONNECTED == rc || SUCCESS == rc)) {

        //Max time the yield function will wait for read messages
        rc = aws_iot_mqtt_yield(&client, 100);
        if(NETWORK_ATTEMPTING_RECONNECT == rc) {
            // If the client is attempting to reconnect we will skip the rest of the loop.
            continue;
        }

        ESP_LOGI(TAG, "Stack remaining for task '%s' is %d bytes", pcTaskGetTaskName(NULL), uxTaskGetStackHighWaterMark(NULL));
        vTaskDelay(3000 / portTICK_RATE_MS);
        sprintf(cPayload, "%s : %d ", "WiFi RSSI", wifi_app_get_rssi());
        paramsQOS0.payloadLen = strlen(cPayload);
        rc = aws_iot_mqtt_publish(&client, TOPIC, TOPIC_LEN, &paramsQOS0);

        sprintf(cPayload, "%s : %.1f, %s : %.1f", "Temperature", getTemperature(), "Humidity", getHumidity());
        paramsQOS1.payloadLen = strlen(cPayload);
        rc = aws_iot_mqtt_publish(&client, TOPIC, TOPIC_LEN, &paramsQOS1);
        if (rc == MQTT_REQUEST_TIMEOUT_ERROR) {
            ESP_LOGW(TAG, "QOS1 publish ack not received.");
            rc = SUCCESS;
        }
         ESP_LOGE(TAG, "An error occurred in the main loop.");
    abort();
    }
}

void aws_iot_start(void){
    if (task_aws_iot == NULL){
        xTaskCreatePinnedToCore(&aws_iot_task, "aws_iot_task",AWS_IOT_TASK_STACK_SIZE, NULL, AWS_IOT_TASK_PRIORITY, &task_aws_iot, AWS_IOT_TASK_CORE_ID);
    }
}