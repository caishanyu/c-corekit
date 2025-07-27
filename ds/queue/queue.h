#ifndef _QUEUE_H_
#define _QUEUE_H_

/*
    Include Files
*/

#include "dlist/dlist.h"

/*
    Typedef
*/

// 队列声明
typedef struct queue queue;

// 函数指针
typedef dlist_show_func queue_show_func;

// 队列操作结构
typedef struct _queue_ops
{
    queue* (*queue_create)(queue_show_func);    // 创建队列
    STATUS (*queue_destroy)(queue*);    // 销毁队列
    STATUS (*queue_push)(queue*, void*);    // 入队
    STATUS (*queue_pop)(queue*, void*, unsigned int); // 出队
    STATUS (*queue_display)(queue*);    // 打印队列
    STATUS (*queue_top)(queue*, void*, unsigned int); // 获取队头
    STATUS (*queue_get_size)(queue*, unsigned int *);   // 获取队列长度
}queue_ops;

/*
    Extern symbols
*/

extern queue_ops queue_operations;

/*
    Functions declaration
*/

// 创建队列
static inline queue* queue_create(IN queue_show_func func)
{
    return queue_operations.queue_create(func);
}

// 销毁队列
static inline STATUS queue_destroy(IN queue *q)
{
    if(!q)  return ERR_BAD_PARAM;
    return queue_operations.queue_destroy(q);
}

// 入队
static inline STATUS queue_push(
    IN queue *q,
    IN void *data
)
{
    if(!q)  return ERR_BAD_PARAM;
    return queue_operations.queue_push(q, data);
}

// 出队
static inline STATUS queue_pop(
    IN queue *q,
    OUT void *data,
    IN unsigned int len
)
{
    if(!q)  return ERR_BAD_PARAM;
    return queue_operations.queue_pop(q, data, len);
}

// 获取队头元素
static inline STATUS queue_top(
    IN queue *q,
    OUT void *data,
    IN unsigned int len
)
{
    if(!q)  return ERR_BAD_PARAM;
    return queue_operations.queue_top(q, data, len);
}

// 打印队列
static inline STATUS queue_display(IN queue *q)
{
    if(!q)  return ERR_BAD_PARAM;
    return queue_operations.queue_display(q);
}

// 获取队列长度
static inline STATUS queue_get_size(
    IN queue *q,
    OUT unsigned int *len
)
{
    if(!q)  return ERR_BAD_PARAM;
    return queue_operations.queue_get_size(q, len);
}

#if QUEUE_TEST
// 测试接口
void queue_test();
#endif

#endif