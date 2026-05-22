---
task_id: 20260522-gc-industrialization
title: CRAG GC 工业化补齐路线图（修正版）
task_type: roadmap
lifecycle_status: in-progress
phase: roadmap
created_at: 2026-05-22 18:00:00 +08:00
updated_at: 2026-05-22 19:35:00 +08:00
current_dir: docs/dev/in-progress/20260522-gc-industrialization
roadmap_or_plan: docs/dev/in-progress/20260522-gc-industrialization/roadmap-v1-01.md
child_execution_mode: auto
auto_continue: true
auto_stop_policy: blocking-only
dispatch_model: sequential
recommended_next_child: gc-p2-05
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
| ✅ **已实现** | 14 | G-1 Page decommission、G-2 FindPage、G-3 BGC-YoungGC segfault、G-6 DependentHandle、G-8 跨 page 压缩、G-9 LOH segment 释放、G-10 BGC 并发 sweep、G-11 自适应调速、G-13 GCMemoryInfo、G-14 GCNotification、G-18 写屏障假阳性、G-19 Promotion 阈值、G-20 LowLatency 模式、G-24 Card bundle |
| 🔶 **部分实现需补齐** | 7 | G-7 ETW 原生事件、G-12 Finalizer OOM、G-23 堆布局优化高级、G-25 并发根扫描完善、G-27 平台验证、G-28 压力调度闭环、G-30 测试增强 |
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
2026-05-22 19:35→10:34：gc-p2-04（Finalizer OOM 保证）已完成。
- 实现 Emergency Reserve 64KB bump-pointer pool in MarkSweepOldGen（CAS 线程安全）
- 实现 IsFinalizerThread() 通过 std::thread::id 识别 Finalizer 线程
- 集成到 NurseryAllocateSlow、BGC 循环、Collect、ForceComplete 四条路径
- 新增 4 个紧急保留区测试 + 8 个原有 Finalizer 测试，全部 PASS
- 63 个 GC 测试全部 PASS
已归档 completed。

## 下一步
启动 gc-p2-05：内存压力主动调度反馈闭环（G-28）

## latest_stop_point
gc-p2-01(ARM64 写屏障) → gc-p2-02(大页支持) → gc-p2-03(平台验证) → gc-p2-04(Finalizer OOM) → 已归档 completed。下一步：gc-p2-05
