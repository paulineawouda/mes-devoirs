#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <pthread.h>
#include "../include/common.h"

char sensor_prefix[MAX_ID_LEN];

void* send_temp(void* arg) {
    int idx = *(int*)arg;
    free(arg);

    srand(time(NULL) ^ (idx << 16));
    int delay = rand() % 7;
    sleep(delay);

    float temp = ((float)(rand() % 4000) / 100.0f) - 5.0f; // tra -5 e 35

    char sensor_id[MAX_ID_LEN];
    snprintf(sensor_id, MAX_ID_LEN, "%s_%d", sensor_prefix, idx);

    TemperatureData data;
    strcpy(data.sensor_id, sensor_id);
    data.temperature = temp;

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in hub_addr;
    hub_addr.sin_family = AF_INET;
    hub_addr.sin_port = htons(HUB_PORT);
    hub_addr.sin_addr.s_addr = INADDR_ANY;

    connect(sockfd, (struct sockaddr*)&hub_addr, sizeof(hub_addr));
    send(sockfd, &data, sizeof(data), 0);

    // Attesa risposta con numero attuatori (dummy nel nostro caso)
    int count = 0;
    recv(sockfd, &count, sizeof(int), 0);
    printf("[Sensore %s] Temperatura inviata: %.2f°C -> %d attuatori raggiunti\n", sensor_id, temp, count);

    close(sockfd);
    return NULL;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <Nt> <sensor_prefix>\n", argv[0]);
        exit(1);
    }

    int Nt = atoi(argv[1]);
    strcpy(sensor_prefix, argv[2]);

    pthread_t threads[Nt];
    for (int i = 0; i < Nt; ++i) {
        int* id = malloc(sizeof(int));
        *id = i;
        pthread_create(&threads[i], NULL, send_temp, id);
    }

    for (int i = 0; i < Nt; ++i) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}
