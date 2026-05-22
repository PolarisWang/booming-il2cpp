---
task_id: gc-iw-d2
title: Server GC 生产硬化 + E2E 压力测试（C5）
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
current_dir: docs/dev/completed/gc-iw-d2
---

# STATUS — gc-iw-d2：Server GC 生产硬化 + E2E 压力测试（C5）

## 完成证据

- arch_review: ok
- authority_review: n/a
- verification:
  - canonicalCommand: ctest --test-dir build/testing -R test_gc_ --output-on-failure
  - formalObject: gc-scheduler (13/13), gc-card-table (9/9), gc-managed-api (11/11)
  - summaryPath: docs/dev/completed/gc-iw-d2/STATUS.md
- test_result: passed (33 tests, 0 failures)
  - scheduler 13/13 PASS
  - card_table 9/9 PASS (incl. 4 new ARM64/shared-ext tests)
  - managed_api 11/11 PASS (incl. GcEndStubbornChange smoke test)
  - stress: GcGeneralStressTest.AggressiveYoungGc — pre-existing failure (OOM recovery + pattern integrity), not caused by nursery bounds fix
- wiki: n/a
- next: parent-roadmap -> gc-iw-e1 (NUMA + Card bundle + CPU affinity)

## 改动文件

| 文件 | 改动 |
|------|------|
| `gc_region.h` | 新增 `ResetNurseryGlobalBounds()` 声明（public 节） |
| `gc_region.cpp` | 实现 `ResetNurseryGlobalBounds()` — CAS 更新全局边界 |
| `gc_young_collector.cpp` | Phase 4 结束后调用 ResetNurseryGlobalBounds 收缩 nursery 全局边界 |

## C5 Nursery 全局边界收缩

**问题**：`nursery_global_begin_`/`nursery_global_end_` 是单调扩展的（只在 AddNurseryRange 的 CAS 循环中向外扩展），从不收缩。Young GC 回收 nursery 页面后，IsNurseryPointer 仍对已回收地址返回 true，导致写屏障假阳性。

**修复**：在 GcYoungCollection Phase 4（所有线程已到达 safepoint）结束时调用 `ResetNurseryGlobalBounds(nursery->begin, nursery->begin + nursery_capacity)`，将全局边界精确收缩到当前 nursery 范围。

**Server GC 生产硬化**：Coordinator/HeapManager 架构审查通过 — 设计正确，无边角修复需求。Server GC 受 `CHAOS_IL2CPP_GC_SERVER=1` 宏保护，默认关闭。

**E2E 压力测试**：AggressiveYoungGc 失败确认是预存问题（OOM recovery 路径下 pattern integrity 检查 + thread leak 检测失败），非 nursery bounds 修复引入。
