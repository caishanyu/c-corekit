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

// 遍历顺序
typedef enum
{
    DLIST_ORDER,    // 顺序
    DLIST_REVERSE,  // 逆序
}DLIST_ORDER_TYPE;

// 链表操作
typedef struct _dlist_ops
{
    dlist* (*dlist_create)(dlist_show_func);   // 创建
    STATUS (*dlist_destroy)(dlist*);          // 销毁
    STATUS (*dlist_display)(dlist*, DLIST_ORDER_TYPE);            // 打印链表
    /* get */
    STATUS (*dlist_get_size)(dlist*, unsigned int *);   // 获取长度
    STATUS (*dlist_get_data)(dlist*, unsigned int, void*, unsigned int);    // 获取元素
    /* add */
    STATUS (*dlist_insert)(dlist*, unsigned int, void*);    // 插入节点
    /* del */
    STATUS (*dlist_remove)(dlist*, unsigned int);   // 移除元素
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
    IN dlist_show_func show_func
)
{
    return dlist_operations.dlist_create(show_func);
}

// 销毁链表
static inline STATUS dlist_destroy(IN dlist *dl)
{
    return dlist_operations.dlist_destroy(dl);
}

// 打印链表
static inline STATUS dlist_display(
    IN dlist *dl, 
    IN DLIST_ORDER_TYPE order
)
{
    return dlist_operations.dlist_display(dl, order);
}

// 获取当前长度
static inline STATUS dlist_get_size(
    IN dlist *dl,
    OUT unsigned int *size
)
{
    return dlist_operations.dlist_get_size(dl, size);
}

// 获取idx位置元素
static inline STATUS dlist_get_data(
    IN dlist* dl,
    IN unsigned int idx,
    IN OUT void *data,
    IN unsigned int len
)
{
    return dlist_operations.dlist_get_data(dl, idx, data, len);
}

// 获取第一个数据元素
static inline STATUS dlist_get_head(
    IN dlist *dl,
    IN OUT void *data,
    IN unsigned int len
)
{
    return dlist_operations.dlist_get_data(dl, 1, data, len);
}

// 获取最后一个数据元素
static inline STATUS dlist_get_tail(
    IN dlist *dl,
    IN OUT void *data,
    IN unsigned int len
)
{
    unsigned int dl_len = 0;
    STATUS val = dlist_operations.dlist_get_size(dl, &dl_len);
    return val || dlist_operations.dlist_get_data(dl, 1, data, len);
}

// 插入
static inline STATUS dlist_insert(
    IN dlist *dl,
    IN unsigned int idx,
    IN void *data
)
{
    return dlist_operations.dlist_insert(dl, idx, data);
}

// 插入链表尾部
static inline STATUS dlist_append_tail(
    IN dlist *dl,
    IN void *data
)
{
    unsigned int len = 0;
    STATUS val = dlist_operations.dlist_get_size(dl, &len);
    return val || dlist_operations.dlist_insert(dl, len+1, data);
}

// 插入链表头部
static inline STATUS dlist_append_head(
    IN dlist *dl,
    IN void *data
)
{
    return dlist_operations.dlist_insert(dl, 1, data);
}

// 移除idx位置元素
static inline STATUS dlist_remove(
    IN dlist *dl,
    IN unsigned int idx
)
{
    return dlist_operations.dlist_remove(dl, idx);
}

// 移除链表头元素
static inline STATUS dlist_remove_head(
    IN dlist *dl
)
{
    return dlist_operations.dlist_remove(dl, 1);
}

// 移除链表尾元素
static inline STATUS dlist_remove_tail(
    IN dlist *dl
)
{
    unsigned int len = 0;
    STATUS val = dlist_operations.dlist_get_size(dl, &len);
    return val || dlist_operations.dlist_remove(dl, len);
}

#endif