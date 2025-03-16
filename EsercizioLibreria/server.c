#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <netdb.h>
#include <unistd.h>
#include "list.h"

#define BUF_SIZE 1000

int port = 8000;

int main() {
    struct sockaddr_in serv_addr;
    struct sockaddr_in cli_addr;
    LIST lettori; 
    LIST libri; 
    ItemType item;
    ItemType *lettore, *libro; 
    int ret; 
    
    lettori = NewList(); 
    libri = NewList(); 

    // Socket opening
    int sockfd = socket(PF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("Error opening socket");
        exit(1);
    }

    int options = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &options,
                   sizeof(options)) < 0) {
        perror("Error on setsockopt");
        exit(1);
    }

    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);

    // Address bindind to socket
    if (bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1) {
        perror("Error on binding");
        exit(1);
    }

    // Maximum number of connection kept in the socket queue
    if (listen(sockfd, 20) == -1) {
        perror("Error on listen");
        exit(1);
    }

    socklen_t address_size = sizeof(cli_addr);

    while (1) {
        printf("\n---\nIn attesa di client\n");

        // New connection acceptance
        int newsockfd = accept(sockfd, (struct sockaddr*)&cli_addr, &address_size);
        if (newsockfd == -1) {
            perror("Error on accept");
            exit(1);
        }

        // Message reception
        if (recv(newsockfd, &item, sizeof(ItemType), 0) == -1) {
            perror("Error on receive");
            exit(1);
        }

        printf("Ricevuto il messaggio:\n");
        PrintItem(item); 

        if (item.tipo == TIPO_EDITORE) {
          lettore = NULL;
          do {
            lettore = Find(lettori, item);
            if (lettore != NULL && item.quantita > 0) {
              item.quantita--;
              ret = 1; 
              if (send(lettore->sockfd, &ret, sizeof(ret), 0) == -1) {
                perror("Error on send");
                exit(1);
              }
              close(lettore->sockfd); 
              lettori = Dequeue(lettori, *lettore); 
              printf("Trovato un lettore in attesa: rimaste %d copie\n", item.quantita); 
            }
          } while (lettore != NULL && item.quantita > 0); 
          
          if (item.quantita > 0) {
            printf("titolo inserito nel magazzino:\n");
            PrintItem(item);
            libro = Find(libri, item);
            if (libro != NULL) {
              libro->quantita += item.quantita;
            }
            else {
              libri = EnqueueOrdered(libri, item);
            }
            ret = 1; 
            if (send(newsockfd, &ret, sizeof(ret), 0) == -1) {
              perror("Error on send");
              exit(1);
            }
            close(newsockfd); 
          }
        }
        else if (item.tipo == TIPO_LETTORE) {
          item.sockfd = newsockfd; 
          libro = Find(libri, item);
          if (libro != NULL) {
            printf("trovato il libro nel magazzino\n");
            PrintItem(*libro);
            ret = 1; 
            if (send(item.sockfd, &ret, sizeof(ret), 0) == -1) {
              perror("Error on send");
              exit(1);
            }
            close(item.sockfd);
            libro->quantita--;
            if (libro->quantita == 0) {
              printf("non ci sono piu' copie: rimozione del libro dalla lista\n");
              libri = Dequeue(libri, *libro);
            }
          }
          else {
            printf("il libro richiesto non presente nel magazzino : attesa\n");
            lettori = EnqueueLast(lettori, item); 
          }
        }
        else {
          printf("tipo non valido :  errore\n");
        } 
        
        printf("\nlista dei lettori:\n");
        PrintList(lettori);
        printf("\nlista dei libri:\n");
        PrintList(libri);
    }

    close(sockfd);
    return 0;
}
