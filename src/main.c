#include <pthread.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sync.h>
#include <worker1.h>
#include <worker2.h>
#include <logging.h>

#define handle_error(msg) \
    do { perror(msg); exit(1); } while (0)

typedef struct {
    pthread_t worker_tid;
    worker1_args args;
} worker1_thread_info_t;

typedef struct {
    pthread_t worker_tid;
    worker2_args args;
} worker2_thread_info_t;

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

    // setup the sync mutex
    sync_t *sync = calloc(sizeof(sync_t), 1);
    if (sync_init(sync) != 0) {
        handle_error("sync_init");
    }

    // prepare the thread info
    worker1_thread_info_t *worker1_thread_info = calloc(sizeof(worker1_thread_info_t), FIRST_NUM);
    if (worker1_thread_info == NULL)
        handle_error("calloc worker1_thread_info_t");
    // 10 first actions
    for (int tidx = 0; tidx < FIRST_NUM; tidx++) {
        // prep the args - alternate between job type
        worker1_thread_info_t *t_info = &worker1_thread_info[tidx];
        t_info->args.id = tidx;
        t_info->args.sync = sync;
        if (tidx % 2 == 0) {
            t_info->args.type = MULT;
            t_info->args.args.multiply.in = &input;
            t_info->args.args.multiply.factor = &fact[tidx/2];
            t_info->args.args.multiply.out = &out[tidx];
        } else {
            t_info->args.type = DIV;
            t_info->args.args.divide.in = &input;
            t_info->args.args.divide.div = &div[tidx/2];
            t_info->args.args.divide.out = &out[tidx];
        }
        // add to the thread sync mutex
        if (sync_add(sync, 1) != 0) {
            handle_error("sync_add");
        }
        // start the worker thread
        int ret = pthread_create(&t_info->worker_tid, NULL, &worker1, &t_info->args);
        if (ret != 0) {
            handle_error("pthread_create");
        }
        // let it go
        pthread_detach(t_info->worker_tid);
    }

    // wait for completion
    logger("waiting for layer1 to complete", lfp, true);
    sync_lock(sync);

    // threads done, so can now free the info
    free(worker1_thread_info);

    // print the results
    sprintf(log_str, "%s", "layer 1 result: ");
    char res_str[32];
    for (int idx = 0; idx < FIRST_NUM; idx++) {
        sprintf(res_str, "%f, ", out[idx]);
        strcat(log_str, res_str);
    }
    logger(log_str, lfp, true);

    // set the second layer dataset - 5 scalars
    double scal[] = {0.5, 0.7, 0.9, 1.05, 0.2};

    // set the next output
    double out2[SECOND_NUM] = {};

    // prepare the thread info
    worker2_thread_info_t *worker2_thread_info = calloc(sizeof(worker2_thread_info_t), SECOND_NUM);
    if (worker2_thread_info == NULL)
        handle_error("calloc worker2_thread_info_t");
    // 5 actions
    for (int tidx = 0, inidx = 0; tidx < SECOND_NUM; tidx++, inidx+=2) {
        // prep the args - alternate between job type
        worker2_thread_info_t *t_info = &worker2_thread_info[tidx];
        t_info->args.id = tidx;
        t_info->args.sync = sync;
        t_info->args.args.in1 = &out[inidx];
        t_info->args.args.in2 = &out[inidx+1];
        t_info->args.args.scaler = &scal[tidx];
        t_info->args.args.out = &out2[tidx];
        // add to the thread sync mutex
        if (sync_add(sync, 1) != 0) {
            handle_error("sync_add");
        }
        // start the worker thread
        int ret = pthread_create(&t_info->worker_tid, NULL, &worker2, &t_info->args);
        if (ret != 0) {
            handle_error("pthread_create");
        }
        // let it go
        pthread_detach(t_info->worker_tid);
    }

    // wait for completion
    logger("waiting for layer2 to complete", lfp, true);
    sync_lock(sync);

    // threads done, so can now free the info and the sync
    free(worker2_thread_info);
    sync_close(sync);
    free(sync);

    // print the results
    sprintf(log_str, "%s", "layer 2 result: ");
    for (int idx = 0; idx < SECOND_NUM; idx++) {
        sprintf(res_str, "%f, ", out2[idx]);
        strcat(log_str, res_str);
    }
    logger(log_str, lfp, true);

    // final derivation done inline here sum and 1/5th
    double final = 0;
    for (int idx = 0; idx < SECOND_NUM; idx++) {
        final = final + out2[idx];
    }
    final = final * 0.2;

    sprintf(log_str, "final result: %f", final);
    logger(log_str, lfp, true);

    // flush and close the logger file
    fflush(lfp);
    fclose(lfp);

    return 0;
}