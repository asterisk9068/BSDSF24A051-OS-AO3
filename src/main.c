#define _POSIX_C_SOURCE 200809L
#include "shell.h"

int main(int argc, char **argv) {
    (void)argc;
    (void)argv;

    char *cmdline = NULL;
    char **arglist = NULL;

    while (1) {
        cmdline = read_cmd(PROMPT, stdin);
        if (cmdline == NULL) {
            // EOF reached (Ctrl+D)
            if (isatty(STDIN_FILENO)) {
                printf("\n");
            }
            break;
        }

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

    return 0;
}
