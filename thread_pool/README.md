# thread_pool

**目录**

- [概述](#概述)
- [线程池原理](#线程池原理)
- [API说明](#api说明)
- [注意事项](#注意事项)

## 概述

该线程池库提供了一个高效的**任务调度**机制，允许使用者创建固定数量的工作线程，并**异步**执行任务

主要特性如下：

- **线程管理**：可配置工作线程数量，最大数量由`THREAD_COUNT_MAX`决定
- **任务队列**：由环形缓冲区实现的任务队列，最大容量由`THREAD_QUEUE_SIZE_MAX`决定
- **线程同步**：使用互斥锁和条件变量保证线程安全
- **优雅关闭**：支持安全关闭线程池，回收所有资源

## 线程池原理

线程池，顾名思义就是创建一组线程并统一管理，在需要使用线程时（一般是执行某项任务），调度线程进行执行

线程池由几个关键组件构成：

1. 工作线程：预先创建的一组线程，负责从任务队列中取出任务，并执行
2. 任务队列：一个先进先出(FIFO)队列，存储待处理任务
3. 互斥锁：保护共享资源的访问
4. 条件变量：协调线程间的任务通知机制

线程池的工作流程如下：

1. 初始化阶段：
    - 创建指定数量的工作线程
    - 初始化任务队列，可以使用环形缓冲区
    - 初始化同步机制，包括互斥锁初始化和条件变量初始化
2. 任务提交阶段：
    - 调用者通过`thread_pool_add_task`提交任务
    - 任务经过封装，加入队列尾部，`signal`条件变量
3. 任务执行阶段：
    - 处于`wait`状态的工作线程被唤醒，只有一个线程拿到锁并执行
    - 执行线程从队列头部取出任务，执行
    - 其余被唤醒的线程在`while`中检查条件，无任务则继续休眠
    - 线程进入空闲状态，等待下一个任务
4. 关闭阶段
    - 主线程设置关闭标志
    - 唤醒所有工作线程
    - 工作线程检查关闭标志，进行退出
    - 线程池等待所有工作线程结束
    - 释放所有资源

## API说明

### 创建线程池
```c
thread_pool_t* thread_pool_create(unsigned int thread_count, unsigned int queue_size);
```

- `thread_count`: 工作线程数量
- `queue_size`: 任务队列容量
- 返回: 成功返回线程池指针，失败返回`NULL`

### 添加任务

```c
STATUS thread_pool_add_task(thread_pool_t *pool, task_func func, void *args);
```

`pool`: 线程池指针
`func`: 任务函数指针（类型为`void (*)(void*)`）
`args`: 传递给任务函数的参数
- 返回: 成功返回`OK`，队列满返回`ERR_THREAD_POOL_TASK_QUEUE_FULL`。使用者需自行处理逻辑（比如`while`检查返回值）

### 销毁线程池

```c
STATUS thread_pool_destroy(thread_pool_t *pool);
```

- `pool`: 要销毁的线程池指针
- 返回: 成功返回`OK`

### 使用示例

```c
#include "thread_pool.h"

// 示例任务函数
void sample_task(void *arg) {
    int num = *(int *)arg;
    printf("Task %d processed by thread %lu\n", num, pthread_self());
    free(arg); // 清理参数
}

int main() {
    // 创建4线程+10容量的线程池
    thread_pool_t *pool = thread_pool_create(4, 10);
    
    // 添加20个任务
    for (int i = 0; i < 20; i++) {
        int *arg = malloc(sizeof(int));
        *arg = i;
        while (thread_pool_add_task(pool, sample_task, arg) != OK) {
            sched_yield(); // 队列满时让出CPU
        }
    }
    
    sleep(2); // 等待任务完成
    thread_pool_destroy(pool); // 销毁线程池
    return 0;
}
```

## 注意事项

1. 任务函数负责释放自己的参数内存
2. 队列满时添加任务会返回错误，调用者需处理重试逻辑
3. 销毁线程池会阻塞直到所有任务完成
4. 任务函数应避免长时间阻塞，以免影响其他任务执行
5. 任务不应依赖特定线程的执行顺序
