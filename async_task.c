#include "async_task.h"
#include <string.h>
#include <malloc.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>

static void *_inner_func(void *arg) {
    async_task_t *task = arg;

    pthread_detach(pthread_self());
    void *res = task->func(task->ud);
    int canceled = 0;
    pthread_mutex_lock(&task->_mtx);
    canceled = task->_canceled;
    pthread_mutex_unlock(&task->_mtx);

    if (!canceled) {
        if (task->cb) {
            task->cb(res, task->ud);
        }
    }

    if (res) free(res);
    chivox_async_release(task);
    return NULL;
}

async_task_t *chivox_async_create(async_func_t func, async_cb_t cb, void *ud) {
    if (!func) return NULL;
    async_task_t *task = malloc(sizeof *task);
    if (task) {
        memset(task, 0, sizeof *task);
        task->func = func;
        task->cb = cb;
        task->ud = ud;

        task->_ref_count = 1;
        task->_canceled = 0;

        pthread_mutex_init(&task->_mtx, NULL);

        chivox_async_retain(task);
        if (0 != pthread_create(&task->_tid, NULL, _inner_func, (void *)task)) {
            chivox_async_release(task);
            chivox_async_release(task);
            return NULL;
        }
    }

    return task;
}

void chivox_async_retain(async_task_t *task) {
    if (task) {
        pthread_mutex_lock(&task->_mtx);
        task->_ref_count ++;
        pthread_mutex_unlock(&task->_mtx);
    }
}

void chivox_async_release(async_task_t *task) {
    int do_free = 0;
    if (task) {
        pthread_mutex_lock(&task->_mtx);
        assert(task->_ref_count > 0);
        task->_ref_count --;
        if (task->_ref_count == 0) {
            do_free = 1;
        }
        pthread_mutex_unlock(&task->_mtx);
        if (do_free) {
            pthread_mutex_destroy(&task->_mtx);
            free(task);
        }
    }
}

void chivox_async_cancel(async_task_t *task) {
    if (task) {
        pthread_mutex_lock(&task->_mtx);
        task->_canceled = 1;
        pthread_mutex_unlock(&task->_mtx);
    }
}

