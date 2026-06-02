# PAL 消费者 `#if` 清理 —— 最终审计报告

**总览**: 28 条匹配，含 3 条注释（非实际 `#if`），25 个实际 `#if` 块。全部已评估为结构性或平台固有，无需进一步清理。

## common（5 实际块，全部保留）

| # | 文件 | 行 | 内容 | POSIX 分支 | 判定 |
|---|------|-----|------|-----------|------|
| 1 | `profile_globals.cpp` | 6 | `#include <windows.h>` | `#include <time.h>/<thread>` | 安全 — 互斥包含 |
| 2 | `profile_globals.cpp` | 34 | `QueryPerformanceFrequency` | `clock_gettime` | 安全 — 互斥调用 |
| 3 | `seh_compat.h` | 10 | `__try/__except` 宏 | `if(true)/else` 死代码 | 安全 — 完整 POSIX 分支 |
| 4 | `trace.h` | 103 | `localtime_s` | `localtime_r` | 安全 — 完整 POSIX 分支 |
| 5 | `log.h` | 89 | `localtime_s` | `localtime_r` | 安全 — 完整 POSIX 分支 |

## jit（10 实际块，全部保留）

| # | 文件 | 行 | 内容 | 判定理由 |
|---|------|-----|------|---------|
| 6 | `jit_precode.cpp` | 38 | include guard: `<windows.h>` for DWORD(SEH filter) | windows.h 仍需用于 CHAOS_SEH_FILTER_ALL |
| 7 | `jit_precode.cpp` | 278 | `SafeCompileWithCatch` — `__try/__except` | SEH 无法 PAL 抽象，POSIX no-op 正确 |
| 8 | `WinSehHandler.cpp` | 16 | include guard: `<windows.h>` + `<intrin.h>` | Win32 VEH/CRT 固有 |
| 9 | `WinSehHandler.cpp` | 319 | VEH callback 前向声明 | Win32 VEH 概念 |
| 10 | `WinSehHandler.cpp` | 325 | `AddVectoredExceptionHandler` | Win32 VEH 概念 |
| 11 | `WinSehHandler.cpp` | 357 | `GetSehHandler()` 存根 | Win32 only |
| 12 | `WinSehHandler.cpp` | 371 | VEH handler 实现 ~500 行 | 完全 Win32 SEH 机制 |
| 13 | `jit_helpers.h` | 41 | `EmitLoadTlsTlab`/`InitTlsTlabInfo` 声明 | x64 TLS 内联编码 |
| 14 | `jit_helpers.cpp` | 294 | `__readgsqword` + inline x64 encoding | x64 特有，非可移植 |
| 15 | `jit_seh_handler_internal.cpp` | 24 | include guard: `<windows.h>` + `<intrin.h>` | 测试专用 SEH personality |
| 16 | `LinuxSehHandler.h` | 82 | GetSehHandler 别名 | 向后兼容，已正确 |

## runtime-core（6 匹配：4 实际 + 2 注释，全部保留）

| # | 文件 | 行 | 内容 | 判定理由 |
|---|------|-----|------|---------|
| — | `gc/gc_etw.h` | 12 | 注释（非实际 `#if`） | 仅注释 |
| 17 | `thread_state.cpp` | 311 | suspend: `suspend_event`(Win32) vs `suspend_seq`/futex(POSIX) | 结构性 — 不同同步机制 |
| 18 | `marshal_api.cpp` | 405 | BSTR helper: `MarshalStringToBSTR` | Win32 COM ABI |
| 19 | `marshal_api.cpp` | 431 | BSTR helper: `MarshalPtrToStringBSTR` | Win32 COM ABI |
| 20 | `marshal_api.cpp` | 626 | VARIANT: `ChaosGetObjectForNativeVariant` | Win32 COM ABI |
| 21 | `marshal_api.cpp` | 693 | VARIANT: `ChaosGetNativeVariantForObject` | Win32 COM ABI |

## eventpipe/diagnostics（4 实际块，全部保留）

| # | 文件 | 行 | 内容 | 判定理由 |
|---|------|-----|------|---------|
| 22 | `ep_config.h` | 24 | pipe 名称模板: L"wide" vs narrow | 平台字符串类型差异 |
| 23 | `ep_platform.h` | 19 | EventPipe 平台抽象层选择器 | 结构性 — 文件即抽象层 |
| 24 | `dbg_transport.cpp` | 12 | include guard: `<windows.h>`/`<io.h>` | Win32 控制台固有 |
| 25 | `dbg_transport.cpp` | 37 | `_setmode(_fileno(stdin), _O_BINARY)` | Win32 控制台二进制模式 |

## pal（2 条匹配，均为注释）

| # | 文件 | 行 | 内容 |
|---|------|-----|------|
| — | `pal/pal_com.h` | 6 | 注释 |
| — | `pal/pal_error.h` | 6 | 注释 |

## 统计

- 初始总数（PAL 项目启动前）: ~180+ 块
- 已清理: ~155+ 块（PAL 消费者迁移 Tracks 1-11 + Rounds 1-5）
- 剩余: **25 个实际 `#if` 块**（全部验证为结构性或平台固有）
- 状态: **清理完成** ✓
