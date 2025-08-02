#ifndef _THREAD_POOL_H
#define _THREAD_POOL_H

/*
    Include files
*/

#include <pthread.h>
#include "def.h"

/*
    Defines
*/

#define THREAD_COUNT_MAX    (10)
#define THREAD_QUEUE_SIZE_MAX   (10)

/*
    typedef
*/

typedef struct thread_pool_t thread_pool_t;

#endif