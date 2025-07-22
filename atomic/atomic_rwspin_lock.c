#include "atomic.h"

#define WRITER_BIT  (1U << 31)   // 高比特位用于写者标志
#define READER_MASK (~WRITER_BIT) // 低31位用于读者计数

// 初始化读写锁
void rwlock_init(rw_spinlock_t *lock)
{
    atomic_init(&lock->state, 0);
    atomic_flag_clear(&lock->writer_lock); // 初始化为未锁定状态
}

// 读锁定函数
void rwlock_rdlock(rw_spinlock_t *lock) 
{
    unsigned int old_state, new_state;
    while (1) 
    {
        // 加载当前状态（宽松内存序：无需同步）
        old_state = atomic_load_explicit(&lock->state, memory_order_relaxed);
        
        // 检查是否有写者持有或等待（检查高比特位）
        if (old_state & WRITER_BIT) {
            thrd_yield(); // 让出CPU避免忙等
            continue;
        }
        
        // 尝试增加读者计数
        new_state = old_state + 1;
        
        // CAS操作：若状态未变则更新（获取内存序确保临界区顺序）
        if (atomic_compare_exchange_weak_explicit(&lock->state, &old_state, new_state, memory_order_acquire, memory_order_relaxed)) 
        {
            break; // 成功获取读锁
        }
    }
}

// 读解锁函数
void rwlock_rdunlock(rw_spinlock_t *lock) 
{
    // 减少读者计数（释放内存序确保临界区操作完成）
    atomic_fetch_sub_explicit(&lock->state, 1, memory_order_release);
}

// 写锁定函数
void rwlock_wrlock(rw_spinlock_t *lock) 
{
    // 先获取写者互斥锁（防止多个写者竞争）
    // 0->1，则可以退出循环
    while (atomic_flag_test_and_set_explicit(&lock->writer_lock, memory_order_acquire)) 
    {
        thrd_yield(); // 让出CPU避免忙等
    }
    
    unsigned int old_state;
    // 设置写者标志（高比特位），并等待现有读者退出
    do 
    {
        old_state = atomic_load_explicit(&lock->state, memory_order_relaxed);
        // 循环直到无读者且设置写者标志成功
    } while((old_state & READER_MASK) != 0 || 
        !atomic_compare_exchange_weak_explicit(&lock->state, &old_state, old_state | WRITER_BIT, memory_order_acquire, memory_order_relaxed));
}

// 写解锁函数
void rwlock_wrunlock(rw_spinlock_t *lock) 
{
    // 清除写者标志（释放内存序确保写操作完成）
    atomic_fetch_and_explicit(&lock->state, ~WRITER_BIT, memory_order_release);
    // 释放写者互斥锁
    atomic_flag_clear_explicit(&lock->writer_lock, memory_order_release);
}