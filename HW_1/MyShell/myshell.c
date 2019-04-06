
/******************************************************** Includes */

#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

/******************************************************* Constants */

#define BUFFER_SIZE         100
#define MAX_ARGUMENTS       10
#define ARGS_DELIMETER      " "

#define RUN_IN_BACKGROUND   "&"

#define STRING_END          '\0'

/************************************************************ Main */

int main(void)
{

    // redirection of errors to output

    close(2);
    dup(1);

    // Variables

    char command[BUFFER_SIZE];

    char *args[MAX_ARGUMENTS];

    int currentPid = (int) getpid();

    // Read input from user until 'exit' command is entered
    
    while (1)
    {

        // Read input from user and check if 'exit' command is entered

        fprintf(stdout, "my-shell> ");
        memset(command, 0, BUFFER_SIZE);
        fgets(command, BUFFER_SIZE, stdin);
        if(strncmp(command, "exit", 4) == 0)
        {
            break;
        }

        // Extracting arguments from user input

        command[(int) strlen(command)-1] = STRING_END;

        char *token = strtok(command, ARGS_DELIMETER);

        int argsIndex = 0;

        while (token != NULL) {
            args[argsIndex++] = token;
            token = strtok(NULL, ARGS_DELIMETER);
        }

        // Checking if ends with &

        int waitflag = 1;

        if (argsIndex>0 && strcmp(args[argsIndex-1], RUN_IN_BACKGROUND) == 0){
            waitflag = 0;
            argsIndex--;
        }

        args[argsIndex] = NULL;

        // Run command

        pid_t pid = fork();

        if (getpid() != currentPid){

            execvp(args[0], args);

        }else{

            // Wait for son's process to finish if '&' wasn't entered

            if (waitflag){

                waitpid(pid, NULL, 0);

            }

        }

    }

    return 0;

}
