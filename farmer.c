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
    int i;
    for(i = 1; i < NROF_WORKERS; i++){
        if(processID > 0) processID = fork();
    }

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

        MQ_RESULT_MESSAGE results[MD5_LIST_NROF];
        int resultsRecevied = 0, msgRecevied = 0;
        int id = 0, frr = 0;

        while(msgRecevied < JOBS_NROF){
            mq_getattr(mq_fd_result, &attr);
            if(attr.mq_curmsgs > 0){
                int j;
                for(j = 0; j < attr.mq_curmsgs; j++){
                    mq_receive (mq_fd_result, (char *) &result, sizeof (result), NULL);
                    msgRecevied++;
                    if(strcmp(result.result, "") != 0){
                        MQ_RESULT_MESSAGE r, temp;
                        r.id = result.id;
                        sprintf(r.result, result.result);
                        int k;
                        for(k = 0; k < resultsRecevied; k++){
                            if(results[k].id > r.id){
                                temp.id = results[k].id;
                                sprintf(temp.result, results[k].result);
                                results[k].id = r.id;
                                sprintf(results[k].result, r.result);
                                r.id = temp.id;
                                sprintf(r.result, temp.result);
                            }
                        }
                        results[resultsRecevied].id = r.id;
                        sprintf(results[resultsRecevied].result, r.result);
                        resultsRecevied++;
                    }
                }
            }

            mq_getattr(mq_fd_job, &attr);
            if(attr.mq_curmsgs < MQ_MAX_MESSAGES && id < JOBS_NROF){
                job.md5hash = md5_list[id];
                job.letter = ALPHABET_START_CHAR + (id % ALPHABET_NROF_CHAR);
                job.first = ALPHABET_START_CHAR;
                job.last = ALPHABET_END_CHAR;
                job.id = id++;
//                printf ("parent: sending %d\n", job.id);
                mq_send (mq_fd_job, (char *) &job, sizeof (job), 0);
            }
        }

        for(i = 0; i < NROF_WORKERS; i++){
            job.id = -1;
            mq_send (mq_fd_job, (char *) &job, sizeof (job), 0);
            printf ("parent: sending %d\n", job.id);
        }

        for(i = 0; i < NROF_WORKERS; i++){
          mq_receive (mq_fd_result, (char *) &result, sizeof (result), NULL);
          printf ("parent: receiving %d\n", result.id);
        }

        mq_close (mq_fd_result);
        mq_close (mq_fd_job);
        mq_unlink (mq_name1);
        mq_unlink (mq_name2);

        for(i = 0; i < MD5_LIST_NROF; i++){
            printf("%d\n", results[i].id);
        }

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
