#include "atomic.h"
#include "def.h"

// 初始化自旋锁（静态初始化）
#define SPINLOCK_INIT { ATOMIC_FLAG_INIT }

// 动态初始化函数
void spinlock_init(spinlock_t *lock) {
    atomic_flag_clear_explicit(&lock->lock_flag, memory_order_release);
}

// 获取锁（阻塞直到成功）
void spinlock_lock(spinlock_t *lock) {
    // atomic_flag_test_and_set_explicit：读取当前值，设置为true，返回旧值
    // 0->1表示占到锁，退出while
    // memory_order_release 防止后续操作重排到获取之前
    while (atomic_flag_test_and_set_explicit(&lock->lock_flag, memory_order_acquire)) {
        // 优化：减少CPU压力
#if defined(__x86_64__) || defined(__i386__)
        __builtin_ia32_pause(); // x86架构的PAUSE指令
#elif defined(__aarch64__)
        __asm__ __volatile__("yield"); // ARM架构的YIELD指令
#else
        // 通用架构：插入编译屏障
        atomic_signal_fence(memory_order_acq_rel);
#endif
    }
}

// 尝试获取锁（非阻塞）
bool spinlock_trylock(spinlock_t *lock) {
    return !atomic_flag_test_and_set_explicit(&lock->lock_flag, memory_order_acquire);
}

// 释放锁
void spinlock_unlock(spinlock_t *lock) {
    // memory_order_release 防止前面操作重排到释放之后
    atomic_flag_clear_explicit(&lock->lock_flag, memory_order_release);
}

spinlock_t counter_lock = SPINLOCK_INIT;
int shared_counter = 0;

static int thread_func(void *arg) {
    for (int i = 0; i < 1000000; i++) {
        spinlock_lock(&counter_lock);
        shared_counter++; // 临界区操作
        spinlock_unlock(&counter_lock);
    }
    return 0;
}

void test_aotmic_spinlock(void **state)
{
    (void)state;    // 避免未使用警告
#if SELF_TEST
    thrd_t t1, t2;
    
    thrd_create(&t1, thread_func, NULL);
    thrd_create(&t2, thread_func, NULL);
    
    thrd_join(t1, NULL);
    thrd_join(t2, NULL);
    
    assert_int_equal(shared_counter, 1000000*2);
#endif
}
