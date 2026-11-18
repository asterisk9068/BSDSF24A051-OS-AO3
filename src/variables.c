#define _POSIX_C_SOURCE 200809L
#include "variables.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct Variable {
    char *name;
    char *value;
    struct Variable *next;
} Variable;

static Variable *var_head = NULL;

void var_init(void) {
    var_head = NULL;
}

static int is_valid_var_name(const char *name) {
    if (!name || (!isalpha((unsigned char)*name) && *name != '_')) {
        return 0;
    }
    name++;
    while (*name) {
        if (!isalnum((unsigned char)*name) && *name != '_') {
            return 0;
        }
        name++;
    }
    return 1;
}

int var_set(const char *name, const char *value) {
    if (!is_valid_var_name(name)) {
        return -1;
    }

    const char *val_to_store = value ? value : "";

    Variable *curr = var_head;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            free(curr->value);
            curr->value = strdup(val_to_store);
            return 0;
        }
        curr = curr->next;
    }

    Variable *new_node = malloc(sizeof(Variable));
    if (!new_node) return -1;

    new_node->name = strdup(name);
    new_node->value = strdup(val_to_store);
    new_node->next = var_head;
    var_head = new_node;

    return 0;
}

const char *var_get(const char *name) {
    if (!name) return NULL;

    Variable *curr = var_head;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            return curr->value;
        }
        curr = curr->next;
    }

    // Fall back to environment
    return getenv(name);
}

void var_print_all(void) {
    Variable *curr = var_head;
    while (curr) {
        printf("%s=%s\n", curr->name, curr->value);
        curr = curr->next;
    }
}

int is_assignment_statement(const char *cmd) {
    if (!cmd) return 0;
    while (*cmd && isspace((unsigned char)*cmd)) cmd++;
    if (!*cmd) return 0;

    const char *eq = strchr(cmd, '=');
    if (!eq || eq == cmd) return 0;

    // Check name before '=': must have no spaces and be valid identifier
    const char *p = cmd;
    if (!isalpha((unsigned char)*p) && *p != '_') return 0;
    p++;
    while (p < eq) {
        if (!isalnum((unsigned char)*p) && *p != '_') return 0;
        p++;
    }
    return 1;
}

int handle_assignment(const char *cmd) {
    if (!cmd) return -1;
    while (*cmd && isspace((unsigned char)*cmd)) cmd++;

    const char *eq = strchr(cmd, '=');
    if (!eq) return -1;

    int name_len = eq - cmd;
    char name[256];
    if (name_len >= (int)sizeof(name)) return -1;
    strncpy(name, cmd, name_len);
    name[name_len] = '\0';

    const char *val_raw = eq + 1;
    // Strip surrounding quotes if present
    char val_buf[1024];
    int vlen = strlen(val_raw);
    if ((val_raw[0] == '"' && val_raw[vlen - 1] == '"' && vlen >= 2) ||
        (val_raw[0] == '\'' && val_raw[vlen - 1] == '\'' && vlen >= 2)) {
        strncpy(val_buf, val_raw + 1, vlen - 2);
        val_buf[vlen - 2] = '\0';
    } else {
        strncpy(val_buf, val_raw, sizeof(val_buf) - 1);
        val_buf[sizeof(val_buf) - 1] = '\0';
    }

    return var_set(name, val_buf);
}

char *var_expand_string(const char *input) {
    if (!input) return NULL;
    if (!strchr(input, '$')) {
        return strdup(input);
    }

    size_t out_cap = strlen(input) + 256;
    char *output = malloc(out_cap);
    if (!output) return strdup(input);

    size_t out_pos = 0;
    const char *p = input;

    while (*p) {
        if (*p == '$') {
            p++;
            char var_name[256];
            int vpos = 0;

            if (*p == '{') {
                p++;
                while (*p && *p != '}' && vpos < (int)sizeof(var_name) - 1) {
                    var_name[vpos++] = *p++;
                }
                if (*p == '}') p++;
            } else {
                while (*p && (isalnum((unsigned char)*p) || *p == '_') && vpos < (int)sizeof(var_name) - 1) {
                    var_name[vpos++] = *p++;
                }
            }
            var_name[vpos] = '\0';

            const char *val = var_get(var_name);
            const char *sub = val ? val : "";
            size_t sub_len = strlen(sub);

            while (out_pos + sub_len + 1 >= out_cap) {
                out_cap *= 2;
                output = realloc(output, out_cap);
            }
            memcpy(output + out_pos, sub, sub_len);
            out_pos += sub_len;
        } else {
            if (out_pos + 2 >= out_cap) {
                out_cap *= 2;
                output = realloc(output, out_cap);
            }
            output[out_pos++] = *p++;
        }
    }
    output[out_pos] = '\0';

    return output;
}

void var_free_all(void) {
    Variable *curr = var_head;
    while (curr) {
        Variable *next = curr->next;
        free(curr->name);
        free(curr->value);
        free(curr);
        curr = next;
    }
    var_head = NULL;
}
