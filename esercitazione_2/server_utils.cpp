#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include "server.cpp" 
#include "server.h"

// Fonction pour configurer le serveur
void setup_server(int *server_fd) {
    struct sockaddr_in address;
    int opt = 1;

    if ((*server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(*server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(*server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(*server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
}

// Fonction pour gérer le client
void handle_client(ClientData *client_data) {
    printf("Numero ricevuto: %d\n", client_data->number);
    increment_and_send(client_data);
    close(client_data->socket);
}

// Fonction pour incrémenter et envoyer le nombre au client
void increment_and_send(ClientData *client_data) {
    client_data->number += 1;
    send(client_data->socket, &client_data->number, sizeof(client_data->number), 0);
    printf("Numero inviato: %d\n", client_data->number);
}
