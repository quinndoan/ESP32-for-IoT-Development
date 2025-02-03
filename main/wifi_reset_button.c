#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "driver/gpio.h"
#include "esp_log.h"

#include "task_common.h"
#include "wifi_app.h"
#include "wifi_reset_button.h"
#include <portmacro.h>

static const char TAG[] = "wifi_reset_button";
// semaphore handle
SemaphoreHandle_t wifi_reset_semaphore = NULL;

// ISR handler for wifi reset interrupt boot
void IRAM_ATTR wifi_reset_button_isr_handler(void *arg){
    // ham nay trigger semaphore de task bat dau lam viec trong vong for
    xSemaphoreGiveFromISR(wifi_reset_semaphore, NULL);
}

void wifi_reset_button_task(void *pvParam){
    for (;;){
        if (xSemaphoreTake(wifi_reset_semaphore, portMAX_DELAY)== pdTRUE){
            // doi semaphore

            ESP_LOGI(TAG, "WIFI RESET BUTTON INTERRUPT OCCURRED");
            wifi_app_send_message(WIFI_APP_MSG_USER_REQUESTED_STA_DISCONNECT);
            vTaskDelay(2000);// chinh lai xem cho dung CL Rate
        }
    }
}

void wifi_reset_button_config(void){
    // create binary semaphore
    wifi_reset_semaphore = xSemaphoreCreateBinary();

    // configure GPIO and set as input
    esp_rom_gpio_pad_select_gpio(WIFI_RESET_BUTTON);
    gpio_set_direction(WIFI_RESET_BUTTON, GPIO_MODE_INPUT);

    // enable interrupt on falling edge
    gpio_set_intr_type(WIFI_RESET_BUTTON, GPIO_INTR_NEGEDGE);

    // create wifi reset button task
    xTaskCreatePinnedToCore(&wifi_reset_button_task, "wifi_reset_button",WIFI_RESET_BUTTON_TASK_STACK_SIZE, NULL, WIFI_RESET_BUTTON_PRIORITY, WIFI_RESET_BUTTON_CORE_ID);

    // install gpio ISR service
    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);

    // attach the interrupt service routine
    gpio_isr_handler_add(WIFI_RESET_BUTTON, wifi_reset_button_isr_handler, NULL);

}