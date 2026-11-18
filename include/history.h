#ifndef HISTORY_H
#define HISTORY_H

#define MAX_HISTORY 100

/**
 * history_init - Initializes the command history storage.
 */
void history_init(void);

/**
 * history_add - Adds a command line to history.
 * @cmd: The command line string to add.
 */
void history_add(const char *cmd);

/**
 * history_print - Displays the history with 1-based line numbers.
 */
void history_print(void);

/**
 * history_get - Retrieves the n-th command from history.
 * @n: 1-based command number.
 *
 * Return: Pointer to command string, or NULL if not found.
 */
const char *history_get(int n);

/**
 * history_get_last - Retrieves the most recent command from history.
 *
 * Return: Pointer to command string, or NULL if history is empty.
 */
const char *history_get_last(void);

/**
 * history_free - Deallocates all history storage memory.
 */
void history_free(void);

#endif /* HISTORY_H */
