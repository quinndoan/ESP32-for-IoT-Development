#include "nvsImplement.h"
#include <string.h>
#include <stdio.h>
#include "esp_log.h"
#include "nvs_flash.h"
#include "wifi_app.h"

// tag monitor the nvs 
static const char TAG[] = "nvs";

// nvs name space for mode credidential
const char app_nvs_sta_creds_namespace[] = "stacreds";

esp_err_t app_nvs_save_sta_creds(void){
    nvs_handle handle;
    esp_err_t esp_err;
    ESP_LOGI(TAG, "app_nvs_save_credential: saving station mode credentials to flash");
    wifi_config_t *wifi_sta_config = wifi_app_get_wifi_config();

    if (wifi_sta_config){
        esp_err = nvs_open(app_nvs_sta_creds_namespace, NVS_READWRITE, &handle);
        if (esp_err != ESP_OK){
            printf("error when open nvs");
            return esp_err;
        }
        // set up ssid and pass

        // set ssif
        esp_err = nvs_set_blob(handle, "ssid", wifi_sta_config->sta.ssid, MAX_SSID_LENGTH);
        if (esp_err != ESP_OK){
            printf("app_nvs_save_sta_creds: Error (%s) setting SSID to NVS!\n", esp_err_to_name(esp_err));
            return esp_err;
        }
        // set password
        esp_err = nvs_set_blob(handle, "password", wifi_sta_config->sta.password, MAX_PASSWORD_LENGTH);
        if (esp_err != ESP_OK){
            printf("app_nvs_save_sta_creds: Error (%s) setting password", esp_err_to_name(esp_err));
            return esp_err;
        }

        // commit credential to NVS
        esp_err = nvs_commit(handle);
        if (esp_err != ESP_OK){
            printf("app_nvs_save_sta_creds: Error (%s) committing to flash", esp_err_to_name(esp_err));
            return esp_err;

        }
        nvs_close(handle);
        ESP_LOGI (TAG, "app_nvs_save_sta_creds: wrote wifi_sta_config: ssid (%s), password (%s)", wifi_sta_config->sta.ssid, wifi_sta_config->sta.password);

    }
    printf("app_nvs_save_sta_creds: return ESP_OK\n");
    return ESP_OK;
}

/**
 * Loads the previously saved credentials from NVS.
 * @return true if previously saved credentials were found.
 */
bool app_nvs_load_sta_creds(void){
    nvs_handle handle;
    esp_err_t esp_err;
    ESP_LOGI (TAG, "app_nvs_load_sta_creds: Loading wifi credential");
    if (nvs_open(app_nvs_sta_creds_namespace, NVS_READONLY, &handle)== ESP_OK){
        wifi_config_t *wifi_sta_config = wifi_app_get_wifi_config();
        memset(wifi_sta_config, 0x00, sizeof(wifi_config_t));

        // alocate buffer
        size_t wifi_config_size = sizeof(wifi_config_t);
        uint8_t *wifi_config_buff = (uint8_t *)malloc(sizeof(uint8_t)*wifi_config_size);
        memset(wifi_config_buff, 0x00, sizeof(wifi_config_size));

        // load ssid
        wifi_config_size = sizeof(wifi_sta_config->sta.ssid);
        esp_err = nvs_get_blob(handle, "ssid", wifi_config_buff, &wifi_config_size);
        if (esp_err != ESP_OK){
            free(wifi_config_buff);
            printf("app_nvs: no ssid station found");
            return false;
        }
        memcpy(wifi_sta_config->sta.ssid, wifi_config_buff, wifi_config_size);

        // load password
        wifi_config_size = sizeof(wifi_sta_config->sta.password);
		esp_err = nvs_get_blob(handle, "password", wifi_config_buff, &wifi_config_size);
		if (esp_err != ESP_OK)
		{
			free(wifi_config_buff);
			printf("app_nvs_load_sta_creds: (%s) retrieving password!\n", esp_err_to_name(esp_err));
			return false;
		}
		memcpy(wifi_sta_config->sta.password, wifi_config_buff, wifi_config_size);

        free(wifi_config_buff);
		nvs_close(handle);

		printf("app_nvs_load_sta_creds: SSID: %s Password: %s\n", wifi_sta_config->sta.ssid, wifi_sta_config->sta.password);
		return wifi_sta_config->sta.ssid[0] != '\0';
    }
    else
	{
		return false;
	}
}

esp_err_t app_nvs_clear_sta_creds(void)
{
	nvs_handle handle;
	esp_err_t esp_err;
	ESP_LOGI(TAG, "app_nvs_clear_sta_creds: Clearing Wifi station mode credentials from flash");

	esp_err = nvs_open(app_nvs_sta_creds_namespace, NVS_READWRITE, &handle);
	if (esp_err != ESP_OK)
	{
		printf("app_nvs_clear_sta_creds: Error (%s) opening NVS handle!\n", esp_err_to_name(esp_err));
		return esp_err;
	}

	// Erase credentials
	esp_err = nvs_erase_all(handle);
	if (esp_err != ESP_OK)
	{
		printf("app_nvs_clear_sta_creds: Error (%s) erasing station mode credentials!\n", esp_err_to_name(esp_err));
		return esp_err;
	}

	// Commit clearing credentials from NVS
	esp_err = nvs_commit(handle);
	if (esp_err != ESP_OK)
	{
		printf("app_nvs_clear_sta_creds: Error (%s) NVS commit!\n", esp_err_to_name(esp_err));
		return esp_err;
	}
	nvs_close(handle);

	printf("app_nvs_clear_sta_creds: returned ESP_OK\n");
	return ESP_OK;
}
