---
task_id: 20260530-optimization-campaign-reboot
task_type: roadmap
phase: roadmap
roadmap_or_plan: roadmap-v1-01.md
dispatch_doc: DISPATCH.md
dispatch_model: hybrid
child_execution_mode: auto
auto_continue: true
auto_stop_policy: blocking-only
---

# 优化战役重启 — 全数据驱动优化流水线

## 设计摘要

### 边界拍板
- 采用方案 C：全数据驱动优化，Phase 0→1→2 三阶段结构
- 多 Agent 并行执行（Track A + Track B 同时启动）
- enum-parsing 作为 P0 优化目标，要求 602ns → < 60ns
- 全量规划有时限（预计 4-6 周）

### Authority 决策
- benchmark_scanner.py 归入 `verification/analysis/` 模块
- Skill SKILL.md 由本 roadmap 的 Track B 子任务重写
- enum-parsing 代码修改由 Track C 子任务在 worktree 中完成

### 当前结论
- blocked_questions: []
- question_clearance: cleared
- clearance_confirmed_by_user: true

### 风险评估摘要
| 风险 | 缓解 |
|------|------|
| Batch pipeline 大量失败 | --continue-on-error，部分数据也够用 |
| enum-parsing 架构固有瓶颈 | 接受 3x 改进，文档说明 |
| 多 Agent 文件冲突 | Track A/B 操作不同文件集 |
| Skill 重构影响现有工作流 | 旧 skill 保留兼容直到迁移完成 |

### 三优先级权衡结论
| 维度 | 结论 |
|------|------|
| P1 性能最优 | benchmark_scanner 零开销；enum-parsing 目标 10x+ |
| P2 方案完美 | Phase 0→1→2 职责清晰；修复三明治模型 |
| P3 热更适配 | enum 优化不破坏 hotupdate 路径 |

## 调度状态

dispatch_doc: DISPATCH.md
dispatch_model: hybrid
active_batches: [batch-6]
completed_batches: [batch-1, batch-2, batch-3, batch-4, batch-5]
terminals_active: []
pending_batches: [batch-6]

## 最近摘要

全部 6 个 batch 已完成。enum-parsing 优化 AOT +688% (48.4M→381.6M ops/s)，16 子任务全部完成。

## 子任务映射

| task_id | phase | status | owner | purpose | batch_id |
|---------|-------|--------|-------|---------|----------|
| T-A1 | Phase 0 | ready | terminal-1 | benchmark_scanner.py 实现 | batch-1 |
| T-B1 | Phase 1 | ready | terminal-2 | 三阶段 Skill 架构设计 | batch-1 |
| T-A2 | Phase 0 | blocked | — | Batch Pipeline 执行 | batch-2 |
| T-A3 | Phase 0 | completed | — | Priority Queue 生成 | batch-3 |
| T-B2 | Phase 1 | blocked | — | Freeze 时序修复 | batch-2 |
| T-B3 | Phase 1 | blocked | — | ROI 预估门禁 | batch-2 |
| T-B4 | Phase 1 | blocked | — | 修复三明治模型 | batch-2 |
| T-C1 | Phase 2 | completed | — | enum-parsing 瓶颈深潜 | batch-3 |
| T-C2 | Phase 2 | completed | — | enum-parsing 优化 Attempt 1-3 | batch-4 |
| T-C3 | Phase 2 | completed | — | enum-parsing 回归验证 + Doc + Merge | batch-5 |
| T-D1 | Phase 3 | completed | — | 增量 Benchmark | batch-3 |
| T-D2 | Phase 3 | completed | — | 自动 Regression Detection | batch-4 |
| T-D3 | Phase 3 | completed | — | Family 依赖图分析 | batch-4 |
| T-E1 | Phase 4 | completed | — | 跨 Session Benchmark 历史 | batch-5 |
| T-E2 | Phase 4 | completed | — | Auto-Bisect | batch-5 |
| T-FINAL | 验收 | completed | — | — | 全量自测 + 验收 | batch-6 |

## 最近停止点

## 下一步

启动 batch-1 两个并行子任务：T-A1 (benchmark_scanner) + T-B1 (Skill 三阶段设计)。

## recommended_next_child

T-A1 (benchmark_scanner.py 实现) — 立即启动
T-B1 (三阶段 Skill 架构设计) — 立即启动（并行）
