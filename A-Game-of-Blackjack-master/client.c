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
#include <stdlib.h>
void play(int sock)
{

    struct cards hand[12];
    int count = 0;
    printf("Waiting for the game to begin....\n");
    while (1)
    {
        int n;
        char buffer[BUFFER_SIZE];
        if (0 > (n = read(sock, buffer, BUFFER_SIZE)))
        {
            /* error("Error reading from client"); */
            printf("Response from socket  timed out\n");
        }
        else
        {
            printf("%s \n", buffer);
        }

        if (0 > (n = read(sock, buffer, BUFFER_SIZE)))
        {
            /* error("Error reading from client"); */
            printf("Response from socket  timed out\n");
        }
        else
        {
            printf("%s \n", buffer);
        }
        // Get two card from dealer
        if (0 > (n = read(sock, buffer, BUFFER_SIZE)))
        {
            /* error("Error reading from client"); */
            printf("Response from socket  timed out\n");
        }
        else
        {
            //String is: INITCARD-num1-name1-num2-name2
            char tmp[5][10];
            int c = 0;
            char *token = strtok(buffer, "-");
            while (token != NULL)
            {
                //printf("%s\n", token);
                strcpy(tmp[c++], token);
                token = strtok(NULL, "-");
            }
            printf("Card 1 is: %s %s\n", tmp[1], tmp[2]);
            // hand[count].number = atoi(tmp[1]);
            // strcpy(hand[count].name, tmp[2]);
            // count++;
            printf("Card 2 is: %s %s\n", tmp[3], tmp[4]);
            // hand[count].number = atoi(tmp[3]);
            // strcpy(hand[count].name, tmp[4]);
            // count++;
        }

        //Now take the input from the user
        int d;

        while (1)
        {
            scanf("%d", &d);
            if (d == 1)
            {

                int nwritten;
                strcpy(buffer, "HIT");
                printf("Sending...%s\n", buffer);
                //Read card from server send to client
                if (BUFFER_SIZE != (nwritten = write(sock, buffer, BUFFER_SIZE)))
                    error("Error! Couldn't write to server");
                if (0 > (n = read(sock, buffer, BUFFER_SIZE)))
                {
                    /* error("Error reading from client"); */
                    printf("Response from socket  timed out\n");
                }
                if (strcmp(buffer, "LOSE") == 0)
                {
                   
                    strcpy(buffer, "OUT");
                    int nwritten;
                    if (BUFFER_SIZE != (nwritten = write(sock, buffer, BUFFER_SIZE)))
                    error("Error! Couldn't write to server");
                    printf("You lose because total > 21\n");
                    break;
                }

                char tmp[5][10];
                int c = 0;
                char *token = strtok(buffer, "-");
                while (token != NULL)
                {
                    //printf("%s\n", token);
                    strcpy(tmp[c++], token);
                    token = strtok(NULL, "-");
                }
                printf("%s %s\n", tmp[1], tmp[2]);
                // hand[count].number = atoi(tmp[1]);
                // strcpy(hand[count].name, tmp[2]);
                // count++;
                if (strcmp(tmp[3], "LOSE") == 0)
                {
                    strcpy(buffer, "OUT");
                    int nwritten;
                    if (BUFFER_SIZE != (nwritten = write(sock, buffer, BUFFER_SIZE)))
                    error("Error! Couldn't write to server");
                    printf("You Lose\n");
                    break;
                }
            }
            else
            {
                strcpy(buffer, "STAND");
                int nwritten;
                printf("Sending....%s\n", buffer);
                if (BUFFER_SIZE != (nwritten = write(sock, buffer, BUFFER_SIZE)))
                    error("Error! Couldn't write to server");
                break;
            }
        }
        if (0 > (n = read(sock, buffer, BUFFER_SIZE)))
        {
            /* error("Error reading from client"); */
            printf("Response from socket  timed out\n");
        }
        else
        {
            printf("\n%s\n", buffer);
        }

        if (0 > (n = read(sock, buffer, BUFFER_SIZE)))
        {
            /* error("Error reading from client"); */
            printf("Response from socket  timed out\n");
        }
        else
        {
            
            char tmp[30][10];
            int x;
            int tick;
            int c = 0;
            char *token = strtok(buffer, "-");
            while (token != NULL)
            {
                //printf("%s\n", token);
                strcpy(tmp[c++], token);
                token = strtok(NULL, "-");
            }
            printf("%s: %s(point): ",tmp[1],tmp[2]);
            x = atoi(tmp[3]);
            // printf("%d\n",x);
            for( tick=4;tick<4 + x*2 ;tick++) {
                printf("%s ",tmp[tick]);
                tick++;
                printf("%s - ",tmp[tick]);
            }
            printf("\n");
            printf("%s: ",tmp[tick]);
            tick++;
            printf("%s(point):  ",tmp[tick]);
            x = atoi(tmp[++tick]);
            tick++;
            int p = tick + x*2;
            for(tick;tick<p ;tick++) {
                printf("%s ",tmp[tick]);
                tick++;
                printf("%s - ",tmp[tick]);
            }
            printf("\n");
            printf("%s: ",tmp[tick]);
            tick++;
            printf("%s(point):  ",tmp[tick]);
            x = atoi(tmp[++tick]);
            tick++;
            p = tick + x*2;
            for( tick;tick<p ;tick++) {
                printf("%s ",tmp[tick]);
                tick++;
                printf("%s - ",tmp[tick]);
            }
            printf("\n");

        }
    }
    //return;
}

int main(int argc, char *argv[])
{
    int sockfd = 0, n = 0;
    char recvBuff[1024];
    struct sockaddr_in serv_addr;
    int money = 0;
    if (argc != 2)
    {
        printf("\n Usage: %s <ip of server> \n", argv[0]);
        return 1;
    }

    memset(recvBuff, '0', sizeof(recvBuff));
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Error : Could not create socket \n");
        return 1;
    }

    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(5000);

    if (inet_pton(AF_INET, argv[1], &serv_addr.sin_addr) <= 0)
    {
        printf("\n inet_pton error occured\n");
        return 1;
    }

    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\n Error : Connect Failed \n");
        return 1;
    }

    else
    {
        char buffer[BUFFER_SIZE];
        int nwritten;
        int err = 0;
        int choose = 0;
        while (err != 1)
        {
            printf("1. Log in\n");
            printf("2. Sign in\n");
            printf("3. Exit\n");
            printf("Choose ?\n");
            scanf("%d", &choose);
            char name[50];
            char password[50];
            switch (choose)
            {
            case 1:
                printf("Input name:\t");
                scanf("%s", name);
                printf("Password: \t");
                scanf("%s", password);
                char sendStr[60];
                strcpy(sendStr, "LOGIN-");
                strcat(sendStr, name);
                strcat(sendStr, "-");
                strcat(sendStr, password);
                printf("client send data  %s\n", sendStr);
                if (BUFFER_SIZE != (nwritten = write(sockfd, sendStr, BUFFER_SIZE)))
                    error("Error! Couldn't write to server");
                if (0 > (n = read(sockfd, buffer, BUFFER_SIZE)))
                {
                    /* error("Error reading from client"); */
                    printf("Response from socket  timed out\n");
                }
                else
                {
                    printf("%s\n", buffer);

                    if (strcmp(buffer, "UNAUTH") == 0)
                    {
                        err = 1;
                        printf("Log in Failed\nDo you want login\n1. Yes\n2. No\n");
                        scanf("%d", &choose);
                        if (choose == 1)
                        {
                            err = 2; // continue login
                            close(sockfd);
                            if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
                            {
                                printf("\n Error : Could not create socket \n");
                                return 1;
                            }
                            if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
                            {
                                printf("\n Error : Connect Failed \n");
                                return 1;
                            }
                        }
                    }
                    else
                    {
                        err = 0;
                        char *token = strtok(buffer, "-");
                        // Lấy ra toàn bộ token
                        while (token != NULL)
                        {
                            printf(" %s\n", token); //In mỗi token ra
                            token = strtok(NULL, "-");
                            if(token != NULL)
                                money = atoi(token);
                            printf("Money is: %d\n", money);
                        }
                    }
                }
                if (err == 0)
                {
                    printf("Connected to server \n");
                    play(sockfd);
                    perror("play");
                    err = 1; // Play done
                }
                break;
            case 2:
                printf("Connected to server \n");
                printf("Input name:\t");
                scanf("%s", name);
                printf("Password: \t");
                scanf("%s", password);
                char sendStr2[60];
                strcpy(sendStr2, "REGISTER-");
                strcat(sendStr2, name);
                strcat(sendStr2, "-");
                strcat(sendStr2, password);
                printf("client send data  %s\n", sendStr2);
                if (BUFFER_SIZE != (nwritten = write(sockfd, sendStr2, BUFFER_SIZE)))
                    error("Error! Couldn't write to server");
                if (0 > (n = read(sockfd, buffer, BUFFER_SIZE)))
                {
                    /* error("Error reading from client"); */
                    printf("Response from socket  timed out\n");
                }
                else
                {
                    printf("%s\n", buffer);

                    if (strcmp(buffer, "UNAUTH") == 0)
                    {
                        err = 1;
                        printf("Register Failed\nDo you want Register\n1. Yes\n2. No\n");
                        scanf("%d", &choose);
                        if (choose == 1)
                        {
                            err = 2; // continue Register
                            close(sockfd);
                            if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
                            {
                                printf("\n Error : Could not create socket \n");
                                return 1;
                            }
                            if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
                            {
                                printf("\n Error : Connect Failed \n");
                                return 1;
                            }
                        }
                    }
                    else
                    {
                        err = 0;
                        char *token = strtok(buffer, "-");
                        // Lấy ra toàn bộ token
                        while (token != NULL)
                        {
                            printf(" %s\n", token); //In mỗi token ra
                            token = strtok(NULL, "-");
                            if(token != NULL)
                                money = atoi(token);
                            printf("Money is: %d\n", money);
                        }
                    }
                }
                if (err == 0)
                {
                    printf("Connected to server \n");
                    play(sockfd);
                    perror("play");
                    err = 1; // Play done
                }
                break;
            default:
                err = 1;
                close(sockfd);
                break;
            }
        }
    }

    return 0;
}