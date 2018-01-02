#ifndef _CHIVOX_ASYNC_TASK_H_
#define _CHIVOX_ASYNC_TASK_H_

#include <pthread.h> 

#ifdef __cplusplus
extern "C" {
#endif

    struct async_task;

    typedef void *(*async_func_t)(struct async_task *task, void *arg, int len);
    typedef void (*async_cb_t)(struct async_task *task, void *res);

    typedef struct async_task{
        async_func_t func;
        async_cb_t cb;
        void *ud;
        void *arg;
        int arg_len;

        pthread_t _tid;
        pthread_mutex_t _mtx;
        int _canceled;
        int _ref_count;
    } async_task_t;

    async_task_t *chivox_async_create(async_func_t func, async_cb_t cb, void *ud, void *arg, int arg_len);
    void chivox_async_delete(async_task_t *task);

#ifdef __cplusplus
}
#endif

#endif
