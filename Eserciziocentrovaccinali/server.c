#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <unistd.h>
#include <netdb.h>

#include "list.h"


#define BUF_SIZE 1000


int port = 8000;

int verificaFornitura(itemType fornitore, LIST centri);

LIST invioFornitura(itemType fornitore, LIST centri);


int main() 
{
	struct sockaddr_in serv_addr;
	struct sockaddr_in cli_addr;
	LIST fornitori;
	LIST centri;
	LIST tmp;
	itemType item;
	
	fornitori = NewList();
	centri = NewList();

	// Socket opening
	int sockfd = socket( PF_INET, SOCK_STREAM, 0 );  
	if ( sockfd == -1 ) 
	{
		perror("Error opening socket");
		exit(1);
	}
	
	int options = 1;
	if(setsockopt (sockfd, SOL_SOCKET, SO_REUSEADDR, &options, sizeof (options)) < 0) {
		perror("Error on setsockopt");
		exit(1);
	}

	bzero( &serv_addr, sizeof(serv_addr) );
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(port);

	// Address bindind to socket
	if ( bind( sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr) ) == -1 ) 
	{
		perror("Error on binding");
		exit(1);
	}
	
	// Maximum number of connection kept in the socket queue
	if ( listen( sockfd, 20 ) == -1 ) 
	{
		perror("Error on listen");
		exit(1);
	}


	socklen_t address_size = sizeof( cli_addr );	

	while(1) 
	{
		printf("\n---\n");
		
		// New connection acceptance		
		int newsockfd = accept( sockfd, (struct sockaddr *)&cli_addr, &address_size );      
		if (newsockfd == -1) 
		{
			perror("Error on accept");
			exit(1);
		}
				
		// Message reception
		if ( recv( newsockfd, &item, sizeof(item), 0 ) == -1) {
			perror("Error on receive");
			exit(1);
		}
		
 		printf("Ricevuta una richiesta:\n");
		PrintItem(item);
		item.sockfd = newsockfd;

		if (item.tipo == TIPO_FORNITORE) {
			printf("Fornitore: verifica se e' possibile la distribuzione\n");
			if (verificaFornitura(item, centri)) {
				printf("\n** Inizio distribuzione\n");
				centri = invioFornitura(item, centri);
			}
			else {
				printf("\n** Impossibile rifornire: fornitore in attesa\n");
				fornitori = EnqueueLast(fornitori, item);
			}
		}
		else if (item.tipo == TIPO_CENTRO) {
			printf("Centro vaccinale: verifica se sia possibile la fornitura\n");
			centri = EnqueueOrdered(centri, item);
			tmp = fornitori;
			while (!isEmpty(tmp)) {
				if (verificaFornitura(tmp->item, centri)) {
					printf("\n** Inizio distribuzione\n");
					centri = invioFornitura(tmp->item, centri);
				}
				tmp = tmp->next;
			}
			
		}
		else {
			printf("tipo messaggio %d non valido\n", item.tipo);
			close(newsockfd);
		}
		
		printf("\n** fornitori:\n");
		PrintList(fornitori);
		printf("\n** centri vaccinali:\n");
		PrintList(centri);
	}

	close(sockfd);
	return 0;
}

int verificaFornitura(itemType fornitore, LIST centri) {
	LIST tmp;
	int quantita_fornita, quantita_residua;
  	
  	tmp = centri;
  	quantita_fornita = 0;
  	quantita_residua = fornitore.quantita;
  	while (!isEmpty(tmp)) {
  		if (tmp->item.quantita <= quantita_residua) {
  			quantita_fornita += tmp->item.quantita;
  			quantita_residua -= tmp->item.quantita;
  		}
  		tmp = tmp->next;
  	}
  	
  	printf("Fornitore: ");
  	PrintItem(fornitore);
  	printf("  puo' distribuire %d vaccini, quota minima di distribuzione %d, residuo %d: ", quantita_fornita, fornitore.quantita_min, quantita_residua);
  	
  	if (quantita_fornita >= fornitore.quantita_min) {
  		printf("INIZIO DISTRIBUZIONE\n");
  		return 1;
  	}
  	else {
  		printf("ATTESA\n");
  		return 0;
  	}
}

LIST invioFornitura(itemType fornitore, LIST centri) {
	itemType* ordine;
	itemType fine_lista;
	int quantita_fornita, quantita_residua;
  	quantita_fornita = 0;
  	quantita_residua = fornitore.quantita;
  	
  	do {
  		ordine = FindFirstAvailable(centri, quantita_residua);
  		if (ordine != NULL) {
  			printf("Centro vaccinale da servire: ");
  			PrintItem(*ordine);
  			quantita_fornita += ordine->quantita;
  			quantita_residua -= ordine->quantita;
  		
  			printf("  invio al fornitore il nominativo del centro: ");
  			PrintItem(*ordine);
			if ( send(fornitore.sockfd, ordine, sizeof(itemType), 0 ) == -1) {
				perror("Error on send");
				exit(1);
			}
			
			printf("  invio al centro il nominativo del fornitore: ");
  			PrintItem(fornitore);
			if ( send(ordine->sockfd, &fornitore, sizeof(fornitore), 0 ) == -1) {
				perror("Error on send");
				exit(1);
			}
			close(ordine->sockfd);
  		
  			centri = Dequeue(centri, *ordine);	
  			printf("  quantita' vaccini rimasti %d, quantita' residua %d\n", quantita_fornita, quantita_residua);
  		}
  		else {
  			printf("non trovato altri ordini: quantita' vaccini rimasti %d, quantita' residua %d\n", quantita_fornita, quantita_residua);
  		}
  	} while (ordine != NULL);
 	printf("invio al fornitore l'item di fine lista\n");
 	strncpy(fine_lista.nome,"fine_lista", LENMAX);
 	fine_lista.quantita = -1;
	fine_lista.quantita_min = -1;
 	fine_lista.tipo = -1;
 	fine_lista.sockfd = -1;
	PrintItem(fine_lista);
	if ( send(fornitore.sockfd, &fine_lista, sizeof(fine_lista), 0 ) == -1) {
		perror("Error on send");
		exit(1);
	} 	
	close(fornitore.sockfd);
	
	return centri;
}


