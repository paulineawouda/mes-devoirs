#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <pthread.h>
#include "../include/common.h"

#define MAX_CLIENTS 100

int server_sock;
int client_socks[MAX_CLIENTS];
TemperatureData pending_data[MAX_CLIENTS];
int connected_clients = 0;
int J;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void* sensor_handler(void* arg) {
    int sock = *(int*)arg;
    free(arg);
    TemperatureData data;
    recv(sock, &data, sizeof(data), 0);

    pthread_mutex_lock(&lock);
    pending_data[connected_clients] = data;
    client_socks[connected_clients] = sock;
    connected_clients++;

    if (connected_clients >= J) {
        int server_fd = socket(AF_INET, SOCK_STREAM, 0);
        struct sockaddr_in serv_addr;
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(HUB_PORT);
        serv_addr.sin_addr.s_addr = INADDR_ANY;

        connect(server_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

        for (int i = 0; i < connected_clients; ++i) {
            send(server_fd, &pending_data[i], sizeof(TemperatureData), 0);

            int ack = 1;  // Simuliamo che un attuatore abbia ricevuto la misura
            send(client_socks[i], &ack, sizeof(int), 0);
            close(client_socks[i]);
        }

        connected_clients = 0;
        close(server_fd);
    }

    pthread_mutex_unlock(&lock);
    return NULL;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <J>\n", argv[0]);
        exit(1);
    }
    J = atoi(argv[1]);

    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in hub_addr;
    hub_addr.sin_family = AF_INET;
    hub_addr.sin_port = htons(HUB_PORT);
    hub_addr.sin_addr.s_addr = INADDR_ANY;

    bind(server_sock, (struct sockaddr*)&hub_addr, sizeof(hub_addr));
    listen(server_sock, MAX_CONN);

    printf("Hub in ascolto sulla porta %d... (J = %d)\n", HUB_PORT, J);

    while (1) {
        struct sockaddr_in cli_addr;
        socklen_t clilen = sizeof(cli_addr);
        int* newsock = malloc(sizeof(int));
        *newsock = accept(server_sock, (struct sockaddr*)&cli_addr, &clilen);
        pthread_t tid;
        pthread_create(&tid, NULL, sensor_handler, newsock);
        pthread_detach(tid);
    }

    return 0;
}
