#include <stdatomic.h>
#include <pthread.h>
#include <stdio.h>

atomic_long counter = ATOMIC_VAR_INIT(0);  // 静态初始化

void* thread_func(void* arg) {
    for (int i = 0; i < 1000000; i++) {
        // 原子自增操作
        atomic_fetch_add_explicit(&counter, 1, memory_order_relaxed);
    }
    return 0;
}

int main() {
    pthread_t t1 = 0;
    pthread_t t2 = 0;
    pthread_create(&t1, NULL, thread_func, NULL);
    pthread_create(&t2, NULL, thread_func, NULL);
    
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    
    printf("Final count: %ld\n", atomic_load(&counter));
    return 0;
}
