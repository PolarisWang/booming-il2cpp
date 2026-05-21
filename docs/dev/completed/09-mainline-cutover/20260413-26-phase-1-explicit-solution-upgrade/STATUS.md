---
task_id: 20260413-26-phase-1-explicit-solution-upgrade
title: Phase 1 Explicit Solution Upgrade
task_type: plan
lifecycle_status: completed
phase: archived
created_at: 2026-04-13 09:08:24 +08:00
updated_at: 2026-04-13 09:33:43 +08:00
current_dir: docs/dev/completed/20260413-26-phase-1-explicit-solution-upgrade
parent_task_id: 20260413-24-solution-core-pack-subject-consolidation
source_task_id: 20260413-24-solution-core-pack-subject-consolidation
source_relation: roadmap-child
active: false
---

## 关键文档

- parent_design: `docs/dev/in-progress/20260413-24-solution-core-pack-subject-consolidation/design-v1-01.md`
- parent_roadmap: `docs/dev/in-progress/20260413-24-solution-core-pack-subject-consolidation/roadmap-v1-01.md`
- plan: `docs/dev/completed/20260413-26-phase-1-explicit-solution-upgrade/plan-v1-01.md`

## 当前判断

- current_focus: Phase 1 已完成，显式 `.sln` source 的 Python/C# 双侧主通路已经打通，并形成后续 `SolutionCorePack` 合并可依赖的执行基线。
- why_done: 如果 `convert` / `project-graph` 仍把 `.sln` 当 `.csproj` 读取，后续 retained subject 的物理合并都会建立在错误前提上；现在这个缺口已经补齐。
- done_definition: `.sln` source 可以被 host-input build、declared/compiled catalog、C# convert/project graph 正确消费；第一批 archetype 与 retained subject 已具备显式 solution skeleton，定向回归测试通过。

## 最近摘要

- 2026-04-13 09:08:24 +08:00: 激活 Phase 1，先打通 `.sln` source 的 host-input build、declared catalog 与 compiled catalog 通路。
- 2026-04-13 09:33:43 +08:00: 补齐 `Chaos.IL2CPP.Driver` 对 `source.primaryProjectPath` 的消费，修复 C# `convert/project-graph` 把 `.sln` 当作 `.csproj` 读取的问题；`tests/unit/planning/test_project_graph.py` 与相关回归全部通过。

## 下一步

- next_action: 返回父 roadmap，激活 `20260413-27-phase-2-solution-core-pack-consolidation`，开始建立 `SolutionCorePack` 并吸收 archetype/mainline/performance 主体内容。
- owner: codex
- trigger: 需要开始 `SolutionCorePack` 物理合并并保持 compatibility alias 暂留到 Phase 6 时立即恢复。

## 风险 / 阻塞

### risks

- Phase 2 会开始引入 `SolutionCorePack` 新顶层 retained subject，需要控制迁移粒度，避免过早改动 registry/CLI 默认入口。
- `tests/` 中仍有大量对真实 retained subject 的 fixture 依赖，但这部分清理已明确后置到 roadmap 尾声处理，避免现在与主体迁移交叉扰动。

### blockers

- 当前无外部 blocker。
