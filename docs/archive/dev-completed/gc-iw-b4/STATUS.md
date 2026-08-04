---
task_id: gc-iw-b4
title: retired_arrays_ 泄漏修复（C9）
task_type: plan
lifecycle_status: completed
phase: completed
parent_task_id: 20260523-crag-gc-industrialization-sequel
source_relation: roadmap-child
clearance_source: parent-roadmap
created_at: 2026-05-23
updated_at: 2026-05-23
current_dir: docs/dev/completed/gc-iw-b4
---

# STATUS — gc-iw-b4：retired_arrays_ 泄漏修复（C9）

## 问题

`RebuildPageArray()` 每次调用都会交换 `page_array_`，将旧数组追加到 `retired_arrays_`，但数组从不释放（注释说"keep alive forever"以保护并发读者）。随着 GC 周期运行，`retired_arrays_` 无限增长，导致内存泄漏。

## 方案

有界退役数量 `kMaxRetiredPageArrays=4`。当 `RebuildPageArray()` 退役新数组时，若已超过上限，释放最旧的数组。保留 4 代意味着任何在 exchange 前加载了 `page_array_` 指针的读者都能在其保留的代中找到数组；读者被调度器抢占跨越 4 次连续 rebuild 的概率可忽略。

## 完成证据

- arch_review: ok（常量 + bounded retire 逻辑，无结构影响）
- authority_review: n/a
- verification:
  - unitTest: 6/6 (old_gen)
  - stressTest: 3/3 (gc_stress_test)
  - pipeline: 10/10 (garbage-collection)
- test_result: passed
- wiki: n/a
- next: gc-iw-b5（Finalizer 线程池替代 std::async）
