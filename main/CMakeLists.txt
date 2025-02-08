# idf_component_register(SRCS "aws_iot_core.c" "main.c" "mqtt_demo_mutual_auth.c" "DHT22.c" "sntp_time_sync.c" "wifi_reset_button.c" "nvsImplement.c" "wifi_app.c" "http_server.c"
#                     INCLUDE_DIRS ""
#                     EMBED_FILES webpage/app.css webpage/app.js webpage/favicon.ico webpage index.html webpage jquery-3.3.1.min.js)

#                     target_add_binary_data(${COMPONENT_TARGET} "certs/aws_root_ca_pem" TEXT)
#                     target_add_binary_data(${COMPONENT_TARGET} "certs/certificate_pem_crt" TEXT)
#                     target_add_binary_data(${COMPONENT_TARGET} "certs/private_pem_key" TEXT)

idf_component_register(SRCS "main.c" "mqtt_demo_mutual_auth.c" "DHT22.c" "sntp_time_sync.c" "wifi_reset_button.c" "nvsImplement.c" "wifi_app.c" "http_server.c"
						INCLUDE_DIRS "."
						EMBED_FILES webpage/app.css webpage/app.js webpage/favicon.ico webpage/index.html webpage/jquery-3.3.1.min.js)

target_add_binary_data(${COMPONENT_TARGET} "certs/aws_root_ca_pem" TEXT)
target_add_binary_data(${COMPONENT_TARGET} "certs/certificate_pem_crt" TEXT)
target_add_binary_data(${COMPONENT_TARGET} "certs/private_pem_key" TEXT)

set(PARTITION_TABLE_CUSTOM "partitions.csv")
set(PARTITION_TABLE_CUSTOM_FILENAME "${CMAKE_SOURCE_DIR}/partitions.csv")
set(PARTITION_TABLE_CUSTOM_DEFINED ON)
