# 编译器和标志
CC      := gcc
CFLAGS  := -std=c11 -pedantic -Wall -Wextra
LDFLAGS := -lpthread -lrt -lcmocka
INC     := -Icommon -Iatomic

# 目录设置
BIN_DIR := bin
OBJ_DIR := obj

# 源文件和目标文件
SRCS := atomic/atomic_counter.c atomic/atomic_spin_lock.c atomic/atomic_queue.c main.c
OBJS := $(SRCS:%.c=$(OBJ_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

# 最终目标
TARGET := $(BIN_DIR)/main

# 默认目标
all: $(TARGET)

# 链接可执行文件，链接库放在最后边，防止依赖关系
$(TARGET): $(OBJS) | $(BIN_DIR)
	$(CC) $^ -o $@ $(LDFLAGS)		

# 编译规则（包含依赖生成）
$(OBJ_DIR)/%.o: %.c | $(OBJ_DIR)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(INC) -MMD -MP -c $< -o $@

# 创建目录
$(BIN_DIR) $(OBJ_DIR):
	mkdir -p $@

# 包含自动生成的依赖
-include $(DEPS)

# 清理
clean:
	rm -rf $(BIN_DIR) $(OBJ_DIR)

.PHONY: all clean