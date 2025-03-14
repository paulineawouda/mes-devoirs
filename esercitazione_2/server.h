#ifndef SERVER_H
#define SERVER_H

#define PORT 8080
#define BUFFER_SIZE 1024

// Structure des données client
typedef struct {
    int socket;
    int number;
} ClientData;

// Prototypes des fonctions
void setup_server(int *server_fd);
void handle_client(ClientData *client_data);
void increment_and_send(ClientData *client_data);

#endif
