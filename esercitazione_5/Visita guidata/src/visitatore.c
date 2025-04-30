#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include "../include/common.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <numero_persone>\n", argv[0]);
        return 1;
    }

    VisitatoreInfo v;
    v.numero_persone = atoi(argv[1]);

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERVER_PORT);
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Errore di connessione al server");
        return 1;
    }

    char tipo[] = "VISITATORE";
    send(sockfd, tipo, strlen(tipo) + 1, 0);
    send(sockfd, &v, sizeof(VisitatoreInfo), 0);

    char buffer[BUFFER_SIZE];
    int n = recv(sockfd, buffer, sizeof(buffer), 0);
    if (n > 0) {
        if (strncmp(buffer, "NESSUNA", 7) == 0) {
            printf("[Visitatore] Nessuna guida disponibile al momento.\n");
        } else {
            printf("[Visitatore] Partecipi alla visita con guida: %s\n", buffer);
        }
    }

    close(sockfd);
    return 0;
}
