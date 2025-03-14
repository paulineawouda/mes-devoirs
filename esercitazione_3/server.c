#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <unistd.h>
#include <netdb.h>

#include "list.h"


#define NUM_PLAYERS 3


int port = 8000;



int main() 
{
	struct sockaddr_in serv_addr;
	struct sockaddr_in cli_addr;

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



	itemType msg;

	srand(time(NULL));
  
	LIST ranking = NewList();
	LIST players = NewList();


	while(1) 
	{
		printf("\nWaiting for a new player...\n");
		
		// New connection acceptance		
		int newsockfd = accept( sockfd, (struct sockaddr *)&cli_addr, &address_size );      
		if (newsockfd == -1) 
		{
			perror("Error on accept");
			exit(1);
		}
		
		// Message reception
		if ( recv( newsockfd, &msg, sizeof(itemType), 0 ) == -1) 
		{
			perror("Error on receive");
			exit(1);
		}		
		
		printf("\nPlayer '%s' joined the match\n",msg.nome);
		itemType* player = Find(ranking,msg);
   
		if (player == NULL) 
		{
			printf("Player '%s' connected for the first time\n",msg.nome);
			msg.num_partite = 0;
			msg.punteggio = 0;
			msg.sockd = 0;
			ranking = EnqueueLast(ranking,msg);
		}
		else {
			printf("Player '%s' has already played %d matches\n", player->nome, player->num_partite);
		}
    
		msg.sockd = newsockfd;
		players = EnqueueLast(players, msg);
		printf("Players waiting for the game:\n");
		LIST it;
		for(it = players; it != NULL; it = it->next) {				
			player = Find(ranking, it->item);
			PrintItem(*player);
		}
    
		if (getLength(players) == NUM_PLAYERS) 
		{
			printf("\nPlaying the game...\n");
			
						
			// Increasing number of matches
			for(it = players; it != NULL; it = it->next) {				
				player = Find(ranking, it->item);
				player->num_partite++;
			}
			
			
			// Players winning points...
			int points;		
			for (points = 3; points > 0; --points) 
			{
			
				/* Deciding the winner */
				int remainingPlayers = getLength(players);				
				int winningIndex = rand() % remainingPlayers;
				itemType winningPlayer = getItemAtPos(players, winningIndex);
				
				
				/* Attributing points */
				player = Find(ranking, winningPlayer);
				player->punteggio += points;				
				
				printf("Player %s wins %d points. Total score: %d\n", player->nome, points, player->punteggio);
				
				
				/* Sending the result of the match */
				if (send(winningPlayer.sockd, &points, sizeof(int), 0) == -1) 
				{
					perror("Error sending the result to a winner");
					exit(1);
				}
				close(winningPlayer.sockd);
				players = Dequeue(players,*player);
			}
			
			
			/* Remaining players (losers) */
			while (!isEmpty(players)) 
			{
				itemType loser = getHead(players);
				printf("Player %s lose\n",loser.nome);
				
				int points = 0;
				if (send(loser.sockd,&points,sizeof(int), 0) == -1) 
				{
					perror("Error sending the result to a loser");
					exit(1);
				}
				close(loser.sockd);
				players = DequeueFirst(players);
			}
			printf("\nRanking:\n");
			PrintList(ranking);
		}
	}

	close(sockfd);
	return 0;
}
