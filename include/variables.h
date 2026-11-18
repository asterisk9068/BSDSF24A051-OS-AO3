#ifndef VARIABLES_H
#define VARIABLES_H

/**
 * var_init - Initializes the variable storage linked list.
 */
void var_init(void);

/**
 * var_set - Sets or updates a shell variable.
 * @name: Variable name.
 * @value: Variable value.
 *
 * Return: 0 on success, -1 on invalid name.
 */
int var_set(const char *name, const char *value);

/**
 * var_get - Retrieves value of a shell variable (or environment).
 * @name: Variable name.
 *
 * Return: Pointer to value string, or NULL if unset.
 */
const char *var_get(const char *name);

/**
 * var_print_all - Prints all shell variables in NAME=value format (for 'set').
 */
void var_print_all(void);

/**
 * is_assignment_statement - Checks if a command string is a variable assignment.
 * @cmd: The command string.
 *
 * Return: 1 if assignment statement (e.g. VAR=val), 0 otherwise.
 */
int is_assignment_statement(const char *cmd);

/**
 * handle_assignment - Parses and stores a variable assignment.
 * @cmd: The assignment string (e.g. VAR=val).
 *
 * Return: 0 on success, -1 on error.
 */
int handle_assignment(const char *cmd);

/**
 * var_expand_string - Replaces $VAR references with variable values.
 * @input: The string to expand.
 *
 * Return: Dynamically allocated expanded string. Caller must free.
 */
char *var_expand_string(const char *input);

/**
 * var_free_all - Deallocates all variable storage memory.
 */
void var_free_all(void);

#endif /* VARIABLES_H */
