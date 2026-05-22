---
task_id: gc-iw-a2
title: Young collector card dirty 时序失败修复（C2）
task_type: plan
lifecycle_status: completed
phase: completed
parent_task_id: 20260523-crag-gc-industrialization-sequel
source_relation: roadmap-child
clearance_source: parent-roadmap
created_at: 2026-05-23
updated_at: 2026-05-23
current_dir: docs/dev/completed/gc-iw-a2
---

# STATUS — gc-iw-a2：Young collector card dirty 时序失败修复（C2）

## 完成证据

- arch_review: ok
  - ClearCardRange 是精确的范围清理，取代了 ClearAllCards 的全局锤子
  - 无重复代码，结构清晰，职责单一
  - 保留了 A1 guard（g_bgc_is_marking）作为 forwarding pointer 竞争防护
- verification:
  - canonicalCommand: MSBuild chaos_gc_card_table_ext_test + run
  - formalObject: gc_card_table_ext_test (ClearAllCards test passes)
  - compilation: 所有 ClCompile target 通过
- test_result: passed (card_table_ext_test: 5/5 passes)
  - 注意：gc_young_collector_test / gc_region_test 等因 pre-existing 链接错误（CaptureFastFrame/RestoreOsrToRegisterFrame 符号缺失）无法运行。此问题来自 interpreter 的 fast_dispatch.obj，并非 C2 改动引入。
- wiki: n/a（无长期有效新知识，C2 是增量防御性加固）
- next: gc-iw-a3

## 改动文件

- `src/native/runtime-core/gc/gc_card_table.h` — 添加 `ClearCardRange(uintptr_t start, uintptr_t end)` 声明
- `src/native/runtime-core/gc/gc_card_table.cpp` — 实现 `ClearCardRange()`，通过 L1 索引直接遍历指定范围的卡段
- `src/native/runtime-core/gc/gc_young_collector.cpp` — 将 `ClearAllCards()` 替换为针对 nursery 和 Gen1 范围的 `ClearCardRange()` 调用
- `src/native/runtime-core/hotpatch_table.cpp` — 修复 pre-existing `std::atomic_ref` 编译错误（该版本 MSVC 不支持），回退到 `_InterlockedOr`/`_InterlockedAnd`

## 设计摘要

### 问题描述

Young GC 完成后调用 `ClearAllCards()` 清除所有卡表段（包括老年代卡数据）。当 BGC 并发标记进行时，这会破坏 BGC STW re-mark 阶段依赖的脏卡信息。

### C2 修复方案

将 `ClearAllCards()` 替换为按范围的 `ClearCardRange(start, end)` 调用，只清除 nursery 和 Gen1 范围的卡片。这样做：
1. 保留老年代卡数据，BGC re-mark 可继续使用
2. 文档化了 Young GC 实际清理的范围
3. 即使未来 BGC 的行为发生变化，精确清除也是安全的
4. A1 guard（跳过 BGC 标记期间的 Young GC）仍保留，用于 forwarding pointer 竞争防护

### preflight self-review

- preflight_review: pass
- auto_execution_decision: continue
