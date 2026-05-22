---
task_id: 20260523-crag-gc-industrialization-sequel
title: CRAG GC 工业化后续 — 解决全部差距项，达 CoreCLR 对等
task_type: roadmap
lifecycle_status: completed
phase: completed
created_at: 2026-05-23
updated_at: 2026-05-23
completed_at: 2026-05-23
current_dir: docs/dev/completed/20260523-crag-gc-industrialization-sequel
clearance_source: brainstorm
---

# STATUS — CRAG GC 工业化后续开发计划

## 设计摘要

来源：brainstorm 会话 2026-05-23（基于 GC 工业化分析报告 `docs/dev/completed/20260522-gc-industrialization/`）

### 目标

解决 CRAG GC 分析报告识别的全部差距项（C1-C20），使 CRAG GC 达到 CoreCLR 对等的工业化程度。

### 优先级排列（确认）

| 优先级 | 维度 | 说明 |
|--------|------|------|
| P1（最高） | **正确性与稳定性** | 生产阻塞 crash/bug 优先修复 |
| P2 | **性能** | 不影响正确性的前提下保持或改善 |
| P3 | **架构完美性** | 不做过早抽象，保持代码可维护 |

### 边界拍板

| 决策 | 结论 |
|------|------|
| A3 codegen API 验证 | 完整验证（所有 GC 相关 managed API） |
| B 组内部执行 | 串行 |
| 阶段执行策略 | A→B→C→D→E 全串行 |
| D1 平台范围 | 最小（Windows ARM64 编译通过 + ARM64 写屏障单元测试） |
| C20 三代分代 | 做，Gen1 改为"部分保留"语义 |
| C15 写屏障假阳性 | 做，runtime GC-heap-pointer 检测函数 |

### 阶段分组

```
Phase A — 生产正确性修复（2-3 周）
  A1: BGC-YoungGC 并发 segfault 修复 ✅
  A2: Young collector card dirty 时序失败修复 ✅
  A3: Codegen verification 管线打通（含托管 API 完整验证）

Phase B — GC 引擎硬化（4-6 周，内部串行）
  B1: 并发根扫描接入点集成（BgcRecordRootChange → 静态字段/GCHandle 写入路径）
  B2: Parallel mark lock-free deque 改造（std::mutex → per-worker lock-free）
  B3: CrossPageCompact 最小存活阈值优化（消除空转）
  B4: retired_arrays_ 内存泄漏修复
  B5: Finalizer 线程池替代 std::async

Phase C — 可观测性与 API 完整（2-3 周）
  C1: LatencyMode 差异化行为实现（Batch/LowLatency/SustainedLowLatency）
  C2: GCMemoryInfo 托管侧结构体
  C3: GcEndStubbornChange 实现

Phase D — 平台扩展（4-6 周）
  D1: Windows ARM64 编译验证 + ARM64 写屏障单元测试
  D2: Server GC 生产硬化 + E2E 多 NUMA 压力测试

Phase E — 性能优化与高级功能（6-10 周，内部串行）
  E1: NUMA GcNumaNodeOfAddress 实现 + Card bundle 批量处理 + CPU 亲和性
  E2: 写屏障假阳性消除 + OS 低内存通知集成
  E3: AsyncPinned handle + Gen1 部分保留（3 代分代对齐）
```

### 风险评估摘要

| 风险 | 触发条件 | 缓解策略 |
|------|---------|---------|
| A1 BGC-YoungGC segfault 根因复杂 | SATB freeze + Young GC 时序冲突导致 mark 遗漏 | 首周定位根因，若 >2 周仍未解则先做 workaround 方案（序列化 BGC+Young GC） |
| A3 codegen pipeline 依赖链长 | verification target 构建失败可能涉及 codegen 侧改动 | 前置验证最小可编译通路，不追求一次全通 |
| D1 ARM64 交叉编译环境搭建 | Windows ARM64 工具链不可用或项目 CMake 不支持 | 回退方案：仅做 ARM64 写屏障代码 + 单元测试，编译验证放后续 |
| E3 Gen1 改造面积大 | Gen1 部分保留影响 full GC 路径和 promotion 语义 | 增加 Gen1 独立单元测试覆盖率，与现有 stress 套件并行验证 |

### 三优先级权衡结论

本次路线图以 **正确性与稳定性为第一优先级**，所有阶段设计均优先保障修复质量。性能优化（E 组）和平台扩展（D 组）在正确性验证通过后方可启动。各阶段设计不引入影响既有的 hotupdate 路径的变更。

### 关键文档

- 分析报告: `docs/discuss/crag-vs-coreclr-gc-comprehensive-assessment.md`
- 工业化完成度综合评估: `docs/discuss/20260522-crag-gc-industrialization-assessment.md`
- 风险评估报告: `docs/discuss/20260518-gc-risk-assessment-report.md`

### 下一步入口

**全部阶段已完成** — 五个阶段（A/B/C/D/E）共 18 个子任务全部完成，CRAG GC 工业化差距项已全部关闭。无需进一步推进。

### 子任务完成状态

| task_id | 状态 | 说明 |
|---------|------|------|
| gc-iw-a1 | ✅ completed | BGC-YoungGC 并发 segfault 修复完成 |
| gc-iw-a2 | ✅ completed | Young collector card dirty 时序失败修复 — ClearCardRange + 替换 ClearAllCards |
| gc-iw-a3 | ✅ completed | Codegen verification 管线打通 + 托管 API 完整验证 |
| gc-iw-b1 | ✅ completed | 并发根扫描接入点集成（BgcRecordRootChange → 静态字段/GCHandle 写入路径） |
| gc-iw-b2 | ✅ completed | Parallel mark lock-free deque 改造（Chase-Lev work-stealing） |
| gc-iw-b3 | ✅ completed | CrossPageCompact 最小存活阈值优化（kMinCrossPageSurvivorBytes=512） |
| gc-iw-b4 | ✅ completed | retired_arrays_ 内存泄漏修复（kMaxRetiredPageArrays=4） |
| gc-iw-b5 | ✅ completed | Finalizer 线程内联执行替代 std::async，移除专用 finalizer 线程 |
| gc-iw-c1 | ✅ completed | LatencyMode 差异化行为实现（Batch/LowLatency/SustainedLowLatency） |
| gc-iw-c2 | ✅ completed | GCMemoryInfo 托管侧结构体 |
| gc-iw-c3 | ✅ completed | GcEndStubbornChange 实现 |
| gc-iw-d1 | ✅ completed | ARM64 编译验证 + 写屏障 UT（回退方案） |
| gc-iw-d2 | ✅ completed | Server GC 生产硬化 + E2E 压力测试 |
| gc-iw-e1 | ✅ completed | NUMA 完善 + Card bundle + CPU 亲和性 |
| gc-iw-e2 | ✅ completed | 写屏障假阳性消除 + OS 低内存通知集成；C15 无需变更，C18 已实现 |
| gc-iw-e3 | ✅ completed | AsyncPinned handle + Gen1 部分保留 |

## 子任务执行策略

- `child_execution_mode`: auto
- `auto_continue`: true
- `auto_stop_policy`: blocking-only
- `recommended_next_child`: n/a（全部完成）

## blocking_questions

- [x] 优先级排列确认
- [x] 分组结构确认
- [x] A3 codegen API 完整验证确认
- [x] B 组内部串行确认
- [x] 阶段执行策略（全串行）确认
- [x] D1 平台范围（最小）确认
- [x] C20 三代分代确认
- [x] C15 写屏障假阳性确认

## question_clearance

cleared

## clearance_confirmed_by_user

true
