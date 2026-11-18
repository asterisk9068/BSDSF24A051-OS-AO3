#ifndef CONTROL_H
#define CONTROL_H

/**
 * is_if_command - Checks if a command line begins with an 'if' control structure.
 * @cmdline: The raw command line.
 *
 * Return: 1 if it starts with 'if', 0 otherwise.
 */
int is_if_command(const char *cmdline);

/**
 * execute_if_block - Reads and executes an if-then-else-fi control structure.
 * @initial_line: The first line that started with 'if'.
 *
 * Return: Exit code of the executed block or condition.
 */
int execute_if_block(char *initial_line);

#endif /* CONTROL_H */
