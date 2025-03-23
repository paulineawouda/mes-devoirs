#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <netdb.h>
#include <unistd.h>

#include "list.h"

char* host_name = "127.0.0.1"; /* local host */
int port = 8000;

int main(int argc, char* argv[]) {
    struct sockaddr_in serv_addr;
    struct hostent* server;
    struct Messaggio msg;
    int num_titoli, i;
    ItemType item;
    char* endc;

    if ((server = gethostbyname(host_name)) == 0) {
        perror("Error resolving local host\n");
        exit(1);
    }

    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = ((struct in_addr*)(server->h_addr))->s_addr;
    serv_addr.sin_port = htons(port);

    int sockfd = socket(PF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("Error opening socket\n");
        exit(1);
    }

    if (connect(sockfd, (void*)&serv_addr, sizeof(serv_addr)) == -1) {
        perror("Error connecting to socket\n");
        exit(1);
    }

    msg.tipo = TIPO_INVESTITORE;
    strcpy(msg.item.nome, "");
    msg.item.titolo_quantita = 0;
    msg.item.titolo_prezzo_corrente = -1;
    msg.item.titolo_prezzo_minimo = -1;
    printf("invio di un messaggio di tipo %d:\n", msg.tipo);

    if (send(sockfd, &msg, sizeof(msg), 0) == -1) {
        perror("Error sending message\n");
        exit(1);
    }

    printf(
        "inviato messaggio: in attesa di ricevere la lista dei titoli "
        "quotati\n");

    if (recv(sockfd, &num_titoli, sizeof(num_titoli), 0) == -1) {
        perror("Error receiving message\n");
        exit(1);
    }
    printf("ci sono %d titoli quotati\n", num_titoli);
    for (i = 0; i < num_titoli; ++i) {
        if (recv(sockfd, &item, sizeof(item), 0) == -1) {
            perror("Error receiving message\n");
            exit(1);
        }
        PrintItem(item);
    }
    printf("inserire il titolo da acquistare: ");
    fgets(item.nome, sizeof(item.nome),
          stdin);  // Safe way to read to a buffer. 'gets' is considered to be
                   // unsafe
    if ((endc = strchr(item.nome, '\n')) != NULL) {
        *endc = '\0';
    }

    item.titolo_quantita = 1;
    item.titolo_prezzo_corrente = -1;
    item.titolo_prezzo_minimo = -1;
    printf("richiesta del titolo: ");
    PrintItem(item);
    if (send(sockfd, &item, sizeof(item), 0) == -1) {
        perror("Error sending message\n");
        exit(1);
    }

    close(sockfd);

    return 0;
}
