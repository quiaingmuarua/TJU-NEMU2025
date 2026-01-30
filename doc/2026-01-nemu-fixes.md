# 2026-01 NEMU 修复记录

本轮修复的目标是：让 `testcase` 全部通过，并解决 NEMU 在系统调用 / 浮点 / 日志性能上的问题。

## 修复点概览

### 指令与中断支持
- 新增并挂接 `cmovs/cmovne/cmovns`（解决 soft-float 产生的 cmovcc 指令）
- 实现最小软中断路径（`int 0x80`），并补齐 `lidt/iret/sti/cli`
- 实现 `pusha/popa`（内核中断入口需要）
- 实现 `in/out` 端口 I/O（串口输出依赖）
- 增加 IDTR 数据结构并解析 IDT gate

### 内存访问
- `lnaddr_read/lnaddr_write` 支持 len=3（`lgdt/lidt` 读取 24-bit base）

### 构建与测试
- `FLOAT.a` 包含 `FLOAT_vfprintf.o`
- `integral` / `quadratic-eq` 强制软浮点编译参数，避免 x87
- 关闭 NEMU DEBUG/LOG 输出，避免大循环用例（如 matrix-mul）被日志拖死
- `make run` 支持 `AUTO=1`，`run-c` 自动 `c\nq\n` 防止管道 EOF 崩溃

## 影响到的文件（主要）

NEMU：
- `nemu/include/cpu/reg.h`
- `nemu/src/cpu/exec/exec.c`
- `nemu/src/cpu/exec/misc/misc.c`
- `nemu/src/cpu/exec/data-mov/{cmov*,lidt*}`
- `nemu/src/memory/memory.c`
- `nemu/include/common.h`

Kernel / Testcase / Makefile：
- `kernel/include/common.h`
- `testcase/Makefile.part`
- `Makefile`

## 使用说明

快速运行单个用例：
```
make run-c USERPROG=obj/testcase/hello
```

完整测试：
```
make test
```

如果需要更快的测试运行，保持 `nemu/include/common.h` 里 DEBUG/LOG 为关闭状态。

