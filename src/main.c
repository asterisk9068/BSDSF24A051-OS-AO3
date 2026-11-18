#define _POSIX_C_SOURCE 200809L
#include "shell.h"
#include <ctype.h>

static int is_empty_line(const char *line) {
    if (line == NULL) return 1;
    while (*line) {
        if (!isspace((unsigned char)*line)) {
            return 0;
        }
        line++;
    }
    return 1;
}

int main(int argc, char **argv) {
    (void)argc;
    (void)argv;

    char *cmdline = NULL;
    char **arglist = NULL;

    history_init();

    while (1) {
        cmdline = read_cmd(PROMPT, stdin);
        if (cmdline == NULL) {
            // EOF reached (Ctrl+D)
            if (isatty(STDIN_FILENO)) {
                printf("\n");
            }
            break;
        }

        if (is_empty_line(cmdline)) {
            free(cmdline);
            cmdline = NULL;
            continue;
        }

        // Handle !n or !! history re-execution before tokenization
        if (cmdline[0] == '!') {
            const char *prev_cmd = NULL;
            if (cmdline[1] == '!') {
                prev_cmd = history_get_last();
            } else {
                int n = atoi(cmdline + 1);
                if (n > 0) {
                    prev_cmd = history_get(n);
                }
            }

            if (prev_cmd == NULL) {
                fprintf(stderr, "myshell: %s: event not found\n", cmdline);
                free(cmdline);
                cmdline = NULL;
                continue;
            }

            printf("%s\n", prev_cmd);
            char *expanded = strdup(prev_cmd);
            free(cmdline);
            cmdline = expanded;
        }

        // Add command to history
        history_add(cmdline);

        arglist = tokenize(cmdline);
        if (arglist != NULL && arglist[0] != NULL) {
            if (!handle_builtin(arglist)) {
                execute(arglist);
            }
        }

        if (arglist != NULL) {
            free_tokens(arglist);
            arglist = NULL;
        }

        free(cmdline);
        cmdline = NULL;
    }

    history_free();

    return 0;
}
