#define _POSIX_C_SOURCE 200809L
#include "jobs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

static Job job_list[MAX_JOBS];
static int next_job_id = 1;

void jobs_init(void) {
    next_job_id = 1;
    for (int i = 0; i < MAX_JOBS; i++) {
        job_list[i].id = 0;
        job_list[i].pid = 0;
        job_list[i].cmd = NULL;
        job_list[i].active = 0;
    }
}

int jobs_add(pid_t pid, const char *cmd) {
    for (int i = 0; i < MAX_JOBS; i++) {
        if (!job_list[i].active) {
            job_list[i].id = next_job_id++;
            job_list[i].pid = pid;
            job_list[i].cmd = strdup(cmd);
            job_list[i].active = 1;
            printf("[%d] %d\n", job_list[i].id, pid);
            return job_list[i].id;
        }
    }
    fprintf(stderr, "myshell: job list full\n");
    return -1;
}

void jobs_reap(void) {
    int status;
    pid_t pid;

    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        for (int i = 0; i < MAX_JOBS; i++) {
            if (job_list[i].active && job_list[i].pid == pid) {
                printf("[%d]+  Done  %s\n", job_list[i].id, job_list[i].cmd);
                free(job_list[i].cmd);
                job_list[i].cmd = NULL;
                job_list[i].active = 0;
                break;
            }
        }
    }
}

void jobs_print(void) {
    int found = 0;
    for (int i = 0; i < MAX_JOBS; i++) {
        if (job_list[i].active) {
            printf("[%d]  Running  %d  %s\n", job_list[i].id, (int)job_list[i].pid, job_list[i].cmd);
            found = 1;
        }
    }
    if (!found) {
        // Optional notice if empty, or standard shells print nothing
    }
}

void jobs_free(void) {
    for (int i = 0; i < MAX_JOBS; i++) {
        if (job_list[i].active && job_list[i].cmd) {
            free(job_list[i].cmd);
            job_list[i].cmd = NULL;
            job_list[i].active = 0;
        }
    }
}
