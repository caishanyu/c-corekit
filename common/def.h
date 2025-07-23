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

#define likely(x)   __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

typedef enum
{
    ERROR = -1,
    OK = 0,
}STATUS;

#if SELF_TEST
#define malloc(size) _test_malloc(size, __FILE__, __LINE__)
#define free(ptr)    _test_free(ptr, __FILE__, __LINE__)
#endif

#endif