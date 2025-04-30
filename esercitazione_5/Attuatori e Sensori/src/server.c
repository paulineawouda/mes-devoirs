#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <pthread.h>
#include "../include/common.h"

struct Attuatore {
    char id[MAX_ID_LEN];
    float tgoal;
    char sensori_sottoscritti[MAX_SENSORS][MAX_ID_LEN];
    int sensori_count;
    int socket;
};

struct Attuatore attuatori[MAX_ACTUATORS];
int att_count = 0;

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void* handle_attuatore(void* arg) {
    int sock = *(int*)arg;
    free(arg);

    char buffer[BUFFER_SIZE];
    recv(sock, buffer, sizeof(buffer), 0);

    if (strncmp(buffer, "UNSUB", 5) == 0) {
        char id[MAX_ID_LEN];
        sscanf(buffer + 6, "%s", id);
        pthread_mutex_lock(&lock);
        for (int i = 0; i < att_count; ++i) {
            if (strcmp(attuatori[i].id, id) == 0) {
                close(attuatori[i].socket);
                attuatori[i] = attuatori[--att_count];
                break;
            }
        }
        pthread_mutex_unlock(&lock);
        close(sock);
        return NULL;
    }

    struct Attuatore a;
    sscanf(buffer, "%s %f %d", a.id, &a.tgoal, &a.sensori_count);

    char* p = strchr(buffer, '\n') + 1;
    for (int i = 0; i < a.sensori_count; ++i) {
        sscanf(p, "%s", a.sensori_sottoscritti[i]);
        p = strchr(p, '\n') + 1;
    }

    a.socket = sock;
    pthread_mutex_lock(&lock);
    attuatori[att_count++] = a;
    pthread_mutex_unlock(&lock);

    return NULL;
}

void* server_thread(void* arg) {
    int sockfd = *(int*)arg;
    struct sockaddr_in cli_addr;
    socklen_t clilen = sizeof(cli_addr);

    while (1) {
        int* newsockfd = malloc(sizeof(int));
        *newsockfd = accept(sockfd, (struct sockaddr*)&cli_addr, &clilen);
        pthread_t tid;
        pthread_create(&tid, NULL, handle_attuatore, newsockfd);
        pthread_detach(tid);
    }
    return NULL;
}

void* hub_listener(void* arg) {
    int sockfd;
    struct sockaddr_in serv_addr, cli_addr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(HUB_PORT);

    bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    listen(sockfd, MAX_CONN);

    socklen_t clilen = sizeof(cli_addr);
    while (1) {
        int newsockfd = accept(sockfd, (struct sockaddr*)&cli_addr, &clilen);
        TemperatureData data;
        recv(newsockfd, &data, sizeof(data), 0);

        pthread_mutex_lock(&lock);
        int counter = 0;
        for (int i = 0; i < att_count; ++i) {
            for (int j = 0; j < attuatori[i].sensori_count; ++j) {
                if (strcmp(attuatori[i].sensori_sottoscritti[j], data.sensor_id) == 0) {
                    send(attuatori[i].socket, &data, sizeof(data), 0);
                    counter++;
                }
            }
        }
        pthread_mutex_unlock(&lock);

        // Invia numero attuatori al sensore
        send(newsockfd, &counter, sizeof(int), 0);
        close(newsockfd);
    }
    return NULL;
}

int main() {
    int sockfd;
    struct sockaddr_in serv_addr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(SERVER_PORT);

    bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    listen(sockfd, MAX_CONN);

    pthread_t t1, t2;
    pthread_create(&t1, NULL, server_thread, &sockfd);
    pthread_create(&t2, NULL, hub_listener, NULL);

    printf("Server avviato sulle porte %d (client) e %d (hub)...\n", SERVER_PORT, HUB_PORT);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    return 0;
}
