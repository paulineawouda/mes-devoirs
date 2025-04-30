// src/guida.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include "../include/common.h"

int main(int argc, char* argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <nome_guida> <min> <max>\n", argv[0]);
        return 1;
    }

    GuidaInfo g;
    strncpy(g.nome, argv[1], MAX_NAME_LEN);
    g.min_visite = atoi(argv[2]);
    g.max_visite = atoi(argv[3]);

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERVER_PORT);
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Errore di connessione al server");
        return 1;
    }

    char tipo[] = "GUIDA";
    send(sockfd, tipo, strlen(tipo) + 1, 0);
    send(sockfd, &g, sizeof(GuidaInfo), 0);

    printf("[Guida %s] Registrata. Attesa inizio visita...\n", g.nome);

    int totale = 0;
    recv(sockfd, &totale, sizeof(int), 0);
    printf("[Guida %s] Inizio visita con %d partecipanti.\n", g.nome, totale);

    close(sockfd);
    return 0;
}