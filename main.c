#include "atomic/atomic.h"

int main()
{
    test_atomic_counter();
    
    test_aotmic_spinlock();

    test_lock_free_queue();

    return 0;
}