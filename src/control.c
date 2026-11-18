#define _POSIX_C_SOURCE 200809L
#include "control.h"
#include "shell.h"
#include <ctype.h>
#include <readline/readline.h>
#include <readline/history.h>

static char *trim_whitespace(char *str) {
    if (!str) return NULL;
    while (*str && isspace((unsigned char)*str)) str++;
    if (!*str) return str;
    char *end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) {
        *end = '\0';
        end--;
    }
    return str;
}

int is_if_command(const char *cmdline) {
    if (!cmdline) return 0;
    while (*cmdline && isspace((unsigned char)*cmdline)) cmdline++;
    if (strncmp(cmdline, "if", 2) == 0 && (isspace((unsigned char)cmdline[2]) || cmdline[2] == '\0')) {
        return 1;
    }
    return 0;
}

int execute_if_block(char *initial_line) {
    char *cond_cmd = NULL;
    char *then_cmds[MAX_ARGS];
    int then_count = 0;
    char *else_cmds[MAX_ARGS];
    int else_count = 0;

    char *line = trim_whitespace(initial_line);
    if (strncmp(line, "if", 2) == 0) {
        line += 2;
        while (*line && isspace((unsigned char)*line)) line++;
    }

    // Check if 'then' is on the same line
    char *then_ptr = strstr(line, "then");
    if (then_ptr && (then_ptr == line || isspace((unsigned char)*(then_ptr - 1)) || *(then_ptr - 1) == ';')) {
        // Cut condition before 'then'
        char *cond_part = line;
        char *end_cond = then_ptr - 1;
        while (end_cond > cond_part && (*end_cond == ';' || isspace((unsigned char)*end_cond))) {
            *end_cond = '\0';
            end_cond--;
        }
        *then_ptr = '\0';
        cond_cmd = strdup(trim_whitespace(cond_part));

        // Commands after 'then'
        char *after_then = then_ptr + 4;
        while (*after_then && isspace((unsigned char)*after_then)) after_then++;
        if (*after_then) {
            then_cmds[then_count++] = strdup(after_then);
        }
    } else {
        cond_cmd = strdup(line);
    }

    enum { STATE_THEN_WAIT, STATE_IN_THEN, STATE_IN_ELSE, STATE_DONE } state;
    state = (then_ptr != NULL) ? STATE_IN_THEN : STATE_THEN_WAIT;

    while (state != STATE_DONE) {
        const char *prompt = isatty(STDIN_FILENO) ? "> " : "";
        char *subline = readline(prompt);
        if (subline == NULL) {
            // EOF reached unexpectedly
            break;
        }

        // Add to history
        if (*subline) {
            add_history(subline);
            sh_history_add(subline);
        }

        char *trimmed = trim_whitespace(subline);
        if (strcmp(trimmed, "then") == 0) {
            state = STATE_IN_THEN;
        } else if (strncmp(trimmed, "then ", 5) == 0) {
            state = STATE_IN_THEN;
            then_cmds[then_count++] = strdup(trim_whitespace(trimmed + 5));
        } else if (strcmp(trimmed, "else") == 0) {
            state = STATE_IN_ELSE;
        } else if (strncmp(trimmed, "else ", 5) == 0) {
            state = STATE_IN_ELSE;
            else_cmds[else_count++] = strdup(trim_whitespace(trimmed + 5));
        } else if (strcmp(trimmed, "fi") == 0) {
            state = STATE_DONE;
        } else {
            if (state == STATE_IN_THEN) {
                if (then_count < MAX_ARGS - 1) {
                    then_cmds[then_count++] = strdup(trimmed);
                }
            } else if (state == STATE_IN_ELSE) {
                if (else_count < MAX_ARGS - 1) {
                    else_cmds[else_count++] = strdup(trimmed);
                }
            }
        }
        free(subline);
    }

    int result = 0;
    if (cond_cmd && *cond_cmd) {
        int cond_exit = execute_line(cond_cmd);
        if (cond_exit == 0) {
            for (int i = 0; i < then_count; i++) {
                result = execute_line(then_cmds[i]);
            }
        } else {
            for (int i = 0; i < else_count; i++) {
                result = execute_line(else_cmds[i]);
            }
        }
    }

    if (cond_cmd) free(cond_cmd);
    for (int i = 0; i < then_count; i++) free(then_cmds[i]);
    for (int i = 0; i < else_count; i++) free(else_cmds[i]);

    return result;
}
