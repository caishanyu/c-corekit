#!/bin/bash

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