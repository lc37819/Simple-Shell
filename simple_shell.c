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
int parse_integer(const char *str, int *output);
void exit_command(char **parsed_command, char *command);
void proc_command(char **parsed_command);
void execute_command(char **parsed_command, char *command);
int is_blank(const char *command);
void save_command(char *command);
void history_command(void);

int main(int argc, char **argv) {

    (void)argv;

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

        if (strcmp(command, "history") != 0) save_command(command);

        parsed_command = parse_command(command);

        if (parsed_command[0]) {

            if (strcmp(parsed_command[0], "proc") == 0) {
                proc_command(parsed_command);
            }

            else if (strcmp(parsed_command[0], "exit") == 0) {
                exit_command(parsed_command, command);
            }

            else if (strcmp(parsed_command[0], "history") == 0) {
                history_command();
            }

            else {
                execute_command(parsed_command, command);
            }
        }

        for (int i = 0; parsed_command[i]; i++) {
            free(parsed_command[i]);
        }

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

int parse_integer(const char *str, int *output) {
    char *end;
    long value = strtol(str, &end, 10);

    if (end == str || *end != '\0') {
        return -1;
    }

    *output = (int)value;
    return 0;
}

void exit_command(char **parsed_command, char *command) {
    int arg_count = 0;
    for (int i = 1; parsed_command[i]; i++) {
        arg_count++;
    }

    if (arg_count > 1) {
        fprintf(stderr, "Error: Invalid number of arguments.\n");
        return;
    }

    if (parsed_command[1]) {
        int exit_status;

        if (parse_integer(parsed_command[1], &exit_status) == 0) {
            for (int i = 0; parsed_command[i]; i++) {
                free(parsed_command[i]);
            }

            free(command);
            free(parsed_command);
            exit(exit_status);
        }
        else {
            fprintf(stderr, "Error: Invalid exit status '%s'.\n", parsed_command[1]);
        }
    }
    else {
        for (int i = 0; parsed_command[i]; i++) {
            free(parsed_command[i]);
        }

        free(command);
        free(parsed_command);
        exit(0);
    }
}

void proc_command(char **parsed_command) {
    if (!parsed_command[1] || parsed_command[2]) {
        fprintf(stderr, "Error: Invalid number of arguments.\n");
        return;
    }

    char *filepath = parsed_command[1];

    char fullpath[512] = "/proc/";
    strncat(fullpath, filepath, sizeof(fullpath) - strlen(fullpath) - 1);

    FILE *file = fopen(fullpath, "r");
    if (!file) {
        perror("Error opening file");
        return;
    }

    char *line = NULL;
    size_t size = 0;
    ssize_t read;
    while ((read = getline(&line, &size, file)) != -1) {
        printf("%s", line);
    }

    free(line);
    fclose(file);
}

void execute_command(char **parsed_command, char *command) {
    pid_t pid;
    int status;

    pid = fork();

    if (pid == 0) {
        if (execvp(parsed_command[0], parsed_command) == -1) {
            for (int i = 0; parsed_command[i]; i++) {
                free(parsed_command[i]);
            }
            free(parsed_command);
            free(command);
            exit(1);
        }
    }
    else if (pid < 0) {
        perror("Error: Forking error occured.\n");
    }
    else {
        waitpid(pid, &status, 0);
    }
}

int is_blank(const char *command) {
    for (int i = 0; command[i] != '\0'; i++) {
        if (!isspace((unsigned char)command[i])) {
            return 0;
        }
    }
    return 1;
}

void save_command(char *command) {
    if (is_blank(command)) {
        return;
    }

    char *home_directory = getenv("HOME");
    char file_path[1024];

    snprintf(file_path, sizeof(file_path), "%s/.421sh", home_directory);

    FILE *file = fopen(file_path, "a");
    if (!file) {
        perror("Error opening file");
        return;
    }

    fprintf(file, "%s\n", command);
    fclose(file);
}

void history_command(void) {
    char *home_directory = getenv("HOME");
    char file_path[1024];
    char *commands[10] = {NULL};
    int count = 0;
    char *line = NULL;
    size_t len = 0;

    snprintf(file_path, sizeof(file_path), "%s/.421sh", home_directory);

    FILE *file = fopen(file_path, "r");
    if (!file) {
        perror("Error opening file");
        return;
    }

    while (getline(&line, &len, file) != -1) {
        if (count >= 10) {
            free(commands[count % 10]);
        }
        commands[count % 10] = strdup(line);
        count++;
    }

    int start = count < 10 ? 0 : count - 10;
    for (int i = start; i < count; i++) {
        printf("%s", commands[i % 10]);
        free(commands[i % 10]);
    }

    free(line);
    fclose(file);
}
