//
// Created by yu on 2018/5/18.
//

#ifndef SRC_THREADPOLL_H
#define SRC_THREADPOLL_H
typedef struct task{
    void (*func)(void*);
    void* arg;
    struct tssk* next;
}task_t;

typedef struct threadpool{
    pthread_mutex_t lock;    // 互斥锁
    pthread_cond_t cond;    // 条件变量
    pthread_t *threads;    // 线程
    task_t *head;    // 任务链表
    int thread_count;    // 线程数
    int queue_size;    // 任务链表长
    int shutdown;     // 关机模式
    int started;
}threadpool_t;

typedef enum{
    tp_invalid = -1,
    tp_lock_fail = -2,
    tp_already_shutdown = -3,
    tp_cond_broadcast = -4,
    tp_thread_fail = -5,
}threadpool_error_t;














#endif //SRC_THREADPOLL_H
