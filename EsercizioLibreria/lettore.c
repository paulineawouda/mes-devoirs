#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <netdb.h>
#include <unistd.h>

#include "list.h"

#define BUF_SIZE 1000

char* host_name = "127.0.0.1"; /* local host */
int port = 8000;

int main(int argc, char* argv[]) {
    struct sockaddr_in serv_addr;
    struct hostent* server;
    ItemType msg; 
    int ret; 

    if (argc < 2) {
       printf("Usage: %s titolo\n", argv[0]);
       exit(-1);
    }
    msg.tipo = TIPO_LETTORE; 
    strncpy(msg.titolo, argv[1], STRSIZE-1); 
    msg.quantita = -1; 


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

    printf("Invio del messaggio:\n");
    PrintItem(msg); 

    /* This sends the string plus the string terminator '\0' */
    if (send(sockfd, &msg, sizeof(ItemType), 0) == -1) {
        perror("Error on send\n");
        exit(1);
    }

    printf("Message sent. Waiting for response...\n");

    if (recv(sockfd, &ret, sizeof(int), 0) == -1) {
        perror("Error in receiving response from server\n");
        exit(1);
    }

    printf("Risposta del server %d\n", ret);

    close(sockfd);

    return 0;
}
