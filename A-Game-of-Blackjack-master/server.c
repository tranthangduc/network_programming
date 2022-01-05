#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include "common.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

int main(int argc, char *argv[])
{
    int listenfd = 0, connfd = 0;
    struct sockaddr_in serv_addr;
    struct users users[10];
    char sendBuff[BUFFER_SIZE];
    time_t ticks;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    //Initializing the Socket
    if (listenfd < 0)
    {
        error("Error creating socket\n");
    }
    memset(&serv_addr, '0', sizeof(serv_addr));
    memset(sendBuff, '0', sizeof(sendBuff));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(5000);

    //Binding the socket to the address
    if (bind(listenfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {

        error("Error binding to socket\n");
    }

    listen(listenfd, 5);

    while (1)
    {
        //Initializing one socket for each client
        int clientSockFd[MAX_PLAYERS];
        int number_of_clients = 0;
        struct sockaddr_in clientAddress;
        socklen_t clientLen = sizeof(clientAddress);
        int pid;
        printf("\n Server waiting to begin a new game.....\n\n");

        // For two player
        while (number_of_clients < MAX_PLAYERS)
        {
            printf("Waiting for next player to connect \n \n");
            FILE *f = fopen("data.txt", "rw");
            int nwritten;
            if ((clientSockFd[number_of_clients] = accept(listenfd, (struct sockaddr *)&clientAddress, &clientLen)) < 0)
            {
                error("Error accepting next client\n");
            }
            if (0 > (nwritten = read(clientSockFd[number_of_clients], sendBuff, BUFFER_SIZE)))
            {
                /* error("Error reading from client"); */
                printf("Response from socket  timed out\n");
            }
            else
            {
                printf("Get data from client %s\n", sendBuff);
                char *token = strtok(sendBuff, "-");
                // Lấy ra toàn bộ token
                char name[50];
                char password[50];
                int type = 0; // LOGIN = 0; REGISTER = 1
                int money = 0;
                int i = 0;
                while (token != NULL)
                {
                    // printf(" %s\n", token); //In mỗi token ra
                    if (i == 0)
                    {
                        if (strcmp(token, "LOGIN") == 0)
                            type = 0;
                        if (strcmp(token, "REGISTER") == 0)
                            type = 1;
                    }
                    if (i == 1)
                    {
                        strcpy(name, token);
                    }
                    if (i == 2)
                    {
                        strcpy(password, token);
                    }
                    i++;
                    token = strtok(NULL, "-");
                }
                i = 0;
                //printf("%s %s %d\n",name,password,type);
                if (type == 0) // LOGIN
                {
                    while (!feof(f) && i == 0)
                    {

                        char n[50], pass[50];
                        fscanf(f, "%s %s %d\n", n, pass, &money);
                        //printf("%s %s %d\n", n, password, money);
                        if (strcmp(name, n) == 0)
                        {
                            if (strcmp(password, pass) == 0)
                            {
                                i = 1; // login success
                                users[number_of_clients].sock = clientSockFd[number_of_clients];
                                strcpy(users[number_of_clients].name,n);
                                users[number_of_clients].money = money;
                            }
                                
                        }
                    }
                    fclose(f);
                    char sendStr[60];
                    if (i == 1)
                    {
                        printf("Player is connected on socket %d\n", clientSockFd[number_of_clients]);
                        strcpy(sendStr, "AUTH-");
                        char moneyS[12];
                        sprintf(moneyS, "%d", money);
                        strcat(sendStr, moneyS);
                    }
                    else
                    {
                        strcpy(sendStr, "UNAUTH");
                    }
                    // int length = strlen(sendStr);
                    if (BUFFER_SIZE != (nwritten = write(clientSockFd[number_of_clients], sendStr, BUFFER_SIZE)))
                        error("Error! Couldn't write to client");
                    if (i == 1)
                    {
                        number_of_clients++;
                    }
                }
                if (type == 1)
                { // REGISTER
                    while (!feof(f) && i == 0)
                    {

                        char n[50], pass[50];
                        fscanf(f, "%s %s %d\n", n, pass, &money);
                        //printf("%s %s %d\n", n, password, money);
                        if (strcmp(name, n) == 0)
                        {
                            i = 1; // register failed
                        }
                    }
                    fclose(f);
                    char sendStr[60];
                    if (i == 0)
                    {
                        printf("Player is connected on socket %d\n", clientSockFd[number_of_clients]);
                        strcpy(sendStr, "AUTH-");
                        char moneyS[10];
                        strcat(sendStr, "1000");
                        // char str[50];
                        // strcpy(str, name);
                        // strcat(str, " ");
                        // strcat(str, password);
                        // strcat(str, " 1000\n");
                        users[number_of_clients].sock = clientSockFd[number_of_clients];
                        strcpy(users[number_of_clients].name,name);
                        users[number_of_clients].money = 1000;
                        f = fopen("./data.txt", "a");
                        fprintf(f, "\n%s %s %d", name,password,1000);
                        fclose(f);
                    }
                    else
                    {
                        strcpy(sendStr, "UNAUTH");
                    }
                    if (BUFFER_SIZE != (nwritten = write(clientSockFd[number_of_clients], sendStr, BUFFER_SIZE)))
                        error("Error! Couldn't write to client");
                    if (i == 0)
                    {
                        number_of_clients++;
                    }
                }
            }
        }

        printf("Let the games begin \n");
        //At this stage all the players in the game are connected

        //At this stage -- Create a new process
        pid = fork();
        // printf("pid: %d\n", pid);
        switch (pid)
        {
        case -1:
            error("A new process cannot be created");
        case 0:
        {
            //New process created:
            // printf("number: %d\n",number_of_clients);
            char arguments[MAX_PLAYERS+7][300];
            char *args[MAX_PLAYERS + 8];
            strcpy(arguments[0], "./dealer");
            args[0] = arguments[0];
            int i;
            for (i = 0; i < number_of_clients; i++)
            {
                sprintf(arguments[i + 1], "%d", clientSockFd[i]);
                args[i + 1] = arguments[i + 1];
                // printf("%s\t",args[i+1]);
            }
            // for (i = number_of_clients + 1; i <= MAX_PLAYERS + 1; i++)
            // {
            //     args[i] = 0;
            //     // printf("%s\t",args[i]);
            // }
            int j =0;
            for (i=4;i<10;i++) {
                strcpy(arguments[i],users[j].name); 
                args[i] = arguments[i];
                sprintf(arguments[++i], "%d", users[j].money);
                args[i] = arguments[i];
                j++;
            }
            args[10] = 0;
            execv("./dealer", args);
            // printf("Check\n");
            perror("error");
            error("exec didn't succeed");
        }
        //Default : Parent process
        default:
        {
            break;
        }
        }

        close(clientSockFd[0]);
        close(clientSockFd[1]);
        close(clientSockFd[2]);

        //break;
        sleep(100);
    }
}