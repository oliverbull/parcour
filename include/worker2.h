

typedef struct {
    double *in1;
    double *in2;
    double *scaler;
    double *out;
} worker_combine_args;

typedef struct {
    int id;
    sync_t *sync;
    worker_combine_args args;
} worker2_args;

// worker thread entry
void *worker2(void *arg);
