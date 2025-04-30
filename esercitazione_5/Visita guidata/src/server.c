#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include "../include/common.h"

typedef struct {
    GuidaInfo guida;
    int conn_fd;
    int visitatori_totali;
    int visitatori_attesi[MAX_VISITATORI];
    int visitatori_fd[MAX_VISITATORI];
    int gruppo_count;
    int attiva;
} GuidaSlot;

GuidaSlot guide[MAX_GUIDE];
int guide_count = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void* handle_client(void* arg) {
    int client_fd = *(int*)arg;
    free(arg);

    char buffer[BUFFER_SIZE];
    recv(client_fd, buffer, sizeof(buffer), 0);

    if (strncmp(buffer, "GUIDA", 5) == 0) {
        GuidaInfo g;
        recv(client_fd, &g, sizeof(GuidaInfo), 0);

        pthread_mutex_lock(&mutex);
        GuidaSlot* slot = &guide[guide_count++];
        slot->guida = g;
        slot->conn_fd = client_fd;
        slot->visitatori_totali = 0;
        slot->gruppo_count = 0;
        slot->attiva = 1;
        pthread_mutex_unlock(&mutex);

        printf("[SERVER] Guida %s registrata (min: %d, max: %d)\n", g.nome, g.min_visite, g.max_visite);

        while (1) {
            pthread_mutex_lock(&mutex);
            if (slot->visitatori_totali >= g.min_visite) {
                send(slot->conn_fd, &slot->visitatori_totali, sizeof(int), 0);
                for (int i = 0; i < slot->gruppo_count; ++i) {
                    send(slot->visitatori_fd[i], g.nome, strlen(g.nome) + 1, 0);
                    close(slot->visitatori_fd[i]);
                }
                close(slot->conn_fd);
                slot->attiva = 0;
                pthread_mutex_unlock(&mutex);
                break;
            }
            pthread_mutex_unlock(&mutex);
            sleep(1);
        }

    } else if (strncmp(buffer, "VISITATORE", 10) == 0) {
        VisitatoreInfo v;
        recv(client_fd, &v, sizeof(VisitatoreInfo), 0);

        pthread_mutex_lock(&mutex);
        int guida_trovata = 0;
        int scelta = -1;
        int min_sovra = 9999;

        for (int i = 0; i < guide_count; ++i) {
            if (!guide[i].attiva) continue;
            GuidaInfo* g = &guide[i].guida;
            int spazio = g->max_visite - guide[i].visitatori_totali;

            if (spazio >= v.numero_persone) {
                int sovra = spazio - v.numero_persone;
                if (sovra < min_sovra) {
                    min_sovra = sovra;
                    scelta = i;
                    guida_trovata = 1;
                }
            }
        }

        if (!guida_trovata) {
            char msg[] = "NESSUNA GUIDA DISPONIBILE";
            send(client_fd, msg, strlen(msg) + 1, 0);
            close(client_fd);
        } else {
            GuidaSlot* g = &guide[scelta];
            g->visitatori_attesi[g->gruppo_count] = v.numero_persone;
            g->visitatori_fd[g->gruppo_count] = client_fd;
            g->visitatori_totali += v.numero_persone;
            g->gruppo_count++;
        }

        pthread_mutex_unlock(&mutex);
    }

    return NULL;
}

int main() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(SERVER_PORT);

    bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    listen(sockfd, 10);

    printf("[SERVER] In ascolto sulla porta %d...\n", SERVER_PORT);

    while (1) {
        struct sockaddr_in cli_addr;
        socklen_t clilen = sizeof(cli_addr);
        int* newsock = malloc(sizeof(int));
        *newsock = accept(sockfd, (struct sockaddr*)&cli_addr, &clilen);

        pthread_t tid;
        pthread_create(&tid, NULL, handle_client, newsock);
        pthread_detach(tid);
    }

    close(sockfd);
    return 0;
}
