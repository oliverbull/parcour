#include <pthread.h>
#include <sync.h>

// init the sync instance
int sync_init(sync_t *sync) {
    if (sync == NULL) {
        return 1;
    }
    // setup the structure and take the lock
    sync->count = 0;
    if (pthread_mutex_init(&sync->mutex, NULL) != 0) {
        return 2;
    }
    if (pthread_mutex_init(&sync->count_mutex, NULL) != 0) {
        return 2;
    }
    if (pthread_mutex_lock(&sync->mutex) != 0) {
        return 3;
    }
    return 0;
}

// increment
int sync_add(sync_t *sync, int num) {
    if (sync == NULL) {
        return 1;
    }
    pthread_mutex_lock(&sync->count_mutex);
    sync->count += num;
    pthread_mutex_unlock(&sync->count_mutex);
    return 0;
}

// decrement - count is capped at 0
int sync_dec(sync_t *sync, int num) {
    if (sync == NULL) {
        return 1;
    }
    pthread_mutex_lock(&sync->count_mutex);
    sync->count -= num;
    if (sync->count <= 0) {
        sync->count = 0;
        if (pthread_mutex_unlock(&sync->mutex) != 0) {
            pthread_mutex_unlock(&sync->count_mutex);
            return 3;
        }
    }
    pthread_mutex_unlock(&sync->count_mutex);
    return 0;
};

// sync lock - unlocked when count is 0 or less
int sync_lock(sync_t *sync) {
    if (sync == NULL) {
        return 1;
    }
    return pthread_mutex_lock(&sync->mutex);
};

// sync close to destroy the mutex (sync ptr is callers responsibility)
int sync_close(sync_t *sync) {
    if (sync == NULL) {
        return 1;
    }
    if (pthread_mutex_destroy(&sync->mutex) != 0) {
        return 2;
    }
    return 0;
};