#ifndef _ATOMIC_H_
#define _ATOMIC_H_

#include "def.h"

#include <stdatomic.h>
#include <stdbool.h>
#include <threads.h> // C11标准支持的线程库
#include <pthread.h>
#include <stdio.h>

/*========== type ==========*/

// 自旋锁结构体
typedef struct {
    atomic_flag lock_flag;  // 0-锁空闲，1-锁被占用
} spinlock_t;

/*========== func ==========*/

/* 测试原子计数器 */
int test_atomic_counter();

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
int test_aotmic_spinlock();

#endif