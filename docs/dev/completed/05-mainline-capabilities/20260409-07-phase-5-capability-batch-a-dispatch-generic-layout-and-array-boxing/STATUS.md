---
task_id: 20260409-07-phase-5-capability-batch-a-dispatch-generic-layout-and-array-boxing
title: Phase 5 Capability Batch A Dispatch Generic Layout And Array Boxing
task_type: plan
lifecycle_status: completed
phase: archived
created_at: 2026-04-09 15:20:01 +08:00
updated_at: 2026-04-09 21:43:44 +08:00
current_dir: docs/dev/completed/20260409-07-phase-5-capability-batch-a-dispatch-generic-layout-and-array-boxing
parent_task_id: 20260409-01-il2cpp-mainline-performance-core-and-full-subject-roadmap
source_task_id: 20260409-01-il2cpp-mainline-performance-core-and-full-subject-roadmap
source_relation: roadmap-child
active: false
---

## 关键文档

- brainstorm: `docs/dev/completed/20260409-07-phase-5-capability-batch-a-dispatch-generic-layout-and-array-boxing/brainstorm-v1-01.md`
- design: `docs/dev/completed/20260409-07-phase-5-capability-batch-a-dispatch-generic-layout-and-array-boxing/design-v1-01.md`
- roadmap_or_plan: `docs/dev/completed/20260409-07-phase-5-capability-batch-a-dispatch-generic-layout-and-array-boxing/plan-v1-01.md`
- latest_progress: `docs/dev/completed/20260409-07-phase-5-capability-batch-a-dispatch-generic-layout-and-array-boxing/notes/progress-v1-05.md`

## Review Gate

- review_status: approved-by-user
- issue: `Phase 5` 需要在“完整 subject 不分裂”的前提下，同时解决 proof slice 入口选择、dispatch 语义路径，以及 arrays/boxing 的 loader/runtime 补洞。
- recommendation: 继续沿用一个 `MainlineFeaturePack`，为 capability correctness 增加多个 proof slice，并在 matrix 级叠加 `source.entry` override；dispatch 优先走“能去虚则去虚，不能去虚才走 runtime virtual path”，arrays/boxing 单独增设新 lowering family。
- next_action: 无。`Phase 5` 已按“功能与 correctness 完成，perf 入口打通，带负载 perf baseline 留待后续独立复核”的口径归档。

## 当前判断

- current_focus: `dispatch`、`generic-layout`、`array-boxing` 三条能力已在完整 `MainlineFeaturePack` 上闭环，统一 perf 入口也已接通；后续主线回到父 roadmap，推进 `Phase 6` 的 `delegates/exceptions` 与 `Phase A` closure。
- why_now: 用户已接受本阶段的 perf 口径，即“本次 run 足以证明 perf pipeline 可用，但不把带负载 baseline regression 直接等同于 steady-state 代码回退”；因此当前 child 可以正式归档。
- done_definition: planner 支持 matrix-scoped `source.entry` override；`MainlineFeaturePack` 有独立 capability proof slices；Batch A focused correctness 与统一 perf 入口通过。

## 最近摘要

- 2026-04-09 21:22:07 +08:00: Windows 直接编译路径已修正到与 `VsDevCmd` 一致的 MSVC 工具链；`find_visual_cpp_executable()` 现优先使用 `VCToolsInstallDir`，否则选择 `vswhere` 返回的最新 toolset，避免再次落到 `14.29.30133` 并触发 `STL1001`。相关回归：`python -m unittest tests.tooling.run.test_cmake_bootstrap` 已通过。
- 2026-04-09 21:22:07 +08:00: `NativeReferenceLoweringPlanner` 已对齐 generic-layout 的真实 lowering contract，允许 `static-forwarder` 与 `instance-field-getter` 两类 shape；`python -m unittest tests.unit.run.test_phase5_capability_batch_a` 已通过。
- 2026-04-09 21:22:07 +08:00: 三条 focused correctness 已全部通过：`windows-dispatch-check` -> `20260409-204130-windows-a5e2`，`windows-generic-layout-check` -> `20260409-205624-windows-af35`，`windows-array-boxing-check` -> `20260409-210422-windows-71c3`。
- 2026-04-09 21:22:07 +08:00: “运行卡死”根因已收敛到 `current.json` 指针清理失败，而不是 subject 仍在执行；在当前 Windows 工作区中删除 freshly-written pointer 会报 `Access is denied`，因此 `reporting.py` 与 `operation_reporting.py` 已改为“优先删除，失败则回写最终状态”。回归：`python -m unittest tests.integration.run.test_reporting_layout tests.tooling.run.test_operation_reporting tests.tooling.run.test_subject_command` 已通过。
- 2026-04-09 21:24:01 +08:00: 进一步补跑组合 Python 套件 `python -m unittest tests.unit.run.test_subject_planner tests.unit.run.test_subject_manifest_schema tests.unit.run.test_phase5_capability_batch_a tests.unit.run.test_subject_reporting tests.unit.run.test_subject_workers tests.unit.run.test_subject_workers_perf tests.tooling.run.test_subject_command tests.tooling.run.test_cmake_bootstrap tests.integration.run.test_reporting_layout tests.tooling.run.test_operation_reporting`，共 56 项全部通过。
- 2026-04-09 21:34:59 +08:00: `python build/toolchains/run/run.py test subject --id subject/MainlineFeaturePack --goal perf.profile --validation-profile perf-profile --variant PROFILE --json` 已在 `20260409-212758-windows-1a96` 跑通，run summary 为 `ok`，`current.json` 也已正确落为 `status=ok`。
- 2026-04-09 21:34:59 +08:00: 同次 perf run 的 baseline compare 为 `regressed`：`meanDurationMs=52.327` 高于基线 `50.0`，`maxDurationMs=180.221` 高于基线 `80.0`；但 5 个样本中首样本 `180.221ms` 明显异常，去掉首样本后其余 4 次均值约 `20.354ms`、最大值约 `29.14ms`。这更像冷启动/并发负载污染，而不是 steady-state 的确定性回退。
- 2026-04-09 21:43:44 +08:00: 用户已接受“功能与 correctness 完成，perf 入口打通，但带负载 perf baseline 留待独立复核”的阶段口径；`Phase 5` 因此按 completed 归档。

## 下一步

- next_action: 无。后续回到父 roadmap，启动 `Phase 6` child `20260409-08-phase-6-capability-batch-b-delegate-exception-and-phase-a-closure` 的 design review。
- owner: codex
- trigger: `Phase 5` 验收口径已被用户确认接受。

## 风险 / 阻塞

### risks

- 如果把 `Phase 5` 改成多 subject 分裂推进，`MainlineFeaturePack` 会失去作为 full subject 主宿主的价值。
- 如果直接做一个 mega proof entry，会把 proof correctness、能力边界、故障定位、perf evidence 重新搅在一起。
- `arrays/boxing` 是 Batch A 中最容易拖慢节奏的缺口，必须明确限制首批 opcode / helper 范围。
- Windows native/perf 验证对环境洁净度非常敏感；当前 perf regression 很可能混入了用户并发构建与首样本冷启动开销，不能直接等同于 steady-state 代码退化。

### blockers

- 无。

## Completion Notes

- 2026-04-09 21:43:44 +08:00: `find_visual_cpp_executable()` 已与 `VsDevCmd` 的 `VCToolsInstallDir` 对齐，修复旧 toolset 误选导致的 `STL1001`。
- 2026-04-09 21:43:44 +08:00: `NativeReferenceLoweringPlanner` 已完成 `static-forwarder` / `instance-field-getter` 对齐，`dispatch` / `generic-layout` / `array-boxing` 三条 focused correctness 全部通过。
- 2026-04-09 21:43:44 +08:00: `reporting.py` / `operation_reporting.py` 已在 Windows 指针删除失败时回退为写入最终状态，`current.json` 不再长期误报 `running`。
- 2026-04-09 21:43:44 +08:00: 验证证据：
  - `python -m unittest tests.unit.run.test_subject_planner tests.unit.run.test_subject_manifest_schema tests.unit.run.test_phase5_capability_batch_a tests.unit.run.test_subject_reporting tests.unit.run.test_subject_workers tests.unit.run.test_subject_workers_perf tests.tooling.run.test_subject_command tests.tooling.run.test_cmake_bootstrap tests.integration.run.test_reporting_layout tests.tooling.run.test_operation_reporting`
  - `python build/toolchains/run/run.py test subject --id subject/MainlineFeaturePack --goal correctness.dev --matrix windows-dispatch-check --validation-profile proof-dispatch --variant CHECK --json`，run id `20260409-204130-windows-a5e2`
  - `python build/toolchains/run/run.py test subject --id subject/MainlineFeaturePack --goal correctness.dev --matrix windows-generic-layout-check --validation-profile proof-generic-layout --variant CHECK --json`，run id `20260409-205624-windows-af35`
  - `python build/toolchains/run/run.py test subject --id subject/MainlineFeaturePack --goal correctness.dev --matrix windows-array-boxing-check --validation-profile proof-array-boxing --variant CHECK --json`，run id `20260409-210422-windows-71c3`
  - `python build/toolchains/run/run.py test subject --id subject/MainlineFeaturePack --goal perf.profile --validation-profile perf-profile --variant PROFILE --json`，run id `20260409-212758-windows-1a96`

## 关联任务

### parent

- `20260409-01-il2cpp-mainline-performance-core-and-full-subject-roadmap`

### source

- `20260409-01-il2cpp-mainline-performance-core-and-full-subject-roadmap`（roadmap-child）

### children

- 无。
