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

#define MAX_PLAYERS 3
#define BUFFER_SIZE 4000
struct users
{
	char name[40];
	int money;
	int sock;
};

struct cards
{
	char name[40];
	int number;
};

void error(const char *msg)
{
	fprintf(stderr, "%s\n", msg);
	exit(1);
}

void red()
{
	printf("\033[1;31m");
}
void yellow()
{
	printf("\033[1;33m");
}
void black()
{
	printf("\033[0;30m");
}
void green()
{
	printf("\033[0;32m");
}
void blue()
{
	printf("\033[0;34m");
}
void purple()
{
	printf("\033[0;35m");
}
void cyan()
{
	printf("\033[0;36m");
}
void white()
{
	printf("\033[0;37m");
}
void reset()
{
	printf("\033[0m");
}