#ifndef _ATOMIC_H_
#define _ATOMIC_H_

#define _POSIX_C_SOURCE 200112L

#include "def.h"

#include <stdatomic.h>
#include <stdbool.h>
#include <threads.h> // C11标准支持的线程库
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/*========== type ==========*/

// 自旋锁结构体
typedef struct {
    atomic_flag lock_flag;  // 0-锁空闲，1-锁被占用
} spinlock_t;

// 无锁队列节点结构
typedef struct Node{
    void *data;
    _Atomic(struct Node*) next;
}Node;
// 无锁队列结构
typedef struct{
    _Atomic(Node*) head;
    _Atomic(Node*) tail;
}LockFreeQueue;

// 自旋读写锁结构
typedef struct {
    atomic_uint state;       // 原子状态：高1位=写者标志，低31位=读者计数
    atomic_flag writer_lock; // 写者互斥锁（自旋锁）
} rw_spinlock_t;

/*========== func ==========*/

/* 测试原子计数器 */
void test_atomic_counter(void **state);

/* spin_lock */
// 动态初始化函数
void spinlock_init(spinlock_t *lock);
// 获取锁（阻塞直到成功）
void spinlock_lock(spinlock_t *lock);
// 尝试获取锁（非阻塞）
bool spinlock_trylock(spinlock_t *lock);
// 释放锁
void spinlock_unlock(spinlock_t *lock);
// 测试原子自旋锁
void test_aotmic_spinlock(void **state);

/* lockFreeQueue */
// 初始化队列
STATUS queue_init(LockFreeQueue* q);
// 入队
STATUS enqueue(LockFreeQueue *q, void *data);
// 出队操作
void* dequeue(LockFreeQueue* q);
// 销毁队列
STATUS queue_close(LockFreeQueue* q);
// 测试
void test_lock_free_queue(void **state);

#endif