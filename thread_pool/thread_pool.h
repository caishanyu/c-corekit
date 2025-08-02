#ifndef _THREAD_POOL_H
#define _THREAD_POOL_H

/*
    Include files
*/

#include <pthread.h>
#include "def.h"

/*
    Defines
*/

#define THREAD_COUNT_MAX    (50)
#define THREAD_QUEUE_SIZE_MAX   (50)

/*
    typedef
*/

typedef struct thread_pool_t thread_pool_t;

typedef void (*task_func)(void*);

typedef struct thread_pool_ops
{
    // 创建
    thread_pool_t* (*thread_pool_create)(unsigned int, unsigned int);
    // 添加任务
    STATUS (*thread_pool_add_task)(thread_pool_t*, task_func, void*);
    // 销毁
    STATUS (*thread_pool_destroy)(thread_pool_t*);
}thread_pool_ops;

/*
    extern symbols
*/

extern thread_pool_ops thread_pool_operations;

/*
    functions
*/

// 创建线程池
static inline thread_pool_t* thread_pool_create(
    IN unsigned int thread_count,
    IN unsigned int queue_size
)
{
    return thread_pool_operations.thread_pool_create(thread_count, queue_size);
}

// 往线程池添加任务
static inline STATUS thread_pool_add_task(
    IN thread_pool_t *pool,
    IN task_func func,
    IN void *args
)
{
    return thread_pool_operations.thread_pool_add_task(pool, func, args);
}

// 销毁线程池
static inline STATUS thread_pool_destroy(
    IN thread_pool_t *pool
)
{
    return thread_pool_operations.thread_pool_destroy(pool);
}

// 测试接口
#if THREAD_POOL_TEST
void thread_pool_test();
#endif

#endif