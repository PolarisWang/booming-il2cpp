---
task_id: 20260411-02-full-project-ingestion
title: Full Project Ingestion And Build Graph
task_type: plan
lifecycle_status: completed
phase: archived
created_at: 2026-04-11 14:00:00 +08:00
updated_at: 2026-04-11 16:00:00 +08:00
current_dir: docs/dev/completed/20260411-02-full-project-ingestion
parent_task_id: 20260409-10-total-solution-and-ios-hot-update-analysis
source_task_id: 20260409-10-total-solution-and-ios-hot-update-analysis
source_relation: child_of_roadmap_phase_1
active: false
---

## 关键文档

- parent_roadmap: `docs/dev/in-progress/20260409-10-total-solution-and-ios-hot-update-analysis/roadmap-v1-01.md`
- phase_architecture: `docs/dev/in-progress/20260409-10-total-solution-and-ios-hot-update-analysis/phase-architecture-v1-01.md`
- native_output: `docs/dev/in-progress/20260409-10-total-solution-and-ios-hot-update-analysis/native-output-structure-v1-01.md`
- plan: `docs/dev/completed/20260411-02-full-project-ingestion/plan-v1-01.md`
- latest_progress: `docs/dev/completed/20260411-02-full-project-ingestion/notes/progress-v1-02.md`

## 当前判断

- current_focus: 已完成 `ProjectReference` 闭包、`PackageReference` 基础解析、`project-graph.json` 产出、`LoadMultiple` 多程序集加载与 cross-assembly reachability 打通。
- why_now: 后续 `AOT Runtime Completeness`、`Engine Binding Contract` 与 `Hot Update Skeleton` 都依赖“输入不再是假定单 DLL”的基线；本阶段完成后，父 roadmap 可以切入 Phase 2。
- done_definition: `tests/unit/run/test_project_graph.py`、`tests/unit/run/test_repo_layout.py` 与主线 run/tooling/shared-contract 回归全部通过；主线 convert 已能稳定处理三个 golden project fixture。

## 最近摘要

- 2026-04-11 15:00:00 +08:00: 接管 `20260411-02-full-project-ingestion`，先用 `subjects/golden/*` 与 `test_project_graph.py` 制造干净 RED。
- 2026-04-11 15:30:00 +08:00: 新建 `Chaos.IL2CPP.ProjectGraph`，落地 `ProjectGraphLoader`、`PackageResolver`、`LoadedWorldModel`，并将 Driver/Pipeline 切到多程序集 world。
- 2026-04-11 15:45:00 +08:00: 修正 run/testing 的 subject discovery 边界，避免 `subjects/golden/*` 污染标准 subject registry 与 planner。
- 2026-04-11 16:00:00 +08:00: Phase 1 全部验证通过，任务归档；主线下一步切到 `20260411-03-aot-runtime-completeness`。

## 下一步

- next_action: 激活 `20260411-03-aot-runtime-completeness`，优先核对现有 `MainlineFeaturePack` 与主线能力覆盖，再按 TDD 补 interface dispatch / runtime completeness 的真实缺口。
- owner: codex
- trigger: Phase 1 依赖链已闭环，父 roadmap 进入下一阶段执行。

## 验证

- passed: `python -m pytest tests/unit/run/test_repo_layout.py -v`
- passed: `python -m pytest tests/unit/run/test_project_graph.py -v`
- passed: `python -m pytest tests/contracts/shared/ tests/unit/run/ tests/tooling/run/ -q`

## 风险 / 阻塞

### risks

- 当前 `generated/assemblies/<Assembly>/` 先冻结为 Phase 1 所需的最小目录骨架；真正按类型拆分的 production codegen 仍属于后续 runtime completeness / production emitter 扩展面。
- `PackageResolver` 当前只解析 direct package 的首个 runtime/compile DLL，足够支撑 Phase 1 图谱与 convert，不等于完整 NuGet 资产裁剪策略。

### blockers

- 无。
