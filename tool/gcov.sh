#!/bin/bash

# dlist
echo "[enter dlist path]"
cd obj/ds/dlist

echo "[generate soft link to src files]"
ln -s ../../../ds/dlist/dlist.c .

echo "[generate dlist gcov]"
gcov dlist.c

# 收集数据
lcov --capture --directory . --output-file coverage.info

# 生成HTML
genhtml coverage.info  --output-directory coverage_report

echo "[return top path]"
cd -

# queue
echo "[enter queue path]"
cd obj/ds/queue

echo "[generate soft link to src files]"
ln -s ../../../ds/queue/queue.c .

echo "[generate queue gcov]"
gcov queue.c

# 收集数据
lcov --capture --directory . --output-file coverage.info

# 生成HTML
genhtml coverage.info  --output-directory coverage_report

echo "[return top path]"
cd -

# stack
echo "[enter stack path]"
cd obj/ds/stack

echo "[generate soft link to src files]"
ln -s ../../../ds/stack/stack.c .

echo "[generate stack gcov]"
gcov stack.c

# 收集数据
lcov --capture --directory . --output-file coverage.info

# 生成HTML
genhtml coverage.info  --output-directory coverage_report

echo "[return top path]"
cd -

# hash_table
echo "[enter hash_table path]"
cd obj/ds/hash_table

echo "[generate soft link to src files]"
ln -s ../../../ds/hash_table/hash_table.c .

echo "[generate hash_table gcov]"
gcov hash_table.c

# 收集数据
lcov --capture --directory . --output-file coverage.info

# 生成HTML
genhtml coverage.info  --output-directory coverage_report

echo "[return top path]"
cd -