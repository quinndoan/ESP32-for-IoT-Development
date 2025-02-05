#ifndef MAIN_TASKS_COMMON_H
#define MAIN_TASKS_COMMON_H

#define CONFIG_LOG_MAXIMUM_LEVEL            1
#define WIFI_APP_TASK_STACK_SIZE            4096  // Kích thước stack cho WiFi Application Task (4KB). Dùng để lưu trữ dữ liệu cục bộ và ngữ cảnh của tác vụ.
#define WIFI_APP_TASK_PRIORITY              5     // Độ ưu tiên của tác vụ WiFi. Ưu tiên cao hơn HTTP Server Task và Monitor Task.
#define WIFI_APP_TASK_CORE_ID               0     // Tác vụ này được chạy trên Core 0 của ESP32.

// HTTP Server Task
#define HTTP_SERVER_TASK_STACK_SIZE         8192  // Kích thước stack cho HTTP Server Task (8KB). HTTP Server cần nhiều bộ nhớ hơn để xử lý các yêu cầu từ client.
#define HTTP_SERVER_TASK_PRIORITY           4     // Độ ưu tiên của HTTP Server Task. Thấp hơn WiFi Task để đảm bảo WiFi luôn hoạt động ổn định.
#define HTTP_SERVER_TASK_CORE_ID            0     // Tác vụ này cũng được chạy trên Core 0.

// HTTP Server Monitor Task
#define HTTP_SERVER_MONITOR_STACK_SIZE      4096  // Kích thước stack cho HTTP Server Monitor Task (4KB). Đủ để giám sát trạng thái HTTP Server.
#define HTTP_SERVER_MONITOR_PRIORITY        3     // Độ ưu tiên thấp nhất trong các tác vụ vì nó không cần xử lý tức thì.
#define HTTP_SERVER_MONITOR_CORE_ID         0     // Tác vụ giám sát cũng được đặt chạy trên Core 0.

#define WIFI_RESET_BUTTON_TASK_STACK_SIZE   2048
#define WIFI_RESET_BUTTON_PRIORITY          6
#define WIFI_RESET_BUTTON_CORE_ID           0

// SNTP task
#define SNTP_TIME_SYNC_TASK_STACK_SIZE      4096
#define SNTP_TIME_SYNC_TASK_PRIORITY        4
#define STNP_TIME_SYNC_CORE_ID              0

// AWS IoT task
#define AWS_IOT_TASK_STACK_SIZE             9216
#define AWS_IOT_TASK_PRIORITY               6
#define AWS_IOT_TASK_CORE_ID                1

#endif