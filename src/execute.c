#define _POSIX_C_SOURCE 200809L
#include "shell.h"
#include <fcntl.h>
#include <ctype.h>

typedef struct {
    char **argv;
    char *input_file;
    char *output_file;
    int append;
} SimpleCommand;

static void free_simple_command(SimpleCommand *cmd) {
    if (cmd->argv) {
        free_tokens(cmd->argv);
        cmd->argv = NULL;
    }
    if (cmd->input_file) {
        free(cmd->input_file);
        cmd->input_file = NULL;
    }
    if (cmd->output_file) {
        free(cmd->output_file);
        cmd->output_file = NULL;
    }
}

/**
 * tokenize_smart - Tokenizes a string respecting quotes and redirection operators.
 */
static char **tokenize_smart(const char *str) {
    int bufsize = MAX_ARGS;
    char **tokens = malloc(bufsize * sizeof(char *));
    if (!tokens) return NULL;

    int pos = 0;
    const char *p = str;

    while (*p) {
        while (*p && isspace((unsigned char)*p)) p++;
        if (!*p) break;

        char token_buf[MAX_LINE];
        int tpos = 0;

        if (*p == '<') {
            token_buf[tpos++] = *p++;
        } else if (*p == '>') {
            token_buf[tpos++] = *p++;
            if (*p == '>') {
                token_buf[tpos++] = *p++;
            }
        } else {
            char quote = 0;
            while (*p) {
                if (quote) {
                    if (*p == quote) {
                        quote = 0;
                        p++;
                    } else {
                        token_buf[tpos++] = *p++;
                    }
                } else {
                    if (*p == '\'' || *p == '"') {
                        quote = *p++;
                    } else if (isspace((unsigned char)*p) || *p == '<' || *p == '>' || *p == '|') {
                        break;
                    } else {
                        token_buf[tpos++] = *p++;
                    }
                }
            }
        }

        token_buf[tpos] = '\0';
        tokens[pos++] = strdup(token_buf);

        if (pos >= bufsize - 1) {
            bufsize *= 2;
            tokens = realloc(tokens, bufsize * sizeof(char *));
        }
    }
    tokens[pos] = NULL;
    return tokens;
}

static int parse_simple_command(const char *cmd_str, SimpleCommand *cmd) {
    cmd->argv = NULL;
    cmd->input_file = NULL;
    cmd->output_file = NULL;
    cmd->append = 0;

    char **raw_tokens = tokenize_smart(cmd_str);
    if (!raw_tokens || !raw_tokens[0]) {
        if (raw_tokens) free(raw_tokens);
        return 0;
    }

    int bufsize = MAX_ARGS;
    cmd->argv = malloc(bufsize * sizeof(char *));
    int argc = 0;

    for (int i = 0; raw_tokens[i] != NULL; i++) {
        if (strcmp(raw_tokens[i], "<") == 0) {
            if (raw_tokens[i + 1] != NULL) {
                cmd->input_file = strdup(raw_tokens[++i]);
            }
        } else if (strcmp(raw_tokens[i], ">") == 0) {
            if (raw_tokens[i + 1] != NULL) {
                cmd->output_file = strdup(raw_tokens[++i]);
                cmd->append = 0;
            }
        } else if (strcmp(raw_tokens[i], ">>") == 0) {
            if (raw_tokens[i + 1] != NULL) {
                cmd->output_file = strdup(raw_tokens[++i]);
                cmd->append = 1;
            }
        } else {
            cmd->argv[argc++] = strdup(raw_tokens[i]);
            if (argc >= bufsize - 1) {
                bufsize *= 2;
                cmd->argv = realloc(cmd->argv, bufsize * sizeof(char *));
            }
        }
    }
    cmd->argv[argc] = NULL;
    free_tokens(raw_tokens);

    // Variable expansion step
    for (int i = 0; i < argc; i++) {
        char *expanded = var_expand_string(cmd->argv[i]);
        free(cmd->argv[i]);
        cmd->argv[i] = expanded;
    }
    if (cmd->input_file) {
        char *exp = var_expand_string(cmd->input_file);
        free(cmd->input_file);
        cmd->input_file = exp;
    }
    if (cmd->output_file) {
        char *exp = var_expand_string(cmd->output_file);
        free(cmd->output_file);
        cmd->output_file = exp;
    }

    return (argc > 0);
}

int execute(char **arglist) {
    if (arglist == NULL || arglist[0] == NULL) {
        return 0;
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("myshell: fork failed");
        return -1;
    } else if (pid == 0) {
        if (execvp(arglist[0], arglist) == -1) {
            perror(arglist[0]);
            exit(EXIT_FAILURE);
        }
    } else {
        int status;
        if (waitpid(pid, &status, 0) == -1) {
            perror("myshell: waitpid failed");
            return -1;
        }
        return status;
    }

    return 0;
}

static int execute_single(SimpleCommand *cmd, int is_background, const char *raw_cmd) {
    if (!cmd->argv || !cmd->argv[0]) return 0;

    const char *bnames[] = {"cd", "exit", "pwd", "help", "jobs", "history", "set", NULL};
    int is_builtin = 0;
    for (int i = 0; bnames[i]; i++) {
        if (strcmp(cmd->argv[0], bnames[i]) == 0) {
            is_builtin = 1;
            break;
        }
    }

    if (is_builtin && !is_background) {
        int saved_in = -1, saved_out = -1;
        if (cmd->input_file) {
            saved_in = dup(STDIN_FILENO);
            int fd = open(cmd->input_file, O_RDONLY);
            if (fd < 0) {
                perror(cmd->input_file);
                if (saved_in >= 0) close(saved_in);
                return -1;
            }
            dup2(fd, STDIN_FILENO);
            close(fd);
        }
        if (cmd->output_file) {
            saved_out = dup(STDOUT_FILENO);
            int flags = O_WRONLY | O_CREAT | (cmd->append ? O_APPEND : O_TRUNC);
            int fd = open(cmd->output_file, flags, 0644);
            if (fd < 0) {
                perror(cmd->output_file);
                if (saved_in >= 0) { dup2(saved_in, STDIN_FILENO); close(saved_in); }
                if (saved_out >= 0) close(saved_out);
                return -1;
            }
            dup2(fd, STDOUT_FILENO);
            close(fd);
        }

        handle_builtin(cmd->argv);

        if (saved_in >= 0) {
            dup2(saved_in, STDIN_FILENO);
            close(saved_in);
        }
        if (saved_out >= 0) {
            dup2(saved_out, STDOUT_FILENO);
            close(saved_out);
        }
        return 0;
    }

    // External command or background builtin
    pid_t pid = fork();
    if (pid < 0) {
        perror("myshell: fork failed");
        return -1;
    } else if (pid == 0) {
        if (cmd->input_file) {
            int fd = open(cmd->input_file, O_RDONLY);
            if (fd < 0) {
                perror(cmd->input_file);
                exit(EXIT_FAILURE);
            }
            dup2(fd, STDIN_FILENO);
            close(fd);
        }
        if (cmd->output_file) {
            int flags = O_WRONLY | O_CREAT | (cmd->append ? O_APPEND : O_TRUNC);
            int fd = open(cmd->output_file, flags, 0644);
            if (fd < 0) {
                perror(cmd->output_file);
                exit(EXIT_FAILURE);
            }
            dup2(fd, STDOUT_FILENO);
            close(fd);
        }

        if (is_builtin) {
            handle_builtin(cmd->argv);
            exit(EXIT_SUCCESS);
        }

        if (execvp(cmd->argv[0], cmd->argv) == -1) {
            perror(cmd->argv[0]);
            exit(EXIT_FAILURE);
        }
    } else {
        if (is_background) {
            jobs_add(pid, raw_cmd);
            return 0;
        } else {
            int status;
            if (waitpid(pid, &status, 0) == -1) {
                perror("myshell: waitpid failed");
                return -1;
            }
            if (WIFEXITED(status)) {
                return WEXITSTATUS(status);
            } else if (WIFSIGNALED(status)) {
                return 128 + WTERMSIG(status);
            }
            return status;
        }
    }

    return 0;
}

int execute_pipeline(char *cmdline, int is_background) {
    if (!cmdline || !*cmdline) return 0;

    char *segments[MAX_ARGS];
    int num_segments = 0;

    char seg_buf[MAX_LINE];
    int spos = 0;
    char quote = 0;
    const char *p = cmdline;

    while (*p) {
        if (quote) {
            if (*p == quote) quote = 0;
            seg_buf[spos++] = *p++;
        } else {
            if (*p == '\'' || *p == '"') {
                quote = *p;
                seg_buf[spos++] = *p++;
            } else if (*p == '|') {
                seg_buf[spos] = '\0';
                segments[num_segments++] = strdup(seg_buf);
                spos = 0;
                p++;
                continue;
            } else {
                seg_buf[spos++] = *p++;
            }
        }
    }
    seg_buf[spos] = '\0';
    segments[num_segments++] = strdup(seg_buf);

    SimpleCommand cmds[MAX_ARGS];
    int valid_cmds = 0;

    for (int i = 0; i < num_segments; i++) {
        if (parse_simple_command(segments[i], &cmds[valid_cmds])) {
            valid_cmds++;
        }
        free(segments[i]);
    }

    if (valid_cmds == 0) {
        return 0;
    }

    if (valid_cmds == 1) {
        int ret = execute_single(&cmds[0], is_background, cmdline);
        free_simple_command(&cmds[0]);
        return ret;
    }

    // Pipeline with valid_cmds >= 2
    int prev_read_fd = -1;
    pid_t pids[MAX_ARGS];

    for (int i = 0; i < valid_cmds; i++) {
        int pipefd[2];
        if (i < valid_cmds - 1) {
            if (pipe(pipefd) < 0) {
                perror("myshell: pipe failed");
                return -1;
            }
        }

        pids[i] = fork();
        if (pids[i] < 0) {
            perror("myshell: fork failed");
            return -1;
        } else if (pids[i] == 0) {
            if (i > 0) {
                dup2(prev_read_fd, STDIN_FILENO);
                close(prev_read_fd);
            }
            if (i < valid_cmds - 1) {
                dup2(pipefd[1], STDOUT_FILENO);
                close(pipefd[1]);
                close(pipefd[0]);
            }

            if (cmds[i].input_file) {
                int fd = open(cmds[i].input_file, O_RDONLY);
                if (fd < 0) {
                    perror(cmds[i].input_file);
                    exit(EXIT_FAILURE);
                }
                dup2(fd, STDIN_FILENO);
                close(fd);
            }
            if (cmds[i].output_file) {
                int flags = O_WRONLY | O_CREAT | (cmds[i].append ? O_APPEND : O_TRUNC);
                int fd = open(cmds[i].output_file, flags, 0644);
                if (fd < 0) {
                    perror(cmds[i].output_file);
                    exit(EXIT_FAILURE);
                }
                dup2(fd, STDOUT_FILENO);
                close(fd);
            }

            if (handle_builtin(cmds[i].argv)) {
                exit(EXIT_SUCCESS);
            }

            if (execvp(cmds[i].argv[0], cmds[i].argv) == -1) {
                perror(cmds[i].argv[0]);
                exit(EXIT_FAILURE);
            }
        } else {
            if (i > 0) {
                close(prev_read_fd);
            }
            if (i < valid_cmds - 1) {
                close(pipefd[1]);
                prev_read_fd = pipefd[0];
            }
        }
    }

    if (is_background) {
        jobs_add(pids[valid_cmds - 1], cmdline);
        for (int i = 0; i < valid_cmds; i++) {
            free_simple_command(&cmds[i]);
        }
        return 0;
    }

    int last_status = 0;
    for (int i = 0; i < valid_cmds; i++) {
        int status;
        waitpid(pids[i], &status, 0);
        if (i == valid_cmds - 1) {
            if (WIFEXITED(status)) {
                last_status = WEXITSTATUS(status);
            } else if (WIFSIGNALED(status)) {
                last_status = 128 + WTERMSIG(status);
            } else {
                last_status = status;
            }
        }
        free_simple_command(&cmds[i]);
    }

    return last_status;
}

int execute_line(char *cmdline) {
    if (!cmdline || !*cmdline) return 0;

    // Split by ';' respecting quotes
    char *cmds[MAX_ARGS];
    int count = 0;

    char buf[MAX_LINE];
    int bpos = 0;
    char quote = 0;
    const char *p = cmdline;

    while (*p) {
        if (quote) {
            if (*p == quote) quote = 0;
            buf[bpos++] = *p++;
        } else {
            if (*p == '\'' || *p == '"') {
                quote = *p;
                buf[bpos++] = *p++;
            } else if (*p == ';') {
                buf[bpos] = '\0';
                cmds[count++] = strdup(buf);
                bpos = 0;
                p++;
                continue;
            } else {
                buf[bpos++] = *p++;
            }
        }
    }
    buf[bpos] = '\0';
    cmds[count++] = strdup(buf);

    int last_status = 0;
    for (int i = 0; i < count; i++) {
        char *str = cmds[i];
        while (*str && isspace((unsigned char)*str)) str++;
        if (*str) {
            // Trim trailing whitespace
            int len = strlen(str);
            while (len > 0 && isspace((unsigned char)str[len - 1])) {
                str[--len] = '\0';
            }

            int is_bg = 0;
            if (len > 0 && str[len - 1] == '&') {
                is_bg = 1;
                str[--len] = '\0';
                while (len > 0 && isspace((unsigned char)str[len - 1])) {
                    str[--len] = '\0';
                }
            }

            if (*str) {
                if (is_assignment_statement(str)) {
                    handle_assignment(str);
                    last_status = 0;
                } else {
                    last_status = execute_pipeline(str, is_bg);
                }
            }
        }
        free(cmds[i]);
    }

    return last_status;
}
