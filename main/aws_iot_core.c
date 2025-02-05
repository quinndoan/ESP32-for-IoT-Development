#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>

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

// add later after add the folder cloned from github
// #include "aws_iot_config.h"
// #include "aws_iot_log.h"
// #include "aws_iot_version.h"
// #include "aws_iot_mqtt_client_interface.h"
static const char *TAG = "aws_iot";

// task handle AWS IOT
static TaskHandle_t task_aws_iot = NULL;

// embedded the CA root cer, Thing cer, device private key
extern const uint8_t aws_root_ca_pem_start[] asm("_binary_aws_root_ca_pem_start");
extern const uint8_t certificate_pem_crt_start[] asm("_binary_certificate_pem_crt_start");
extern const uint8_t private_pem_key_start[] asm("_binary_private_pem_key_start");

// // default MQTT host url 
// char HostAddress[255] = AWS_IOT_MQTT_HOST;
// // default port
// uint32_t port = AWS_IOT_MQTT_PORT;

void aws_iot_task(void *param){
    char cPayLoad[100];
    int32_t i =0;
    
}