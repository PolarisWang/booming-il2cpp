---
task_id: 20260414-01-il2cpp-hotupdate-capability-closure
title: IL2CPP And HotUpdate Capability Closure
task_type: roadmap
lifecycle_status: completed
phase: archived
created_at: 2026-04-14 01:06:01 +08:00
updated_at: 2026-04-14 04:36:13 +08:00
current_dir: docs/dev/completed/20260414-01-il2cpp-hotupdate-capability-closure
parent_task_id:
source_task_id: 20260413-30-tests-taxonomy-and-subject-project-pruning-roadmap
source_relation: follow-up-after-retained-subject-cutover
active: false
---

## 关键文档

- reference_previous_roadmap: `docs/dev/completed/20260413-30-tests-taxonomy-and-subject-project-pruning-roadmap/roadmap-v1-02.md`
- brainstorm: `docs/dev/completed/20260414-01-il2cpp-hotupdate-capability-closure/brainstorm-v1-01.md`
- design: `docs/dev/completed/20260414-01-il2cpp-hotupdate-capability-closure/design-v1-01.md`
- roadmap_or_plan: `docs/dev/completed/20260414-01-il2cpp-hotupdate-capability-closure/roadmap-v1-01.md`
- capability_matrix: `wiki/03-功能模块/06-il2cpp核心架构/05-架构参考/il2cpp-hotupdate-capability-matrix.md`
- solution_archetypes: `wiki/03-功能模块/06-il2cpp核心架构/05-架构参考/solution-archetype-matrix.md`
- full_project_hotupdate: `wiki/03-功能模块/06-il2cpp核心架构/05-架构参考/full-project-hotupdate-workflow.md`
- reference_corelib_policy: `wiki/03-功能模块/06-il2cpp核心架构/05-架构参考/reference-assembly-and-corelib-policy.md`
- unsupported_policy: `wiki/03-功能模块/06-il2cpp核心架构/05-架构参考/unsupported-feature-policy.md`

## 当前结论

- current_focus: Phase 0-7 全部完成，retained subjects、framework metadata、pipeline/reporting、architecture docs 与真实运行证据已经闭环。
- why_now: 本 roadmap 的目标是把 IL2CPP + 热更能力收口到三类 retained subject 与稳定 architecture contract；该目标已经满足。
- success_definition:
  - `subjects/` 顶层仅保留 `SolutionCorePack`、`HotUpdateHostPack`、`MixedExecutionFeaturePack`
  - `Chaos.TestFramework` 成为 subject 侧统一声明接口
  - capability matrix / solution archetype / hot-update workflow 已落入 `wiki/03-功能模块/06-il2cpp核心架构/05-架构参考`
  - retained subjects proof / benchmark / archetype matrix 均有真实运行证据
  - registry refresh 与全量 `pytest` 已通过

## 最近摘要

- 2026-04-14 01:06:01 +08:00: 启动 capability closure roadmap，冻结 Q0-Q8 方向。
- 2026-04-14 01:42:47 +08:00: 完成 Phase 0 vocabulary / archetype / subject target freeze。
- 2026-04-14 10:12:00 +08:00: 完成 Phase 1-6，`Chaos.TestFramework`、retained subject cutover、pipeline/reporting、architecture docs 已全部落地。
- 2026-04-14 04:36:13 +08:00: 完成 Phase 7 验证收口，修复 `CoreLibReferenceSolution` framework-dependent runtime config、同步残留测试/文档断言，并跑通全量回归。

## 下一步

- next_action: 无。本 roadmap 已完成并归档；若后续继续扩展 IL2CPP/热更能力或恢复移动端主机验证，请另起 follow-up。
- owner: codex
- trigger: none

## 风险 / 阻塞

### risks

- 当前无新增风险；移动端主机与分发侧的长期工作仍由挂起任务 `20260411-05-mobile-runtime-host` 单独跟踪。

### blockers

- 当前无外部 blocker。

## 最终验证

- `python build/toolchains/run/run.py test registry refresh --json`
- `python build/toolchains/run/run.py test subject --id subject/SolutionCorePack --json`
- `python build/toolchains/run/run.py test subject --id subject/SolutionCorePack --goal perf.release --json`
- `python build/toolchains/run/run.py test subject --id subject/SolutionCorePack --matrix windows-archetype-simple-lib-managed-output --json`
- `python build/toolchains/run/run.py test subject --id subject/SolutionCorePack --matrix windows-archetype-multi-project-managed-output --json`
- `python build/toolchains/run/run.py test subject --id subject/SolutionCorePack --matrix windows-archetype-package-reference-managed-output --json`
- `python build/toolchains/run/run.py test subject --id subject/SolutionCorePack --matrix windows-archetype-reference-assembly-managed-output --json`
- `python build/toolchains/run/run.py test subject --id subject/SolutionCorePack --matrix windows-archetype-corelib-reference-managed-output --json`
- `python build/toolchains/run/run.py test subject --id subject/SolutionCorePack --matrix windows-archetype-mixed-reference-closure-managed-output --json`
- `python build/toolchains/run/run.py test subject --id subject/HotUpdateHostPack --json`
- `python build/toolchains/run/run.py test subject --id subject/HotUpdateHostPack --goal perf.release --json`
- `python build/toolchains/run/run.py test subject --id subject/HotUpdateHostPack --matrix windows-archetype-full-project-managed-output --json`
- `python build/toolchains/run/run.py test subject --id subject/MixedExecutionFeaturePack --json`
- `python build/toolchains/run/run.py test subject --id subject/MixedExecutionFeaturePack --goal perf.release --json`
- `python build/toolchains/run/run.py test subject --id subject/MixedExecutionFeaturePack --matrix windows-archetype-mixed-bridge-managed-output --json`
- `python build/toolchains/run/run.py test subject --id subject/MixedExecutionFeaturePack --goal perf.release --matrix windows-native-perf --json`
- `python build/toolchains/run/run.py test subject --id subject/MixedExecutionFeaturePack --goal perf.release --matrix windows-interpreter-perf --json`
- `python -m pytest -q`
- result: `787 passed, 40 skipped`
