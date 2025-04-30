#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <pthread.h>
#include "../include/common.h"

#define HISTORY_SIZE 100
#define M 5

typedef struct {
    char sensor_id[MAX_ID_LEN];
    float history[HISTORY_SIZE];
    int count;
} SensorHistory;

SensorHistory histories[MAX_SENSORS];
int sensor_count = 0;
float Tgoal;
char attuatore_id[MAX_ID_LEN];
int sockfd;

void update_history(const char* id, float temp) {
    for (int i = 0; i < sensor_count; ++i) {
        if (strcmp(histories[i].sensor_id, id) == 0) {
            if (histories[i].count < HISTORY_SIZE) {
                histories[i].history[histories[i].count++] = temp;
            } else {
                memmove(&histories[i].history[0], &histories[i].history[1], (HISTORY_SIZE - 1) * sizeof(float));
                histories[i].history[HISTORY_SIZE - 1] = temp;
            }
            return;
        }
    }
}

float media(const float* values, int count) {
    if (count < 3) return -1000; // valore invalido
    float sum = 0;
    for (int i = 0; i < count && i < M; ++i) sum += values[i];
    return sum / ((count < M) ? count : M);
}

void receive_loop() {
    TemperatureData data;
    while (1) {
        int r = recv(sockfd, &data, sizeof(data), 0);
        if (r <= 0) break;
        if (strlen(data.sensor_id) == 0) break;

        update_history(data.sensor_id, data.temperature);

        int below = 0, total = 0;
        for (int i = 0; i < sensor_count; ++i) {
            float m = media(histories[i].history, histories[i].count);
            if (m > -1000) {
                total++;
                if (m < Tgoal) below++;
            }
        }

        if (total >= 1) {
            if (below > total / 2)
                printf("[Attuatore %s] ACCENSIONE riscaldamento\n", attuatore_id);
            else
                printf("[Attuatore %s] SPEGNIMENTO riscaldamento\n", attuatore_id);
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc < 4) {
        fprintf(stderr, "Usage: %s <id> <Tgoal> <sensore1> [sensore2 ...] | --unsubscribe <id>\n", argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "--unsubscribe") == 0) {
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        struct sockaddr_in serv_addr;
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(SERVER_PORT);
        serv_addr.sin_addr.s_addr = INADDR_ANY;

        connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
        char msg[BUFFER_SIZE];
        snprintf(msg, sizeof(msg), "UNSUB %s", argv[2]);
        send(sockfd, msg, strlen(msg) + 1, 0);
        close(sockfd);
        printf("Disiscrizione attuatore %s completata.\n", argv[2]);
        return 0;
    }

    strcpy(attuatore_id, argv[1]);
    Tgoal = atof(argv[2]);
    sensor_count = argc - 3;
    for (int i = 0; i < sensor_count; ++i) {
        strcpy(histories[i].sensor_id, argv[i + 3]);
        histories[i].count = 0;
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERVER_PORT);
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

    char buffer[BUFFER_SIZE];
    int len = snprintf(buffer, sizeof(buffer), "%s %.2f %d\n", attuatore_id, Tgoal, sensor_count);
    for (int i = 0; i < sensor_count; ++i) {
        len += snprintf(buffer + len, sizeof(buffer) - len, "%s\n", histories[i].sensor_id);
    }
    send(sockfd, buffer, strlen(buffer) + 1, 0);

    receive_loop();
    close(sockfd);
    return 0;
}
