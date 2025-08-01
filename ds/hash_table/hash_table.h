#ifndef _HASH_TABLE_H
#define _HASH_TABLE_H

/*
    Include files
*/

#include <stdbool.h>
#include "dlist/dlist.h"

/*
    typedefs
*/

// 哈希函数指针
typedef unsigned int (*hash_func)(void *data);
// 打印节点函数指针
typedef dlist_show_func hash_table_show_func;
// 数据比较函数指针
typedef bool (*cmp_func)(void *d1, void *d2);
// 哈希表声明，隐藏成员
typedef struct hash_table hash_table;
// 哈希表操作集合
typedef struct
{
    // 创建哈希表
    hash_table* (*hash_table_create)(unsigned int, hash_func, cmp_func, hash_table_show_func);
    // 销毁哈希表
    STATUS (*hash_table_destroy)(hash_table*);
    // 加入哈希表
    STATUS (*hash_table_insert)(hash_table*, void*);
    // 移除哈希表
    STATUS (*hash_table_remove)(hash_table*, void*);
    // 检查哈希表是否存在元素
    bool (*hash_table_contain)(hash_table*, void*);
}hash_table_ops;

/*
    Extern symbols
*/

extern hash_table_ops hash_table_operations;

/*
    Functions
*/

// 创建哈希表
static inline hash_table* hash_table_create(
    IN unsigned int bucket_size,
    IN hash_func hash,
    IN cmp_func cmp,
    IN hash_table_show_func show
)
{
    return hash_table_operations.hash_table_create(bucket_size, hash, cmp, show);
}

// 销毁哈希表
static inline STATUS hash_table_destroy(IN hash_table *hs)
{
    return hash_table_operations.hash_table_destroy(hs);
}

// 加入哈希表
static inline STATUS hash_table_insert(
    IN hash_table *hs,
    IN void *data
)
{
    return hash_table_operations.hash_table_insert(hs, data);
}

// 移除哈希表
static inline STATUS hash_table_remove(
    IN hash_table *hs,
    IN void *data
)
{
    return hash_table_operations.hash_table_remove(hs, data);
}

// 检查数据是否存在
static inline bool hash_table_contain(
    IN hash_table *hs,
    IN void *data
)
{
    return hash_table_operations.hash_table_contain(hs, data);
}

#if HASH_TABLE_TEST
void hash_table_test();
#endif

#endif