/*
    Include files
*/

#include "hash_table.h"

/*
    typedefs
*/

// 哈希表结构
struct hash_table
{
    void** bucket_list;         // 桶链表
    unsigned int bucket_count;  // 桶的数量
    hash_func hash; // 哈希函数
};

/*
    Functions
*/

// 创建哈希表
hash_table* _hash_table_create(
    IN unsigned int bucket_size,
    IN hash_func hash,
    IN hash_table_show_func show
)
{
    hash_table *ht = NULL;
    unsigned int i = 0;
    dlist *dl = NULL;

    if(unlikely(bucket_size == 0 || NULL == hash))
    {
        DBG("bad in param for create hash table");
        return NULL;
    }

    // 申请hash_table空间
    ht = (hash_table*)malloc(sizeof(hash_table));
    if(unlikely(NULL == ht))
    {
        DBG("malloc space of hash table fail");
        goto error;
    }
    memset(ht, 0, sizeof(hash_table));

    // 申请桶数组
    ht->bucket_list = (void**)malloc(sizeof(void*)*bucket_size);
    if(unlikely(NULL == ht->bucket_list))
    {
        DBG("malloc space of bucket lists fail");
        goto error;
    }

    // 每个桶创建一个链表指向
    for(i = 0; i < bucket_size; ++ i)
    {
        dl = (void*)dlist_create(show);
        if(unlikely(NULL == dl))
        {
            DBG("malloc dlist of bucket [%lu] fail", i);
            goto error;
        }

        ht->bucket_list[i] = (void*)dl;
    }

    ht->bucket_count = bucket_size;
    ht->hash = hash;

    return ht;

error:
    for(i = 0; ht && ht->bucket_list && i < bucket_size; ++ i)
    {
        if(ht->bucket_list[i])
        {
            free(ht->bucket_list[i]);
        }
        else
        {
            break;
        }
    }
    if(ht && ht->bucket_list) free(ht->bucket_list);
    if(ht)  free(ht);
    return NULL;
}

// 销毁哈希表
static STATUS _hash_table_destroy(hash_table *hs)
{
    unsigned int i = 0;
    STATUS rv = 0;

    if(unlikely(!hs))
    {
        return ERR_BAD_PARAM;
    }

    // 逐个销毁桶
    for(i = 0; i < hs->bucket_count; ++ i)
    {
        rv = dlist_destroy((dlist*)(hs->bucket_list)[i]);
        if(unlikely(rv != OK))
        {
            DBG("destroy bucket [%lu] fail", i);
            return ERROR;
        }
    }

    free(hs->bucket_list);
    free(hs);

    return OK;
}

/*
    Variables
*/

// 哈希表操作变量
hash_table_ops hash_table_operations = {
    .hash_table_create = _hash_table_create,
    .hash_table_destroy = _hash_table_destroy,
};