#include <pthread.h>

typedef struct {
    int count;
    pthread_mutex_t count_mutex;
    pthread_mutex_t mutex;
} sync_t;

// init the sync instance
int sync_init(sync_t *sync);
// increment
int sync_add(sync_t *sync, int num);
// decrement - count is capped at 0
int sync_dec(sync_t *sync, int num);
// sync lock - unlocked when count is 0 or less
int sync_lock(sync_t *sync);
// sync close to destroy the mutex (sync ptr is callers responsibility)
int sync_close(sync_t *sync);
