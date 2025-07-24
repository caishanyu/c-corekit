#include "list.h"

/*
    typedef
*/

// 链表节点
typedef struct _list_node
{
    void *data;
    struct _list_node *next;
}list_node;

// 链表
typedef struct _list
{
    list_node *head;            // 头指针
    list_node *tail;            // 尾指针
    unsigned int capcality;     // 容量
    unsigned int size;          // 当前长度
}list;

