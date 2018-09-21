/*
 * Operating Systems  [2INCO]  Practical Assignment
 * Interprocess Communication
 *
 * Contains definitions which are commonly used by the farmer and the workers
 *
 */

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
#include <mqueue.h>
#include <sys/wait.h>   /* for waitpid() */

#ifndef COMMON_H
#define COMMON_H
// maximum size for any message in the tests
#define MAX_MESSAGE_LENGTH	6


// TODO: put your definitions of the datastructures here

typedef struct
{
    // a data structure with 5 members
    int                     id;
    long               md5hash;
    char                    letter;
    char                    first;
    char                    last;
} MQ_JOB_MESSAGE;

typedef struct
{
    // a data structure with 2 members
    int                     id;
    char                    result[20];
} MQ_RESULT_MESSAGE;
#endif

#define STUDENT_NAME        "AlessioAndHampus"
