#ifndef __DEF__H
#define __DEF__H

#define SELF_TEST   (1)

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#if SELF_TEST
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
}STATUS;

#if SELF_TEST
#define malloc(size) _test_malloc(size, __FILE__, __LINE__)
#define free(ptr)    _test_free(ptr, __FILE__, __LINE__)
#endif

#endif