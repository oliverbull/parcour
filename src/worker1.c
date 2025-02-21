#include <stdlib.h>
#include <unistd.h>
#include <logging.h>
#include <sync.h>
#include <worker1.h>


// worker_multiply thread to perform multiplication
int worker_multiply(worker_multiply_args *args) {

    // check for null pointers
    if (args == NULL || args->in == NULL || args->factor == NULL || args->out == NULL) {
        logger("worker_multiply(): null input", NULL, true);
        return 1;
    }
    // simulate some processing time
    sleep(2);
    // perform the calculation
    *args->out = *args->in * *args->factor;
    
    return 0;
}

// worker_divide thread to perform division
int worker_divide(worker_divide_args *args) {

    // check for null pointers
    if (args == NULL || args->in == NULL || args->div == NULL || args->out == NULL) {
        logger("worker_divide(): null input", NULL, true);
        return 1;
    }
    // simulate some processing time
    sleep(1);
    // perform the calculation
    *args->out = *args->in / *args->div;

    return 0;
}

// worker thread entry
void *worker1(void *arg) {

    // get the args structure
    worker1_args *w1_args = arg;
    // check for null pointer
    if (w1_args == NULL) {
        logger("worker1(): null input", NULL, true);
        return NULL;
    }

    char log[64];
    sprintf(log, "running worker1 ID: %d", w1_args->id);
    logger(log, NULL, true);

    // call the appropriate worker
    switch (w1_args->type) {
        case MULT:
            worker_multiply(&w1_args->args.multiply);
            break;
        case DIV:
            worker_divide(&w1_args->args.divide);
            break;
        default:
            logger("worker1(): unhandled worker type", NULL, true);
    }
    // update the sync tracker
    sync_dec(w1_args->sync, 1);
}
