#ifndef _DEBUG_H_
#define _DEBUG_H_

/*
    Include Files
*/
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <pthread.h>
#include <stdarg.h>

/*
    Extern Symbols
*/

extern pthread_mutex_t debug_mutex;

/*
    Defines
*/

// 定义ANSI颜色代码
#define COLOR_RESET   "\033[0m"
#define COLOR_RED     "\033[31m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_BLUE    "\033[34m"
#define COLOR_MAGENTA "\033[35m"
#define COLOR_CYAN    "\033[36m"
#define COLOR_WHITE   "\033[37m"
#define COLOR_BOLD    "\033[1m"

static inline void _dbg_print(const char* timestamp, long ms, 
                             pid_t pid, long tid, 
                             const char* fmt, ...) {
    // 打印头部信息（带颜色）
    fprintf(stderr, COLOR_BOLD COLOR_CYAN "[%d" COLOR_RESET ":" \
            COLOR_BOLD COLOR_BLUE "%lu" COLOR_RESET " " \
            COLOR_GREEN "%s" COLOR_YELLOW ".%03ld" COLOR_RESET "] ",
            pid, tid, timestamp, ms);
    
    // 打印用户消息（白色）
    fprintf(stderr, COLOR_WHITE);
    
    // 处理可变参数
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    
    // 重置颜色并换行
    fprintf(stderr, COLOR_RESET "\n");
}

// 调试打印开关
#define DBG_ON  (1)

#if DBG_ON
// DBG打印，开启DBG_ON时有打印
#define DBG(...) do { \
    pthread_mutex_lock(&debug_mutex); \
    struct timeval _tv; \
    gettimeofday(&_tv, NULL); \
    struct tm* _tm = localtime(&_tv.tv_sec); \
    char _ts_buf[24]; \
    strftime(_ts_buf, sizeof(_ts_buf), "%Y-%m-%d %H:%M:%S", _tm); \
    _dbg_print(_ts_buf, _tv.tv_usec / 1000, getpid(), \
              (unsigned long)pthread_self(), __VA_ARGS__); \
    pthread_mutex_unlock(&debug_mutex); \
} while(0)

#else

#define DBG(fmt, ...)

#endif

#endif

