# COMMON

公共基础库

## 调试模块

- 提供`DBG()`宏用于调试
- `DBG_ON`宏用于控制`DBG_ON()`的行为，`0`时无打印

## 公共定义

- `likely`和`unlikely`用于分支预测优化
- `IN`标识函数输入参数，`OUT`标识函数输出参数
- `STATUS`类型错误码
