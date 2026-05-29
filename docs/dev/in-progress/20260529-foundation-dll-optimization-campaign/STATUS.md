---
task_type: roadmap
phase: roadmap
roadmap_or_plan: roadmap-v1-01.md
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
child_execution_mode: auto
auto_continue: true
auto_stop_policy: blocking-only
dispatch_model: hybrid
recent_summary: >
  foundation-dll 全 family 优化战役 roadmap 已创建。147 个 family（capability 级别）分布在 16 个
  assembly 中。每个 family 作为独立子任务，使用 dev-optimization-campaign 全自动流水线。
  方案 B — 完全独立并行，按可用终端容量自由派发。
  dev-optimization-campaign 保证全自动：Setup → Verify → Fix → Perf → HotUpdate → Commit → Merge → CI → Cleanup
---

# Foundation DLL 全量优化战役 — Brainstorm 结论

> **任务 ID:** `20260529-foundation-dll-optimization-campaign`
> **日期:** 2026-05-29

## 边界拍板

- **范围**: 全部 147 个 capability family（16 个 assembly）
- **非目标**: 不涉及 codegen 新增 IL 指令支持，不涉及新的架构重构
- **每个 family 的三步走**: 正确性验证与修复 → benchmark 优化 → hotupdate 正确性修复
- **执行技能**: `dev-optimization-campaign` 全自动流水线（14 步），从 Setup Worktree 到 Merge Cleanup 全自动
- **入口命令**: `dev-optimization-campaign` 技能，以 `<slug>` 为参数

## Authority 决策

- **Owner**: 当前会话用户
- **执行方式**: 按可用终端容量自由派发 family（slug），每个终端跑一个 `dev-optimization-campaign <slug>`
- **Merge 策略**: 方案 B — 完全独立。每个 campaign 执行 Step 11 自动 merge 到 main，Step 12 CI 验证，Step 13 pull 确认干净
- **Branch 命名**: `claim/<slug>/<worker-id>`
- **保证 main 正常工作**: campaign 内置 merge 冲突处理策略 + CI 验证 + 退出前 pull 确认

## 当前结论

总计 **147 个 family**，按 assembly 分组。详见 `roadmap-v1-01.md`。

## 三优先级权衡结论

- **P1 性能最优**: benchmark timing 必须 > 0，vs .NET 8 差距控制在 20% 以内
- **P2 方案完美性**: 按 dev-optimization-campaign 既有流水线执行，不额外增加架构改动
- **P3 HotUpdate 支持**: semantic_changed 必须 > 0，overhead ≤ 100%

**裁决**: 三个维度全部执行，不互相妥协。遇到冲突时按 P1 > P2 > P3 裁决。

## 风险评估摘要

| 风险 | 概率 | 影响 | 缓解 |
|------|------|------|------|
| 独立并行导致重复修底层 bug | 中 | 低 | 各 branch 独立 merge，codegen 修复在 merge 后自然同步到 main |
| 同一共享基础设施文件并行修改产生 merge 冲突 | 低 | 中 | 人工解决 merge 冲突，scope 较小 |
| 某个 family 三轮修复仍不过 | 低 | 低 | 按 campaign 流程写入 blocker.md，跳过该 family |
| 终端容量限制导致部分 batch 空等 | 低 | 低 | batch 内交叉分配大小 family 均衡负载 |

## Watch Items

- SPCoreLib 的 codegen 修复是否影响其他 family 的验证基线
- 共享 infrastructure 文件（`_contracts/`、`scripts/`）的 merge 冲突频度
- benchmark vs .NET 8 差距是否在某些 family 超出 20%

## 下一步入口

Roadmap 已就绪。按 roadmap 派发子任务到各终端执行。

## 关键文档

- `roadmap-v1-01.md` — 完整 roadmap，含全部 147 个 family 的子任务表
- 执行引用 `dev-optimization-campaign` 技能，以 `<slug>` 为参数
