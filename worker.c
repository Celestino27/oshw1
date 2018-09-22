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
#include <errno.h>          // for perror()
#include <unistd.h>         // for getpid()
#include <mqueue.h>         // for mq-stuff
#include <time.h>           // for time()
#include <complex.h>

#include "common.h"
#include "md5s.h"

static void rsleep (int t);

char* mq_jobs;
char* mq_result;

int main (int argc, char * argv[])
{

    if(argc > 2){
      mq_jobs = argv[1];
      mq_result = argv[2];
      printf("-WORKER-\n%s\n%s\n", mq_jobs, mq_result);
    } else {
      perror("Failure to start worker correctly");
      exit(1);
    }

    mqd_t               mq_fd_job;
    mqd_t               mq_fd_result;
    MQ_JOB_MESSAGE  job;
    MQ_RESULT_MESSAGE result;

    mq_fd_job = mq_open (mq_jobs, O_RDONLY);
    mq_fd_result = mq_open (mq_result, O_WRONLY);

    // read the message queue and store it in the request message
    job.id = 0;

    while(job.id > -1){
        mq_receive (mq_fd_job, (char *) &job, sizeof (job), NULL);

//        printf ("child: received: %d\n", job.id);

        result.id = job.id;
        if(job.letter == job.first){
            sprintf(result.result, ":)");
        } else {
            sprintf(result.result, "");
        }
        mq_send (mq_fd_result, (char *) &result, sizeof (result), 0);
    }

    mq_close (mq_fd_result);
    mq_close (mq_fd_job);

    exit(0);
    // TODO:
    // (see message_queue_test() in interprocess_basic.c)
    //  * open the two message queues (whose names are provided in the arguments)
    //  * repeatingly:
    //      - read from a message queue the new job to do
    //      - wait a random amount of time (e.g. rsleep(10000);)
    //      - do that job
    //      - write the results to a message queue
    //    until there are no more tasks to do
    //  * close the message queues
}

/*
 * rsleep(int t)
 *
 * The calling thread will be suspended for a random amount of time
 * between 0 and t microseconds
 * At the first call, the random generator is seeded with the current time
 */
static void rsleep (int t)
{
    static bool first_call = true;

    if (first_call == true)
    {
        srandom (time (NULL) % getpid ());
        first_call = false;
    }
    usleep (random() % t);
}
