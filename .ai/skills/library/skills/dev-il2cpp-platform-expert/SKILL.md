---
name: dev-il2cpp-platform-expert
description: 跨平台 PAL 专家 — 平台抽象层、构建系统、CI/CD、跨平台移植、Unity IL2CPP/Mono 对标
---

> ⚠️ **本文件通过 Skill 工具加载，作用是注入领域知识到当前对话上下文。**
> 本文件**不是可执行的 agent**。当前 Agent 需阅读下方"执行流程"作为实现参考。

# dev-il2cpp-platform-expert — 跨平台 PAL 专家

## 领域边界

### 我负责的

**PAL（Portability Abstraction Layer）** — `src/native/pal/`：
- 动态库加载（`pal_dl_*.cpp`）
- 时间测量（`pal_time_*.cpp`，5 平台实现）
- 虚拟内存管理（`pal_mem_*.cpp`）
- CPU 特性检测（`pal_cpu_*.cpp`，x86_64 SIMD/ARM64 NEON）
- 同步原语（`pal_sync_*.cpp`，互斥体、信号量、RWLock、事件）
- 线程管理（`pal_thread_*.cpp`、`pal_preempt_*.cpp` — 抢占式挂起）
- 异常处理/信号（`pal_eh_*.cpp`，Crash Handler）
- COM 互操作（`pal_com_*.cpp`）
- ETW/事件管道（`pal_etw_*.cpp`）
- 编码转换（`pal_encoding_*.cpp`）
- 错误码映射（`pal_error_*.cpp`）
- 内存压力检测（`pal_low_mem_*.cpp`、`pal_heap_*.cpp`）
- 可唤醒等待（`pal_wakeable_*.cpp`）

**构建系统与 CI/CD**：
- CMakePresets.json：debug/asan/profile/ship 四档配置
- 跨平台工具链：android-arm64、ios-arm64、linux-arm64、linux-x64、macos
- CI 管线：8 个 GitHub Actions workflow
- 三档运行时配置：CHECK（调试）/ PROFILE（分析）/ SHIP（发布）

### 我不负责的

- **运行时核心逻辑**（GC、VTable、解释器）→ 超出范围，标记 remaining，原因：需要运行时域知识
- **JIT 代码生成** → 超出范围，标记 remaining，原因：需要 JIT 域知识
- **AOT codegen** → 超出范围，标记 remaining，原因：需要 CodeGen 域知识
- **测试管线**（foundation-dll pipeline）→ 超出范围，标记 remaining，原因：需要测试域知识

---

## 触发条件

- 从 `dev-il2cpp-core-agent` 路由（关键词：PAL、跨平台、Linux、macOS、Android、ARM64、Windows、CMakePresets、toolchain、CI、GitHub Actions、信号处理、动态库加载、内存管理、线程挂起）
- 新增平台移植（如 iOS、WebAssembly、游戏主机）
- 平台特定 bug（Linux 信号、macOS 编译、Android Bionic libc 差异）

---

## 知识域

### 核心源文件

| 文件 | 行数 | 职责 |
|------|------|------|
| `pal.h` | — | PAL 统一头文件，平台无关 API 声明 |
| `pal_types.h` | — | 平台基础类型定义（CHAOS_IL2CPP_INT32 等） |
| `pal_dl_win32.cpp / _posix.cpp` | 2.5K / 1.2K | 动态库加载（LoadLibrary / dlopen） |
| `pal_time_win32.cpp` | 2.0K | `QueryPerformanceCounter` / `QueryPerformanceFrequency` |
| `pal_time_linux.cpp` | 1.0K | `clock_gettime(CLOCK_MONOTONIC)` |
| `pal_time_macos.cpp` | 1.7K | `mach_absolute_time()` + `timebase_info` 校准 |
| `pal_time_android.cpp` | 1.1K | `clock_gettime(CLOCK_MONOTONIC)`（无 `CLOCK_TAI`） |
| `pal_mem_win32.cpp` | 2.7K | `VirtualAlloc` / `VirtualFree` / `MEM_LARGE_PAGES` |
| `pal_mem_posix.cpp` | 4.2K | `mmap` / `munmap` / `madvise` |
| `pal_sync_win32.cpp` | 6.2K | `SRWLOCK` / `ConditionVariable` / `WaitForSingleObject` |
| `pal_sync_posix.cpp` | 15K | `pthread_mutex` / `pthread_rwlock` / `futex` / `sem_t` |
| `pal_thread_win32.cpp` | 3.0K | `CreateThread` / `SetThreadDescription` / TLS |
| `pal_thread_posix.cpp` | 3.0K | `pthread_create` / `pthread_setname_np` |
| `pal_preempt_win32.cpp` | 1.4K | 线程抢占挂起（`SuspendThread` / `GetThreadContext`） |
| `pal_preempt_posix.cpp` | 3.3K | `SIGUSR2` 信号 + `sigsetjmp` |
| `pal_preempt_android.cpp` | 0.8K | Android 无 `SIGUSR2` 兼容 |
| `pal_preempt_apple.cpp` | 0.8K | Apple M1/M2 兼容 |
| `pal_cpu_win32.cpp` | 3.5K | `__cpuid` / `__cpuidex` |
| `pal_cpu_posix.cpp` | 3.8K | `/proc/cpuinfo` / `getauxval(AT_HWCAP)` |
| `pal_eh_win32.cpp` | 0.8K | `SetUnhandledExceptionFilter` |
| `pal_eh_posix.cpp` | 6.9K | `sigaction`(SIGSEGV/SIGABRT/SIGBUS) + `backtrace` |
| `pal_etw_win32.cpp` | 2.4K | `EventWrite` / `EventRegister` |
| `pal_etw_posix.cpp` | 0.8K | LTTng-UST / stderr fallback |
| `pal_low_mem_linux.cpp` | 2.5K | `/proc/meminfo` / cgroup 内存限制检测 |
| `pal_low_mem_win32.cpp` | 1.6K | `GlobalMemoryStatusEx` / `MEMORYSTATUSEX` |
| `pal_crash_win32.cpp` | 1.4K | `MiniDumpWriteDump` |
| `pal_crash_posix.cpp` | 2.1K | `/proc/self/maps` + signal handler |

### 平台支持矩阵

| 子系统 | Windows | Linux | macOS | Android | iOS |
|--------|---------|-------|-------|---------|-----|
| 动态库加载 | ✅ LoadLibrary | ✅ dlopen | ✅ dlopen | ✅ dlopen | 🔄 |
| 时间测量 | ✅ QPC | ✅ clock_gettime | ✅ mach_time | ✅ clock_gettime | 🔄 |
| 虚拟内存 | ✅ VirtualAlloc | ✅ mmap | ✅ mmap | ✅ mmap | 🔄 |
| CPU 检测 | ✅ __cpuid | ✅ /proc/cpuinfo | ✅ sysctl | ✅ getauxval | 🔄 |
| 同步原语 | ✅ SRWLOCK | ✅ pthread+futex | ✅ pthread | ✅ pthread | 🔄 |
| 线程管理 | ✅ CreateThread | ✅ pthread | ✅ pthread | ✅ pthread | 🔄 |
| 线程抢占 | ✅ SuspendThread | ✅ SIGUSR2 | ✅ SIGUSR2 | ⚠️ 受限 | 🔄 |
| 异常处理 | ✅ VEH | ✅ sigaction | ✅ sigaction | ✅ sigaction | 🔄 |
| ETW | ✅ EventWrite | ✅ stderr | ✅ stderr | ❌ | ❌ |
| 编码转换 | ✅ WideCharToMultiByte | ✅ iconv | ✅ iconv | ✅ iconv | 🔄 |
| 内存压力 | ✅ MEMORYSTATUSEX | ✅ /proc/meminfo | ❌ | ✅ cgroup | ❌ |

### 三档构建配置

| 配置 | LOG_LEVEL | TRACE | ASSERT | PROFILE | CMake Preset | 用途 |
|------|-----------|-------|--------|---------|-------------|------|
| CHECK | 3 (DEBUG) | 启用 | 启用 | 启用 | `--preset debug` | 日常开发 |
| PROFILE | 2 (INFO) | 启用 | 关 | 启用 | `--preset profile` | 性能分析 |
| SHIP | 0 (ERROR) | 关 | 关 | 关 | `--preset ship` | 发布 |

### CI/CD 管线（8 个 workflow）

| Workflow | 触发 | 职责 |
|----------|------|------|
| `aot-regression.yml` | PR/main push | AOT 编译回归检查 |
| `arm64-jit-ci.yml` | PR/main push | ARM64 JIT 交叉编译+QEMU 测试 |
| `ci-framework.yml` | 被调用 | 通用 CI 框架（矩阵构建） |
| `codegen-regression.yml` | PR | codegen snapshot 回归 |
| `foundation-dll-regression.yml` | PR | foundation-dll 全量管线 |
| `hybrid-regression.yml` | PR | AOT+JIT 混合模式回归 |
| `jit-baseline.yml` | 定时 | JIT 性能基线采集 |
| `notify.yml` | 完成 | 构建结果通知 |

### 跨平台工具链

| 工具链 | 目标 | 编译器 |
|--------|------|--------|
| `android-arm64.cmake` | Android AArch64 | Android NDK clang |
| `ios-arm64.cmake` | iOS AArch64 | Apple Clang (Xcode) |
| `linux-arm64.cmake` | Linux AArch64 | aarch64-linux-gnu-g++ |
| `linux-arm64-native.cmake` | Linux ARM64 Native | 本地 g++ |
| `linux-x64.cmake` | Linux x86_64 | x86_64-linux-gnu-g++ |
| `macos-reference.cmake` | macOS x86_64 | Apple Clang |
| `windows-x64-reference.cmake` | Windows x64 | MSVC |

---

## 对标参考：Unity IL2CPP & Mono PAL

### Unity IL2CPP

Unity IL2CPP 的 PAL 层被称为 `il2cpp-platform`，定位与 Chaos PAL 类似：

| 功能 | Unity IL2CPP | Chaos IL2CPP | 差异 |
|------|-------------|-------------|------|
| 动态库 | `pal_dl.cpp` | `pal_dl_*.cpp` | Chaos 按平台拆分更清晰 |
| 线程 | `pal_thread.cpp` | `pal_thread_*.cpp` | 类似，Chaos 多平台文件隔离 |
| 同步 | 无独立 PAL | `pal_sync_*.cpp` (15K) | Chaos 更完善的 futex 支持 |
| 时间 | `pal_time.cpp` | `pal_time_*.cpp` | Chaos 支持 5 平台 |
| CPU 特性 | 嵌入 runtime | `pal_cpu_*.cpp` | Chaos 独立检测模块 |
| 内存 | 无独立 PAL | `pal_mem_*.cpp` + `pal_heap_*.cpp` | Chaos 更完整的虚拟内存抽象 |
| 异常处理 | 依赖 CRT | `pal_eh_*.cpp` + 独立 SEH | Chaos 双平台 EH 实现 |
| 线程抢占 | 无 | `pal_preempt_*.cpp` | Chaos 独有（GC safepoint 需要） |

**核心差异**：Unity IL2CPP 的 PAL 层更薄（嵌入运行时），Chaos IL2CPP 的 PAL 层是独立可测试模块，对多平台支持的抽象更彻底。

### Mono PAL

Mono 的跨平台层称为 `mono-io-layer` / `mono-coop-mutex`：

| 功能 | Mono | Chaos IL2CPP | 差异 |
|------|------|-------------|------|
| IO 层 | `mono-io-layer`（完整 POSIX 抽象） | `pal_io_*.cpp` | Chaos 更精简 |
| 协作式锁 | `mono-coop-mutex` | `pal_sync_*.cpp` | 类似，Chaos 使用 futex |
| GC 桥接 | `mono-gc.h` 平台层 | `pal_low_mem_*.cpp` | Chaos 无 GC 桥接（GC 在运行时中） |
| 信号处理 | `mono-sig-handler` | `pal_eh_*.cpp` | Chaos 更专注 crash + SIGUSR2 |
| 动态加载 | `mono-dl` | `pal_dl_*.cpp` | 接口语义一致 |

---

## 执行流程

### Step 1: 加载平台语境

```
1. 确认目标平台:
   - Windows x64 / Linux x64 / Linux ARM64 / macOS / Android / iOS
2. 确认构建配置:
   - CHECK / PROFILE / SHIP
   - 对应 CMakePresets.json 中的 preset
3. 确认 CI 管线要求:
   - 是否需要新增 workflow
   - 是否需要交叉编译
```

### Step 2: PAL 新增/修改流程

```
1. 新增 PAL 函数:
   a. 在 chaos/pal/pal_xxx.h 声明统一 API
   b. 在 pal_xxx_win32.cpp 实现 Windows 版本
   c. 在 pal_xxx_posix.cpp 实现 POSIX 通用版本
   d. 如有必要: pal_xxx_linux.cpp / pal_xxx_macos.cpp / pal_xxx_android.cpp
   e. 在 CMakeLists.txt 用生成器表达式添加条件编译
   
2. 修改平台特定实现:
   a. 确认所有平台的对应实现都需要修改
   b. 如果某平台无法实现 → 提供 stub（编译时 #ifdef 或运行时 fallback）
   
3. 验证:
   a. Windows: MSVC 编译 + 运行
   b. Linux: 交叉编译或 WSL 测试
   c. macOS/Android/iOS: CI workflow 触发
```

### Step 3: 新平台移植

```
1. 新增 pal_time_<platform>.cpp / pal_preempt_<platform>.cpp 等
2. 新增 build/toolchains/<platform>.cmake 工具链文件
3. 新增 .github/workflows/<platform>-ci.yml
4. 在 CMakePresets.json 添加新 preset
5. 测试: CI 触发交叉编译 + smoke test
```

---

## 已知故障模式

| 模式 | 症状 | 根因 | 修复 |
|------|------|------|------|
| Linux 信号丢失 | SIGUSR2 未触发 | Android 无 SIGUSR2 | 使用 `signalfd` 或 `SIGRTMIN` |
| macOS 时间校准 | mach_time 漂移 | 未调用 `mach_timebase_info` | 初始化时校准 |
| ARM64 无 cpuid | CPU 特性检测失败 | ARM64 使用 `getauxval` | 用 `AT_HWCAP` / `AT_HWCAP2` |
| MSVC + GCC ABI | 跨平台 struct layout | 对齐方式不同 | 用 `#pragma pack` / 固定布局 |
| LLVM vs GCC | `_mm256_*` intrinsic 差异 | 不同编译器的 intrinsic 签名 | 用 `<x86intrin.h>` 通用头 |
| QEMU 用户态 | ARM64 信号不准 | QEMU 不模拟 `SIGUSR2` | CI 跳过信号测试 |
| Android Bionic | 缺少 `pthread_cancel` | Bionic libc 限制 | 用 `pthread_kill` + 信号替代 |
| 工具链版本漂移 | Linux ARM64 交叉编译断 | sysroot 版本不匹配 | CI 固定 toolchain 版本 |

---

## 性能基线

| 操作 | Windows (MSVC) | Linux (GCC) | macOS (Clang) | 说明 |
|------|---------------|-------------|---------------|------|
| 动态库加载 | ~5ms | ~2ms | ~3ms | LoadLibrary vs dlopen |
| 时间戳获取 | ~10ns | ~8ns | ~12ns | QPC vs clock_gettime vs mach_time |
| 互斥体加锁 | ~15ns | ~12ns | ~14ns | SRWLOCK vs futex vs pthread |
| 线程创建 | ~100µs | ~30µs | ~50µs | CreateThread vs pthread_create |
| 虚拟内存分配 | ~2µs | ~1µs | ~1.5µs | VirtualAlloc vs mmap |
| CPU 特性检测 | ~50ns | ~500ns | ~100ns | cpuid vs /proc/cpuinfo vs sysctl |

---

## 架构约束

1. **PAL 不依赖运行时** — PAL 是最底层，不能 include runtime-core 的任何头文件
2. **PAL 不分配 GC 内存** — 使用 `malloc/free`（raw domain）
3. **信号处理单一所有权** — 只有一个模块可以安装信号处理器（PAL crash/eH）
4. **所有慢路径必须有 fast path** — 如 `pal_time.h` 内联 TSC 回退
5. **条件编译优于运行时检测** — 平台差异在编译期解决，不引入运行时 ifdef 分支

---

## 输出格式

```
✅ done: [已处理的子任务 ID 列表]
⏳ remaining: [未处理的子任务 ID 列表 + 原因]
```
