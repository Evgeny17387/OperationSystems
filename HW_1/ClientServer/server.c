
/************************************************************************ Includes */

#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>

#include <unistd.h>
#include <string.h>
#include <strings.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <ctype.h>

/********************************************************** Consts */

							/** Taken from https://en.wikipedia.org/wiki/List_of_Eurovision_Song_Contest_winners **/
							/**************************************************************************************/

							  // Year		Host     		Winner        									Points	Margin	Runner-up
const char * const list[][6] = {{"1956",	"Lugano",		"Switzerland",									"N/A",	"N/A",	"N/A"},
								{"1957",	"Frankfurt",	"Netherlands",									"31",	"14",	"France"},
								{"1958",	"Hilversum",	"France",										"27",	"3",	"Switzerland"},
								{"1959",	"Cannes",		"Netherlands",									"21",	"5",	"United-Kingdom"},
								{"1960",	"London",		"France",										"32",	"7",	"United-Kingdom"},
								{"1961",	"Cannes",		"Luxembourg",									"31",	"7",	"United-Kingdom"},
								{"1962",	"Luxembourg",	"France",										"26",	"13",	"Monaco"},
								{"1963",	"London",		"Denmark",										"42",	"2",	"Switzerland"},
								{"1964",	"Copenhagen",	"Italy",										"49",	"32",	"United-Kingdom"},
								{"1965",	"Naples",		"Luxembourg",									"32",	"6",	"United-Kingdom"},
								{"1966",	"Luxembourg",	"Austria",										"31",	"15",	"Sweden"},
								{"1967",	"Vienna",		"United-Kingdom",								"47",	"25",	"Ireland"},
								{"1968",	"London",		"Spain",										"29",	"1",	"United-Kingdom"},
								{"1969",	"Madrid",		"Spain, United-Kingdom, Netherlands, France",	"18",	"N/A",	"N/A"},
								{"1970",	"Amsterdam",	"Ireland",										"32",	"6",	"United-Kingdom"},
								{"1971",	"Dublin",		"Monaco",										"128",	"12",	"Spain"},
								{"1972",	"Edinburgh",	"Luxembourg",									"128",	"14",	"United-Kingdom"},
								{"1973",	"Luxembourg",	"Luxembourg",									"129",	"4",	"Spain"},
								{"1974",	"Brighton",		"Sweden",										"24",	"6",	"Italy"},
								{"1975",	"Stockholm",	"Netherlands",									"152",	"14",	"United-Kingdom"},
								{"1976",	"The Hague",	"United-Kingdom",								"164",	"17",	"France"},
								{"1977",	"London",		"France",										"136",	"15",	"United-Kingdom"},
								{"1978",	"Paris",		"Israel",										"157",	"32",	"Belgium"},
								{"1979",	"Jerusalem",	"Israel",										"125",	"9",	"Spain"},
								{"1980",	"The Hague",	"Ireland",										"143",	"15",	"Germany"},
								{"1981",	"Dublin",		"United-Kingdom",								"136",	"4",	"Germany"},
								{"1982",	"Harrogate",	"Germany",										"161",	"61",	"Israel"},
								{"1983",	"Munich",		"Luxembourg",									"142",	"6",	"Israel"},
								{"1984",	"Luxembourg",	"Sweden",										"145",	"8",	"Ireland"},
								{"1985",	"Gothenburg",	"Norway",										"123",	"18",	"Germany"},
								{"1986",	"Bergen",		"Belgium",										"176",	"36",	"Switzerland"},
								{"1987",	"Brussels",		"Ireland",										"172",	"31",	"Germany"},
								{"1988",	"Dublin",		"Switzerland",									"137",	"1",	"United-Kingdom"},
								{"1989",	"Lausanne",		"Yugoslavia",									"137",	"7",	"United-Kingdom"},
								{"1990",	"Zagreb",		"Italy",										"149",	"17",	"Ireland, France"},
								{"1991",	"Rome",			"Sweden",										"146",	"0",	"France"},
								{"1992",	"Malmo",		"Ireland",										"155",	"16",	"United-Kingdom"},
								{"1993",	"Millstreet",	"Ireland",										"187",	"23",	"United-Kingdom"},
								{"1994",	"Dublin",		"Ireland",										"226",	"60",	"Poland"},
								{"1995",	"Dublin",		"Norway",										"148",	"29",	"Spain"},
								{"1996",	"Oslo",			"Ireland",										"162",	"48",	"Norway"},
								{"1997",	"Dublin",		"United-Kingdom",								"227",	"70",	"Ireland"},
								{"1998",	"Birmingham",	"Israel",										"172",	"6",	"United-Kingdom"},
								{"1999",	"Jerusalem",	"Sweden",										"163",	"17",	"Iceland"},
								{"2000",	"Stockholm",	"Denmark",										"195",	"40",	"Russia"},
								{"2001",	"Copenhagen",	"Estonia",										"198",	"21",	"Denmark"},
								{"2002",	"Tallinn",		"Latvia",										"176",	"12",	"Malta"},
								{"2003",	"Riga",			"Turkey",										"167",	"2",	"Belgium"},
								{"2004",	"Istanbul",		"Ukraine",										"280",	"17",	"Serbia-And-Montenegro"},
								{"2005",	"Kiev",			"Greece",										"230",	"38",	"Malta"},
								{"2006",	"Athens",		"Finland",										"292",	"44",	"Russia"},
								{"2007",	"Helsinki",		"Serbia",										"268",	"33",	"Ukraine"},
								{"2008",	"Belgrade",		"Russia",										"272",	"42",	"Ukraine"},
								{"2009",	"Moscow",		"Norway",										"387",	"169",	"Iceland"},
								{"2010",	"Oslo",			"Germany",										"246",	"76",	"Turkey"},
								{"2011",	"Dusseldorf",	"Azerbaijan",									"221",	"32",	"Italy"},
								{"2012",	"Baku",			"Sweden",										"372",	"113",	"Russia"},
								{"2013",	"Malmo",		"Denmark",										"281",	"47",	"Azerbaijan"},
								{"2014",	"Copenhagen",	"Austria",										"290",	"52",	"Netherlands"},
								{"2015",	"Vienna",		"Sweden",										"365",	"62",	"Russia"},
								{"2016",	"Stockholm",	"Ukraine",										"534",	"23",	"Australia"},
								{"2017",	"Kiev",			"Portugal",										"758",	"143",	"Bulgaria"},
								{"2018",	"Lisbon",		"Israel",										"529",	"93",	"Cyprus"}};

#define LIST_LENGTH 			63

#define BUFFER_SIZE 			300
#define PORTNO 					8888

#define MAX_ARGUMENTS       	10
#define ARGS_DELIMETER      	" "

#define MAX_WINNERS       		10
#define WINNERS_DELIMETER     	", "
#define MAX_WINNER_NAME			100

#define COMMAND_WINNER			"winner"
#define COMMAND_VICTORIOUS		"victorious"
#define COMMAND_RUNNER_UP		"runner-up"

#define END_SESSION_MANUAL 		"exit\n"
#define END_SESSION_SCRIPT 		"exit"

#define ERROR_CODE_SUCCESS		0
#define ERROR_CODE_FAILURE		1

/************************************************************ Auxiliary functions */

int check_year_single(int year){
	if (year < 1956 || year > 2018){
		return ERROR_CODE_FAILURE;
	}
	return ERROR_CODE_SUCCESS;
}

int check_year_double(int year_from, int year_to){
	if (year_from > year_to){
		return ERROR_CODE_FAILURE;
	}else if (check_year_single(year_from) == ERROR_CODE_FAILURE){
		return ERROR_CODE_FAILURE;
	}else if (check_year_single(year_to) == ERROR_CODE_FAILURE){
		return ERROR_CODE_FAILURE;
	}
	return ERROR_CODE_SUCCESS;
}

void to_lower(const char* buffer_in, char* buffer_out)
{
	for(int i = 0; buffer_in[i]; i++)
	{
		buffer_out[i] = tolower(buffer_in[i]);
	}
}

void capitalize(char* buffer)
{
	buffer[0] = toupper(buffer[0]);
	int buffer_len = strlen(buffer);
	for(int i = 0; i < strlen(buffer); i++)
	{
		if(((buffer[i] == ' ') || (buffer[i] == '-')) && ((i + 1) < buffer_len))
		{
			buffer[i + 1] = toupper(buffer[i + 1]);
		}
	}
}

void print_winner(int year, char* buffer)
{
	int index = year - 1956;
	memset(buffer, 0, BUFFER_SIZE);

	switch(year)
	{
		case 1956:
			sprintf(buffer, "The %s Eurovision (hosted by %s) winner is %s.\n", list[index][0], list[index][1], list[index][2]);
			break;		
		case 1969:
			sprintf(buffer, "The %s Eurovision (hosted by %s) winners are %s with %s points.\n", list[index][0], list[index][1], list[index][2], list[index][3]);
			break;
		default:
			sprintf(buffer, "The %s Eurovision (hosted by %s) winner is %s with %s points. %s came second by a margin of %s points.\n", list[index][0], list[index][1], list[index][2], list[index][3], list[index][5], list[index][4]);
	}
}

int HowManyTimes(char* country, int from_year, int to_year, int column){
	int count = 0;
	int i = from_year - 1956;
	for (; i < to_year - 1956 + 1; i++){
		char winner[MAX_WINNER_NAME];
		strcpy(winner, list[i][column]);
        char *token = strtok(winner, WINNERS_DELIMETER);
        while (token != NULL) {
			if (strcmp(token, country) == 0){
				count++;
			}
            token = strtok(NULL, WINNERS_DELIMETER);
        }
	}
	return count;
}

void print_times_victorious(char* country, int from_year, int to_year, char* buffer)
{
	to_lower(country, country);
	capitalize(country);
	memset(buffer, 0, BUFFER_SIZE);
	
	int times = HowManyTimes(country, from_year, to_year, 2);

	sprintf(buffer, "%s has won the Eurovision %d %s between %d to %d.\n", country, times, times == 1 ? "time" : "times", from_year, to_year);
}

void print_times_runner_up(char* country, int from_year, int to_year, char* buffer)
{
	to_lower(country, country);
	capitalize(country);
	memset(buffer, 0, BUFFER_SIZE);

	int times = HowManyTimes(country, from_year, to_year, 5);

	sprintf(buffer, "%s has taken the Eurovision's second-place %d %s between %d to %d.\n", country, times, times == 1 ? "time" : "times", from_year, to_year);	
}

void print_error(char* buffer)
{
	memset(buffer, 0, BUFFER_SIZE);
	sprintf(buffer, "error\n");
}

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

/************************************************************************** Main */

int main(void)
{

	// Variables

    char query[BUFFER_SIZE];
    char result[BUFFER_SIZE];

	struct sockaddr_in serv_addr;
	struct sockaddr_in cli_addr;

	int sockfd;
	int newsockfd;

	int clilen;

	// Socket opening

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
		error("ERROR opening socket");

	// Socket cinfiguring

	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &(int){ 1 }, sizeof(int)) < 0)
		error("ERROR configuring socket");

	// Server Configuration

	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(PORTNO);

	// Listen

	if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
		error("ERROR on binding");

	listen(sockfd, 5);

	// Socket opening & connect

	clilen = sizeof(cli_addr);
	newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
	if (newsockfd < 0)
		error("ERROR on accept");

	printf("[Server] Connected\n");

	// Loop messages

	while (1){

		// Read from socket

		bzero(query, BUFFER_SIZE);
		if (full_read(newsockfd, query, BUFFER_SIZE) < 0)
			error("ERROR reading from socket");

		// Check if terminate session

		if (!strcmp(query, END_SESSION_MANUAL) || !strcmp(query, END_SESSION_SCRIPT)){
			break;
		}

		printf("%s", query);

        // Applying command

        if (query[strlen(query) - 1] == '\n'){
        	query[strlen(query) - 1] = 0;
        }

        // Extracting command and arguments from user input

    	char *args[MAX_ARGUMENTS];

        char *token = strtok(query, ARGS_DELIMETER);

        int argsIndex = 0;

        while (token != NULL) {
            args[argsIndex++] = token;
            token = strtok(NULL, ARGS_DELIMETER);
        }

		// Check if line ends with '\n'

		bzero(result, BUFFER_SIZE);

        if (strcmp(args[0], COMMAND_WINNER) == 0){

        	if (argsIndex != 2){
				print_error(result);
				if (full_write(newsockfd, result, BUFFER_SIZE) < 0)
					error("ERROR writing to socket");
    			continue;
        	}

			int year = atoi(args[1]);
			if (check_year_single(year) == ERROR_CODE_FAILURE){
        		print_error(result);
				if (full_write(newsockfd, result, BUFFER_SIZE) < 0)
					error("ERROR writing to socket");
    			continue;
			}

			print_winner(year, result);

        }else{

        	if (argsIndex != 4){
				print_error(result);
				if (full_write(newsockfd, result, BUFFER_SIZE) < 0)
					error("ERROR writing to socket");
    			continue;
        	}

	        if (strcmp(args[0], COMMAND_VICTORIOUS) == 0){
				int year_from = atoi(args[1]);
				int year_to = atoi(args[2]);
				if (check_year_double(year_from, year_to) == ERROR_CODE_FAILURE){
	        		print_error(result);
					if (full_write(newsockfd, result, BUFFER_SIZE) < 0)
						error("ERROR writing to socket");
	    			continue;
				}

				print_times_victorious(args[3], year_from, year_to, result);
	        }else if (strcmp(args[0], COMMAND_RUNNER_UP) == 0){
				int year_from = atoi(args[1]);
				int year_to = atoi(args[2]);
				if (check_year_double(year_from, year_to) == ERROR_CODE_FAILURE){
	        		print_error(result);
					if (full_write(newsockfd, result, BUFFER_SIZE) < 0)
							error("ERROR writing to socket");
		    			continue;
				}

				print_times_runner_up(args[3], year_from, year_to, result);
	        }else{
	        	print_error(result);
				if (full_write(newsockfd, result, BUFFER_SIZE) < 0)
					error("ERROR writing to socket");
	        }

    	}

		// Write to socket

		if (full_write(newsockfd, result, BUFFER_SIZE) < 0)
			error("ERROR writing to socket");

	}

	// Close connection

	close(sockfd);
	close(newsockfd);

	return 0;

}
