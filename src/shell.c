#define _POSIX_C_SOURCE 200809L
#include "shell.h"

char *read_cmd(const char *prompt, FILE *fp) {
    if (prompt != NULL) {
        printf("%s", prompt);
        fflush(stdout);
    }

    char *line = NULL;
    size_t bufsize = 0;
    ssize_t chars_read = getline(&line, &bufsize, fp);

    if (chars_read == -1) {
        free(line);
        return NULL;
    }

    // Strip trailing newline character if present
    if (chars_read > 0 && line[chars_read - 1] == '\n') {
        line[chars_read - 1] = '\0';
    }

    return line;
}

char **tokenize(char *cmdline) {
    if (cmdline == NULL) {
        return NULL;
    }

    int bufsize = MAX_ARGS;
    char **tokens = malloc(bufsize * sizeof(char *));
    if (!tokens) {
        perror("myshell: allocation error");
        exit(EXIT_FAILURE);
    }

    char *cmd_copy = strdup(cmdline);
    if (!cmd_copy) {
        perror("myshell: allocation error");
        free(tokens);
        return NULL;
    }

    int position = 0;
    char *token = strtok(cmd_copy, " \t\r\n\a");
    while (token != NULL) {
        tokens[position] = strdup(token);
        if (!tokens[position]) {
            perror("myshell: allocation error");
            free(cmd_copy);
            free_tokens(tokens);
            exit(EXIT_FAILURE);
        }
        position++;

        if (position >= bufsize - 1) {
            bufsize *= 2;
            tokens = realloc(tokens, bufsize * sizeof(char *));
            if (!tokens) {
                perror("myshell: allocation error");
                free(cmd_copy);
                exit(EXIT_FAILURE);
            }
        }

        token = strtok(NULL, " \t\r\n\a");
    }
    tokens[position] = NULL;
    free(cmd_copy);

    return tokens;
}

void free_tokens(char **tokens) {
    if (tokens == NULL) {
        return;
    }

    for (int i = 0; tokens[i] != NULL; i++) {
        free(tokens[i]);
    }
    free(tokens);
}
