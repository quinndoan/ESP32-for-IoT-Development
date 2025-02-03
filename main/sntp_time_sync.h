#ifndef MAIN_SNTP_TIME_SYNC
#define MAIN_SNTP_TIME_SYNC

// start the http server sync task
void sntp_time_sync_task_start(void);

// return local time if set in buffer type
char* sntp_time_sync_get_time(void);

#endif