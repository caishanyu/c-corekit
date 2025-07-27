/*
    Include Files
*/

#include "stack.h"

/*
    Typedef
*/

// 栈结构
struct stack
{
    dlist *dl;
};

/*
    Functions
*/

// 创建栈
static stack* _stack_create(IN stack_show_func func)
{
    stack *s = (stack*)malloc(sizeof(stack));
    if(NULL == s)
    {
        DBG("malloc stack space fail");
        return NULL;
    }

    s->dl = dlist_create(func);
    if(NULL == s->dl)
    {
        DBG("malloc base dlist fail");
        free(s);
        return NULL;
    }

    return s;
}

// 销毁栈
static STATUS _stack_destroy(IN stack *s)
{
    STATUS rv = dlist_destroy(s->dl);
    if(OK != rv)
    {
        DBG("destroy base dlist fail");
        return rv;
    }
    free(s);
    return OK;
}

// 入栈
static inline STATUS _stack_push(IN stack *s, IN void *data)
{
    return dlist_append_tail(s->dl, data);
}

// 出栈
static inline STATUS _stack_pop(IN stack *s, OUT void *data, IN unsigned int len)
{
    STATUS rv = dlist_get_tail(s->dl, data, len);
    return rv | dlist_remove_tail(s->dl);
}

// 获取栈头
static inline STATUS _stack_top(IN stack *s, OUT void *data, IN unsigned int len)
{
    return dlist_get_tail(s->dl, data, len);
}

// 获取栈长度
static inline STATUS _stack_get_size(IN stack *s, OUT unsigned int *len)
{
    return dlist_get_size(s->dl, len);
}

// 打印栈
static inline STATUS _stack_display(IN stack *s)
{
    return dlist_display(s->dl, DLIST_ORDER);
}

#if STACK_TEST

static void test_show_func(void* data)
{
    printf("%d", *((int*)data));
}

void stack_test()
{
#if CMOCKA_TEST
    stack *s = NULL;
    int a[5] = {0,1,2,3,4};
    unsigned int size = sizeof(int);
    int data = 0;
    unsigned int len = 0;

    s = stack_create(test_show_func);
    assert_non_null(s);

    stack_display(s);

    assert_int_not_equal(OK, stack_push(NULL, &a[0]));
    assert_int_not_equal(OK, stack_pop(NULL, &data, size));
    assert_int_not_equal(OK, stack_pop(s, NULL, size));
    assert_int_not_equal(OK, stack_pop(s, &data, 0));
    assert_int_not_equal(OK, stack_top(s, &data, size));
    assert_int_not_equal(OK, stack_top(s, NULL, size));
    assert_int_not_equal(OK, stack_top(NULL, &data, size));
    assert_int_not_equal(OK, stack_top(s, &data, 0));

    assert_return_code(OK, stack_push(s, &a[0]));
    assert_return_code(OK, stack_push(s, &a[1]));
    assert_return_code(OK, stack_push(s, &a[2]));
    assert_return_code(OK, stack_push(s, &a[3]));
    assert_return_code(OK, stack_push(s, &a[4]));
    // 0-1-2-3-4(top)

    stack_display(s);

    assert_return_code(OK, stack_pop(s, &data, size));
    assert_int_equal(4, data);
    assert_return_code(OK, stack_pop(s, &data, size));
    assert_int_equal(3, data);
    assert_return_code(OK, stack_pop(s, &data, size));
    assert_int_equal(2, data);
    
    assert_return_code(OK, stack_get_size(s, &len));
    assert_int_equal(2, len);

    assert_return_code(OK, stack_top(s, &data, size));
    assert_int_equal(1, data);

    assert_return_code(OK, stack_pop(s, &data, size));
    assert_int_equal(1, data);
    assert_return_code(OK, stack_pop(s, &data, size));
    assert_int_equal(0, data);

    assert_int_equal(OK, stack_destroy(s));
#endif
}

#endif

/*
    Variables
*/

// 栈操作集合
stack_ops stack_operations = {
    .stack_create = _stack_create,
    .stack_destroy = _stack_destroy,
    .stack_push = _stack_push,
    .stack_pop = _stack_pop,
    .stack_top = _stack_top,
    .stack_get_size = _stack_get_size,
    .stack_display = _stack_display,
};