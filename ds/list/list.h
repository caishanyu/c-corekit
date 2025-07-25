#ifndef _LIST_H_
#define _LIST_H_

/*
    Include Files
*/
#include <pthread.h>
#include "def.h"

/*
    typedef
*/

typedef struct _dlist dlist;  // 隐藏成员

// 函数指针
typedef void (*dlist_show_func)(void *data);

// 链表操作
typedef struct _dlist_ops
{
    dlist* (*dlist_create)(unsigned int, dlist_show_func);   // 创建
    void (*dlist_destroy)(dlist*);          // 销毁
}dlist_ops;

/*
    Extern Symbol
*/

extern dlist_ops dlist_operations;

/*
    API
*/

// 创建链表
static inline dlist* dlist_create(
    unsigned int capacity,
    dlist_show_func show_func
)
{
    return dlist_operations.dlist_create(capacity, show_func);
}

// 销毁链表
static inline void dlist_destroy(dlist *dl)
{
    dlist_operations.dlist_destroy(dl);
}

#endif