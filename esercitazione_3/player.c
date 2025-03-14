#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <netdb.h>

#include "list.h"


char * host_name = "127.0.0.1";
int port = 8000;



int main(int argc, char *argv[]) 
{
	if (argc < 2) 
	{
		printf("\nUsage: %s 'player_name'\n\n",argv[0]);
		return -1;
	}
	
	
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
	
	

  
	itemType msg;
	strcpy(msg.nome,argv[1]);
	msg.num_partite = 0;
	msg.punteggio = 0;
	msg.sockd = 0;

	if ( send(sockfd,&msg, sizeof(msg), 0) == -1 ) 
	{
		perror("Error sending partecipation\n");
		exit(1);
	}

	printf("\nWaiting the game to start...\n");
	
	int score;
	if ( recv(sockfd, &score, sizeof(score), 0) == -1 ) 
	{
		perror("Error receiving score from server\n");
		exit(1);
	}

	printf("My score is %d\n\n",score);

	close(sockfd);

	return 0;
}
