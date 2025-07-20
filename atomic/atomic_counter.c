#include "atomic.h"

atomic_long counter = ATOMIC_VAR_INIT(0);  // 静态初始化

static void* thread_func(void* arg) {
    for (int i = 0; i < 1000000; i++) {
        // 原子自增操作
        atomic_fetch_add_explicit(&counter, 1, memory_order_relaxed);
    }
    return 0;
}

void test_atomic_counter(void **state)
{
    (void)state;
#if SELF_TEST
    pthread_t t1 = 0;
    pthread_t t2 = 0;
    pthread_create(&t1, NULL, thread_func, NULL);
    pthread_create(&t2, NULL, thread_func, NULL);
    
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    
    assert_int_equal(counter, 1000000*2);
#endif
    return 0;
}
