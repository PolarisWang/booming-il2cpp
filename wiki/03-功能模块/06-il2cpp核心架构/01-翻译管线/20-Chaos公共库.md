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
| `unordered_dense.h` | `CHAOS_IL2CPP_UNORDERED_DENSE_MAP(K,V)` | 高性能 open-addressing hash map 包装器 |
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

## unordered_dense 性能对比

`<chaos/unordered_dense.h>` 封装了 [ankerl::unordered_dense](https://github.com/martinus/unordered_dense) v4.8.1（robin-hood 开放寻址法）。

### 宏定义

```cpp
#include <chaos/unordered_dense.h>

// 默认方式（使用 ankerl 内置 wyhash）
CHAOS_IL2CPP_UNORDERED_DENSE_MAP(K, V)                // → ankerl::unordered_dense::map<K, V>

// Identity hash 方式（对已均匀分布的 key 跳过 wyhash）
CHAOS_IL2CPP_UNORDERED_DENSE_MAP_IDENTITY(K, V)       // → ankerl::unordered_dense::map<K, V, chaos::il2cpp::common::identity_hash>
```

### identity_hash

`chaos::il2cpp::common::identity_hash` 是专为已均匀分布的 key 设计的高效哈希，标记了 `is_avalanching` 以跳过 ankerl 内部的 wyhash mixing。通过模板 SFINAE 支持三类 key：

| Key 类型 | 实现 | 适用场景 |
|----------|------|---------|
| 任意整数类型 | `static_cast<uint64_t>(static_cast<make_unsigned_t<T>>(v))` | 顺序 ID、已哈希值、指针转整数 |
| 枚举类型 | `static_cast<uint64_t>(static_cast<underlying_type_t<T>>(v))` | RegionKind 等枚举 key |
| 指针类型 `T*` | `reinterpret_cast<uint64_t>(ptr)` | TypeInfoHandle、MethodInfoHandle 等句柄指针 |

**什么时候用 identity_hash（替代默认 wyhash）：**

| Key 分布特性 | 使用 | 原因 |
|-------------|------|------|
| 堆指针（ASLR 分布均匀） | ✅ MAP_IDENTITY | 指针低 bit 已随机，wyhash 纯浪费 |
| 单调递增 ID（RegionId、handle） | ✅ MAP_IDENTITY | identity 零碰撞，wyhash 反而破坏 |
| 已哈希的值（FNV-1a stable_id、组合 bit-packed） | ✅ MAP_IDENTITY | 已经是均匀分布，无需再 wyhash |
| 字符串（string key） | ❌ 不可用 | identity 对 string 会取地址而非内容 |
| 小范围枚举 / bool | ⚠️ 评估后使用 | 低熵 key 用 identity 会导致大量碰撞 |

### 性能对比（MSVC 2022 Release, N=100k）

| 操作 | std (ms) | dense (ms) | ratio |
|------|----------|-----------|-------|
| **顺序插入（no reserve）** | 7.13 | 2.57 | **0.36x** |
| **顺序插入（with reserve）** | 7.29 | 2.48 | **0.34x** |
| **随机插入（no reserve）** | 9.08 | 5.89 | **0.65x** |
| **随机插入（with reserve）** | 5.46 | 3.23 | **0.59x** |
| **Lookup 100% hit** | 0.92 | 0.39 | **0.43x** |
| **Lookup 100% miss** | 0.98 | 0.43 | **0.43x** |
| **Lookup 90/10 mixed** | 0.83 | 0.42 | **0.51x** |
| **Pointer insert (void\*)** | 6.85 | 1.88 | **0.27x** |
| **Pointer lookup** | 1.59 | 1.35 | **0.85x** |
| **String insert** | 2.80 | 0.72 | **0.26x** |
| **String lookup** | 0.49 | 0.35 | **0.72x** |
| **Erase all** | 4.02 | 0.50 | **0.13x** |
| **Erase 25% + reinsert** | 1.93 | 0.32 | **0.17x** |
| **Iteration** | 0.53 | 0.04 | **0.08x** |
| **Copy construction** | 9.86 | 0.46 | **0.05x** |
| **Clear + repopulate** | 9.36 | 0.65 | **0.07x** |
| **Large burst 500K** | 72.03 | 14.08 | **0.20x** |
| **Pathological hash** | 173.84 | 174.86 | **1.01x** |

完整报告见 `tests/perf/unordered-map-bench/benchmark-report.md`

**要点解读：**
- unordered_dense 在几乎所有场景大幅领先 std，尤其 erase（0.13x）、iteration（0.08x）、copy 构造（0.05x）、clear+repopulate（0.07x）
- 唯一持平场景是 pathological hash（hash=const 0），两者均退化为 O(n)，dense 未额外退化
- 连续内存布局带来 cache locality 优势，遍历性能 ~12x 倍于 std 链式桶

### 哈希质量敏感度分析

不同哈希函数对 dense 和 std 的影响差异显著（N=50000）：

| 哈希质量 | 操作 | dense/std ratio | 说明 |
|---------|------|----------------|------|
| **Identity** (h(k)=k, 完美分布) | Insert | **0.21x** | dense 大幅领先 |
| **Identity** | Lookup | 0.4x–5.2x | 两者接近，std 方差大(±5x)、dense 方差小(±1.01x) |
| **Colliding** (h(k)=k & 0xFF, 256槽) | Insert | **0.26x** | dense 远优于 std |
| **Colliding** | Lookup | **0.23x** | dense 远优于 std |
| **SplitMix64** (标准质量) | Insert | **0.34x** | dense 大幅领先 |
| **SplitMix64** | Lookup | 0.5x–4.8x | 同 Identity，std 方差大、dense 稳定 |
| **Pathological** (const 0) | Insert | **1.01x** | 两者持平，均退化为 O(n) |

**关键发现：**

1. **Colliding 哈希下 dense 远优于 std** — 仅有 256 个 distinct hash 时，std 链式桶导致长链，而 dense 的 robin-hood 探查通过位移均衡负载，插入和查找均保持 ~0.2x 的压倒性优势

2. **Good hash 下 lookup 两者接近** — 当哈希完美分布时，两者 lookup 的绝对耗时相当（0.3-1ms 量级），dense 的 **run-to-run 方差显著更低**（~1% vs std 的 5-10%），表现更可预测

3. **极端碰撞两者同退化为 O(n)** — Pathological hash 下 robin-hood 无法补偿，dense = std，说明 dense 不劣化但也不免疫极端哈希

### 使用限制

| # | 限制 | 说明 |
|---|------|------|
| 1 | **Iterator 在 rehash 后失效** | 任何触发扩容的 insert 会使所有迭代器失效。不能在 insert 调用之间持有迭代器。与 `std::unordered_map` 不同。 |
| 2 | **指针/引用不稳定** | insert 和 erase 可能移动元素内存位置。元素的指针和引用在修改操作后不再可靠。只适合存储值类型（或裸指针）。 |
| 3 | **不支持异构查找** | `std::unordered_map` 支持 C++20 transparent hash（例如 `find("key"sv)` 在 `map<string, T>` 上）。unordered_dense 不支持。必须使用精确的 key 类型。 |
| 4 | **内存连续性** | 整个表是一个连续内存块（单个分配），不像 `std::unordered_map` 的链式桶。这对缓存友好但大表可能分配较大连续空间。 |
| 5 | **哈希质量敏感** | 开放寻址法在哈希质量差时退化为 O(n)（与 `std::unordered_map` 一样）。对自定义类型，务必使用高质量哈希。 |

### 适用范围

**推荐场景**：高频查找表（type→handler）、读多写少负载、批量插入后只读、Copy 密集模式、遍历密集模式。

**不推荐场景**：频繁插入删除且需要持用元素指针、异构查找、元素在 map 中移动后再取引用、要求严格的迭代器稳定性。

### 工程注意事项

- **已转换的模块**：以下模块已从 `std::unordered_map` 切换到 `CHAOS_IL2CPP_UNORDERED_DENSE_MAP`：
  - `monitor.h` — `intptr_t` key, heap-allocated value (pointer stable)
  - `bootstrap.cpp` — `uint32_t` key, `void*` value (write-once, read-only after init)
  - `sync_mutex.cpp` — `void*` key → **MAP_IDENTITY** (pointer key optimization), `SyncBlock*` value
  - `method_replacement.cpp` — `uint32_t` key, struct value (by-value, no ptr stability needed)
  - `engine_bridge.cpp` — `uintptr_t` key, struct value (mutex-protected, no cross-op ptr holding)
- **保留为 `std::unordered_map` 的模块**（依赖迭代器/指针稳定性）：
  - `thread.h` — 返回 `ThreadRuntimeEntry*` raw pointer 跨越 insert
  - `wait_handle.cpp` — 返回 `WaitHandleEntry*` raw pointer 跨越 mutex unlock
  - `patch_loader.h` — 返回 `c_str()` pointer 指向 map 内 string
- **`collection.h` 开关**：定义 `CHAOS_IL2CPP_USE_DENSE_MAP` 宏可使 `collection.h` 内部的 runtime storage 使用 dense map 替代 `std::unordered_map`。
- **`native_types.h` 不受影响**：`CHAOS_IL2CPP_UNORDERED_MAP` 保持映射到 `std::unordered_map`，不影响 codegen 输出。dense map 为 opt-in。

### Dense Map 选型决策指南

开发者在编写或 review 代码时，遇到 `unordered_map` 选型，按以下优先级判断：

```
代码中需要 map？
├─ 需要 iterator/pointer 跨 insert 稳定性？           → CHAOS_IL2CPP_UNORDERED_MAP (std)
├─ 需要 heterogeneous lookup (C++20 transparent hash)？ → CHAOS_IL2CPP_UNORDERED_MAP (std)
├─ 热更新 codegen 输出路径？                           → CHAOS_IL2CPP_UNORDERED_MAP (std)
├─ key 是 string 且查找前需要构造临时 key？             → CHAOS_IL2CPP_UNORDERED_MAP (std)
├─ map 作为类成员长期持有、频繁 insert/erase 交替？    → 评估 rehash 开销，默认 std
├─ value 类型 > 128 字节？                             → 评估 copy 开销，默认 std
└─ 以上都不是 → 优先使用 CHAOS_IL2CPP_UNORDERED_DENSE_MAP，特别当：
   ├─ 读多写少（batch insert → read-only）
   ├─ key 为整型 / 指针类型
   ├─ 高频查找（type→handler、id→vtable 等）
   ├─ 密集遍历
   ├─ 频繁 copy 构造（snapshot/checkpoint）
   └─ 需要低延迟方差（实时性要求）
```

**经验法则**：新写的查找表、注册表、缓存类，只要不需要 iterator 稳定性和异构查找，优先用 `CHAOS_IL2CPP_UNORDERED_DENSE_MAP`。只有明确依赖 std 语义（稳定迭代器、异构查找、codegen 兼容）时才回退到 `CHAOS_IL2CPP_UNORDERED_MAP`。

## 位置

- 模块: `chaos_common`
- 目录: `src/native/common/chaos/`
