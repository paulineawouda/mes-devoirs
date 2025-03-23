#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <netdb.h>
#include <unistd.h>

#include "list.h"

#define NUM_PLAYERS 3

int port = 8000;

int main() {
    struct sockaddr_in serv_addr;
    struct sockaddr_in cli_addr;
    struct Messaggio msg;
    LIST titoli, tmp, next;
    ItemType item;
    int num_titoli;

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

    titoli = NewList();
    while (1) {
        printf("\nWaiting for a new player...\n");

        // New connection acceptance
        int newsockfd =
            accept(sockfd, (struct sockaddr*)&cli_addr, &address_size);
        if (newsockfd == -1) {
            perror("Error on accept");
            exit(1);
        }

        if (recv(newsockfd, &msg, sizeof(msg), 0) == -1) {
            perror("Error sending message\n");
            exit(1);
        }
        printf("ricevuto un messaggio di tipo %d con item:\n", msg.tipo);
        PrintItem(msg.item);

        if (msg.tipo == TIPO_AGENTE) {
            printf(
                "un agente vuole vendere un titolo: lo mettiamo in attese di "
                "un investitore\n");
            msg.item.sockd = newsockfd;
            msg.item.ricavo_totale = 0;
            titoli = EnqueueLast(titoli, msg.item);
        } else if (msg.tipo == TIPO_INVESTITORE) {
            num_titoli = getLength(titoli);
            printf(
                "un investitore cerca titoli da vendere: invio della lista "
                "corrente di %d item\n",
                num_titoli);
            if (send(newsockfd, &num_titoli, sizeof(num_titoli), 0) == -1) {
                perror("Error sending message\n");
                exit(1);
            }
            tmp = titoli;
            while (!isEmpty(tmp)) {
                printf("invio di ");
                PrintItem(tmp->item);
                if (send(newsockfd, &tmp->item, sizeof(tmp->item), 0) == -1) {
                    perror("Error sending message\n");
                    exit(1);
                }
                tmp = tmp->next;
            }

            printf("in attesa della scelta dell'investitore\n");
            if (recv(newsockfd, &item, sizeof(item), 0) == -1) {
                perror("Error sending message\n");
                exit(1);
            }
            printf("\nrichiesto il titolo: ");
            PrintItem(item);
            close(newsockfd);

            printf("\naggiornamento dei valori dei titoli\n");
            tmp = titoli;
            while (!isEmpty(tmp)) {
                if (itemCompare(tmp->item, item) == 0) {
                    tmp->item.ricavo_totale += tmp->item.titolo_prezzo_corrente;
                    tmp->item.titolo_quantita--;
                    tmp->item.titolo_prezzo_corrente++;
                    printf(
                        "\naggiornamento valori titolo %s (ricavo attuale "
                        "%d): ",
                        tmp->item.nome, tmp->item.titolo_prezzo_corrente);
                    PrintItem(tmp->item);

                    if (tmp->item.titolo_quantita == 0) {
                        printf("quantita pari a 0: togliere il titolo\n");
                        printf(
                            "comunico all'agente %s l'acquisto di tutti i suoi "
                            "titoli\n",
                            tmp->item.nome);
                        if (send(tmp->item.sockd, &tmp->item, sizeof(tmp->item),
                                 0) == -1) {
                            perror("Error sending message\n");
                            exit(1);
                        }
                        close(tmp->item.sockd);
                        titoli = Dequeue(titoli, item);
                    }
                    tmp = tmp->next;
                } else {
                    tmp->item.titolo_prezzo_corrente--;
                    printf("decremento prezzo titolo: ");
                    PrintItem(tmp->item);
                    if (tmp->item.titolo_prezzo_corrente <
                        tmp->item.titolo_prezzo_minimo) {
                        printf(
                            "titolo sotto la quotazione minima: va tolto dalla "
                            "lista!");
                        printf(
                            "comunico all'agente %s l'eliminazione dalla "
                            "lista\n",
                            tmp->item.nome);
                        if (send(tmp->item.sockd, &tmp->item, sizeof(tmp->item),
                                 0) == -1) {
                            perror("Error sending message\n");
                            exit(1);
                        }
                        close(tmp->item.sockd);
                        next = tmp->next;
                        titoli = Dequeue(titoli, tmp->item);
                        tmp = next;
                    } else {
                        tmp = tmp->next;
                    }
                }
            }
        } else {
            printf("messaggio da client di tipo indefinito %d\n", msg.tipo);
            close(newsockfd);
        }

        printf("\nlista titoli:\n");
        PrintList(titoli);
        printf("------------------\n");
    }

    close(sockfd);
    return 0;
}
