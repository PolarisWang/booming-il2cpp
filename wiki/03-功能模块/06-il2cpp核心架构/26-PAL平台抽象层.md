---
title: PAL 平台抽象层
category: il2cpp核心架构
order: 26
---

# PAL 平台抽象层

## 概述

PAL (Platform Abstraction Layer) 是 Chaos IL2CPP 的平台抽象层，提供统一的 API 封装操作系统服务。采用 Chromium 风格的后缀分离（suffix-split）策略，同一 API 的不同平台实现在编译时通过 CMake 生成器表达式选择。

## 设计原则

1. **Pure C 函数** — `chaos::il2cpp::pal` 命名空间下的纯函数，无虚函数、无模板、零运行时开销
2. **编译时文件选择** — 链接器只看到正确平台的实现文件，无间接调用
3. **不透明句柄** — 平台类型通过 `struct` 前向声明对外隐藏
4. **noexcept** — PAL 函数永不抛异常

## 子系统

| 子系统 | 头文件 | 平台文件 | 功能 |
|--------|--------|---------|------|
| 动态库加载 | `pal_dl.h` | `_win32.cpp`, `_posix.cpp` | dlopen/LoadLibrary, dlsym/GetProcAddress |
| 时间测量 | `pal_time.h` | `_linux.cpp`, `_macos.cpp`, `_win32.cpp`, `_fallback.cpp` | 进程CPU时间、单调时钟、实时时钟 |
| 虚拟内存 | `pal_mem.h` | `_win32.cpp`, `_posix.cpp` | VirtualAlloc/mmap, VirtualProtect/mprotect, 大页 |
| CPU 特性 | `pal_cpu.h` | `_win32.cpp`, `_posix.cpp` | CPUID (x86), getauxval (ARM64) |
| 同步 | `pal_sync.h` | `_win32.cpp`, `_posix.cpp` | Mutex, CondVar, RwLock, Event (含WaitAny/WaitAll) |
| 线程 | `pal_thread.h` | `_win32.cpp`, `_posix.cpp` | Thread create/join, sleep, yield, 栈边界 |
| 崩溃处理 | `pal_crash.h` | `_win32.cpp`, `_posix.cpp` | VEH (Windows), sigaction (POSIX) |

## 构建选择

通过 CMake 生成器表达式进行平台选择。例：

```cmake
$<$<PLATFORM_ID:Windows>:pal_sync_win32.cpp>
$<$<NOT:$<PLATFORM_ID:Windows>>:pal_sync_posix.cpp>
```

## 模块依赖

- `chaos_pal` 是静态库，所有 runtime 模块链接它
- 仅依赖 `chaos_fmt`（日志格式化）和 `chaos_unordered_dense`
- 不得依赖任何 `runtime-core` 模块

## 新增平台

1. 添加新的 `_<platform>.cpp` 实现文件
2. 更新 `CMakeLists.txt` 添加对应的生成器表达式
3. 实现头文件中声明的每个函数
