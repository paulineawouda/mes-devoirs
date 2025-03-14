// client_utils.cpp
#include "client_utils.h"
#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>  // Pour les fonctions de gestion des adresses IP

#define SERVER_IP "127.0.0.1"  // Adresse IP du serveur (localhost ici)
#define PORT 8080               // Port du serveur

// Définition de la fonction connect_to_server
void connect_to_server(int sock, struct sockaddr_in *serv_addr) {
    // Configurer l'adresse du serveur
    serv_addr->sin_family = AF_INET;
    serv_addr->sin_port = htons(PORT);  // Convertir le port en format réseau
    serv_addr->sin_addr.s_addr = inet_addr(SERVER_IP);  // Convertir l'adresse IP en format réseau

    // Connexion au serveur
    if (connect(sock, (struct sockaddr *)serv_addr, sizeof(*serv_addr)) < 0) {
        std::cerr << "Erreur de connexion au serveur !" << std::endl;
        exit(1);  // Arrêter le programme en cas d'erreur
    }
}

// Définition de la fonction send_number
void send_number(int sock, int num) {
    // Envoyer le nombre au serveur
    if (send(sock, &num, sizeof(num), 0) < 0) {
        std::cerr << "Erreur lors de l'envoi du nombre !" << std::endl;
    }
}

// Définition de la fonction receive_response
void receive_response(int sock) {
    int response;
    // Recevoir la réponse du serveur
    if (recv(sock, &response, sizeof(response), 0) < 0) {
        std::cerr << "Erreur lors de la réception de la réponse !" << std::endl;
    } else {
        std::cout << "Réponse reçue du serveur : " << response << std::endl;
    }
}
