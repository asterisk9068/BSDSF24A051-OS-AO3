#define _POSIX_C_SOURCE 200809L
#include "shell.h"
#include <errno.h>

/**
 * builtin_cd - Changes current working directory.
 * @arglist: Arguments array. arglist[1] is optional target directory.
 *
 * Return: 1 on completion.
 */
static int builtin_cd(char **arglist) {
    const char *target = arglist[1];

    if (target == NULL || strcmp(target, "~") == 0) {
        target = getenv("HOME");
        if (target == NULL) {
            fprintf(stderr, "myshell: cd: HOME not set\n");
            return 1;
        }
    } else if (arglist[2] != NULL) {
        fprintf(stderr, "myshell: cd: too many arguments\n");
        return 1;
    }

    if (chdir(target) != 0) {
        perror("cd");
        return 1;
    }

    return 1;
}

/**
 * builtin_pwd - Prints current working directory.
 * @arglist: Arguments array (unused).
 *
 * Return: 1 on completion.
 */
static int builtin_pwd(char **arglist) {
    (void)arglist;
    char *cwd = getcwd(NULL, 0);
    if (cwd != NULL) {
        printf("%s\n", cwd);
        free(cwd);
    } else {
        perror("pwd");
    }
    return 1;
}

/**
 * builtin_exit - Terminates the shell gracefully.
 * @arglist: Arguments array. arglist[1] is optional exit status.
 *
 * Return: Does not return if exit succeeds.
 */
static int builtin_exit(char **arglist) {
    int status = 0;
    if (arglist[1] != NULL) {
        status = atoi(arglist[1]);
    }
    exit(status);
}

/**
 * builtin_help - Displays help information about built-in commands.
 * @arglist: Arguments array (unused).
 *
 * Return: 1 on completion.
 */
static int builtin_help(char **arglist) {
    (void)arglist;
    printf("myshell - supported built-in commands:\n");
    printf("  cd [dir]    Change the current working directory.\n");
    printf("  pwd         Print current working directory.\n");
    printf("  history     Display command history list.\n");
    printf("  help        Display this help message.\n");
    printf("  jobs        Display active jobs (placeholder).\n");
    printf("  exit [code] Exit the shell.\n");
    return 1;
}

/**
 * builtin_history - Displays command history.
 * @arglist: Arguments array (unused).
 *
 * Return: 1 on completion.
 */
static int builtin_history(char **arglist) {
    (void)arglist;
    history_print();
    return 1;
}

/**
 * builtin_jobs - Placeholder for job control.
 * @arglist: Arguments array (unused).
 *
 * Return: 1 on completion.
 */
static int builtin_jobs(char **arglist) {
    (void)arglist;
    printf("Job control not yet implemented.\n");
    return 1;
}

int handle_builtin(char **arglist) {
    if (arglist == NULL || arglist[0] == NULL) {
        return 0;
    }

    if (strcmp(arglist[0], "exit") == 0) {
        return builtin_exit(arglist);
    } else if (strcmp(arglist[0], "cd") == 0) {
        return builtin_cd(arglist);
    } else if (strcmp(arglist[0], "pwd") == 0) {
        return builtin_pwd(arglist);
    } else if (strcmp(arglist[0], "history") == 0) {
        return builtin_history(arglist);
    } else if (strcmp(arglist[0], "help") == 0) {
        return builtin_help(arglist);
    } else if (strcmp(arglist[0], "jobs") == 0) {
        return builtin_jobs(arglist);
    }

    return 0;
}
