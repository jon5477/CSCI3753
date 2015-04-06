/*
 * File: rr_quantum.c
 * Author: Andy Sayler
 * Project: CSCI 3753 Programming Assignment 3
 * Create Date: 2012/03/22
 * Modify Date: 2012/03/22
 * Description: Prints the RR quantum for a given system.
 */

/* Library Includes */
#include <errno.h>
#include <sched.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

#define NANO 1000000000.0

int main(int argc, char* argv[]){

    (void) argc;
    (void) argv;

    struct sched_param param;
    int policy;
    struct timespec tp;
    double qs = 0.0;

    /* Set Policy */
    policy = SCHED_RR;

    /* Set process to max prioty for given scheduler */
    param.sched_priority = sched_get_priority_max(policy);

    /* Set new scheduler policy */
    fprintf(stdout, "Current Scheduling Policy: %d\n", sched_getscheduler(0));
    fprintf(stdout, "Setting Scheduling Policy to: %d\n", policy);
    if(sched_setscheduler(0, policy, &param)){
	perror("Error setting scheduler policy");
	exit(EXIT_FAILURE);
    }
    fprintf(stdout, "New Scheduling Policy: %d\n", sched_getscheduler(0));

    /* Find RR Quantum */
    if(sched_rr_get_interval(getpid(), &tp)){
	perror("Error getting RR Quantum");
	exit(EXIT_FAILURE);
    }

    fprintf(stdout, "tp.tv_sec  = %ld\n", tp.tv_sec);
    fprintf(stdout, "tp.tv_nsec = %ld\n", tp.tv_nsec);
    qs = (tp.tv_nsec / NANO) + tp.tv_sec;
    fprintf(stdout, "RR Quantum  = %f seconds\n", qs);

    return 0;
}
