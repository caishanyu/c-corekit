#include "atomic.h"

// 动态初始化
void rwspinlock_init(rw_spinlock_t *lock)
{
    if(likely(lock))
    {
        atomic_store_explicit(&lock->reader_count, 0, memory_order_relaxed);
        atomic_store_explicit(&lock->writer, 0, memory_order_relaxed);
    }
}

// 获取读锁
void rwspinlock_r_take(rw_spinlock_t *lock)
{
    if(unlikely(!lock))
    {
        return;
    }

    while(1)
    {
        // 是否写独占
        if(atomic_load_explicit(&lock->writer, memory_order_acquire))
        {
            continue;
        }

        // 读者+1
        atomic_fetch_add_explicit(&lock->reader_count, 1, memory_order_release);

        // 如果写者拿到锁，读者退让
        if(atomic_load_explicit(&lock->writer, memory_order_relaxed))
        {
            atomic_fetch_sub_explicit(&lock->reader_count, 1, memory_order_release);
            continue;
        }

        break;
    }

    return;
}

// 释放读锁
void rwspinlock_r_give(rw_spinlock_t *lock)
{
    if(unlikely(!lock))
    {
        return;
    }

    atomic_fetch_sub_explicit(&lock->reader_count, 1, memory_order_release);

    return;
}

// 获取写锁
void rwspinlock_w_take(rw_spinlock_t *lock)
{
    if(unlikely(!lock))
    {
        return;
    }

    while(1)
    {
        // 等待读者退出
        if(atomic_load_explicit(&lock->reader_count, memory_order_acquire))
        {
            continue;
        }
        
        // 尝试获取写锁
        int expect = 0;
        if(atomic_compare_exchange_weak_explicit(&lock->writer, &expect, 1, memory_order_acq_rel, memory_order_relaxed))
        {
            break;
        }
    }

    

    return;
}

// 释放写锁
void rwspinlock_w_give(rw_spinlock_t *lock)
{
    if(unlikely(!lock))
    {
        return;
    }

    atomic_store_explicit(&lock->writer, 0, memory_order_release);

    return;
}