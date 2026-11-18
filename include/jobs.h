#ifndef JOBS_H
#define JOBS_H

#include <sys/types.h>

#define MAX_JOBS 64

typedef struct {
    int id;
    pid_t pid;
    char *cmd;
    int active;
} Job;

/**
 * jobs_init - Initializes the background job list.
 */
void jobs_init(void);

/**
 * jobs_add - Registers a new background job.
 * @pid: Process ID of the background child.
 * @cmd: The command line string.
 *
 * Return: Assigned 1-based job ID.
 */
int jobs_add(pid_t pid, const char *cmd);

/**
 * jobs_reap - Reaps all terminated zombie processes without blocking.
 */
void jobs_reap(void);

/**
 * jobs_print - Displays all active background jobs.
 */
void jobs_print(void);

/**
 * jobs_free - Deallocates all job memory.
 */
void jobs_free(void);

#endif /* JOBS_H */
