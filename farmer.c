/*
 * Operating Systems  [2INCO]  Practical Assignment
 * Interprocess Communication
 *
 * STUDENT_NAME_1 (STUDENT_NR_1)
 * STUDENT_NAME_2 (STUDENT_NR_2)
 *
 * Grading:
 * Students who hand in clean code that fully satisfies the minimum requirements will get an 8.
 * Extra steps can lead to higher marks because we want students to take the initiative.
 * Extra steps can be, for example, in the form of measurements added to your code, a formal
 * analysis of deadlock freeness etc.
 */

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>         // for execlp
#include <mqueue.h>         // for mq

#include "settings.h"
#include "common.h"


int main (int argc, char * argv[])
{
    if (argc != 1)
    {
        fprintf (stderr, "%s: invalid arguments\n", argv[0]);
    }

    //Create the message queues
    pid_t               processID;      /* Process ID from fork() */
    mqd_t               mq_fd_job;
    mqd_t               mq_fd_result;
    MQ_JOB_MESSAGE  job;
    MQ_RESULT_MESSAGE result;
    struct mq_attr      attr;
    char mq_name1[80], mq_name2[80];
    sprintf (mq_name1, "J_%s_%d", STUDENT_NAME, getpid());
    sprintf (mq_name2, "R_%s_%d", STUDENT_NAME, getpid());

    printf("-FARMER-\n%s\n%s\n", mq_name1, mq_name2);

    attr.mq_maxmsg  = MQ_MAX_MESSAGES;
    attr.mq_msgsize = sizeof (MQ_JOB_MESSAGE);
    mq_fd_job = mq_open (mq_name1, O_WRONLY | O_CREAT | O_EXCL, 0600, &attr);

    attr.mq_maxmsg  = MQ_MAX_MESSAGES;
    attr.mq_msgsize = sizeof (MQ_RESULT_MESSAGE);
    mq_fd_result = mq_open (mq_name2, O_RDONLY | O_CREAT | O_EXCL, 0600, &attr);

    printf ("parent pid:%d\n", getpid());
    processID = fork();
    if (processID < 0)
    {
        perror("fork() failed");
        exit (1);
    }
    else
    {
        if (processID == 0)
        {
            printf ("child  pid:%d\n", getpid());
            execlp ("./worker", "worker", mq_name1, mq_name2, (char*) NULL);
            // or try this one:
			      //execlp ("./interprocess_basics", "my_own_name_for_argv0", "first_argument", NULL);

            // we should never arrive here...
            perror ("execlp() failed");
        }

        // fill request message
        job.id = 1;
        job.md5hash = 2;
        job.letter = 'A';
        job.first = 'B';
        job.last = 'C';

        sleep (3);
        // send the request
        printf ("parent: sending...\n");
        mq_send (mq_fd_job, (char *) &job, sizeof (job), 0);

        sleep (3);
        // read the result and store it in the response message
        printf ("parent: receiving...\n");
        mq_receive (mq_fd_result, (char *) &result, sizeof (result), NULL);

        printf ("parent: received: id=%d, result=%s \n", result.id, result.result);

        sleep (1);

        waitpid (processID, NULL, 0);   // wait for the child

        mq_close (mq_fd_result);
        mq_close (mq_fd_job);
        mq_unlink (mq_name1);
        mq_unlink (mq_name2);


        // else: we are still the parent (which continues this program)
        waitpid (processID, NULL, 0);   // wait for the child
        printf ("child %d has been finished\n\n", processID);
    }
    // TODO:
    //  * create the message queues (see message_queue_test() in interprocess_basic.c)
    //  * create the child processes (see process_test() and message_queue_test())
    //  * do the farming
    //  * wait until the chilren have been stopped (see process_test())
    //  * clean up the message queues (see message_queue_test())

    // Important notice: make sure that the names of the message queues contain your
    // student name and the process id (to ensure uniqueness during testing)

    return (0);
}
