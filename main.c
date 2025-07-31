#include "def.h"
#include "atomic/atomic.h"
#include "ds/dlist/dlist.h"
#include "ds/queue/queue.h"
#include "ds/stack/stack.h"
#include "ds/hash_table/hash_table.h"

int main()
{
    const struct CMUnitTest tests[] = {
#if ATOMIC_TEST        
        cmocka_unit_test(test_atomic_counter),  // 注册测试用例
        cmocka_unit_test(test_aotmic_spinlock),
        cmocka_unit_test(test_lock_free_queue),
        cmocka_unit_test(test_rwspinlock),
#endif

#if DLIST_TEST
        cmocka_unit_test(dlist_test),
#endif

#if QUEUE_TEST
        cmocka_unit_test(queue_test),
#endif

#if STACK_TEST
        cmocka_unit_test(stack_test),
#endif

#if HASH_TABLE_TEST
        cmocka_unit_test(hash_table_test),
#endif

        // 可添加更多用例
    };
    
    cmocka_run_group_tests(tests, NULL, NULL); // 运行测试组


    return 0;
}