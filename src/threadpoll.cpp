<<<<<<< HEAD
//
// Created by yu on 2018/5/18.
//

#include "threadpoll.h"

void threadpoll_destory(threadpool_t* poll,int greaceful){
    if(!poll){
        return tp_invalid;
    }
    if(pthread_mutex_lock(&(poll->lock)))
        return tp_lock_fail;

    int err=0;
    do
}


threadpool_t* threadpool_init(int thread_num){
    //分配线程池
    threadpool_t* poll=(threadpool_t*)malloc(sizeof(threadpool_t));

    // threads指针指向线程数组（存放tid），数组大小即为线程数
    pool->thread_count = 0;
    pool->queue_size = 0;
    pool->shutdown = 0;
    pool->started = 0;

    poll->threads=(pthread_t*)malloc(siof(pthread_t)*thread_num);

    // 分配并初始化task头结点
    poll->head=(task_t*)malloc(sizeof(task_t));
    if((poll->head== nullptr) || (poll->threads== nullptr)){
        perror("poll error");
        return nullptr;
    }

    //初始化锁
    if(pthread_mutex_init(&(poll->lock), nullptr)){
        perror("pthread_mutex_init error");
        return nullptr;
    }

    //初始化条件变量
    if(pthread_cond_init(&(poll->cond), nullptr)){
        perror("pthread_cond_init error");
        return nullptr;
    }

    //创建线程
    for(int i=0;i<thread_num;++i){
        if(pthread_create(&(poll->threads[i], NULL,threadpoll_worder,(void*)poll))){
            threadpool_destory(poll,0);
            perror("create pthread error");
            retrun NULL;
        }
        poll->thread_count++;
        poll->started++;
    }


=======
//
// Created by yu on 2018/5/18.
//

#include "threadpoll.h"

void threadpoll_destory(threadpool_t* poll,int greaceful){
    if(!poll){
        return tp_invalid;
    }
    if(pthread_mutex_lock(&(poll->lock)))
        return tp_lock_fail;

    int err=0;
    do
}


threadpool_t* threadpool_init(int thread_num){
    //分配线程池
    threadpool_t* poll=(threadpool_t*)malloc(sizeof(threadpool_t));

    // threads指针指向线程数组（存放tid），数组大小即为线程数
    pool->thread_count = 0;
    pool->queue_size = 0;
    pool->shutdown = 0;
    pool->started = 0;

    poll->threads=(pthread_t*)malloc(siof(pthread_t)*thread_num);

    // 分配并初始化task头结点
    poll->head=(task_t*)malloc(sizeof(task_t));
    if((poll->head== nullptr) || (poll->threads== nullptr)){
        perror("poll error");
        return nullptr;
    }

    //初始化锁
    if(pthread_mutex_init(&(poll->lock), nullptr)){
        perror("pthread_mutex_init error");
        return nullptr;
    }

    //初始化条件变量
    if(pthread_cond_init(&(poll->cond), nullptr)){
        perror("pthread_cond_init error");
        return nullptr;
    }

    //创建线程
    for(int i=0;i<thread_num;++i){
        if(pthread_create(&(poll->threads[i], NULL,threadpoll_worder,(void*)poll))){
            threadpool_destory(poll,0);
            perror("create pthread error");
            retrun NULL;
        }
        poll->thread_count++;
        poll->started++;
    }


>>>>>>> 1245ba3f16ae99ced5c12db10ef08baba47f04dc
}