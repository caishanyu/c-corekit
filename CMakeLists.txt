cmake_minimum_required(VERSION 3.25)    # 指定最低CMake版本

project(c_corekit LANGUAGES C VERSION 1.0)  # 定义项目名称和版本

# 设置C11标准
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)  # 必须支持 C11
set(CMAKE_C_EXTENSIONS OFF)       # 禁用编译器扩展（使用严格标准）

# 添加头文件路径
include_directories(${PROJECT_SOURCE_DIR}/common ${PROJECT_SOURCE_DIR}/ds)

# 查找cmocka库
find_package(cmocka REQUIRED)

# 创建库目标，指定其源文件
add_library(DLIST ${PROJECT_SOURCE_DIR}/ds/dlist/dlist.c)
add_library(QUEUE ${PROJECT_SOURCE_DIR}/ds/queue/queue.c)
add_library(STACK ${PROJECT_SOURCE_DIR}/ds/stack/stack.c)
add_library(HASH_TABLE ${PROJECT_SOURCE_DIR}/ds/hash_table/hash_table.c)
add_library(THREAD_POOL ${PROJECT_SOURCE_DIR}/thread_pool/thread_pool.c)

# 创建可执行文件目标
add_executable(MAIN ${PROJECT_SOURCE_DIR}/main.c)

# 添加全局编译器标志
add_compile_options(-pedantic -Wall -Wextra -fprofile-arcs -ftest-coverage -g -lpthread -lrt -lgcov -lcmocka)

# 链接库到可执行文件
target_link_libraries(MAIN DLIST QUEUE STACK HASH_TABLE THREAD_POOL cmocka)
