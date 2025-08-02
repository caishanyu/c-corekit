/*
    Include files
*/

#include "thread_pool.h"

/*
    Typedef
*/

typedef void (*task_func)(void*);

// 任务节点结构定义
typedef struct task_s
{
    task_func func;    // 任务函数
    void *args; // 输入参数
}task_t;

// 线程池结构定义
struct thread_pool_t
{
    pthread_mutex_t lock;       // 互斥量
    pthread_cond_t notify;      // 条件变量

    pthread_t *thread_arr;      // 线程数组
    unsigned int thread_count;  // 线程数量

    task_t *task_queue;         // 任务队列，数组形式
    unsigned int queue_size;    // 任务队列容量
    unsigned int head;
    unsigned int tail;
    unsigned int task_count;

    bool shutdown_flag;         // 线程池销毁标志
};

/*
    function
*/

// 线程工作函数
static void* thread_worker(void *param)
{
    thread_pool_t *thread_pool = (thread_pool_t*)param;
    task_t task = {0};

    while(1)
    {
        pthread_mutex_lock(&thread_pool->lock); // 上锁

        // 等待任务或者关闭
        while(0 == thread_pool->task_count && false == thread_pool->shutdown_flag)
        {
            pthread_cond_wait(&thread_pool->notify, &thread_pool->lock);    // 必须使用while，防止异常唤醒
        }

        // 处理关闭请求
        if(true == thread_pool->shutdown_flag)
        {
            pthread_mutex_unlock(&thread_pool->lock);
            pthread_exit(NULL);
        }

        // 执行到这里，应从队列中取出任务执行
        // 获取队头
        task.func = thread_pool->task_queue[thread_pool->head].func;
        task.args = thread_pool->task_queue[thread_pool->head].args;
        // 修改队列
        thread_pool->head = (thread_pool->head + 1) % thread_pool->queue_size;
        thread_pool->task_count -= 1;

        pthread_mutex_unlock(&thread_pool->lock);

        // 执行任务
        task.func(task.args);
    }

    return NULL;
}

// 创建线程池
static thread_pool_t* _thread_pool_create(
    IN unsigned int thread_count,
    IN unsigned int queue_size
)
{
    thread_pool_t *ret = NULL;
    unsigned int i = 0;
    unsigned int j = 0;

    // 参数检查
    if(unlikely(thread_count > THREAD_COUNT_MAX || queue_size > THREAD_QUEUE_SIZE_MAX || thread_count <= 0))
    {
        DBG("invalid param");
        return NULL;
    }

    // 申请线程池变量空间
    ret = (thread_pool_t*)malloc(sizeof(thread_pool_t));
    if(unlikely(!ret))
    {
        DBG("malloc space of thread_pool fail");
        goto error;
    }
    memset(ret, 0, sizeof(thread_pool_t));

    // 申请线程数组空间
    ret->thread_arr = (pthread_t*)malloc(sizeof(pthread_t)*thread_count);
    if(!ret->thread_arr)
    {
        DBG("malloc space of threads array fail");
        goto error;
    }
    memset(ret->thread_arr, 0, sizeof(pthread_t)*thread_count);
    ret->thread_count = thread_count;

    // 申请任务队列数组空间
    ret->task_queue = (task_t*)malloc(sizeof(task_t)*queue_size);
    if(unlikely(!ret->task_queue))
    {
        DBG("malloc space of task queue fail");
        goto error;
    }
    memset(ret->task_queue, 0, sizeof(task_t)*queue_size);
    ret->head = 0;
    ret->tail = 0;
    ret->task_count = 0;
    ret->queue_size = queue_size;

    // 初始化同步机制
    if(unlikely(0 != pthread_mutex_init(&ret->lock, NULL)))
    {
        DBG("init mutex fail");
        goto error;
    }
    if(unlikely(0 != pthread_cond_init(&ret->notify, NULL)))
    {
        DBG("init cond fail");
        goto error;
    }

    // 创建线程，进入工作函数
    for(i = 0; i < thread_count; ++ i)
    {
        if(unlikely(0 != pthread_create(&ret->thread_arr[i], NULL, thread_worker, NULL)))
        {
            DBG("create thread %d fail", i);

            // 终止已经创建的线程
            for(j = 0; j < i; ++ j)
                pthread_cancel(ret->thread_arr[j]);

            goto error;
        }
    }

    ret->shutdown_flag = 0;

    return ret;

error:

    pthread_mutex_destroy(&ret->lock);
    pthread_cond_destroy(&ret->notify);
    if(ret->thread_arr) free(ret->thread_arr);
    if(ret->task_queue) free(ret->task_queue);
    if(ret) free(ret);

    return NULL;
}

// 往线程池添加任务
static STATUS _thread_pool_add_task(
    IN thread_pool_t *pool,
    IN task_func func,
    IN void *args
)
{
    if(unlikely(!pool || !func))
    {
        return ERR_BAD_PARAM;
    }

    pthread_mutex_lock(&pool->lock);

    // 检查队列，已满时直接返回错误
    if(pool->task_count == pool->queue_size)
    {
        pthread_mutex_unlock(&pool->lock);
        return ERR_THREAD_POOL_TASK_QUEUE_FULL;
    }

    // 添加任务到队列尾部
    pool->task_queue[pool->tail].func = func;
    pool->task_queue[pool->tail].args = args;
    pool->tail = (pool->tail + 1) % pool->queue_size;
    pool->task_count += 1;

    // signal
    pthread_cond_signal(&pool->notify);
    pthread_mutex_unlock(&pool->lock);

    return OK;
}

// 销毁线程池
static STATUS _thread_pool_destroy(
    IN thread_pool_t *pool
)
{
    unsigned int i = 0;

    if(unlikely(!pool))
    {
        return ERR_BAD_PARAM;
    }

    pthread_mutex_lock(&pool->lock);
    pool->shutdown_flag = true;

    // 唤醒所有线程
    pthread_cond_broadcast(&pool->notify);
    
    pthread_mutex_unlock(&pool->lock);

    // 等待所有线程退出
    for(i = 0; i < pool->thread_count; ++ i)
    {
        pthread_join(pool->thread_arr[i], NULL);
    }

    free(pool->thread_arr);
    free(pool->task_queue);
    free(pool);

    return OK;
}