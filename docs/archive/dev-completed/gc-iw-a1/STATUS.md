---
task_id: gc-iw-a1
title: BGC-YoungGC 并发 segfault 修复 (C1)
task_type: plan
lifecycle_status: completed
phase: completed
created_at: 2026-05-23
updated_at: 2026-05-23
current_dir: docs/dev/completed/gc-iw-a1
parent_task_id: 20260523-crag-gc-industrialization-sequel
source_relation: roadmap-child
clearance_source: parent-roadmap
---

# STATUS — gc-iw-a1: BGC-YoungGC 并发 segfault 修复

## 目标

修复 `gc_bgc_smoke.cpp:TestBgcWithYoungGc` 中 BGC 并发标记与前台 Young GC 之间的交互 segfault（当前被 SEH `__try/__except` 隔离）。使 BGC 与 Young GC 能安全并发执行，消除生产环境随机崩溃风险。

## 执行摘要

### 诊断结论

实际诊断发现两个问题：

1. **P1 (已修复): BgcSweep 中 numa_node use-after-decommit** — `gc_old_gen.cpp` 中 `p->numa_node` 在 `VirtualFree(p, ..., MEM_DECOMMIT)` 之后被读取，导致访问已解除提交的页面而崩溃。**这是测试启动时 crash 的直接根因**。修复：将 numa_node 读取移到 VirtualFree 之前。

2. **P2 (已修复): BGC concurrent mark 期间 Young GC 竞争** — 理论风险：BGC mark stack 可能持有 nursery 旧地址，Young GC 的 Cheney BFS 复制后变为 forwarding pointer。当前代码中 BGC root 扫描已通过 `IsInOldGen()` 过滤 nursery 引用，但作为一种防御性加固，添加了标记期 Young GC 跳过守卫。

### 已应用修复

#### 修复 1: numa_node use-after-decommit（`gc_old_gen.cpp`）
- 将 `p->numa_node` 的读取移到 `VirtualFree()` 调用之前

#### 修复 2: BGC marking 期 Young GC 跳过守卫（`gc_young_collector.cpp`）
- `GcYoungCollection()` 入口添加 `g_bgc_is_marking` 检查
- marking 中跳过后，caller 的分配路径自动回退到 old-gen/full GC
- 产生 `young_collection_skipped_bgc_marking` WARN 日志，表明守卫生效

### 验证结果

| 测试套件 | 结果 |
|---------|------|
| chaos_gc_bgc_smoke (6 tests) | ALL PASS |
| chaos_gc_bgc_race_test | PASS (5 young GCs during mark all skipped) |
| chaos_gc_young_collector_test (5 tests) | ALL PASS |
| chaos_gc_region_test (4 tests) | ALL PASS |
| chaos_gc_api_test | PASS |
| chaos_gc_card_table_test (4 tests) | ALL PASS |
| chaos_gc_bit_utils_test (4 tests) | ALL PASS |
| chaos_gc_bgc_stress_test | PASS (no crash) |

### 构建修复

预存未提交代码中的构建错误也已修复：
- `metadata_interface.h/cpp`: 修复 include 路径 `<contracts/native/v0/unified_metadata.h>` → `<unified_metadata.h>`
- `metadata_interface.cpp`: 修复 `CHAOS_IL2CPP_LOG_DEBUG` 变体使用错误
- `timer_queue.cpp`: 恢复合并中丢失的 `HeapSiftUp` 函数头

## 完成证据

- arch_review: ok
- authority_review: n/a
- verification:
  - canonicalCommand: chaos_gc_bgc_smoke.exe && chaos_gc_bgc_race_test.exe
  - formalObject: gc_bgc_race_test.cpp (new targeted race test)
  - summaryPath: docs/dev/completed/gc-iw-a1/STATUS.md
- test_result: passed (all tests 0 failures)
- wiki: n/a
- next: merge

## 结构告警与架构审视

改动量小、边界清晰，无重复逻辑、无架构退化。`gc_young_collector.cpp` 的单点守卫修改符合职责边界。BGC 与 Young GC 的职责分离保持。

## 风险记录

- 守卫仅在 BGC concurrent mark 期间激活；mark 结束后 Young GC 恢复正常
- 若 nursery 在 mark 期间完全填满，分配回退到 old-gen/full GC，短期可能增加 old-gen 内存压力但安全
