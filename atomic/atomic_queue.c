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
        // 别的线程可能此时已完成入队
        // head->A->B->C(tail, q->tail)->NULL(next)     读tail和next时的队列
        // head->A->B->C(tail)->D(q->tail)->NULL        执行下面判断前一时刻
        // 若是上述情况，重新拿tail和next，更新到最新状态
        if (tail != atomic_load_explicit(&q->tail, memory_order_relaxed)) 
        {
            continue;
        }
        
        // 3. 如果尾指针不是真正的尾部，帮助其他线程推进尾指针
        // 别的线程可能此时添加了节点（修改了队尾节点next），但是没更新tail指针，即另一个线程跳过了步骤3，执行了步骤4的if，还未执行5
        // head->A->B->C(tail, q->tail)->D(next)-> NULL 步骤2后状态
        // 若是上述情况，CAS将q->tail更新到D，然后重新拿tail和next
        if (next != NULL) 
        {
            atomic_compare_exchange_weak_explicit(&q->tail, &tail, next, memory_order_release, memory_order_relaxed);
            continue;
        }
        
        // 4. 尝试将新节点添加到尾部
        // head->A->B->C->D(q->tail, tail)->NULL(next)，next == tail->next，判断成立
        // 将new_node插到队尾，变成 head->A->B->C->D(q->tail, tail)->new_node(tail->next)->NULL
        if (atomic_compare_exchange_weak_explicit(&tail->next, &next, new_node, memory_order_release, memory_order_relaxed))
        {
            // 5. 尝试更新尾指针到新节点
            // head->A->B->C->D(q->tail, tail)->new_node(tail->next)
            // q->tail == tail成立，将q->tail推进到new_node
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
        // head(q->head)->A->B->C(q->tail, tail)->NULL(next)
        head = atomic_load_explicit(&q->head, memory_order_acquire);
        tail = atomic_load_explicit(&q->tail, memory_order_acquire);
        next = atomic_load_explicit(&head->next, memory_order_acquire);
        
        // 2. 检查头指针是否被其他线程修改
        // head(q->head)->A->B->C(q->tail, tail)->NULL(next)
        if (head != atomic_load_explicit(&q->head, memory_order_relaxed)) {
            continue;
        }
        
        // 3. 队列为空的情况
        // 对于本实现，包含一个哨兵节点，空队列即：head(q->head, q->tail, tail)->NULL(next)
        if (head == tail) 
        {
            if (next == NULL) {
                return NULL; // 队列为空
            }
            // 帮助推进尾指针
            // 走到这里说明有别的线程入队，但还没有修改tail，队列此时为
            // head(q->head, q->tail, tail)->A(next)->NULL
            // 那么尝试将q->tail推进到next上
            atomic_compare_exchange_weak_explicit(&q->tail, &tail, next, memory_order_release, memory_order_relaxed);
        } 
        else 
        {
            // 4. 读取数据
            // head->A(next)->B(tail)->NULL
            data = next->data;
            
            // 5. 尝试移动头指针到下一个节点
            // head(q->head)->A(next)->B(tail)->NULL
            // 这里相当于将A当作新的哨兵节点，data域完全不重要
            // A(q->head)->B(q->tail)->NULL
            if (atomic_compare_exchange_weak_explicit(&q->head, &head, next, memory_order_release, memory_order_relaxed)) 
            {
                // 安全释放旧头节点（实际应用中需要更安全的内存回收机制）
                // 仅有一个线程可以进入if body，执行free
                free(head);
                return data;
            }
        }
    }
}

// 销毁队列
STATUS queue_close(LockFreeQueue* q)
{
    if(!q)
    {
        return ERROR;
    }
    
    while(NULL != dequeue(q))   {}

    free(q->head);  // 释放哨兵节点

    return OK;
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
    clock_gettime(CLOCK_MONOTONIC, &start);
    
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

    queue_close(&q);
    
    clock_gettime(CLOCK_MONOTONIC, &end);
    
    double elapsed = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    
    printf("MPMC performance: %.2f ops/sec\n", (num_threads * 1000) / elapsed);

}

#endif

void test_lock_free_queue(void **state)
{
    (void)state;
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

    assert_ptr_equal(q.head, q.tail);
    assert_null(q.tail->next);

    queue_close(&q);
    
    // 性能测试
    printf("\nPerformance testing with 4 threads (2 producers, 2 consumers)...\n");
    performance_test();
#endif
}