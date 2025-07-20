#include "atomic/atomic.h"

int main()
{
    test_atomic_counter();
    
    test_aotmic_spinlock();

    return 0;
}