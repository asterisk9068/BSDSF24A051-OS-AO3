#define _POSIX_C_SOURCE 200809L
#include "shell.h"

int execute(char **arglist) {
    if (arglist == NULL || arglist[0] == NULL) {
        return 0;
    }

    pid_t pid = fork();

    if (pid < 0) {
        perror("myshell: fork failed");
        return -1;
    } else if (pid == 0) {
        // Child process: execute external command
        if (execvp(arglist[0], arglist) == -1) {
            perror(arglist[0]);
            exit(EXIT_FAILURE);
        }
    } else {
        // Parent process: wait for child process to terminate
        int status;
        if (waitpid(pid, &status, 0) == -1) {
            perror("myshell: waitpid failed");
            return -1;
        }
        return status;
    }

    return 0;
}
