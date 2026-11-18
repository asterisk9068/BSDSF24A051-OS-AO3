#define _POSIX_C_SOURCE 200809L
#include "shell.h"
#include <ctype.h>
#include <readline/readline.h>
#include <readline/history.h>

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

static char *command_generator(const char *text, int state) {
    static int list_index, len;
    static const char *builtins[] = {
        "cd", "pwd", "history", "help", "jobs", "exit", NULL
    };

    if (!state) {
        list_index = 0;
        len = strlen(text);
    }

    const char *name;
    while ((name = builtins[list_index++]) != NULL) {
        if (strncmp(name, text, len) == 0) {
            return strdup(name);
        }
    }
    return NULL;
}

static char **myshell_completion(const char *text, int start, int end) {
    (void)end;
    char **matches = NULL;
    if (start == 0) {
        matches = rl_completion_matches(text, command_generator);
    }
    // If start > 0 or no builtin matched, returning NULL falls back to filename completion
    return matches;
}

int main(int argc, char **argv) {
    (void)argc;
    (void)argv;

    char *cmdline = NULL;

    sh_history_init();
    jobs_init();

    // Configure readline completion
    rl_attempted_completion_function = myshell_completion;

    while (1) {
        jobs_reap();

        const char *prompt = isatty(STDIN_FILENO) ? PROMPT : "";
        cmdline = readline(prompt);
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
                prev_cmd = sh_history_get_last();
            } else {
                int n = atoi(cmdline + 1);
                if (n > 0) {
                    prev_cmd = sh_history_get(n);
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

        // Add to both readline history (for arrow keys) and custom history (for history builtin / !n)
        add_history(cmdline);
        sh_history_add(cmdline);

        if (is_if_command(cmdline)) {
            execute_if_block(cmdline);
        } else {
            execute_line(cmdline);
        }

        free(cmdline);
        cmdline = NULL;
    }

    jobs_free();
    sh_history_free();

    return 0;
}
