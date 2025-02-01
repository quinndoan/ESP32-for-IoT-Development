#include <esp_err.h>
#include<stdbool.h>
#ifndef NVS_IMPLEMENT_H
#define NVS_IMPLEMENT_H

esp_err_t app_nvs_save_sta_creds(void);
bool app_nvs_load_sta_creds(void);  // check xem co ssid va password luu truoc chua
esp_err_t app_nvs_clear_sta_creds(void);

#endif
