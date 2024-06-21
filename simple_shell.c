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
    char command[100];

    while(1) {
        printf(">> ");
        fgets(command, 100, stdin);
        size_t length = strlen(command);

        if (length > 0 && command[length - 1] == '\n') {
            command[length - 1] = '\0';
        }

        if (strcmp(command, "exit") == 0) {
            exit(0);
        }
    }
}