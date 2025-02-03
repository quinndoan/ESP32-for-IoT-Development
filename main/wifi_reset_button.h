#ifndef MAIN_WIFI_RESET_BUTTON_H
#define MAIN_WIFI_RESET_BUTTON_H

// default interupt flag
#define ESP_INTR_FLAG_DEFAULT      0
#define WIFI_RESET_BUTTON          0    // vi BOOT gan voi GPIO0

void wifi_reset_button_config(void);

#endif