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
char **parse_command(const char *command);
char *next_segment(const char **command);

int main(int argc, char **argv) {

    if (argc > 1) {
        fprintf(stderr, "Error: Command-line arguments are not allowed.\n");
        return 1;
    }

    user_prompt_loop();

    return 0;
}

/* Prompt the user for a new command */
void user_prompt_loop(void) {
    char *command;
    char **parsed_command;

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

        parsed_command = parse_command(command);

        if (!parsed_command) {
            fprintf(stderr, "Error: Command could not be parsed\n");
            free(command);
            continue;
        }

        /* Used to test if the command is being read correctly */
        printf("user input: %s\n", command);

        /* Used to test if the command is being read and parsed */
        printf("parsed output: ");
        for (int i = 0; parsed_command[i]; i++) {
            printf("[%s] ", parsed_command[i]);
            free(parsed_command[i]);
        }
        printf("\n");

        free(parsed_command);
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

char **parse_command(const char *command) {
    int max_args = 10;
    int args_count = 0;
    char **args = (char **)malloc(max_args * sizeof(char *));

    if (!args) {
        fprintf(stderr, "Error: Memory allocation failed.\n");
        exit(1);
    }

    while (*command) {
        while (isspace(*command)) {
            command++;
        }

        if (*command == '\0') {
            break;
        }

        if (args_count == max_args -1) {
            max_args *= 2;
            args = (char **)realloc(args, max_args * sizeof(char *));
            if (!args) {
                fprintf(stderr, "Error: Memory reallocation failed.\n");
                exit(1);
            }
        }

        args[args_count++] = next_segment(&command);
    }

    args[args_count] = NULL;
    return args;
}

/* Helper Function for parse_command
   Extracts the next segment of the command string and returns it.
   Also updates the command pointer to point to the next segment.
*/
char *next_segment(const char **command) {
    int next_space = first_unquoted_space(*command);
    char *segment;

    if (next_space == -1) {
        segment = unescape(*command, stderr);
        *command += strlen(*command);
    }
    else {
        char *temp = strndup(*command, next_space);
        segment = unescape(temp, stderr);
        free(temp);
        *command += next_space;
    }
    return segment;
}