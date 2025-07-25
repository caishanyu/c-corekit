#include "atomic/atomic.h"
#include "ds/list/list.h"

int main()
{
#if SELF_TEST
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_atomic_counter),  // 注册测试用例
        cmocka_unit_test(test_aotmic_spinlock),
        cmocka_unit_test(test_lock_free_queue),
        cmocka_unit_test(test_rwspinlock),

        // 可添加更多用例
    };
    
    cmocka_run_group_tests(tests, NULL, NULL); // 运行测试组
#endif

    dlist *dl = NULL;
    dl = dlist_create(5, NULL);
    dlist_destroy(dl);

    return 0;
}