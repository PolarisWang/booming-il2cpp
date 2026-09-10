# C7 — ARM64 内存序验证与修复

> **task_id**: c7-arm64-memory-ordering
> **创建日期**: 2026-05-23
> **parent_task_id**: hotupdate-industrialization
> **source_relation**: roadmap-child
> **clearance_source**: parent-roadmap
> **phase**: in-progress

## 目标

验证热更新路径在 ARM64 弱内存模型下的内存序正确性，修复发现的 volatile→atomic 问题。

## 范围

- `hotpatch_table.h/cpp` — `std::atomic_ref` 改造后的内存序验证
- `runtime_stubs/interlocked_stubs.cpp` — `ChaosVolatileRead`/`ChaosVolatileWrite` 修复
- `core/sync_mutex.cpp` — 已存在的平台抽象层确认
- GC 代码 — 已存在的 `#if defined(_MSC_VER)` 分支确认

## 审计结果

### 1. hotpatch_table — 正确 ✓

| 位置 | 模式 | 正确性 |
|------|------|--------|
| `SetPatchedBySlot` (writer, set) | `std::atomic_ref::fetch_or(kHotpatchActive, release)` | **正确** — method_key 在 release 前写入，保证 reader 通过 acquire load 可见 |
| `SetPatchedBySlot` (writer, clear) | `std::atomic_ref::fetch_and(~kHotpatchActive, acq_rel)` + method_key=0 | **正确** — acq_rel 保证 reader 看到 cleared flags 时不会读取 method_key |
| `HotpatchIsActive` (reader) | `std::atomic_ref::load(acquire)` | **正确** — acquire synchronizes-with writer's release |
| `HotpatchShouldKeepNative` (reader) | `std::atomic_ref::load(relaxed)` | **正确** — kKeepNative 在 AOT codegen 时设定，运行时不改变，relaxed 足够 |

### 2. interlocked_stubs.cpp — 已修复 ✓

| 函数 | 原实现 | 问题 | 修复后 |
|------|--------|------|--------|
| `ChaosVolatileRead` | `volatile` 读 | ARM64 MSVC 不保证 acquire 语义 | `std::atomic_load_explicit(acquire)` |
| `ChaosVolatileWrite` | `volatile` 写 | ARM64 MSVC 不保证 release 语义 | `std::atomic_store_explicit(release)` |

### 3. sync_mutex.cpp — 已确认正确 ✓

`AtomicLoadAcquire` 在 `_M_ARM64` 分支使用 `__dmb(0xB)`（full barrier），正确但偏重。不影响正确性，性能不是 Phase 1 关注点。

### 4. GC 代码 (gc_bgc.cpp, gc_mark_bitmap.h, gc_parallel_mark.cpp) — 已确认 ✓

`_InterlockedOr8` 在 MSVC ARM64 上是原子操作 + full barrier，正确。已有 `#if defined(_MSC_VER)` / `#else` 平台抽象。

## 修复清单

| 文件 | 行 | 修改 | 状态 |
|------|----|------|------|
| `src/native/runtime-core/runtime_stubs/interlocked_stubs.cpp:84-92` | `volatile` → `std::atomic_load/store_explicit` | **已修复** |
| `src/native/runtime-core/hotpatch_table.cpp:211-218` | `_InterlockedOr/And` → `std::atomic_ref::fetch_or/and` | C6 已完成 |
| `src/native/runtime-core/hotpatch_table.h:129-137` | fence+raw-read → `std::atomic_ref::load` | C6 已完成 |

## 结论

ARM64 内存序风险已全部排查并修复。关键路径（hotpatch dispatch）使用 `std::atomic_ref` 保证跨平台正确性。ABI stubs（VolatileRead/Write）已改用 `std::atomic` 操作。GC 和 sync_mutex 已有平台抽象层。
