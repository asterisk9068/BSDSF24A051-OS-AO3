#ifndef SHELL_H
#define SHELL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_LINE 1024
#define MAX_ARGS 64
#define PROMPT "myshell> "

/**
 * read_cmd - Displays the prompt and reads a command line from the input stream.
 * @prompt: The prompt string to display.
 * @fp: The input stream (e.g., stdin).
 *
 * Return: Dynamically allocated line buffer, or NULL on EOF/error.
 */
char *read_cmd(const char *prompt, FILE *fp);

/**
 * tokenize - Parses a command line into an array of argument tokens.
 * @cmdline: The raw input string.
 *
 * Return: Dynamically allocated NULL-terminated array of strings.
 */
char **tokenize(char *cmdline);

/**
 * execute - Executes an external command using the fork-exec-wait cycle.
 * @arglist: NULL-terminated array of arguments (arglist[0] is program name).
 *
 * Return: Status code from waitpid, or -1 on error.
 */
int execute(char **arglist);

/**
 * free_tokens - Deallocates memory used by the token array.
 * @tokens: NULL-terminated array of tokens to free.
 */
void free_tokens(char **tokens);

/**
 * handle_builtin - Checks if command is a shell built-in and executes it.
 * @arglist: NULL-terminated array of arguments (arglist[0] is command name).
 *
 * Return: 1 if command was a built-in and handled, 0 otherwise.
 */
int handle_builtin(char **arglist);

#endif /* SHELL_H */
