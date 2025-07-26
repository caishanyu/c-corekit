#include <pthread.h>
#include "debug.h"

// 互斥锁保证dbg线程安全
pthread_mutex_t debug_mutex = PTHREAD_MUTEX_INITIALIZER;

