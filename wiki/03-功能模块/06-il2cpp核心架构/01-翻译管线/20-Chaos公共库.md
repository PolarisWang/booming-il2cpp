# Chaos 公共库（chaos_common）

## 核心职责

零依赖的公共工具库，是所有生成代码和手写运行时的共享基础设施。位于 `src/native/common/chaos/*.h`，通过 `<chaos/common.h>` 统括头文件暴露。

## 16 个头文件总览

| 文件 | 核心类型/函数 | 设计用途 |
|------|-------------|---------|
| `config.h` | `CHAOS_IL2CPP_CONFIG_CHECK/PROFILE/SHIP`, `CHAOS_IL2CPP_DEFAULT_LOG_LEVEL`, `CHAOS_IL2CPP_TRACE_ENABLED` | 构建配置分层：CHECK / PROFILE / SHIP 三级，控制日志级别、trace、assert |
| `native_types.h` | `CHAOS_IL2CPP_INT32`, `CHAOS_IL2CPP_STRING`, `CHAOS_IL2CPP_MUTEX` 等宏 | 标准库抽象层，所有生成代码必须使用宏而非裸类型 |
| `arithmetic.h` | `wrap_add/sub/mul`, checked `div/rem`, `shift_left/right/unsigned` | CIL 算术语义的 C++ 实现，移位掩码 5/6 bit |
| `checked_conv.h` | `checked_conv_ovf_i1/u1/i2/u2` | conv.ovf 指令的溢出检查转换 |
| `load_store.h` | float/double/int64 的 bitcast 存储/加载 | 通过 intptr 槽的值打包和解包，避免 UB |
| `finally.h` | `FinallyGuard`, `make_finally_guard` | RAII 确定性清理 |
| `ptr_tag.h` | `k_managed_pointer_local_slot_tag`, `k_raw_int32_pointer_tag`, `resolve_native_int_slot()` | 指针标记系统，区分 3 种指针类别 |
| `static_init.h` | `ensure_initialized(flag, init)` | `std::call_once` 包装器，线程安全惰性初始化 |
| `format.h` | `format_int32`, `format_float`, `format_double`, `format_string`, `format_append_to` | {fmt} 包装，替换 snprintf 模式 |
| `status.h` | `Status` 枚举（kOk ~ kInternalError）, `is_ok()`, `is_failure()` | 统一错误码 |
| `span.h` | `SpanRuntimeEntry`, `MemoryRuntimeEntry`, `create_array_span_int32` 等 | Span/Memory 运行时条目 |
| `collection.h` | `ListRuntimeStorage<T>`, `DictionaryRuntimeStorage<K,V>`, `require_list_runtime_storage()` | 托管 List/Dictionary 运行时存储 |
| `monitor.h` | `MonitorRuntimeEntry`, `require_monitor_runtime_entry(object)` | System.Threading.Monitor（C# lock） |
| `thread.h` | `ThreadRuntimeEntry`, TLS `current_thread_object`, `allocate_managed_thread_id()` | System.Threading.Thread 运行时模型 |
| `async.h` | `AsyncTask`, `async_yield_create`, `async_task_get_awaiter()` | async/await 状态机运行时基元 |
| `common.h` | 统括 include | 单一 `#include <chaos/common.h>` 入口 |

## 构建配置分层（config.h）

### 三级配置概览

| 特性 | CHECK | PROFILE | SHIP |
|------|-------|---------|------|
| 编译定义 | `CHAOS_IL2CPP_CONFIG_CHECK` | `CHAOS_IL2CPP_CONFIG_PROFILE` | `CHAOS_IL2CPP_CONFIG_SHIP` |
| 日志级别 | DEBUG (3) | INFO (2) | ERROR (0) |
| Trace | 启用 | 启用 | 禁用 |
| Assert | 启用 (abort) | 空操作 | 空操作 |

### 选择默认配置

当未显式指定时，默认为 `CHAOS_IL2CPP_CONFIG_CHECK`：
```cpp
#if !defined(CHAOS_IL2CPP_CONFIG_CHECK) && \
    !defined(CHAOS_IL2CPP_CONFIG_PROFILE) && \
    !defined(CHAOS_IL2CPP_CONFIG_SHIP)
#  define CHAOS_IL2CPP_CONFIG_CHECK
#endif
```

### 日志级别映射

```cpp
// CHAOS_IL2CPP_DEFAULT_LOG_LEVEL
// CHECK  → 3 (DEBUG + INFO + WARN + ERROR)
// PROFILE → 2 (INFO + WARN + ERROR)
// SHIP    → 0 (ERROR only)

// 宏门控：
// CHAOS_IL2CPP_LOG_DEBUG   — level >= 3
// CHAOS_IL2CPP_LOG_INFO    — level >= 2
// CHAOS_IL2CPP_LOG_WARN    — level >= 1
// CHAOS_IL2CPP_LOG_ERROR   — always compiled
```

### Trace 门控

```cpp
// CHECK 和 PROFILE 定义 CHAOS_IL2CPP_TRACE_ENABLED
// SHIP 中所有 CHAOS_IL2CPP_LOG_TRACE_* 宏展开为 ((void)0)
```

### Assert 行为

```cpp
// CHAOS_IL2CPP_ASSERT(cond)
// CHECK 中: if (!(cond)) std::abort()
// PROFILE/SHIP 中: ((void)0)
```

### CMake 集成

在 `runtime-core/CMakeLists.txt` 中通过 `target_compile_definitions` 设置默认配置：
```cmake
target_compile_definitions(chaos_runtime_core PUBLIC CHAOS_IL2CPP_CONFIG_CHECK)
```

上层项目可以通过在包含 chaos 头文件之前定义对应的 `CHAOS_IL2CPP_CONFIG_*` 宏来覆盖配置。

### 位置

- 配置头文件: `src/native/common/chaos/config.h`
- CMake 设置: `src/native/runtime-core/CMakeLists.txt`

## 指针标记系统（ptr_tag.h）

关键设计——用低位标记位区分指针类别，避免单独的结构体跟踪：

| 标记 | 值 | 含义 |
|------|-----|------|
| 无标记 | 0 | 原生指针 |
| `k_managed_pointer_local_slot_tag` | bit 0 | 托管局部变量槽指针 |
| `k_raw_int32_pointer_tag` | bit 1 | 原始 int32 数据指针 |

`resolve_native_int_slot()` 和 `resolve_block_copy_address()` 在执行前会剥离标记。

## 生成代码的宏约束

`native_types.h` 定义了 <cstdint> 和 std 类型的宏别名。所有生成代码必须遵守：

| 裸 std 类型 | 必须使用宏 |
|-------------|-----------|
| `std::int32_t` | `CHAOS_IL2CPP_INT32` |
| `std::string` | `CHAOS_IL2CPP_STRING` |
| `std::vector<T>` | `CHAOS_IL2CPP_VECTOR(T)` |
| `std::unordered_map<K,V>` | `CHAOS_IL2CPP_UNORDERED_MAP(K,V)` |
| `std::unique_ptr<T>` | `CHAOS_IL2CPP_UNIQUE_PTR(T)` |
| `std::mutex` | `CHAOS_IL2CPP_MUTEX` |
| `std::lock_guard<M>` | `CHAOS_IL2CPP_LOCK_GUARD(M)` |
| `std::memcpy(d,s,n)` | `CHAOS_IL2CPP_MEMCPY(d,s,n)` |
| `std::abort()` | `CHAOS_IL2CPP_ABORT()` |

宏语法规则：使用 `()` 括号语法（不是 `<>`），嵌套宏需要中间 using typedef。

## 位置

- 模块: `chaos_common`
- 目录: `src/native/common/chaos/`
