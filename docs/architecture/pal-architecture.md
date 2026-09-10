# PAL 架构规范

## API 表面

PAL API 分为 7 个子系统，每个系统在 `chaos/pal/` 下有一个 `.h` 头文件和 2-4 个平台 `.cpp` 文件。

### 头文件位置

```
src/native/pal/
├── chaos/pal/
│   ├── pal.h              # 聚合头文件
│   ├── pal_types.h        # 公共类型 (PalMemoryProtection)
│   ├── pal_dl.h           # 动态库加载
│   ├── pal_time.h         # 时间测量
│   ├── pal_mem.h          # 虚拟内存
│   ├── pal_cpu.h          # CPU 特性检测
│   ├── pal_sync.h         # 同步原语
│   ├── pal_thread.h       # 线程
│   └── pal_crash.h        # 崩溃处理
├── pal_dl_win32.cpp
├── pal_dl_posix.cpp
├── pal_time_linux.cpp
├── pal_time_macos.cpp
├── pal_time_win32.cpp
├── pal_time_fallback.cpp
├── pal_mem_win32.cpp
├── pal_mem_posix.cpp
├── pal_cpu_win32.cpp
├── pal_cpu_posix.cpp
├── pal_sync_win32.cpp
├── pal_sync_posix.cpp
├── pal_thread_win32.cpp
├── pal_thread_posix.cpp
├── pal_crash_win32.cpp
└── pal_crash_posix.cpp
```

### 命名规范

- 函数前缀: `Pal` + 子系统名，如 `PalEventCreate`, `PalVirtualAlloc`
- 句柄类型: `Pal` + 类型名，如 `PalEvent*`, `PalMutex*`
- 文件命名: `pal_<子系统>_<平台>.cpp`

### CMake 构建选择

```cmake
add_library(chaos_pal STATIC
    chaos/pal/pal_<sub>.h
    $<$<PLATFORM_ID:Windows>:pal_<sub>_win32.cpp>
    $<$<NOT:$<PLATFORM_ID:Windows>>:pal_<sub>_posix.cpp>
    ...)
```

## 平台映射

| PAL API | Windows | Linux | macOS |
|---------|---------|-------|-------|
| PalMutex | CRITICAL_SECTION | pthread_mutex_t | pthread_mutex_t |
| PalCondVar | CONDITION_VARIABLE | pthread_cond_t | pthread_cond_t |
| PalRwLock | SRWLOCK | pthread_rwlock_t | pthread_rwlock_t |
| PalEvent | CreateEvent(HANDLE) | pthread_mutex+condvar | pthread_mutex+condvar |
| PalThread | CreateThread | pthread_create | pthread_create |
| PalCrash | AddVectoredExceptionHandler | sigaction(SIGSEGV等) | sigaction(SIGSEGV等) |

## 错误处理

- 所有 PAL 函数返回 bool/int 表示成功/失败，或返回 nullptr/0
- 不设置 errno，不抛出 C++ 异常
- 不写 stderr 或日志（由调用方处理）

## 消费者迁移指南

将平台 #ifdef 代码迁移到 PAL API 的步骤：

1. 确认 PAL 存在对应 API（不存在则先在 PAL 头文件声明 + 所有平台实现）
2. 在消费者 `.cpp` 中添加 `#include <chaos/pal/pal_xxx.h>` 和 `using chaos::il2cpp::pal::PalXxx;`
3. 替换所有 `#ifdef _WIN32` / `#else` / `#endif` 块为统一 PAL 调用
4. 移除不再需要的平台头文件 include
5. 编译验证（`cmake --preset debug`）
