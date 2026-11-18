#define _POSIX_C_SOURCE 200809L
#include "history.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int id;
    char *cmd;
} HistoryEntry;

static HistoryEntry history_entries[MAX_HISTORY];
static int history_count = 0;
static int total_commands_logged = 0;

void sh_history_init(void) {
    history_count = 0;
    total_commands_logged = 0;
    for (int i = 0; i < MAX_HISTORY; i++) {
        history_entries[i].id = 0;
        history_entries[i].cmd = NULL;
    }
}

void sh_history_add(const char *cmd) {
    if (cmd == NULL || *cmd == '\0') {
        return;
    }

    total_commands_logged++;

    if (history_count < MAX_HISTORY) {
        history_entries[history_count].id = total_commands_logged;
        history_entries[history_count].cmd = strdup(cmd);
        history_count++;
    } else {
        // Buffer full: drop oldest and shift
        free(history_entries[0].cmd);
        for (int i = 1; i < MAX_HISTORY; i++) {
            history_entries[i - 1] = history_entries[i];
        }
        history_entries[MAX_HISTORY - 1].id = total_commands_logged;
        history_entries[MAX_HISTORY - 1].cmd = strdup(cmd);
    }
}

void sh_history_print(void) {
    for (int i = 0; i < history_count; i++) {
        printf("%4d  %s\n", history_entries[i].id, history_entries[i].cmd);
    }
}

const char *sh_history_get(int n) {
    for (int i = 0; i < history_count; i++) {
        if (history_entries[i].id == n) {
            return history_entries[i].cmd;
        }
    }
    return NULL;
}

const char *sh_history_get_last(void) {
    if (history_count > 0) {
        return history_entries[history_count - 1].cmd;
    }
    return NULL;
}

void sh_history_free(void) {
    for (int i = 0; i < history_count; i++) {
        if (history_entries[i].cmd != NULL) {
            free(history_entries[i].cmd);
            history_entries[i].cmd = NULL;
        }
    }
    history_count = 0;
}
