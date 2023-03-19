/* Main program of the customized UNIX Shell
 * By Fadi EID 19/3/2023
 * Open Source, free to use
 * https://github.com/Fadi-Eid */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>

#define MAX_LINE 80        // maximum number of characters acceptable
#define ERROR_CODE 7

int parseCommand(char command[], int commad_size, char* args[], int* args_size);

int main(void)
{
    char* args[MAX_LINE/2 + 1]; // each part of the command only hold 41 chars max
    int should_run = 1;         // set to 1 to exit

    while(should_run)
    {
        printf("Shell >     ");
        fflush(stdout);
        fflush(stdin);

        // get the user input and parse it
        char command[MAX_LINE];
        fgets(command, MAX_LINE, stdin);

        // Check if the length of the input string exceeds the maximum size
        if (strlen(command) == MAX_LINE - 1 && command[MAX_LINE - 2] != '\n') {
            printf("Error: Command too long. Maximum length is %d characters.\n", MAX_LINE - 1);
            // Read and discard any remaining input
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
            continue;
        }

        int NParams;      // variable to store the number of parameters
       if(parseCommand(command, strlen(command), args, &NParams) == -1 || NParams == 0)
        {
            continue;
        }

        if(strcmp(args[0], "exit") == 0)
        {
            printf("Shell released\n\n");
            break;
        }

         if(strcmp(args[0], "clear") == 0)
         {
            system("clear");
            continue;
         }

         if(strcmp(args[0], "help") == 0)
         {
            // print the help menu here
            continue;
         }
        
         int conc = 0;

         if(strcmp(args[NParams-1], "&") == 0) // run concurently (parent will not wait for child)
        {
            args[NParams-1] = NULL;
            conc = 1;
        }


        // fork a child process to execute the command
        int pid = fork();
        

        if(pid == 0)
        {
            // Child process that executes the command
            execvp(args[0], args);

        } else
        {
            // parent process
            if(conc==0)
                waitpid(pid, NULL, 0);
        }



        // free dynamically allocated memory
        for(int i=0; i<NParams; i++)
        {
            free(args[i]);
        }
    }

    return 0;
}




// function that parses a command
int parseCommand(char command[], int command_size, char* args[], int* args_size)
{
    if (command_size == 0) { // Check if command is empty
        return -1;
    }

    if(command[0] == '\n')
    {
        return -1;
    }


    int arg_index = 0;
    int i = 0;
    int start_index = 0;
    int end_index = 0;

    while (i < command_size) {
        if (command[i] == ' ' || command[i] == '\t' || command[i] == '\n') {
            if (end_index > start_index) {
                // Allocate memory for the argument and copy the substring
                args[arg_index] = malloc(end_index - start_index + 1);
                if (args[arg_index] == NULL) {
                    // Error handling: Unable to allocate memory
                    for (int j = 0; j < arg_index; j++) {
                        free(args[j]);
                    }
                    return -1;
                }
                strncpy(args[arg_index], command + start_index, end_index - start_index);
                args[arg_index][end_index - start_index] = '\0';
                arg_index++;
                start_index = end_index + 1;
            } else {
                start_index++;
            }
        }
        end_index++;
        i++;
    }

    // Add the last argument
    if (end_index > start_index) {
        args[arg_index] = malloc(end_index - start_index + 1);
        if (args[arg_index] == NULL) {
            // Error handling: Unable to allocate memory
            for (int j = 0; j < arg_index; j++) {
                free(args[j]);
            }
            return -1;
        }
        strncpy(args[arg_index], command + start_index, end_index - start_index);
        args[arg_index][end_index - start_index] = '\0';
        if (args[arg_index][strlen(args[arg_index]) - 1] == '\n') {
            args[arg_index][strlen(args[arg_index]) - 1] = '\0';
        }
        arg_index++;
    }

    int j;
    *args_size = 0;
    j = 0;

    for (i = 0; i < command_size; i++) {
        if (command[i] == ' ' || command[i] == '\n' || command[i] == '\r' || command[i] == '\t') {
            if (j != 0) {
                args[*args_size][j] = '\0';
                (*args_size)++;
                j = 0;
            }
        } else {
            args[*args_size][j] = command[i];
            j++;
        }
    }

    if (j != 0) {
        args[*args_size][j] = '\0';
        (*args_size)++;
    }

    args[*args_size] = NULL; // Set the last element to NULL to terminate the array

    return 1;
}