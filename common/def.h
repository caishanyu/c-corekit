#ifndef __DEF__H
#define __DEF__H

#define CMOCKA_TEST (1)

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#if CMOCKA_TEST
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#endif

#include "debug.h"

// 分支预测优化
#define likely(x)   __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

// 标识入参和出参
#define IN
#define OUT

// 错误码
typedef enum
{
    ERROR = -1,
    OK = 0,

    ERR_BAD_PARAM,      // 输入参数错误
    ERR_NO_MEMORY,      // 内存不足
    ERR_API_ERROR,      // API报错

    /* DLIST模块错误码 */
    ERR_DLIST_START = 1000,
    ERR_DLIST_FULL,         // DLIST已满
    ERR_DLIST_EMPTY,        // DLIST空
    ERR_DLIST_IDX_ERROR,    // IDX相关错误
    ERR_DLIST_NODE_NOT_EXIST,   // 节点不存在

    /* hash_table模块错误码 */
    ERR_HASH_TABLE_START = 2000,
    ERR_HASH_TABLE_DATA_EXIST,

    /* thread_pool模块 */
    ERR_THREAD_POOL_START = 3000,
    ERR_THREAD_POOL_TASK_QUEUE_FULL,
}STATUS;

/*
    模块调试宏
*/

#define DLIST_TEST  (1)
#define QUEUE_TEST  (1)
#define STACK_TEST  (1)
#define HASH_TABLE_TEST (1)
#define THREAD_POOL_TEST    (1)

/*
    Cmocka测试框架宏
*/

#if CMOCKA_TEST
#define malloc(size) _test_malloc(size, __FILE__, __LINE__)
#define free(ptr)    _test_free(ptr, __FILE__, __LINE__)
#endif

#endif