#include "async_task.h"
#include <string.h>
#include <malloc.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>

static void _task_retain(async_task_t *task) {
    if (task) {
        pthread_mutex_lock(&task->_mtx);
        task->_ref_count ++;
        pthread_mutex_unlock(&task->_mtx);
    }
}

static void _task_release(async_task_t *task) {
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
            printf("chivox_async: free task %p\n", (void *)task);
            pthread_mutex_destroy(&task->_mtx);
            if (task->arg) {
                free(task->arg);
            }
            free(task);
        }
    }
}

static void _task_cancel(async_task_t *task) {
    if (task) {
        pthread_mutex_lock(&task->_mtx);
        task->_canceled = 1;
        pthread_mutex_unlock(&task->_mtx);
    }
}

static void *_inner_func(void *arg) {
    async_task_t *task = arg;

    void *res = task->func(task, task->arg, task->arg_len);
    int canceled = 0;
    pthread_mutex_lock(&task->_mtx);
    canceled = task->_canceled;
    pthread_mutex_unlock(&task->_mtx);

    if (!canceled) {
        if (task->cb) {
            task->cb(task, res);
        }
    }

    printf("chivox_async: free res %p\n", res);
    if (res) free(res);
    _task_release(task);
    pthread_exit(NULL);
    return NULL;
}

async_task_t *chivox_async_create(async_func_t func, async_cb_t cb, void *ud, void *arg, int arg_len) {

    if (!func) return NULL;
    async_task_t *task = malloc(sizeof *task);
    if (task) {
        memset(task, 0, sizeof *task);
        task->func = func;
        task->cb = cb;
        task->ud = ud;
        task->_ref_count = 1;
        task->_canceled = 0;

        if (arg && arg_len > 0) {
            task->arg_len = arg_len;
            task->arg = malloc(arg_len);
            if (!task->arg) {
                free(task);
                return NULL;
            }
            memcpy(task->arg, arg, arg_len);
        }

        pthread_mutex_init(&task->_mtx, NULL);

        pthread_attr_t attr;
        pthread_attr_init (&attr);
        pthread_attr_setdetachstate (&attr, PTHREAD_CREATE_DETACHED);

        _task_retain(task);
        if (0 != pthread_create(&task->_tid, &attr, _inner_func, (void *)task)) {
            _task_release(task);
            _task_release(task);
            pthread_attr_destroy (&attr);
            return NULL;
        }
        pthread_attr_destroy (&attr);
    }

    return task;
}

void chivox_async_delete(async_task_t *task) {
   if (task) {
       _task_cancel(task);
       _task_release(task);
   } 
}
