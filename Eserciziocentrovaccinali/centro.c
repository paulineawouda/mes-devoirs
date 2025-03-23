#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <netdb.h>

#include "list.h"


#define BUF_SIZE 1000


char *host_name = "127.0.0.1"; /* local host */
int port = 8000;


int main(int argc, char *argv[]) 
{
	itemType msg;
	
	if (argc < 3) {
		printf("Usage: %s nome quantita\n", argv[0]);
		return -1;
	}
	strncpy(msg.nome, argv[1], LENMAX);
	msg.quantita = atoi(argv[2]);
	msg.quantita_min = -1;
	msg.tipo = TIPO_CENTRO;
	msg.sockfd = -1;
	
		
	struct sockaddr_in serv_addr;
 	struct hostent* server;	
	
	if ( ( server = gethostbyname(host_name) ) == 0 ) 
	{
		perror("Error resolving local host\n");
		exit(1);
	}

	bzero(&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = ((struct in_addr *)(server->h_addr))->s_addr;
	serv_addr.sin_port = htons(port);
	
	int sockfd = socket( PF_INET, SOCK_STREAM, 0 );
	if ( sockfd == -1 ) 
	{
		perror("Error opening socket\n");
		exit(1);
	}    

	if ( connect(sockfd, (void*)&serv_addr, sizeof(serv_addr) ) == -1 ) 
	{
		perror("Error connecting to socket\n");
		exit(1);
	}


	printf("Invio dell'ordine di vaccini:\n");
	PrintItem(msg);

	/* This sends the string plus the string terminator '\0' */
	if ( send(sockfd, &msg, sizeof(msg), 0) == -1 ) {
		perror("Error on send\n");
		exit(1);
	}

	printf("In attesa di un fornitore:\n");

	if ( recv(sockfd, &msg, sizeof(msg), 0) == -1 ) 
	{
		perror("Error in receiving response from server\n");
		exit(1);
	}

	printf("Rifornito da: ");
	PrintItem(msg);

	close(sockfd);

	return 0;
}



