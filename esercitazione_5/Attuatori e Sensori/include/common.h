// common.h

#ifndef COMMON_H
#define COMMON_H

#define SERVER_PORT 5000
#define HUB_PORT 5001

#define MAX_CONN 10
#define BUFFER_SIZE 1024
#define MAX_SENSORS 10
#define MAX_ACTUATORS 10
#define MAX_ID_LEN 64
#define MAX_TEMP_HISTORY 100

typedef struct {
    char sensor_id[MAX_ID_LEN];
    float temperature;
} TemperatureData;

#endif
