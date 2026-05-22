---
task_id: gc-iw-c3
title: GcEndStubbornChange 实现（C13）
task_type: plan
lifecycle_status: completed
phase: completed
parent_task_id: 20260523-crag-gc-industrialization-sequel
source_relation: roadmap-child
clearance_source: parent-roadmap
preflight_review: pass
auto_execution_decision: continue
created_at: 2026-05-23
updated_at: 2026-05-23
current_dir: docs/dev/completed/gc-iw-c3
---

# STATUS — gc-iw-c3：GcEndStubbornChange 实现（C13）

## 问题

`GcEndStubbornChange` 在 `gc_helpers.h:56` 是空函数体 no-op，注释标记为 "C3 stub"。虽然在 CRAG 的即时 dirty card 写入模型下不需要"stubborn→dirty 批量转换"，但缺失 memory ordering fence 在多线程场景下可能导致 card dirty 在 GC 扫描线程中不可见。

## 方案

CRAG 写屏障在每次写入时直接调用 `DirtyCard()` 立即标记 card（`memory_order_relaxed` 写入），无需 stubborn 延迟转换。`GcEndStubbornChange` 在此架构下等价于 memory ordering fence：

- 使用 `std::atomic_thread_fence(std::memory_order_seq_cst)` 确保 fence 前的所有 card dirty 写入在 GC 扫描线程中可见
- 保留 extern "C" ABI 导出（codegen 生成的代码可能调用此符号）

## 改动文件

| 文件 | 改动 |
|------|------|
| `src/native/runtime-core/gc/gc_helpers.h` | 新增 `#include <atomic>`；替换空函数体为 `atomic_thread_fence(seq_cst)` 实现 + 注释说明 |
| `testing/src/native/runtime-core/gc/gc_managed_api_test.cpp` | 新增 `GcEndStubbornChange` 测试（nullptr、有效对象、批量调用模式） |

## 完成证据

- arch_review: ok（单行 fence 替换，无结构影响）
- authority_review: n/a
- verification:
  - test_gc_managed_api: 11/11 PASS（含新增 GcEndStubbornChange 测试）
  - test_gc_scheduler: 13/13 PASS
- test_result: passed
- wiki: n/a
- next: gc-iw-d1（ARM64 编译验证 + 写屏障 UT）
