# CommonLibrary 模块使用说明

> CommonLibrary (`src/native/common/chaos/`) 是 Chaos IL2CPP 的公共基础库，提供 25 个轻量头文件模块，零运行时依赖（除 fmtlib + unordered_dense）。

---

## 核心类型

### `native_types.h` — 原生类型定义

提供跨平台固定宽度类型、指针包装、标签指针 (tagged pointer) 操作。

```cpp
#include <chaos/native_types.h>

CHAOS_IL2CPP_INTPTR ptr = ...;
bool tagged = (ptr & CHAOS_IL2CPP_RAW_POINTER_TAG) != 0;
auto untagged = chaos_load_indirect(ptr);   // 自动去标签
```

**关键宏：** `CHAOS_IL2CPP_INTPTR`, `CHAOS_IL2CPP_UINTPTR`, `CHAOS_IL2CPP_RAW_POINTER_TAG`
**关键函数：** `chaos_load_indirect`, `chaos_store_indirect`, `resolve_native_int_slot`

### `type_info.h` — 类型元数据布局

提供 MethodTable/VTable/TypeInfo 的静态断言和布局常量。

```cpp
#include <chaos/type_info.h>
static_assert(sizeof(MethodTable) == kMethodTableSize);
```

**关键常量：** `kMethodTableSize`, `kMaxDynamicTypes`, `kTypeShapeGeneric`, `kTypeInfoFlag*`

### `config.h` — 构建配置检测

检测当前构建模式 (CHECK/PROFILE/SHIP)、平台、异常处理模式。

```cpp
#include <chaos/config.h>
#if CHAOS_IL2CPP_CONFIG == CHAOS_IL2CPP_CONFIG_CHECK
    // Debug 模式
#endif
```

### `compiler_hints.h` — 编译器提示宏

```cpp
#include <chaos/compiler_hints.h>
CHAOS_IL2CPP_FORCE_INLINE void hot_func();
if (CHAOS_IL2CPP_LIKELY(ptr != nullptr)) { ... }
```

---

## 基础设施

### `log.h` — 结构化日志

五级日志 (ERROR/WARN/INFO/DEBUG/RAW)，支持 fmtlib 格式化。

```cpp
#include <chaos/log.h>
CHAOS_IL2CPP_LOG_ERROR("Tag", "message");
CHAOS_IL2CPP_LOG_INFO_M("Tag", "value={}", x);    // 格式化版本
CHAOS_IL2CPP_LOG_WRITE_RAW_M("plain text {}\n", x);
```

**线程安全：** 是。内部使用原子操作分配槽位。

### `trace.h` — 追踪系统 (JSONL)

基于环形缓冲区的结构化追踪，用于 CHECK/PROFILE 构建。

```cpp
#include <chaos/trace.h>
CHAOS_IL2CPP_LOG_TRACE("codegen", "emit_method", "count={}", 18);
CHAOS_IL2CPP_LOG_TRACE_INIT();    // 从环境变量初始化
CHAOS_IL2CPP_LOG_TRACE_FLUSH("/tmp/trace.jsonl");
```

**线程安全：** 是（每个线程独立时间戳缓存，原子索引递增）。

### `profile.h` — RDTSC 性能分析

RAII 风格的热点计时器，精度纳秒级。

```cpp
#include <chaos/profile.h>
{ CHAOS_IL2CPP_PROFILE_SCOPE("my_op"); do_work(); }
CHAOS_IL2CPP_PROFILE_DUMP();    // 输出到 stderr
CHAOS_IL2CPP_PROFILE_RESET();   // 清除累积数据
```

**注意：** 默认仅在 PROFILE 构建中启用；CHECK 构建中展开为空操作。`ProfileDump()` 会自动释放已退出线程的退役节点。

### `eh.h` — 异常处理模式检测

```cpp
#include <chaos/eh.h>
#if CHAOS_IL2CPP_EH_MODE == CHAOS_IL2CPP_EH_WIN32_SEH
    // Windows SEH 异常处理
#endif
```

### `status.h` — 状态码

```cpp
#include <chaos/status.h>
using Status = chaos::il2cpp::common::Status;
if (result.IsOk()) { ... }
```

---

## 并发与同步

### `async.h` — 异步任务

轻量级 Task.Run / awaiter 协议，由运行时 ThreadPool 驱动。

```cpp
#include <chaos/async.h>
auto task = async_task_run(delegate_fn);
// 检查完成状态
if (async_task_awaiter_get_is_completed(awaiter_ref)) {
    auto result = async_task_awaiter_get_result_raw(awaiter_ref);
}
```

**错误处理：** 默认 `async_task_awaiter_get_result_raw` 在任务故障时调用 `CHAOS_IL2CPP_ABORT()`。可通过 `async_task_awaiter_get_exception()` 检查异常指针。

### `thread.h` — 线程创建

### `monitor.h` — Monitor 运行时条目

提供对象头 Monitor 运行时条目的哈希表管理。

```cpp
#include <chaos/monitor.h>
auto& entry = require_monitor_runtime_entry(object_handle);
```

### `synchronization.h` — 同步原语

---

## 工具模板

### `load_store.h` — 原子加载/存储 (float/double/int)

```cpp
#include <chaos/load_store.h>
float f = chaos_load_float32(&ptr);
chaos_store_float64(&ptr, 3.14);
```

### `ptr_tag.h` — 指针标签操作

### `finally.h` — RAII finally 守卫

```cpp
#include <chaos/finally.h>
auto guard = CHAOS_IL2CPP_FINALLY([&] { cleanup(); });
guard.Dismiss();  // 取消执行
```

### `static_init.h` — 一次性初始化

```cpp
#include <chaos/static_init.h>
static CHAOS_IL2CPP_ONCE_FLAG flag;
CHAOS_IL2CPP_ENSURE_INITIALIZED(flag, { init_once(); });
```

### `tls_buffer.h` — 线程本地缓冲区

### `collection.h` — 集合运行时存储

Dictionary<K,V> 的原生运行时存储管理（托管对象嵌入的 native 指针）。

```cpp
#include <chaos/collection.h>
auto* storage = require_dictionary_runtime_storage<int, float>(handle);
storage->entries.push_back({1, 3.14f});
```

**线程安全：** 否。调用者需自行同步。

### `span.h` — 轻量数组视图

---

## 数值与类型转换

### `arithmetic.h` — 安全算术运算

带溢出检测的包装算术操作。

```cpp
#include <chaos/arithmetic.h>
auto [result, overflow] = chaos::il2cpp::common::wrap_add(INT32_MAX, 1);
// overflow == true
```

### `checked_conv.h` — 带边界检查的类型转换

```cpp
#include <chaos/checked_conv.h>
auto r = chaos::il2cpp::common::checked_conv_i1(200); // 溢出 → CHAOS_IL2CPP_FAIL
```

### `native_types.h` 中的 `CHAOS_IL2CPP_NUMERIC_LIMITS_*` 宏

---

## 特殊模块

### `asan_interface.h` — ASan 集成

在 ASan 启用时提供内存 poisoning/unpoisoning，禁用时为空操作。

```cpp
#include <chaos/asan_interface.h>
AsanUnpoison(ptr, size);       // 函数形式
CHAOS_IL2CPP_ASAN_POISON(ptr, size);  // 宏形式（热路径零开销）
AsanReadPtrNoCheck(addr);      // 跳过 ASan 的指针读取（保守栈扫描用）
```

### `unordered_dense.h` — 高性能哈希表包装

提供 `ankerl::unordered_dense` 的便捷宏包装。

```cpp
#include <chaos/unordered_dense.h>
CHAOS_IL2CPP_UNORDERED_DENSE_MAP_IDENTITY(int, int) m;  // 跳过 wyhash
CHAOS_IL2CPP_UNORDERED_DENSE_MAP(std::string, int) m2;
CHAOS_IL2CPP_UNORDERED_DENSE_SET(int) s;
```

### `json_reader.h` — JSON 解析器

### `format.h` — 格式化（fmtlib 包装）

### `common.h` — 聚合头文件

包含所有 common 模块。推荐在测试文件中使用：

```cpp
#include <chaos/common.h>   // 一次性包含所有 common 模块
```

---

## 线程安全总结

| 模块 | 线程安全 | 说明 |
|------|---------|------|
| log.h | 是 | 原子槽位分配 |
| trace.h | 是 | 每个线程独立时间戳缓存 |
| profile.h | 是 | 线程局部累积，全局 dump 加锁 |
| async.h | 否 | 需调用者保证单线程访问模式 |
| monitor.h | 是 | 全局哈希表原子操作 |
| collection.h | 否 | 需外部同步 |
| tls_buffer.h | 是 | 线程局部存储 |
| static_init.h | 是 | call_once 语义 |

---

## 测试

所有模块的单元测试位于 `testing/src/native/common/`，使用 GoogleTest 框架：

```bash
# 运行全部 common 测试
ctest -R "test_common" --output-on-failure

# 运行单个测试
./test_common_log
```

测试覆盖 33 个可执行目标，185+ 个测试用例，涵盖单元、压力、并发和模糊测试。
