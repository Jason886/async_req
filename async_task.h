#ifndef _CHIVOX_ASYNC_TASK_H_
#define _CHIVOX_ASYNC_TASK_H_

#include <pthread.h> 

#ifdef __cplusplus
extern "C" {
#endif

    typedef void *(*async_func_t)(void *ud);
    typedef void (*async_cb_t)(void *res, void *ud);

    typedef struct {
        async_func_t func;
        async_cb_t cb;
        void *ud;

        pthread_t _tid;
        pthread_mutex_t _mtx;
        int _canceled;
        int _ref_count;
    } async_task_t;

    async_task_t *chivox_async_create(async_func_t func, async_cb_t cb, void *ud);
    void chivox_async_retain(async_task_t *task);
    void chivox_async_release(async_task_t *task);
    void chivox_async_cancel(async_task_t *task);

#ifdef __cplusplus
}
#endif

#endif
