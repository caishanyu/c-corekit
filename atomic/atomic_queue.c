#include "atomic.h"

static Node* create_node(void *data)
{
    Node* node = (Node*)malloc(sizeof(Node));
    if(!node)
    {
        return NULL;
    }
    
    node->data = data;
    atomic_store_explicit(&node->next, NULL, memory_order_relaxed);

    return node;
}

STATUS queue_init(LockFreeQueue* q) {
    if(!q)
    {
        return ERROR;
    }
    
    // 创建哨兵节点
    Node* sentinel = create_node(NULL);
    if(!sentinel)
    {
        return ERROR;
    }

    // 修改指针
    atomic_store_explicit(&q->head, sentinel, memory_order_relaxed);
    atomic_store_explicit(&q->tail, sentinel, memory_order_relaxed);

    return OK;
}

// 入队
STATUS enqueue(LockFreeQueue *q, void *data)
{
    Node *new_node = NULL;
    Node *tail = NULL;
    Node *next = NULL;

    if(!q)
    {
        return ERROR;
    }

    new_node = create_node(data);
    if(!new_node)
    {
        return ERROR;
    }

    while(1)
    {
        // 1. 读取当前尾指针和它的下一个节点
        tail = atomic_load_explicit(&q->tail, memory_order_acquire);
        next = atomic_load_explicit(&tail->next, memory_order_acquire);
        
        // 2. 检查尾指针是否被其他线程修改
        if (tail != atomic_load_explicit(&q->tail, memory_order_relaxed)) 
        {
            continue;
        }
        
        // 3. 如果尾指针不是真正的尾部，帮助其他线程推进尾指针
        if (next != NULL) 
        {
            atomic_compare_exchange_weak_explicit(&q->tail, &tail, next, memory_order_release, memory_order_relaxed);
            continue;
        }
        
        // 4. 尝试将新节点添加到尾部
        if (atomic_compare_exchange_weak_explicit(&tail->next, &next, new_node, memory_order_release, memory_order_relaxed))
        {
            // 5. 尝试更新尾指针到新节点
            atomic_compare_exchange_weak_explicit(&q->tail, &tail, new_node, memory_order_release, memory_order_relaxed);
            return OK;
        }
    }
}

// 出队操作
void* dequeue(LockFreeQueue* q) {
    Node* head = NULL;
    Node* tail = NULL;
    Node* next = NULL;
    void* data = NULL;

    if(!q)
    {
        return NULL;
    }
    
    while (1) 
    {
        // 1. 读取头指针、尾指针和头节点的下一个节点
        head = atomic_load_explicit(&q->head, memory_order_acquire);
        tail = atomic_load_explicit(&q->tail, memory_order_acquire);
        next = atomic_load_explicit(&head->next, memory_order_acquire);
        
        // 2. 检查头指针是否被其他线程修改
        if (head != atomic_load_explicit(&q->head, memory_order_relaxed)) {
            continue;
        }
        
        // 3. 队列为空的情况
        if (head == tail) 
        {
            if (next == NULL) {
                return NULL; // 队列为空
            }
            // 帮助推进尾指针
            atomic_compare_exchange_weak_explicit(&q->tail, &tail, next, memory_order_release, memory_order_relaxed);
        } 
        else 
        {
            // 4. 读取数据
            data = next->data;
            
            // 5. 尝试移动头指针到下一个节点
            if (atomic_compare_exchange_weak_explicit(&q->head, &head, next, memory_order_release, memory_order_relaxed)) 
            {
                // 安全释放旧头节点（实际应用中需要更安全的内存回收机制）
                free(head);
                return data;
            }
        }
    }
}

#if SELF_TEST

// 测试函数：生产者线程
void* producer(void* arg) {
    LockFreeQueue* q = (LockFreeQueue*)arg;
    for (int i = 0; i < 1000; i++) {
        int* value = malloc(sizeof(int));
        *value = i;
        enqueue(q, value);
    }
    return NULL;
}

// 测试函数：消费者线程
void* consumer(void* arg) {
    LockFreeQueue* q = (LockFreeQueue*)arg;
    int count = 0;
    while (count < 1000) {
        int* value = (int*)dequeue(q);
        if (value) {
            free(value);
            count++;
        }
    }
    return NULL;
}

// 性能测试
void performance_test() {
    LockFreeQueue q;
    queue_init(&q);
    
    const int num_threads = 4;
    pthread_t threads[num_threads];
    
    struct timespec start, end;
    //clock_gettime(CLOCK_MONOTONIC, &start);
    
    // 创建生产者线程
    for (int i = 0; i < num_threads/2; i++) {
        pthread_create(&threads[i], NULL, producer, &q);
    }
    
    // 创建消费者线程
    for (int i = num_threads/2; i < num_threads; i++) {
        pthread_create(&threads[i], NULL, consumer, &q);
    }
    
    // 等待所有线程完成
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }
    
    //clock_gettime(CLOCK_MONOTONIC, &end);
    
    //double elapsed = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    
    //printf("MPMC performance: %.2f ops/sec\n", (num_threads * 1000) / elapsed);
}

#endif

int test_lock_free_queue()
{

#if SELF_TEST
    // 基本功能测试
    LockFreeQueue q;
    queue_init(&q);
    
    printf("Testing basic queue functionality...\n");
    
    // 入队测试
    for (int i = 0; i < 5; i++) {
        int* value = malloc(sizeof(int));
        *value = i;
        enqueue(&q, value);
        printf("Enqueued: %d\n", *value);
    }
    
    // 出队测试
    for (int i = 0; i < 5; i++) {
        int* value = (int*)dequeue(&q);
        if (value) {
            printf("Dequeued: %d\n", *value);
            free(value);
        }
    }
    
    // 性能测试
    printf("\nPerformance testing with 4 threads (2 producers, 2 consumers)...\n");
    performance_test();

#endif
    return 0;
}