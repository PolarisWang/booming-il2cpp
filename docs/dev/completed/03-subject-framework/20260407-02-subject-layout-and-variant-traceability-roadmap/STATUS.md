---
task_id: 20260407-02-subject-layout-and-variant-traceability-roadmap
title: Subject Layout And Variant Traceability Roadmap
task_type: roadmap
lifecycle_status: completed
phase: archived
created_at: 2026-04-07 12:54:52 +08:00
updated_at: 2026-04-08 00:03:55 +08:00
current_dir: docs/dev/completed/20260407-02-subject-layout-and-variant-traceability-roadmap
parent_task_id:
source_task_id: 20260406-03-subject-test-framework-rollout-roadmap
source_relation: architecture-follow-up
active: false
---

## 关键文档

- brainstorm: `docs/dev/completed/20260407-02-subject-layout-and-variant-traceability-roadmap/brainstorm-v1-01.md`
- design: `docs/dev/completed/20260407-02-subject-layout-and-variant-traceability-roadmap/design-v1-01.md`
- roadmap_or_plan: `docs/dev/completed/20260407-02-subject-layout-and-variant-traceability-roadmap/roadmap-v1-01.md`
- latest_progress: `docs/dev/completed/20260407-02-subject-layout-and-variant-traceability-roadmap/notes/progress-v1-02.md`

## 当前判断

- current_focus: 已完成复杂任务的 brainstorm 收敛，并转入父级 roadmap；当前目标是把仓库结构约束、subject 执行脊柱、variant 语义与 wiki 落地顺序拆成可派生的阶段任务。
- why_now: 该调整同时涉及 `tests/`、`subjects/`、`src/`、`contracts/`、`artifacts/subjects/`、`run test subject` 与 `CHECK/PROFILE/SHIP`，无法直接进入单份稳定 plan。
- done_definition: 只有在阶段边界、依赖关系、child task 映射和推荐推进顺序都稳定后，父 roadmap 才算完成；后续实现应以派生 child plan 的形式推进。

## 最近摘要

- 2026-04-07 12:54:52 +08:00: 用户明确要求先以 `brainstorming` 技能对齐复杂任务，再进入阶段化路线图，不允许提前实现或提前写 wiki。
- 2026-04-07 12:54:52 +08:00: 已确认仓库强约束：`tests/` 只放通用测试代码，`subjects/` 成为正式 subject 根，`artifacts/subjects/<id>` 是唯一 subject 中间产物与日志根，`src/` 只保留 il2cpp 核心代码。
- 2026-04-07 12:54:52 +08:00: 已确认 `contracts/examples` 保留全局最小样例语义，但命名改为 `samples`；`analysis/contracts/examples` 不再作为长期真源。
- 2026-04-07 12:54:52 +08:00: 已确认 subject 自带 `xUnit` 单元测试与 C# perf harness，且统一入口保持为 `run test subject --id subject/<id>`。
- 2026-04-07 12:54:52 +08:00: 已确认 `validationProfiles` 独立于 `goal`，`goal` 降级为兼容层；默认命令走 `defaultValidationProfile`。
- 2026-04-07 12:54:52 +08:00: 已确认 `variant=CHECK|PROFILE|SHIP` 从 `codegen` 开始生效，并继续影响 native 宏定义与编译选项；validation 决定默认 variant，但允许 CLI 覆盖。
- 2026-04-07 12:54:52 +08:00: 已确认 canonical 产物按 `artifacts/subjects/<id>/runs/<run-id>/...` 组织，采用 `run -> analysis -> variant -> matrix` 分层，并把 `pipeline-report/` 与 `validations/<kind>/` 分开。
- 2026-04-07 12:54:52 +08:00: 本任务当前无 wiki 更新；待实现阶段真正落地后，再把稳定知识沉淀到 `wiki/`。
- 2026-04-07 13:11:07 +08:00: 已派生 child plan `20260407-03-phase-1-path-resolver-and-contract-samples-cutover`，并将当前 active 执行入口切换到该子任务。
- 2026-04-07 16:47:13 +08:00: `Phase 1`、`Phase 2` 已归档完成，并补齐 `Phase 3` 的执行记录与归档状态。
- 2026-04-07 16:47:13 +08:00: 已派生 `20260407-06-phase-4-variant-aware-codegen-and-native`，当前 roadmap 下一步转为跟踪 `CHECK|PROFILE|SHIP` 变体链路。

## 下一步

- next_action: 跟踪 `20260407-06-phase-4-variant-aware-codegen-and-native` 的执行结果；若其完成，则派生 `Phase 5` child plan。
- owner: codex
- trigger: 仅当 `Phase 4` 的 variant 语义与 codegen/native/report 追踪链路稳定后，才继续派生下一阶段。

## 风险 / 阻塞

### risks

- 如果继续让 `goal` 同时承载 bundle、variant 与矩阵选择，后续 CLI 与 artifact 目录会重新缠结。
- 如果把 `variant` 折进 `matrix`，`CHECK/PROFILE/SHIP` 的宏定义与 native 选项将失去一等可追踪性。
- 如果继续保留全局 `artifacts/logs/tests` 作为 canonical run ledger，会直接违背 subject 产物集中化约束。

### blockers

- 无。

## 关联任务

### parent

- 无。

### source

- `20260406-03-subject-test-framework-rollout-roadmap`（architecture-follow-up）

### children

- `20260407-03-phase-1-path-resolver-and-contract-samples-cutover`
- `20260407-04-phase-2-subject-root-and-manifest-cutover`
- `20260407-05-phase-3-run-scoped-artifact-spine`
- `20260407-06-phase-4-variant-aware-codegen-and-native`
## 2026-04-07 跟进

- `Phase 4` 已把活动 Windows native reference host 切到 `subjects/HelloWorldObject/validation/proof/native-reference/`。
- run-scoped subject artifact 路径、subject public entry 与 `CHECK|PROFILE|SHIP` wiki 已落地。

## 2026-04-08 ��β

- 20260407-06-phase-4-variant-aware-codegen-and-native ����ɲ�ͨ�� Windows native �˵�����֤��run id Ϊ 20260408-000057-windows-2ead��
- ԭ roadmap �� Phase 5: Tooling/Test/Public Cutover �Ľ��������ɼ��� public entry��tests �߽������� subject-aware validation ִ�����������ɣ������ٵ������� child task��
- ԭ roadmap �� Phase 6: Compatibility Closure And Wiki Handoff �Ľ������������� wiki ҳ��ͱ��ֲ��������ɣ������궨�塢Windows native traceability��scratch build Ŀ¼�߽��� canonical artifact ����
- �� roadmap ����������ɲ��鵵�����ٱ��� active ��ڡ�
