---
task_id: gc-p2-07
title: GC 测试覆盖增强：修复预存问题 + 补充用例（G-30）
task_type: plan
lifecycle_status: in-progress
phase: phase-1
created_at: 2026-05-22 23:30:00 +08:00
updated_at: 2026-05-22 23:30:00 +08:00
parent_task_id: 20260522-gc-industrialization
source_task_id: gc-p2-07
source_relation: roadmap-child
clearance_source: parent-roadmap
entry_skill: dev-writing-plans
---

# GC 测试覆盖增强：修复预存问题 + 补充用例（G-30）

## 设计摘要

继承父 roadmap 设计结论。本子任务聚焦 GC 测试覆盖增强，修复已知预存测试问题，补充关键场景测试用例。

### Scope
- 代码范围：`tests/contracts/native/runtime-core/gc_*.cpp`
- 不修改 GC 核心实现代码（gc_region.cpp 等）

### 已知预存问题（4 项）
1. **BGC-YoungGC 交互 segfault** — `gc_bgc_smoke.cpp:TestBgcWithYoungGc()` 预存 segfault
2. **BGC 线程在 stress test 中被禁用** — `gc_stress_test.cpp:1167` BGC thread DISABLED FOR DIAGNOSTIC
3. **Gen1 测试未验证完成** — `gc_gen1_test.cpp` 可能需要修复
4. **BGC root scan test 状态确认** — `gc_bgc_root_scan_test.cpp` 需要确认是否稳定

### 补充用例方向
- BGC + YoungGC 交错场景
- 并发分配 + GC 压力场景
- oversized 分配路径
- NurseryAllocateAtomic > kMaxNurseryAlloc 路径

## Inputs
- 父 roadmap: `docs/dev/in-progress/20260522-gc-industrialization/roadmap-v1-01.md`
- 父 STATUS: `docs/dev/in-progress/20260522-gc-industrialization/STATUS.md`
- 风险评估报告: `docs/discuss/crag-deep-risk-analysis-v3-20260513.md`

## Expected Outputs
1. 修复或明确隔离 4 个预存测试问题
2. 补充至少 3 个新测试场景
3. 全部 31 GC 测试 target 编译通过

## Exit Criteria
1. 预存问题已修复并验证，或已文档化隔离原因
2. 新增测试用例全部 PASS
3. 现有测试全部回归 PASS
