#include <pthread.h>
#include <semaphore.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <worker1.h>
#include <logging.h>

#define handle_error(msg) \
    do { perror(msg); exit(1); } while (0)

typedef struct {
    pthread_t worker1_tid;
    worker1_args args;
} worker1_thread_info_t;

int main() {

    const int FIRST_NUM = 10;
    const int SECOND_NUM = 5;

    // prepare the logfile and log string
    char log_str[1024];
    FILE *lfp = fopen("log.txt", "w");
    if (lfp == NULL) {
        return 1;
    }

    // set the input
    double input = 5.25;

    // set the first layer dataset - 5 mult, 5 div
    double fact[] = {1.1, 2.2, 3.3, 4.4, 5.5};
    double div[] = {1.6, 2.7, 3.8, 4.9, 5.0};

    // set the output
    double out[FIRST_NUM] = {};

    // setup the worker1 semaphore
    sem_t *first_sem = calloc(sizeof(sem_t), 1);
    if (sem_init(first_sem, 0, 0) == -1)
        handle_error("sem_init");

    // prepare the thread info
    worker1_thread_info_t *worker1_thread_info = calloc(sizeof(worker1_thread_info_t), FIRST_NUM);
    if (worker1_thread_info == NULL)
        handle_error("calloc worker1_thread_info_t");
    // 10 first actions
    for (int tidx = 0; tidx < FIRST_NUM; tidx++) {
        // prep the args - alternate between job type
        worker1_thread_info_t *t_info = &worker1_thread_info[tidx];
        t_info->args.id = tidx;
        if (tidx % 2 == 0) {
            t_info->args.type = MULT;
            t_info->args.args.multiply.in = &input;
            t_info->args.args.multiply.factor = &fact[tidx/2];
            t_info->args.args.multiply.out = &out[tidx];
            t_info->args.args.multiply.sem = first_sem;
        } else {
            t_info->args.type = DIV;
            t_info->args.args.divide.in = &input;
            t_info->args.args.divide.div = &div[tidx/2];
            t_info->args.args.divide.out = &out[tidx];
            t_info->args.args.divide.sem = first_sem;
        }
        int ret = pthread_create(&t_info->worker1_tid, NULL, &worker1, &t_info->args);
        if (ret != 0) {
            handle_error("pthread_create");
        }
        // let it go
        pthread_detach(t_info->worker1_tid);
    }

    // wait for completion - todo: make this a proper signal handler
    logger("waiting for layer1 to complete", lfp, true);
    int sem_count = 0;
    while (sem_count < FIRST_NUM) {
        sleep(1);
        sem_getvalue(first_sem, &sem_count);
        sprintf(log_str, "sem count: %d", sem_count);
        logger(log_str, lfp, true);
    }

    // threads done, so can now free the info, close and free semaphore
    free(worker1_thread_info);
    sem_close(first_sem);
    free(first_sem);

    // print the results
    sprintf(log_str, "%s", "layer 1 result: ");
    char res_str[128];
    for (int idx = 0; idx < FIRST_NUM; idx++) {
        sprintf(res_str, "%f, ", out[idx]);
        strcat(log_str, res_str);
    }
    logger(log_str, lfp, true);

    return 0;
}