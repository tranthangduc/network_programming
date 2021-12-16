#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include "common.h"
#include <pthread.h>
#include <time.h>

struct cards pc[52];

int random_array[52];

//This variable keeps a track of the cards pulled out from the deck
int global_track = 0;

// NOTE: tao mang 52 random
void Initialize_random_array()
{
	int i;
	for (i = 0; i < 52; i++)
	{
		random_array[i] = i;
	}

	srand(time(NULL));

	//Shuffling the array
	for (i = 0; i < 52; i++)
	{
		int j = rand() % (i + 1);

		int temp;
		temp = random_array[i];
		random_array[i] = random_array[j];
		random_array[j] = temp;
	}
}

// khoi tao bo bai
void Initialize_card()
{
	int i;
	int j;
	j = 1;
	for (i = 0; i < 52; i++)
	{
		if (i >= 0 && i <= 12)
		{
			strcpy(pc[i].name, "Spades");
			pc[i].number = j;
			j++;
			if (i == 12)
			{
				j = 1;
			}
		}

		else if (i >= 13 && i <= 25)
		{
			strcpy(pc[i].name, "Hearts");
			pc[i].number = j;
			j++;
			if (i == 25)
			{
				j = 1;
			}
		}

		else if (i >= 26 && i <= 38)
		{
			strcpy(pc[i].name, "Diamonds");
			pc[i].number = j;
			j++;
			if (i == 38)
			{
				j = 1;
			}
		}

		else if (i >= 39 && i <= 51)
		{
			strcpy(pc[i].name, "Clubs");
			pc[i].number = j;
			j++;
			if (i == 51)
			{
				j = 1;
			}
		}
	}
}

int clientFd[MAX_PLAYERS];

void start_game(int players)
{
	int j;

	//Initialize cards and let the dealer choose two cards
	Initialize_card();
	int i;
	// NOTE: bao cho 3 thanh vien la dealer dang chon
	for (i = 0; i < MAX_PLAYERS; i++)
	{
		if (clientFd[i] > 0)
		{
			char buffer[BUFFER_SIZE];
			int nwritten;
			strcpy(buffer, "Welcome to the Game Player, Dealer is choosing his cards ....... \n");
			if (BUFFER_SIZE != (nwritten = write(clientFd[i], buffer, BUFFER_SIZE)))
			{
				printf("Error! Couldn't write to player \n");
				//close(clientFd[i]);
				return;
			}
		}
	}

	/* The dealer chooses his cards over here */
	Initialize_random_array();

	//TODO: delete choosing two card in dealer side
	//Choosing two cards
	// int total = 0;
	// int n1 = random_array[global_track];
	// global_track++;
	// int n2 = random_array[global_track];
	// global_track++;
	// total = total + pc[n1].number + pc[n2].number;

	// //Passing the number of the Cards
	// char name1[100];
	// char name2[100];

	// //Passing the Name of the Cards
	// strcpy(name1, pc[n1].name);
	// strcpy(name2, pc[n2].name);

	// printf("%d %s\n", pc[n1].number, name1);
	// printf("%d %s\n", pc[n2].number, name2);

	char buffer[BUFFER_SIZE];
	int nwritten;

	int temp;
	temp = global_track;

	// Init card for 3 client
	struct cards card1[12];
	struct cards card2[12];
	struct cards card3[12];
	//Init total for 3 client
	int total1 = 0;
	int total2 = 0;
	int total3 = 0;

	int n1, n2;
	char ns1[10], ns2[10];
	char sendCard[100];
	//Init 2 card for every client
	//Client 1:
	

	/////////////////////// For the first client /////////////////////////

	int n_first = 2;

	strcpy(buffer, "You are the first player, press HIT/STAND \n");
	if (BUFFER_SIZE != (nwritten = write(clientFd[0], buffer, BUFFER_SIZE)))
	{
		printf("Error! Couldn't write to player \n");
		//close(clientFd[i]);
		return;
	}

	n1 = random_array[global_track];
	global_track++;
	n2 = random_array[global_track];
	global_track++;
	card1[0].number = pc[n1].number;
	total1 =total1+card1[0].number;
	strcpy(card1[0].name, pc[n1].name);
	card1[1].number = pc[n2].number;
	total1 =total1+card1[1].number;
	strcpy(card1[1].name, pc[n2].name);
	sprintf(ns1, "%d", card1[0].number);
	sprintf(ns2, "%d", card1[1].number);
	strcpy(sendCard, "INITCARD-"); // String is: "INITCARD-num1-name1-num2-name2"
	strcat(sendCard, ns1);
	strcat(sendCard, "-");
	strcat(sendCard, card1[0].name);
	strcat(sendCard, "-");
	strcat(sendCard, ns2);
	strcat(sendCard, "-");
	strcat(sendCard, card1[1].name);
	strcpy(buffer,sendCard);
	if (BUFFER_SIZE != (nwritten = write(clientFd[0], buffer, BUFFER_SIZE)))
	{
		printf("Error! Couldn't write to player \n");
		//close(clientFd[i]);
		return;
	}
	printf("Two card is: %s\n", sendCard);
	
				
	if (0 > (nwritten = read(clientFd[0], buffer, BUFFER_SIZE)))
	{
		/* error("Error reading from client"); */
		printf("Response from socket  timed out\n");
	}
	else
	{
		while (1)
		{
			if (total1 > 21)
				{
					char name1[100];
					strcpy(buffer, "LOSE");
					if (BUFFER_SIZE != (nwritten = write(clientFd[0], buffer, BUFFER_SIZE)))
					{
						printf("Error! Couldn't write to player \n");
						//close(clientFd[i]);
						return;
					}
					break;
				}
			if (strcmp(buffer, "HIT") == 0)
			{
				//Get a card in card deck
				int rand = random_array[global_track];
				strcpy(card1[n_first].name, pc[rand].name);
				card1[n_first].number = pc[rand].number;
				total1 += pc[rand].number;
				n_first++;
				global_track++;

				// Create string: CARD-num-name
				char name1[100];
				char num[10];
				sprintf(num, "%d", pc[rand].number);
				strcpy(name1, "CARD");
				strcat(name1, "-");
				strcat(name1, num);
				strcat(name1, "-");
				strcat(name1, pc[rand].name);
				printf("name1 is: %s\n", name1);

				// IF total > 21 , add LOSE to string buffer
				if (total1 > 21)
				{
					//strcpy(buffer, "LOSE");
					strcat(name1, "-");
					strcat(name1, "LOSE");
					strcpy(buffer, name1);
					if (BUFFER_SIZE != (nwritten = write(clientFd[0], buffer, BUFFER_SIZE)))
					{
						printf("Error! Couldn't write to player \n");
						//close(clientFd[i]);
						return;
					}
					break;
				}
				else
				{
					strcpy(buffer, name1);
					if (BUFFER_SIZE != (nwritten = write(clientFd[0], buffer, BUFFER_SIZE)))
					{
						printf("Error! Couldn't write to player \n");
						//close(clientFd[i]);
						return;
					}
				}

				printf("\nUser1 has chosen HIT, you have another chance!\n");
				if (0 > (nwritten = read(clientFd[0], buffer, BUFFER_SIZE)))
				{
					/* error("Error reading from client"); */
					printf("Response from socket  timed out\n");
				}
			}
			else
			{
				//Player has chosen STAND and decided not to continue
				strcpy(buffer, "Your turn over, now wait for the results\n");
				int nwritten;
				if (BUFFER_SIZE != (nwritten = write(clientFd[0], buffer, BUFFER_SIZE)))
				{
					printf("Error! Couldn't write to player \n");
					//close(clientFd[i]);
					return;
				}

				break;
			}
		
		}
	}
				
	printf("First player has chosen %d  cards\n", n_first);

	//////////////////////// FOR THE second client ///////////////////////

	strcpy(buffer, "Player 1 has already chosen cards, your turn press HIT/STAND!\n");
	int n_second = 0;
	strcat(buffer, "You are the Second player, press HIT/STAND \n");
	if (BUFFER_SIZE != (nwritten = write(clientFd[1], buffer, BUFFER_SIZE)))
	{
		printf("Error! Couldn't write to player \n");
		//close(clientFd[i]);
		return;
	}
	n_second=2;
	n1 = random_array[global_track];
	global_track++;
	n2 = random_array[global_track];
	global_track++;
	card2[0].number = pc[n1].number;
	strcpy(card2[0].name, pc[n1].name);
	total2 = total2 + card2[0].number;
	card2[1].number = pc[n2].number;
	total2 = total2 + card2[1].number;
	strcpy(card2[1].name, pc[n2].name);
	sprintf(ns1, "%d", card2[0].number);
	sprintf(ns2, "%d", card2[1].number);
	strcpy(sendCard, "INITCARD-"); // String is: "INITCARD-num1-name1-num2-name2"
	strcat(sendCard, ns1);
	strcat(sendCard, "-");
	strcat(sendCard, card2[0].name);
	strcat(sendCard, "-");
	strcat(sendCard, ns2);
	strcat(sendCard, "-");
	strcat(sendCard, card2[1].name);
	strcpy(buffer,sendCard);
	if (BUFFER_SIZE != (nwritten = write(clientFd[1], buffer, BUFFER_SIZE)))
	{
		printf("Error! Couldn't write to player \n");
		//close(clientFd[i]);
		return;
	}
	printf("Two card is: %s\n", sendCard);
	
	if (0 > (nwritten = read(clientFd[1], buffer, BUFFER_SIZE)))
	{
		/* error("Error reading from client"); */
		printf("Response from socket  timed out\n");
	}
	else
	{
		while (1)
		{
			if (total2> 21)
				{
					char name1[100];
					strcpy(buffer, "LOSE");
					if (BUFFER_SIZE != (nwritten = write(clientFd[1], buffer, BUFFER_SIZE)))
					{
						printf("Error! Couldn't write to player \n");
						//close(clientFd[i]);
						return;
					}
				break;
				}
				
			if (strcmp(buffer, "HIT") == 0)
			{
				//Get a card in card deck
				int rand = random_array[global_track];
				strcpy(card2[n_second].name, pc[rand].name);
				card2[n_second].number = pc[rand].number;
				total2 += pc[rand].number;
				n_second++;
				global_track++;

				// Create string: CARD-num-name
				char name1[100];
				char num[10];
				sprintf(num, "%d", pc[rand].number);
				strcpy(name1, "CARD");
				strcat(name1, "-");
				strcat(name1, num);
				strcat(name1, "-");
				strcat(name1, pc[rand].name);
				printf("name1 is: %s\n", name1);

				// IF total > 21 , add LOSE to string buffer
				if (total2 > 21)
				{
					//strcpy(buffer, "LOSE");
					strcat(name1, "-");
					strcat(name1, "LOSE");
					strcpy(buffer, name1);
					if (BUFFER_SIZE != (nwritten = write(clientFd[1], buffer, BUFFER_SIZE)))
					{
						printf("Error! Couldn't write to player \n");
						//close(clientFd[i]);
						return;
					}
					break;
				}
				else
				{
					strcpy(buffer, name1);
					if (BUFFER_SIZE != (nwritten = write(clientFd[1], buffer, BUFFER_SIZE)))
					{
						printf("Error! Couldn't write to player \n");
						//close(clientFd[i]);
						return;
					}
				}

				printf("\nUser2 has chosen HIT, you have another chance!\n");
				if (0 > (nwritten = read(clientFd[1], buffer, BUFFER_SIZE)))
				{
					/* error("Error reading from client"); */
					printf("Response from socket  timed out\n");
				}
			}
			else
			{
				//Player has chosen STAND and decided not to continue
				strcpy(buffer, "Your turn over, now wait for the results\n");
				int nwritten;
				if (BUFFER_SIZE != (nwritten = write(clientFd[1], buffer, BUFFER_SIZE)))
				{
					printf("Error! Couldn't write to player \n");
					//close(clientFd[i]);
					return;
				}

				break;
			}
		}
	}
				
	printf("Second player has chosen %d  cards\n", n_second);

	//////////////////////// For the third client /////////////////////////////

	strcpy(buffer, "Player 2 has already chosen cards, your turn press HIT/STAND!\n");
	int n_third = 0;
	strcat(buffer, "You are the Third player, press HIT/STAND \n");
	if (BUFFER_SIZE != (nwritten = write(clientFd[2], buffer, BUFFER_SIZE)))
	{
		printf("Error! Couldn't write to player \n");
		//close(clientFd[i]);
		return;
	}
	n_third=2;
	n1 = random_array[global_track];
	global_track++;
	n2 = random_array[global_track];
	global_track++;
	card3[0].number = pc[n1].number;
	strcpy(card3[0].name, pc[n1].name);
	card3[1].number = pc[n2].number;
	strcpy(card3[1].name, pc[n2].name);
	total3 = total3 +card3[0].number;
	total3 = total3 +card3[1].number;
	sprintf(ns1, "%d", card3[0].number);
	sprintf(ns2, "%d", card3[1].number);
	strcpy(sendCard, "INITCARD-"); // String is: "INITCARD-num1-name1-num2-name2"
	strcat(sendCard, ns1);
	strcat(sendCard, "-");
	strcat(sendCard, card3[0].name);
	strcat(sendCard, "-");
	strcat(sendCard, ns2);
	strcat(sendCard, "-");
	strcat(sendCard, card3[1].name);
	strcpy(buffer,sendCard);
	if (BUFFER_SIZE != (nwritten = write(clientFd[2], buffer, BUFFER_SIZE)))
	{
		printf("Error! Couldn't write to player \n");
		//close(clientFd[i]);
		return;
	}
	printf("Two card is: %s\n", sendCard);
	
				
	if (0 > (nwritten = read(clientFd[2], buffer, BUFFER_SIZE)))
	{
		/* error("Error reading from client"); */
		printf("Response from socket  timed out\n");
	}
	else
	{
		while (1)
		{
			if (total3> 21)
				{
					char name1[100];
					strcpy(buffer, "LOSE");
					if (BUFFER_SIZE != (nwritten = write(clientFd[2], buffer, BUFFER_SIZE)))
					{
						printf("Error! Couldn't write to player \n");
						//close(clientFd[i]);
						return;
					}
				break;
				}
				else {
			if (strcmp(buffer, "HIT") == 0)
			{
				//Get a card in card deck
				int rand = random_array[global_track];
				strcpy(card3[n_third].name, pc[rand].name);
				card3[n_third].number = pc[rand].number;
				total3 += pc[rand].number;
				n_third++;
				global_track++;

				// Create string: CARD-num-name
				char name1[100];
				char num[10];
				sprintf(num, "%d", pc[rand].number);
				strcpy(name1, "CARD");
				strcat(name1, "-");
				strcat(name1, num);
				strcat(name1, "-");
				strcat(name1, pc[rand].name);
				printf("name1 is: %s\n", name1);

				// IF total > 21 , add LOSE to string buffer
				if (total3 > 21)
				{
					//strcpy(buffer, "LOSE");
					strcat(name1, "-");
					strcat(name1, "LOSE");
					strcpy(buffer, name1);
					if (BUFFER_SIZE != (nwritten = write(clientFd[2], buffer, BUFFER_SIZE)))
					{
						printf("Error! Couldn't write to player \n");
						//close(clientFd[i]);
						return;
					}
					break;
				}
				else
				{
					strcpy(buffer, name1);
					if (BUFFER_SIZE != (nwritten = write(clientFd[2], buffer, BUFFER_SIZE)))
					{
						printf("Error! Couldn't write to player \n");
						//close(clientFd[i]);
						return;
					}
				}

				printf("\nUser2 has chosen HIT, you have another chance!\n");
				if (0 > (nwritten = read(clientFd[2], buffer, BUFFER_SIZE)))
				{
					/* error("Error reading from client"); */
					printf("Response from socket  timed out\n");
				}
			}
			else
			{
				//Player has chosen STAND and decided not to continue
				strcpy(buffer, "Your turn over, now wait for the results\n");
				int nwritten;
				if (BUFFER_SIZE != (nwritten = write(clientFd[2], buffer, BUFFER_SIZE)))
				{
					printf("Error! Couldn't write to player \n");
					//close(clientFd[i]);
					return;
				}

				break;
			}
		}
		}
	}
				
	printf("Third player has chosen %d  cards\n", n_third);


	///////////////////  Conditions for Scoring  ////////////////////

	int arr[3];
	arr[0] = 0;
	arr[1] = 0;
	arr[2] = 0;

	if (total1 > 21)
	{
		//printf("User 1 loses \n");
		arr[0] = 1;
	}

	if (total2 > 21)
	{
		//printf("User 2 loses \n");
		arr[1] = 1;
	}

	if (total3 > 21)
	{
		//printf("User 3 loses \n");
		arr[2] = 1;
	}

	int b;
	for (b = 0; b < 3; b++)
	{
		if (arr[b] == 1)
		{
			printf("Player %d loses because of total > 21 ! \n", b + 1);
		}
	}
		printf("First player score : %d\n", total1);
		printf("Second player score : %d\n", total2);
		printf("Third player score : %d\n", total3);
	int diff1, diff2, diff3;
	int min;

	int win[3];
	win[0] = 0;
	win[1] = 0;
	win[2] = 0;

	if (total1 > 21 && total2 > 21 && total3 >21)
	{
		win[0] = 0;
		win[1] = 0;
		win[2] = 0;
		diff1 = abs(total1 - 21);
		diff2 = abs(total2 - 21);
		diff3 = abs(total3 - 21);

		//Find the minimum amongst the three

		

		if (diff1 <= diff2 && diff1 <= diff3)
		{
			printf("Player1 Wins!");
			
			win[0] = 1;
			
		}

		if (diff2 <= diff1 && diff2 <= diff3)
		{
			printf("Player2 Wins!\n");
			
			win[1] = 1;
			
		}
		if (diff3 <= diff1 && diff3 <= diff2)
		{
			printf("Player3 Wins!\n");
			win[2] = 1;
			
		}

		for (b = 0; b < 3; b++)
		{
			if (win[b] == 1)
			{
				//This player wins
				strcpy(buffer, "You win!");
				int nwritten;
				if (BUFFER_SIZE != (nwritten = write(clientFd[b], buffer, BUFFER_SIZE)))
				{
					printf("Error! Couldn't write to player \n");
					//close(clientFd[i]);
					return;
				}
			}
			else
			{
				//This player loses
				strcpy(buffer, "You lose!");
				int nwritten;
				if (BUFFER_SIZE != (nwritten = write(clientFd[b], buffer, BUFFER_SIZE)))
				{
					printf("Error! Couldn't write to player \n");
					//close(clientFd[i]);
					return;
				}
			}
		}
	}

	if (total1 <= 21 || total2 <=21 || total3 <= 21)
	{
		if(total1>21)
		{
			win[0] = 0;
			if(total2>21) {
				win[1] = 0;
				win[2] = 1;
				
			} else {
				if(total3 > 21) {
					win[1] = 1;
					win[2] = 0;
					
				} else {
					if(total2 >= total3) {
						win[1] = 1;
						
					}
					if(total3 >= total2) {
						win[2] = 1;
						
					}
				}
			}
		}else {
			if(total2>21) {
				win[1] = 0;
				if (total3 > 21) {
					win[2] = 0;
					win[0] = 1;
					
				} else {
					if(total1 >= total3) {
						win[0] = 1;
						
					}
					if(total3 >= total1) {
						win[2] = 1;
	
						
					}
				}
			} else {
				if(total3>21) {
				win[2] = 0;
				if(total1 >= total2) {
					win[0] = 1;
					
				}
				if(total2 >= total1) {
					win[1] = 1;
					
				}
			}else {
				if (total1 >= total2 && total1 >=total3)
		{
			
			win[0] = 1;
			
		}

		if (total2 >=total1 && total2 >=total3)
		{
			
			
			win[1] = 1;
			
		}
		if (total3 >=total1 && total3 >=total2)
		{
			win[2] = 1;
			
		}
			}
			}
			
			
		}
		for (b = 0; b < 3; b++)
		{
			if (win[b] == 1)
			{
				//This player wins
				printf("Player%d Wins!\n",b+1);
				strcpy(buffer, "You win!\n");
				int nwritten;
				if (BUFFER_SIZE != (nwritten = write(clientFd[b], buffer, BUFFER_SIZE)))
				{
					printf("Error! Couldn't write to player \n");
					//close(clientFd[i]);
					return;
				}
			}
			else
			{
				//This player loses
				strcpy(buffer, "You lose!\n");
				int nwritten;
				if (BUFFER_SIZE != (nwritten = write(clientFd[b], buffer, BUFFER_SIZE)))
				{
					printf("Error! Couldn't write to player \n");
					//close(clientFd[i]);
					return;
				}
			}
		}
		
	}
	close(clientFd[0]);
	close(clientFd[1]);
	close(clientFd[2]);

	///////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////

	//Close each connection -- Both Client and Server Connection --
}

int main(int argc, char *argv[])
{

	int i;
	for (i = 0; i < argc - 1; i++)
	{
		//Initializing the socket numbers
		clientFd[i] = atoi(argv[i + 1]);
	}

	start_game(MAX_PLAYERS);

	return 0;
}