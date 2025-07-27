/*
    Include Files
*/

#include <string.h>
#include "dlist.h"

/*
    typedef
*/

// 链表节点
typedef struct _dlist_node
{
    void *data;
    struct _dlist_node *next;
    struct _dlist_node *prior;
}dlist_node;

// 链表
struct _dlist
{
    dlist_node *head;            // 头指针
    dlist_node *tail;            // 尾指针

    unsigned int size;          // 当前长度

    pthread_mutex_t mutex;      // 链表互斥锁

    dlist_show_func show_func;  // 打印数据
};

/*
    Defines
*/

// 锁
#define DLIST_LOCK(l)   pthread_mutex_lock(&((l)->mutex));
#define DLIST_UNLOCK(l) pthread_mutex_unlock(&((l)->mutex));

/*
    Functions
*/

// 创建链表节点，申请空间，返回指针
static inline dlist_node* dlist_node_create()
{
    dlist_node *node = (dlist_node*)malloc(sizeof(dlist_node));
    if(likely(node))
    {
        memset(node, 0, sizeof(dlist_node));
    }
    else
    {
        DBG("create dlist node fail");
    }
    return node;
}

// 销毁链表节点，回收内存
static inline void dlist_node_destroy(dlist_node *node)
{
    if(likely(node))
    {
        free(node);
    }
}

// 创建链表
static dlist* _dlist_create(
    dlist_show_func show_func
)
{
    dlist *dl = NULL;
    dlist_node *dummy = NULL;
    
    // 申请链表空间
    dl = (dlist*)malloc(sizeof(dlist));
    if(unlikely(NULL == dl))
    {
        DBG("malloc fail\r\n");
        return NULL;
    }
    memset(dl, 0, sizeof(dlist));

    // 创建哑节点
    dummy = dlist_node_create();
    if(unlikely(NULL == dummy))
    {
        DBG("malloc fail\r\n");
        goto error;
    }

    // 创建互斥锁
    if(unlikely(0 != pthread_mutex_init(&(dl->mutex), NULL)))
    {
        DBG("init mutex fail\r\n");
        goto error;
    }

    dl->head = dummy;
    dl->tail = dummy;
    dl->size = 0;
    dl->show_func = show_func;

    DBG("dl create ok: %p\r\n", (void*)dl);
    return dl;

error:
    // 销毁哑节点
    if(dummy)   dlist_node_destroy(dummy);
    // 销毁链表
    if(dl)   free(dl);

    return NULL;
}

// 销毁链表
static STATUS _dlist_destroy(dlist* dl)
{
    dlist_node *ptr = NULL;
    dlist_node *next = NULL;

    if(unlikely(NULL == dl))
    {
        DBG("Invalid in param\r\n");
        return ERR_BAD_PARAM;
    }

    // 销毁链表节点，包括哑节点
    ptr = dl->head;
    while(ptr)
    {
        next = ptr->next;
        dlist_node_destroy(ptr);
        ptr = next;
    }

    // 销毁链表结构
    free(dl);

    DBG("destory dl %p ok\r\n", (void*)dl);

    return OK;
}

// 打印链表
static STATUS _dlist_display(IN dlist *l, IN DLIST_ORDER_TYPE order)
{
    dlist_node *ptr = NULL;
    unsigned int count = 0;

    if(unlikely((!l) || (DLIST_ORDER != order && DLIST_REVERSE != order)))
    {
        return ERR_BAD_PARAM;
    }

    DLIST_LOCK(l);

    ptr = DLIST_ORDER == order ?
                         l->head->next:
                         l->tail;
    while((DLIST_ORDER == order) ? 
            (NULL != ptr) : 
            (ptr != l->head)
    )
    {
        printf("Dlist Node NO.%d:\r\n", ++ count);
        l->show_func(ptr->data);
        printf("\r\n========\r\n");
        ptr = DLIST_ORDER == order ? 
                            ptr->next:
                            ptr->prior;
    }

    DLIST_UNLOCK(l);

    return OK;
}

// 获取链表长度
static STATUS _dlist_get_size(IN dlist *l, OUT unsigned int *size)
{
    if(unlikely(NULL == l || NULL == size))
    {
        return ERR_BAD_PARAM;
    }

    *size = l->size;

    return OK;
}

// 获取链表idx位置元素data
static STATUS _dlist_get_data(dlist *dl, unsigned int idx, void *data, unsigned int len)
{
    unsigned int i = 0;
    dlist_node *ptr = NULL;

    if(unlikely(NULL == dl || NULL == data || 0 == len))
    {
        return ERR_BAD_PARAM;
    }

    DLIST_LOCK(dl);

    // 检查idx合法性
    if(idx > dl->size || idx < 1)
    {
        DLIST_UNLOCK(dl);
        return ERR_DLIST_IDX_ERROR;
    }

    ptr = dl->head;
    for(; i < idx; ++ i)
        ptr = ptr->next;
    
    if(ptr->data)
        memcpy(data, ptr->data, len);
    else
        DBG("idx %d, pdata is NULL", idx);

    DLIST_UNLOCK(dl);

    return OK;
}

// 插入节点，使其成为idx位置
static STATUS _dlist_insert(IN dlist *l, IN unsigned int idx, IN void *data)
{
    dlist_node *prior = NULL;
    dlist_node *next = NULL;
    dlist_node *node = NULL;
    unsigned int i = 0;

    if(unlikely(NULL == l))
    {
        return ERR_BAD_PARAM;
    }

    DLIST_LOCK(l);
    
    // 检查idx合法性
    if(idx < 1 || (idx > (l->size+1)))
    {
        DLIST_UNLOCK(l);
        return ERR_DLIST_IDX_ERROR;
    }

    // 找到插入位置前一个节点，移动idx-1步
    prior = l->head;
    while(i < (idx - 1))
    {
        prior = prior->next;
        ++ i;
    }
    next = prior->next;

    // 创建新节点，用于存储data
    node = dlist_node_create();
    if(unlikely(NULL == node))
    {
        DLIST_UNLOCK(l);
        return ERR_NO_MEMORY;
    }
    node->data = data;

    // 更改next/prior域
    node->prior = prior;
    node->next = next;
    prior->next = node;
    if(next)    next->prior = node;

    // tail移动
    if(idx == (l->size + 1))
    {
        l->tail = node;
    }

    // 长度增加
    l->size += 1;

    DLIST_UNLOCK(l);

    return OK;
}

// 移除元素
static STATUS _dlist_remove(dlist *dl, unsigned int idx)
{
    dlist_node *prior = NULL;
    dlist_node *next = NULL;
    dlist_node *node = NULL;
    unsigned int i = 0;

    if(unlikely(!dl))
    {
        return ERR_BAD_PARAM;
    }

    DLIST_LOCK(dl);

    // 空队列不允许移除
    if(0 == dl->size)
    {
        DLIST_UNLOCK(dl);
        return ERR_DLIST_EMPTY;
    }

    // 检查idx合法性
    if(idx < 1 || idx > dl->size)
    {
        DLIST_UNLOCK(dl);
        return ERR_DLIST_IDX_ERROR;
    }

    // 找到删除节点的前后
    prior = dl->head;
    while(i < (idx - 1))
    {
        prior = prior->next;
        ++ i;
    }
    node = prior->next;
    next = node->next;

    // 调整
    prior->next = next;
    if(next)
    {
        next->prior = prior;
    }

    // 移除node，释放内存
    dlist_node_destroy(node);

    // 调整tail
    if(idx == dl->size)
    {
        dl->tail = prior;
    }

    // 调整长度
    dl->size -= 1;

    DLIST_UNLOCK(dl);

    return OK;
}

/*
    Variables
*/

dlist_ops dlist_operations = {
    .dlist_create = _dlist_create,
    .dlist_destroy = _dlist_destroy,
    .dlist_display = _dlist_display,
    .dlist_get_size = _dlist_get_size,
    .dlist_get_data = _dlist_get_data,
    .dlist_insert = _dlist_insert,
    .dlist_remove = _dlist_remove,
};