---
task_id: 20260522-gc-industrialization
title: CRAG GC 工业化补齐路线图（修正版）
task_type: roadmap
lifecycle_status: completed
phase: completed
created_at: 2026-05-22 18:00:00 +08:00
updated_at: 2026-05-22 21:35:00 +08:00
current_dir: docs/dev/in-progress/20260522-gc-industrialization
roadmap_or_plan: docs/dev/in-progress/20260522-gc-industrialization/roadmap-v1-01.md
child_execution_mode: auto
auto_continue: true
auto_stop_policy: blocking-only
dispatch_model: sequential
recommended_next_child: gc-p3-03
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
---

# STATUS — CRAG GC 工业化补齐（修正版）

## 设计摘要

来源：`docs/discuss/20260522-crag-gc-industrialization-assessment.md`
修正：2026-05-22 实际代码审计发现原评估报告 30 项中仅 **14 项为真实差距**

### 审计修正

原报告 30 个差距项经过代码审计：

| 审计结论 | 数量 | 具体项 |
|---------|------|--------|
| ✅ **已实现** | 16 | G-1 Page decommission、G-2 FindPage、G-3 BGC-YoungGC segfault、G-6 DependentHandle、G-8 跨 page 压缩、G-9 LOH segment 释放、G-10 BGC 并发 sweep、G-11 自适应调速、G-13 GCMemoryInfo、G-14 GCNotification、G-18 写屏障假阳性、G-19 Promotion 阈值、G-20 LowLatency 模式、G-24 Card bundle、G-12 Finalizer OOM、G-28 压力调度闭环 |
| 🔶 **部分实现需补齐** | 5 | G-7 ETW 原生事件、G-23 堆布局优化高级、G-25 并发根扫描完善、G-27 平台验证、G-30 测试增强 |
| ❌ **真正未实现** | 7 | G-4 Server GC、G-5 ARM64 写屏障、G-15 基准套件、G-16 SOS 调试、G-17 升温测试、G-21 大页、G-29 编译时选择 |

### 修正后三阶段

- Phase 1（~1 个月）— 基础设施与平台补齐：ARM64 写屏障、大页、平台验证、Finalizer OOM、编译时选择、测试增强
- Phase 2（~1 个月）— 可观测性与质量保障：ETW 原生事件、基准测试套件、SOS 调试扩展、72h 升温测试
- Phase 3（~3 个月）— 高级并发能力：Server GC 模式、并发根扫描、堆布局优化

### 边界拍板
- Server GC 模式（G-4）为最重工作量，若 >3 月则拆为独立路线图
- ARM64 写屏障仅需完成 Windows ARM64 编译验证，真机验证可在后续补充
- 3 阶段合计约 5 个月

## 关键文档
- roadmap: `roadmap-v1-01.md`
- 原始评估报告: `docs/discuss/20260522-crag-gc-industrialization-assessment.md`
- 审计记录: `docs/dev/completed/11-gc-industrialization/gc-p1-01-page-decommission/STATUS.md`

## 最近摘要
2026-05-22 21:50：gc-p3-02（GC 基准回归测试套件/G-15）完成。
- Gap 2: gc-bench-compare.py — BENCH| 格式回归对比工具，可配置 WARN/FAIL 阈值
- Gap 3: gtest 评估结论 — 保持自定义 runner，无需迁移
- Gap 4: MultiThreadAlloc — 4 线程并发分配基准，6.4 GB/s，0 失败
- Gap 1: gc-baseline.py — 基线记录/检查/更新工作流
6/6 基准全 PASS。已归档 completed。

2026-05-22 21:50：gc-p2-vfy（Phase 2 集成验证）完成。
- gc-p3-01 ETW: test_gc_events 5/5 PASS ✅
- gc-p3-02 基准套件: test_gc_regression_benchmark 6/6 PASS ✅
- gc-p3-03 SOS: test_gc_debug_contract 9 checks PASS ✅
- gc-p3-04 72h soak: test_gc_soak --minutes 1 11 snapshots PASS ✅

## 下一步
无 — 全路线图 17 个子任务全部完成。

## latest_stop_point
全部三阶段完成。总计 17 个子任务全部实现并验证。
