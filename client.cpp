#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>
#include <pthread.h>
#include <iostream>
#include "client_utils.h"


#define PORT 8080
#define NUM_REQUESTS 5

// Struttura per i dati della richiesta
typedef struct {
    int number;
} RequestData;

// Prototipi delle funzioni
void *send_request(void *arg);
int create_socket();
void connect_to_server(int sock, struct sockaddr_in *serv_addr);
void send_number(int sock, int num);
void receive_response(int sock);

int main() {
    srand(time(NULL)); // Inizializzazione del seme per la generazione di numeri casuali
    pthread_t threads[NUM_REQUESTS];
    RequestData request_data[NUM_REQUESTS];

    // Creazione di più richieste client concorrenti
    for (int i = 0; i < NUM_REQUESTS; i++) {
        request_data[i].number = rand() % 100; // Generazione di un numero unico
        pthread_create(&threads[i], NULL, send_request, (void *)&request_data[i]);
        sleep(1); // Pausa per vedere chiaramente l'output
    }

    // Attesa che tutti i thread finiscano
    for (int i = 0; i < NUM_REQUESTS; i++) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}

void *send_request(void *arg) {
    RequestData *request_data = (RequestData *)arg;
    int sock = create_socket();
    struct sockaddr_in serv_addr;

    connect_to_server(sock, &serv_addr);
    send_number(sock, request_data->number);
    receive_response(sock);
    close(sock);
    return NULL;
}

int create_socket() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    
    if (sock < 0) {
        std::cerr << "Erreur lors de la création du socket !" << std::endl;
        return -1;  // Code d'erreur
    }
    
    return sock;  // Retourner le descripteur de socket si tout est ok
}
