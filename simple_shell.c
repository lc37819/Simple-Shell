/*
author: Elton Gbollie
email: lc37819@umbc.edu
description: a simple linux shell designed to perform basic linux commands
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <ctype.h>
#include "utils.h"

// DEFINE THE FUNCTION PROTOTYPES
void user_prompt_loop(void);
char *get_user_command(void);

int main(int argc, char **argv) {

    if (argc > 1) {
        fprintf(stderr, "Error: Command-line arguments are not allowed.\n");
        return 1;
    }

    user_prompt_loop();

    return 0;
}

/*Prompt the user for a new command */
void user_prompt_loop(void) {
    char *command;
    
    while(1) {
        printf(">> ");
        command = get_user_command();
        size_t length = strlen(command);

        if (length > 0 && command[length - 1] == '\n') {
            command[length - 1] = '\0';
        }

        if (strcmp(command, "exit") == 0) {
            free(command);
            exit(0);
        }

        free(command);
    }
}


/* Take input of arbitrary length from the user and returns it to user_prompt_loop() */
char *get_user_command(void) {
    char *command = NULL;
    int buffer_size = 100;
    int length = 0;
    char temp;

    command = (char *)malloc(buffer_size * sizeof(char));
    if (!command) {
        fprintf(stderr, "Error: Memory allocation failed.\n");
        exit(1);
    }

    while(1) {
        temp = getchar();

        if (length >= buffer_size -1) {
            buffer_size *= 2;
            command = realloc(command, buffer_size);
            if (!command) {
                fprintf(stderr, "Error: Memory reallocation failed.\n");
                exit(1);
            }
        }

        if (temp == EOF || temp == '\n') {
            command[length] = '\0';
            return command;
        }

        command[length++] = temp;
    }
}