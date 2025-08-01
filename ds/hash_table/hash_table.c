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
    
    pthread_mutex_t *mtx;       // 粗粒度锁
    
    unsigned int bucket_count;  // 桶的数量
    hash_func hash; // 哈希函数

};

/*
    Defines
*/

#define HS_LOCK(hs)     pthread_mutex_lock(hs->mtx)
#define HS_UNLOCK(hs)   pthread_mutex_unlock(hs->mtx)

/*
    Functions
*/

// 创建哈希表
hash_table* _hash_table_create(
    IN unsigned int bucket_size,
    IN hash_func hash,
    IN cmp_func cmp,
    IN hash_table_show_func show
)
{
    hash_table *ht = NULL;
    unsigned int i = 0;
    dlist *dl = NULL;

    if(unlikely(bucket_size == 0 || NULL == hash || NULL == cmp))
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
        dl = (void*)dlist_create(show, cmp);
        if(unlikely(NULL == dl))
        {
            DBG("malloc dlist of bucket [%lu] fail", i);
            goto error;
        }

        ht->bucket_list[i] = (void*)dl;
    }

    // 初始化锁
    ht->mtx = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
    if(unlikely(!ht->mtx))
    {
        DBG("malloc space of mutex fail");
        goto error;
    }
    if(0 != pthread_mutex_init(ht->mtx, NULL))
    {
        DBG("init mutex fail");
        goto error;
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
    if(ht->mtx)   free(ht->mtx);
    if(ht)  free(ht);
    return NULL;
}

// 销毁哈希表
static STATUS _hash_table_destroy(IN hash_table *hs)
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
    free(hs->mtx);
    free(hs);

    return OK;
}

// 检查数据是否存在表中
static bool _hash_table_contain(
    IN hash_table *hs,
    IN void *data
)
{
    int hs_val = 0;
    dlist *bucket = NULL;

    if(unlikely(!hs || !data))
    {
        return false;
    }

    // 计算哈希值
    hs_val = (hs->hash(data) % hs->bucket_count);
    bucket = (dlist*)hs->bucket_list[hs_val];

    return dlist_contain(bucket, data);
}

// 加入哈希表
static STATUS _hash_table_insert(
    IN hash_table *hs,
    IN void *data
)
{
    unsigned int hash_val = 0;
    dlist *bucket = NULL;
    STATUS ret = OK;

    if(unlikely(!hs || !data))
    {
        return ERR_BAD_PARAM;
    }

    // 计算hash
    hash_val = hs->hash(data) % hs->bucket_count;
    // 找到对应桶
    bucket = (dlist*)hs->bucket_list[hash_val];

    if(true ==dlist_contain(bucket, data))
    {
        return ERR_HASH_TABLE_DATA_EXIST;
    }

    // 尾插到桶中
    ret = dlist_append_tail(bucket, data);
    if(OK != ret)
    {
        DBG("insert to dlist fail");
    }

    return ret;
}

// 移除出哈希表
static STATUS _hash_table_remove(
    IN hash_table *hs,
    IN void *data
)
{
    unsigned int hv = 0;
    dlist *bucket = NULL;

    if(unlikely(!hs || !data))
    {
        return ERR_BAD_PARAM;
    }

    hv = hs->hash(data) % hs->bucket_count;
    bucket = (dlist*)hs->bucket_list[hv];

    return dlist_remove_by_data(bucket, data);
}

// 获取哈希表元素总数
static STATUS _hash_table_get_size(
    IN hash_table *hs,
    OUT unsigned int *size
)
{
    unsigned int bucket_size = 0;
    unsigned int i = 0;

    if(unlikely(!hs || !size))
    {
        return ERR_BAD_PARAM;
    }

    *size = 0;

    HS_LOCK(hs);

    for(; i < hs->bucket_count; ++ i)
    {
        if(OK != dlist_get_size((dlist*)hs->bucket_list[i], &bucket_size))
        {
            HS_UNLOCK(hs);
            *size = 0;
            return ERROR;
        }

        *size += bucket_size;
    }

    HS_UNLOCK(hs);

    return OK;
}

/*
    Variables
*/

// 哈希表操作变量
hash_table_ops hash_table_operations = {
    .hash_table_create = _hash_table_create,
    .hash_table_destroy = _hash_table_destroy,
    .hash_table_insert = _hash_table_insert,
    .hash_table_remove = _hash_table_remove,
    .hash_table_contain = _hash_table_contain,
    .hash_table_get_size = _hash_table_get_size,
};

// 哈希表测试
#if HASH_TABLE_TEST

static inline unsigned int int_hash(void *data)
{
    return *((int*)data) / 11;
}

static void int_display(void* data)
{
    printf("%d", *((int*)data));
}

static bool int_cmp(void *d1, void *d2)
{
    if(!d1 || !d2)  return false;
    return *(int*)d1 == *(int*)d2;
}

void hash_table_test()
{
#if CMOCKA_TEST
    hash_table *hs = NULL;
    int a[5] = {0,1,2,3,4};
    int b = 6;
    int c = 98;
    int i = 0;
    unsigned int size = 0;

    assert_null(hash_table_create(0, int_hash, int_cmp, NULL));
    assert_null(hash_table_create(12, NULL, int_cmp, NULL));
    assert_null(hash_table_create(12, int_hash, NULL, NULL));
    hs = hash_table_create(12, int_hash, int_cmp, int_display);
    assert_non_null(hs);

    assert_int_not_equal(OK, hash_table_insert(NULL, &a[0]));
    assert_int_not_equal(OK, hash_table_insert(hs, NULL));

    assert_int_not_equal(OK, hash_table_get_size(NULL, &size));
    assert_int_not_equal(OK, hash_table_get_size(hs, NULL));

    assert_return_code(OK, hash_table_get_size(hs, &size));
    assert_int_equal(0, size);

    for(i=0; i<5; ++i)
    {
        assert_return_code(OK, hash_table_insert(hs, &a[i]));
    }

    assert_return_code(OK, hash_table_get_size(hs, &size));
    assert_int_equal(5, size);

    for(i=0; i<5; ++i)
        assert_return_code(true, hash_table_contain(hs, &a[i]));
    assert_return_code(false, hash_table_contain(NULL, &a[0]));
    assert_return_code(false, hash_table_contain(hs, NULL));
    assert_return_code(false, hash_table_contain(hs, &b));

    assert_int_not_equal(OK, hash_table_remove(NULL, &b));
    assert_int_not_equal(OK, hash_table_remove(hs, NULL));
    assert_int_not_equal(OK, hash_table_remove(hs, &c));
    for(i=0; i<5; ++i)
        assert_return_code(OK, hash_table_remove(hs, &a[i]));

    assert_return_code(OK, hash_table_get_size(hs, &size));
    assert_int_equal(0, size);

    for(i=0; i<5; ++i)
    {
        assert_return_code(OK, hash_table_insert(hs, &a[i]));
    }

    assert_int_not_equal(OK, hash_table_destroy(NULL));
    assert_return_code(OK, hash_table_destroy(hs));
#endif
}
#endif