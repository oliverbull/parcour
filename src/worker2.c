#include <unistd.h>
#include <logging.h>
#include <sync.h>
#include <worker2.h>


// worker_combine thread to perform division
int worker_combine(worker_combine_args *args) {

    // check for null pointers
    if (args == NULL || args->in1 == NULL || args->in2 == NULL || args->scaler == NULL || args->out == NULL) {
        logger("worker_divide(): null input", NULL, true);
        return 1;
    }
    // simulate some processing time
    sleep(2);
    // perform the calculation - sum of 1 and 2 with the single scaler
    *args->out = (*args->in1 * *args->scaler) + (*args->in2 * *args->scaler);

    return 0;
}

void *worker2(void *arg) {

    // get the args structure
    worker2_args *w2_args = arg;
    // check for null pointer
    if (w2_args == NULL) {
        logger("worker2(): null input", NULL, true);
        return NULL;
    }

    char log[64];
    sprintf(log, "running worker2 ID: %d", w2_args->id);
    logger(log, NULL, true);

    // call the worker
    worker_combine(&w2_args->args);

    // update the sync tracker
    sync_dec(w2_args->sync, 1);
}