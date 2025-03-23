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
    ItemType item;

    if (argc < 5) {
        printf(
            "Uso: %s nome titolo_quantita titolo_prezzo_corrente "
            "titolo_prezzo_minimo\n",
            argv[0]);
        exit(1);
    }

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

    msg.tipo = TIPO_AGENTE;
    strcpy(msg.item.nome, argv[1]);
    msg.item.titolo_quantita = atoi(argv[2]);
    msg.item.titolo_prezzo_corrente = atoi(argv[3]);
    msg.item.titolo_prezzo_minimo = atoi(argv[4]);

    printf("invio del messaggio di tipo %d:\n", msg.tipo);
    PrintItem(msg.item);

    if (msg.item.titolo_prezzo_corrente < msg.item.titolo_prezzo_minimo) {
        printf(
            "il prezzo corrente del titolo %d e' inferiore al prezzo minimo "
            "%d: il titolo viene sospeso prima dell'invio!",
            msg.item.titolo_prezzo_corrente, msg.item.titolo_prezzo_minimo);
        exit(1);
    }

    if (send(sockfd, &msg, sizeof(msg), 0) == -1) {
        perror("Error sending message\n");
        exit(1);
    }

    printf("inviato messaggio: in attesa di ricevere un'offerta\n");

    if (recv(sockfd, &item, sizeof(item), 0) == -1) {
        perror("Error sending message\n");
        exit(1);
    }

    printf("ricavo della vendita: %d\n", item.ricavo_totale);
    PrintItem(item);

    close(sockfd);

    return 0;
}
