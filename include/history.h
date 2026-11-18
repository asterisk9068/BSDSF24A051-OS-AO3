#ifndef HISTORY_H
#define HISTORY_H

#define MAX_HISTORY 100

/**
 * sh_history_init - Initializes the command history storage.
 */
void sh_history_init(void);

/**
 * sh_history_add - Adds a command line to history.
 * @cmd: The command line string to add.
 */
void sh_history_add(const char *cmd);

/**
 * sh_history_print - Displays the history with 1-based line numbers.
 */
void sh_history_print(void);

/**
 * sh_history_get - Retrieves the n-th command from history.
 * @n: 1-based command number.
 *
 * Return: Pointer to command string, or NULL if not found.
 */
const char *sh_history_get(int n);

/**
 * sh_history_get_last - Retrieves the most recent command from history.
 *
 * Return: Pointer to command string, or NULL if history is empty.
 */
const char *sh_history_get_last(void);

/**
 * sh_history_free - Deallocates all history storage memory.
 */
void sh_history_free(void);

#endif /* HISTORY_H */
