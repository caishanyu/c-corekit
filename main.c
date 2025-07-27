#include "atomic/atomic.h"
#include "ds/dlist/dlist.h"

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

        // 可添加更多用例
    };
    
    cmocka_run_group_tests(tests, NULL, NULL); // 运行测试组


    return 0;
}