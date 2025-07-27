#ifndef _STACK_H_
#define _STACK_H_

/*
    Include Files
*/

#include "dlist/dlist.h"

/*
    Typedef
*/

// 栈声明
typedef struct stack stack;

// 函数指针
typedef dlist_show_func stack_show_func;

// 栈操作结构
typedef struct _stack_ops
{
    stack* (*stack_create)(stack_show_func);    // 创建栈
    STATUS (*stack_destroy)(stack*);    // 销毁栈
    STATUS (*stack_push)(stack*, void*);    // 入栈
    STATUS (*stack_pop)(stack*, void*, unsigned int); // 出栈
    STATUS (*stack_display)(stack*);    // 打印栈
    STATUS (*stack_top)(stack*, void*, unsigned int); // 获取栈头
    STATUS (*stack_get_size)(stack*, unsigned int *);   // 获取栈长度
}stack_ops;

/*
    Extern symbols
*/

extern stack_ops stack_operations;

/*
    Functions declaration
*/

// 创建栈
static inline stack* stack_create(IN stack_show_func func)
{
    return stack_operations.stack_create(func);
}

// 销毁栈
static inline STATUS stack_destroy(IN stack *s)
{
    if(!s)  return ERR_BAD_PARAM;
    return stack_operations.stack_destroy(s);
}

// 入栈
static inline STATUS stack_push(
    IN stack *s,
    IN void *data
)
{
    if(!s)  return ERR_BAD_PARAM;
    return stack_operations.stack_push(s, data);
}

// 出栈
static inline STATUS stack_pop(
    IN stack *s,
    OUT void *data,
    IN unsigned int len
)
{
    if(!s)  return ERR_BAD_PARAM;
    return stack_operations.stack_pop(s, data, len);
}

// 获取栈头元素
static inline STATUS stack_top(
    IN stack *s,
    OUT void *data,
    IN unsigned int len
)
{
    if(!s)  return ERR_BAD_PARAM;
    return stack_operations.stack_top(s, data, len);
}

// 打印栈
static inline STATUS stack_display(IN stack *s)
{
    if(!s)  return ERR_BAD_PARAM;
    return stack_operations.stack_display(s);
}

// 获取栈长度
static inline STATUS stack_get_size(
    IN stack *s,
    OUT unsigned int *len
)
{
    if(!s)  return ERR_BAD_PARAM;
    return stack_operations.stack_get_size(s, len);
}

#if STACK_TEST
// 测试接口
void stack_test();
#endif

#endif