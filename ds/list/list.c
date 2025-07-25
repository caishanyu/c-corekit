/*
    Include Files
*/

#include <string.h>
#include "list.h"

/*
    typedef
*/

// 链表节点
typedef struct _dlist_node
{
    void *data;
    struct _dlist_node *next;
}dlist_node;

// 链表
struct _dlist
{
    dlist_node *head;            // 头指针
    dlist_node *tail;            // 尾指针

    unsigned int capcality;     // 容量
    unsigned int size;          // 当前长度

    pthread_mutex_t mutex;      // 链表互斥锁

    dlist_show_func show_func;  // 打印数据
};

/*
    Functions
*/

// 创建链表
static dlist* _dlist_create(
    unsigned int capacity,
    dlist_show_func show_func
)
{
    dlist *dl = NULL;
    dlist_node *dummy = NULL;
    
    // 申请链表空间
    dl = (dlist*)malloc(sizeof(dlist));
    if(NULL == dl)
    {
        perror("malloc fail\r\n");
        return NULL;
    }
    memset(dl, 0, sizeof(dlist));

    // 创建哑节点
    dummy = (dlist_node*)malloc(sizeof(dlist_node));
    if(NULL == dummy)
    {
        perror("malloc fail\r\n");
        goto error;
    }
    memset(dummy, 0, sizeof(dlist_node));

    // 创建互斥锁
    if(0 != pthread_mutex_init(&(dl->mutex), NULL))
    {
        perror("init mutex fail\r\n");
        goto error;
    }

    dl->head = dummy;
    dl->tail = dummy;
    dl->size = 0;
    dl->capcality = capacity;
    dl->show_func = show_func;

    printf("dl create ok: %p\r\n", (void*)dl);
    return dl;

error:
    // 销毁哑节点
    if(dummy)   free(dummy);
    // 销毁链表
    if(dl)   free(dl);

    return NULL;
}

// 销毁链表
static void _dlist_destroy(dlist* dl)
{
    dlist_node *ptr = NULL;
    dlist_node *next = NULL;

    if(NULL == dl)
    {
        perror("Invalid in param\r\n");
        return;
    }

    // 销毁链表节点，包括哑节点
    ptr = dl->head;
    while(ptr)
    {
        next = ptr->next;
        free(ptr);
        ptr = next;
    }

    // 销毁链表结构
    free(dl);

    printf("destory dl %p ok\r\n", (void*)dl);

    return;
}

/*
    Variables
*/

dlist_ops dlist_operations = {
    .dlist_create = _dlist_create,
    .dlist_destroy = _dlist_destroy,
};