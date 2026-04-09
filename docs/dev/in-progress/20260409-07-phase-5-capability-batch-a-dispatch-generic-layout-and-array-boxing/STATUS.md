---
task_id: 20260409-07-phase-5-capability-batch-a-dispatch-generic-layout-and-array-boxing
title: Phase 5 Capability Batch A Dispatch Generic Layout And Array Boxing
task_type: plan
lifecycle_status: in_progress
phase: executing
created_at: 2026-04-09 15:20:01 +08:00
updated_at: 2026-04-09 21:34:59 +08:00
current_dir: docs/dev/in-progress/20260409-07-phase-5-capability-batch-a-dispatch-generic-layout-and-array-boxing
parent_task_id: 20260409-01-il2cpp-mainline-performance-core-and-full-subject-roadmap
source_task_id: 20260409-01-il2cpp-mainline-performance-core-and-full-subject-roadmap
source_relation: roadmap-child
active: true
---

## 关键文档

- brainstorm: `docs/dev/in-progress/20260409-07-phase-5-capability-batch-a-dispatch-generic-layout-and-array-boxing/brainstorm-v1-01.md`
- design: `docs/dev/in-progress/20260409-07-phase-5-capability-batch-a-dispatch-generic-layout-and-array-boxing/design-v1-01.md`
- roadmap_or_plan: `docs/dev/in-progress/20260409-07-phase-5-capability-batch-a-dispatch-generic-layout-and-array-boxing/plan-v1-01.md`

## Review Gate

- review_status: approved-by-user
- issue: `Phase 5` 需要在“完整 subject 不分裂”的前提下，同时解决 proof slice 入口选择、dispatch 语义路径，以及 arrays/boxing 的 loader/runtime 补洞。
- recommendation: 继续沿用一个 `MainlineFeaturePack`，为 capability correctness 增加多个 proof slice，并在 matrix 级叠加 `source.entry` override；dispatch 优先走“能去虚则去虚，不能去虚才走 runtime virtual path”，arrays/boxing 单独增设新 lowering family。
- next_action: 保留三条 correctness focused run 与一次 perf run 的证据；向用户确认这次被并发构建负载污染的 perf regression 应如何处理，再决定是否归档完成。

## 当前判断

- current_focus: `dispatch` / `generic-layout` / `array-boxing` correctness 与 `perf.profile` 执行链路均已完成；当前唯一未决点是 perf baseline compare 在用户并发构建负载下显示 `regressed`，是否应把这视为环境噪音还是阶段阻塞。
- why_now: 用户明确要求不要动正在进行的 `MSBuild/cl.exe` 构建，因此 perf 已在带负载环境中执行；结果可以证明 unified perf 入口可用，但不能自动证明“性能回归已排除”。
- done_definition: planner 支持 matrix-scoped `source.entry` override；`MainlineFeaturePack` 有独立 capability proof slices；Batch A focused correctness 与统一 perf 入口通过。

## 最近摘要

- 2026-04-09 21:22:07 +08:00: Windows 直接编译路径已修正到与 `VsDevCmd` 一致的 MSVC 工具链；`find_visual_cpp_executable()` 现优先使用 `VCToolsInstallDir`，否则选择 `vswhere` 返回的最新 toolset，避免再次落到 `14.29.30133` 并触发 `STL1001`。相关回归：`python -m unittest tests.tooling.run.test_cmake_bootstrap` 已通过。
- 2026-04-09 21:22:07 +08:00: `NativeReferenceLoweringPlanner` 已对齐 generic-layout 的真实 lowering contract，允许 `static-forwarder` 与 `instance-field-getter` 两类 shape；`python -m unittest tests.unit.run.test_phase5_capability_batch_a` 已通过。
- 2026-04-09 21:22:07 +08:00: 三条 focused correctness 已全部通过：`windows-dispatch-check` -> `20260409-204130-windows-a5e2`，`windows-generic-layout-check` -> `20260409-205624-windows-af35`，`windows-array-boxing-check` -> `20260409-210422-windows-71c3`。
- 2026-04-09 21:22:07 +08:00: “运行卡死”根因已收敛到 `current.json` 指针清理失败，而不是 subject 仍在执行；在当前 Windows 工作区中删除 freshly-written pointer 会报 `Access is denied`，因此 `reporting.py` 与 `operation_reporting.py` 已改为“优先删除，失败则回写最终状态”。回归：`python -m unittest tests.integration.run.test_reporting_layout tests.tooling.run.test_operation_reporting tests.tooling.run.test_subject_command` 已通过。
- 2026-04-09 21:24:01 +08:00: 进一步补跑组合 Python 套件 `python -m unittest tests.unit.run.test_subject_planner tests.unit.run.test_subject_manifest_schema tests.unit.run.test_phase5_capability_batch_a tests.unit.run.test_subject_reporting tests.unit.run.test_subject_workers tests.unit.run.test_subject_workers_perf tests.tooling.run.test_subject_command tests.tooling.run.test_cmake_bootstrap tests.integration.run.test_reporting_layout tests.tooling.run.test_operation_reporting`，共 56 项全部通过。
- 2026-04-09 21:34:59 +08:00: `python build/toolchains/run/run.py test subject --id subject/MainlineFeaturePack --goal perf.profile --validation-profile perf-profile --variant PROFILE --json` 已在 `20260409-212758-windows-1a96` 跑通，run summary 为 `ok`，`current.json` 也已正确落为 `status=ok`。
- 2026-04-09 21:34:59 +08:00: 同次 perf run 的 baseline compare 为 `regressed`：`meanDurationMs=52.327` 高于基线 `50.0`，`maxDurationMs=180.221` 高于基线 `80.0`；但 5 个样本中首样本 `180.221ms` 明显异常，去掉首样本后其余 4 次均值约 `20.354ms`、最大值约 `29.14ms`。这更像冷启动/并发负载污染，而不是 steady-state 的确定性回退。

## 下一步

- next_action: 由用户决定 perf regression 的处理策略：
  - 接受当前结果，把 `Phase 5` 视为“功能与 correctness 完成，perf 入口可用，但带负载 perf 信号待后续收敛”
  - 或在空闲环境重跑 perf，再决定是否归档 completed
- owner: codex
- trigger: 用户已确认继续执行。

## 风险 / 阻塞

### risks

- 如果把 `Phase 5` 改成多 subject 分裂推进，`MainlineFeaturePack` 会失去作为 full subject 主宿主的价值。
- 如果直接做一个 mega proof entry，会把 proof correctness、能力边界、故障定位、perf evidence 重新搅在一起。
- `arrays/boxing` 是 Batch A 中最容易拖慢节奏的缺口，必须明确限制首批 opcode / helper 范围。
- Windows native/perf 验证对环境洁净度非常敏感；当前 perf regression 很可能混入了用户并发构建与首样本冷启动开销，不能直接等同于 steady-state 代码退化。

### blockers

- 当前显式阻塞不是代码设计、correctness 缺口，甚至不是 perf 管线不可用，而是“如何解释这次带负载 perf regression”这一验收决策。

## 关联任务

### parent

- `20260409-01-il2cpp-mainline-performance-core-and-full-subject-roadmap`

### source

- `20260409-01-il2cpp-mainline-performance-core-and-full-subject-roadmap`（roadmap-child）

### children

- 无。
