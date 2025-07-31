# c-corekit

该项目包含一组C语言编写的库，用于个人练习

## 项目内容

|类型|内容|路径|进度|
|--|--|--|--|
|数据结构|通用链表|[dlist](ds/dlist/)|done|
|数据结构|队列|[queue](ds/queue/)|done|
|数据结构|栈|[stack](ds/stack/)|done|
|数据结构|哈希表|[hash_table](ds/hash_table/)|doing...|


```
tree
.
├── atomic
│   ├── atomic_counter.c
│   ├── atomic.h
│   ├── atomic_queue.c
│   ├── atomic_rwspin_lock.c
│   ├── atomic_spin_lock.c
│   └── README.md
├── common
│   ├── debug.c
│   ├── debug.h
│   ├── def.h
│   └── README.md
├── ds
│   ├── dlist
│   │   ├── dlist.c
│   │   ├── dlist.h
│   │   └── README.md
│   ├── hash_table
│   │   ├── hash_table.c
│   │   ├── hash_table.h
│   │   └── README.md
│   ├── queue
│   │   ├── queue.c
│   │   ├── queue.h
│   │   └── README.md
│   ├── README.md
│   └── stack
│       ├── README.md
│       ├── stack.c
│       └── stack.h
├── main.c
├── Makefile
├── README.md
└── tool
    └── gcov.sh
```

## 项目构建

项目使用`comcka`进行单元测试，使用`gcov`生成测试覆盖率报告

构建时需要确保安装了对应库，可以查看[cmocka](https://github.com/caishanyu/notes/blob/main/cmocka.md)

```bash
make    # 执行编译

./bin/main # 运行测试

./tool/gcov.sh # 运行脚本，生成覆盖率，确保main被运行过

make clean  # 删除构建产物
```