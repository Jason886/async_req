#include "async_task.h"

void *_task1(async_task_t *task, int argc, char **argv) {
    int count = 0;
    while(1) {
        printf("task1\n");
        usleep(100000);
        count++;
        if (count > 10) {
            break;
        }
    }
    char *res = malloc(35);
    strcpy(res, "task1 result");
    return res;
}

async_task_t *task = 0;

void _task1_cb(async_task_t *taska, void *res) {
    printf("_task1 cb:%s\n", (char *)res);
    chivox_async_delete(taska);
    taska = NULL;
}


int main(int argc, char **argv) {
    //async_task_t *task = chivox_async_create(_task1, _task1_cb, NULL, NULL);
    while(1) {
        int c = getchar();
        if (c == '1') {
            task = chivox_async_create(_task1, _task1_cb, 0, NULL, NULL);
            usleep(20);
            chivox_async_delete(task);

            task = chivox_async_create(_task1, _task1_cb, 0, NULL, NULL);
            usleep(20);
            chivox_async_delete(task);

            task = chivox_async_create(_task1, _task1_cb, 0, NULL, NULL);
            usleep(20);
            chivox_async_delete(task);

            task = chivox_async_create(_task1, _task1_cb, 0, NULL, NULL);

            usleep(20);
            chivox_async_delete(task);
        }
    }
    //sleep();
    return 0;
}
