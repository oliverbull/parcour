
typedef struct {
    double *in;
    double *factor;
    double *out;
} worker_multiply_args;

typedef struct {
    double *in;
    double *div;
    double *out;
} worker_divide_args;

enum worker_type {
    MULT = 0,
    DIV
};

typedef union {
    worker_multiply_args multiply;
    worker_divide_args divide;
} worker1_type_args;

typedef struct {
    int id;
    sync_t *sync;
    enum worker_type type; // overload type as worker selection and arg struct
    worker1_type_args args;
} worker1_args;

// worker thread entry
void *worker1(void *arg);
