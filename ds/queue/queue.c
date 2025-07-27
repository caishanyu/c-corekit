/*
    Include Files
*/

#include "queue.h"

/*
    Typedef
*/

// 队列结构
struct queue
{
    dlist *dl;
};

/*
    Functions
*/

// 创建队列
static queue* _queue_create(IN queue_show_func func)
{
    queue *q = (queue*)malloc(sizeof(queue));
    if(NULL == q)
    {
        DBG("malloc queue space fail");
        return NULL;
    }

    q->dl = dlist_create(func);
    if(NULL == q->dl)
    {
        DBG("malloc base dlist fail");
        free(q);
        return NULL;
    }

    return q;
}

// 销毁队列
static STATUS _queue_destroy(IN queue *q)
{
    STATUS rv = dlist_destroy(q->dl);
    if(OK != rv)
    {
        DBG("destroy base dlist fail");
        return rv;
    }
    free(q);
    return OK;
}

// 入队
static inline STATUS _queue_push(IN queue *q, IN void *data)
{
    return dlist_append_tail(q->dl, data);
}

// 出队
static inline STATUS _queue_pop(IN queue *q, OUT void *data, IN unsigned int len)
{
    STATUS rv = dlist_get_head(q->dl, data, len);
    return rv | dlist_remove_head(q->dl);
}

// 获取队头
static inline STATUS _queue_top(IN queue *q, OUT void *data, IN unsigned int len)
{
    return dlist_get_head(q->dl, data, len);
}

// 获取队列长度
static inline STATUS _queue_get_size(IN queue *q, OUT unsigned int *len)
{
    return dlist_get_size(q->dl, len);
}

// 打印队列
static inline STATUS _queue_display(IN queue *q)
{
    return dlist_display(q->dl, DLIST_ORDER);
}

#if QUEUE_TEST

static void test_show_func(void* data)
{
    printf("%d", *((int*)data));
}

void queue_test()
{
#if CMOCKA_TEST
    queue *q = NULL;
    int a[5] = {0,1,2,3,4};
    unsigned int size = sizeof(int);
    int data = 0;
    unsigned int len = 0;

    q = queue_create(test_show_func);
    assert_non_null(q);

    queue_display(q);

    assert_int_not_equal(OK, queue_push(NULL, &a[0]));
    assert_int_not_equal(OK, queue_pop(NULL, &data, size));
    assert_int_not_equal(OK, queue_pop(q, NULL, size));
    assert_int_not_equal(OK, queue_pop(q, &data, 0));
    assert_int_not_equal(OK, queue_top(q, &data, size));
    assert_int_not_equal(OK, queue_top(q, NULL, size));
    assert_int_not_equal(OK, queue_top(NULL, &data, size));
    assert_int_not_equal(OK, queue_top(q, &data, 0));

    assert_return_code(OK, queue_push(q, &a[0]));
    assert_return_code(OK, queue_push(q, &a[1]));
    assert_return_code(OK, queue_push(q, &a[2]));
    assert_return_code(OK, queue_push(q, &a[3]));
    assert_return_code(OK, queue_push(q, &a[4]));

    queue_display(q);

    assert_return_code(OK, queue_pop(q, &data, size));
    assert_int_equal(0, data);
    assert_return_code(OK, queue_pop(q, &data, size));
    assert_int_equal(1, data);
    assert_return_code(OK, queue_pop(q, &data, size));
    assert_int_equal(2, data);
    
    assert_return_code(OK, queue_get_size(q, &len));
    assert_int_equal(2, len);

    assert_return_code(OK, queue_top(q, &data, size));
    assert_int_equal(3, data);

    assert_return_code(OK, queue_pop(q, &data, size));
    assert_int_equal(3, data);
    assert_return_code(OK, queue_pop(q, &data, size));
    assert_int_equal(4, data);

    assert_int_equal(OK, queue_destroy(q));
#endif
}

#endif

/*
    Variables
*/

// 队列操作集合
queue_ops queue_operations = {
    .queue_create = _queue_create,
    .queue_destroy = _queue_destroy,
    .queue_push = _queue_push,
    .queue_pop = _queue_pop,
    .queue_top = _queue_top,
    .queue_get_size = _queue_get_size,
    .queue_display = _queue_display,
};