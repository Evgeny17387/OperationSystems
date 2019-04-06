
/************************************************************************ Includes */

#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>

#include <unistd.h>
#include <strings.h>
#include <string.h>

#include <netdb.h>

/************************************************************************ Constants */

#define BUFFER_SIZE 300

#define IP "127.0.0.1"
#define PORTNO 8888

#define END_SESSION "close\n"

#define END_FILE 13

/************************************************************************ Auxiliary functions */

void error(char *msg) {
	perror(msg);
	exit(0);
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

	int n;

	// Server - Find

	server = gethostbyname(IP);

	if (server == NULL) {
		fprintf(stderr,"ERROR, no such host\n");
		exit(0);
	}

	printf("[Client] Connected\n");

	// Server - Configuration

	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr_list[0], (char *) &serv_addr.sin_addr.s_addr, server->h_length);
	serv_addr.sin_port = htons(PORTNO);

	// Read queries from user

    while (1)
    {

		// Socket - Open

		sockfd = socket(AF_INET, SOCK_STREAM, 0);
		if (sockfd < 0)
		{
			printf("socket failed\n");
			exit(0);
		}

		// Socket - Connect

		if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
			error("ERROR connecting");

		// Read from User

		fgets(query, BUFFER_SIZE, stdin);

		// Check if terminate session

		if (!strcmp(query, END_SESSION) || ( strlen(query) == 7 && query[strlen(query)-2] == END_FILE ) ){
			terminate = 1;
		}

		// Write to socket

		n = write(sockfd, query, sizeof(query));

		if (n < 0)
			error("ERROR writing to socket");

		// Read from socket

		bzero(result, BUFFER_SIZE);

		n = read(sockfd, result, BUFFER_SIZE-1);

		if (n < 0)
			error("ERROR reading from socket");

    	// Close connection

    	close(sockfd);

		// Terminate session or print result

		if (terminate){
			break;
		}else{
			printf("%s", result);
		}

    }

    return 0;

}
