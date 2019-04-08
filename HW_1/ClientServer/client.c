
/************************************************************************ Includes */

#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>

#include <unistd.h>
#include <string.h>
#include <strings.h>

#include <netdb.h>

/************************************************************************ Constants */

#define BUFFER_SIZE 300

#define IP "127.0.0.1"
#define PORTNO 8888

#define END_SESSION_MANUAL "exit\n"
#define END_SESSION_SCRIPT "exit"

#define END_FILE 13

/************************************************************************ Auxiliary functions */

void error(char *msg) {
	perror(msg);
	exit(1);
}

int full_write(int fd, void* buf, int n)
{
	int res;
	while (n > 0)
	{
		res = write(fd, buf, n);
		if (res == -1)
		{
			return -1;
		}

		if (res == 0)
		{
			return res;
		}

		n =- res;
		buf = (char*)(buf)+res;
	}

	return res;
}

int full_read(int fd, void* buf, int n)
{
	int res;
	while (n > 0)
	{
		res = read(fd, buf, n);
		if (res == -1)
		{
			return -1;
		}

		if (res == 0)
		{
			return res;
		}

		n =- res;
		buf = (char*)(buf)+res;
	}

	return res;
}

/************************************************************************ Main */

int main(void)
{

	// Variables

	int sockfd;
	int portno;

	struct sockaddr_in serv_addr;
	struct hostent *server;

    char query[BUFFER_SIZE];
    char result[BUFFER_SIZE];

	int terminate = 0;

	// Socket - Open

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
		error("socket failed\n");

	// Server - Find

	server = gethostbyname(IP);
	if (server == NULL)
		error("ERROR, no such host\n");

	// Server - Configuration

	bzero((char *)&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	memcpy((void*)&serv_addr.sin_addr.s_addr, (void*)server->h_addr_list[0], server->h_length);
	serv_addr.sin_port = htons(PORTNO);

	// Socket - Connect

	if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
		error("ERROR connecting");

	printf("[Client] Connected\n");

	// Read queries from user

    while (1)
    {

		// Read from User

		bzero(query, BUFFER_SIZE);
        fgets(query, BUFFER_SIZE, stdin);

		// Check if terminate session

		if (!strcmp(query, END_SESSION_MANUAL) || !strcmp(query, END_SESSION_SCRIPT)){
			terminate = 1;
		}

		// Write to socket
		
		if (full_write(sockfd, query, BUFFER_SIZE) < 0)
			error("ERROR writing to socket");

		// Terminate session or print result

		if (terminate){
			break;
		}

		// Read from socket

		bzero(result, BUFFER_SIZE);
		if (full_read(sockfd, result, BUFFER_SIZE) < 0)
			error("ERROR reading from socket");

		printf("%s", result);

    }

	// Close connection

	close(sockfd);

    return 0;

}
