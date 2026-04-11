---
task_id: 20260411-03-aot-runtime-completeness
title: AOT Runtime Completeness
task_type: plan
lifecycle_status: completed
phase: archived
created_at: 2026-04-11 16:05:00 +08:00
updated_at: 2026-04-11 12:51:22 +08:00
current_dir: docs/dev/completed/20260411-03-aot-runtime-completeness
parent_task_id: 20260409-10-total-solution-and-ios-hot-update-analysis
source_task_id: 20260409-10-total-solution-and-ios-hot-update-analysis
source_relation: child_of_roadmap_phase_2
active: false
---

## 关键文档

- parent_roadmap: `docs/dev/in-progress/20260409-10-total-solution-and-ios-hot-update-analysis/roadmap-v1-01.md`
- phase_architecture: `docs/dev/in-progress/20260409-10-total-solution-and-ios-hot-update-analysis/phase-architecture-v1-01.md`
- plan: `docs/dev/completed/20260411-03-aot-runtime-completeness/plan-v1-01.md`
- brainstorm: `docs/dev/completed/20260411-03-aot-runtime-completeness/brainstorm-v1-06.md`
- design: `docs/dev/completed/20260411-03-aot-runtime-completeness/design-v1-06.md`
- latest_progress: `docs/dev/completed/20260411-03-aot-runtime-completeness/notes/progress-v1-07.md`

## 当前判断

- current_focus: 已完成 Phase 2 的 six-batch proof-first 收敛：interface dispatch、async/await、threading/GC、nested EH、marshaling、linker stripping 与 perf governance 均已有最小闭环。
- why_now: `full-project-ingestion` 已提供真实 multi-project 输入层；本任务完成后，父 roadmap 可以无缝切换到 `20260411-04-engine-binding-contract`，为后续 hot update skeleton 与 metadata supplement 冻结 engine-facing 边界。
- done_definition: `plan-v1-01.md` 的 Batch 1-6 已全部闭环，所需 RED/GREEN、专项/全量测试、真实 build、真实 native perf 与 suite 级 convert perf 证据均已取得。

## 最近摘要

- 2026-04-11 03:54:56 +08:00: 完成 Batch 1 interface dispatch 最小 proof slice，并通过专项测试、阶段回归与 managed build。
- 2026-04-11 04:10:23 +08:00: 完成 Batch 2 async/await 最小 proof slice，并通过专项测试、全量回归与 managed build。
- 2026-04-11 05:22:00 +08:00: 完成 Batch 3 threading/GC 最小 proof slice，并通过专项测试、全量回归、managed build 与 native compile。
- 2026-04-11 11:28:13 +08:00: 完成 Batch 4 nested EH 与 marshaling/export metadata 两个最小 proof slice，并通过全量 `tests/unit/run` 回归和 subject / solution build。
- 2026-04-11 12:51:22 +08:00: 完成 Batch 5 stripping/preserve analysis proof 与 Batch 6 perf governance；`InterfaceDispatchProof` native perf 实测 `meanDispatchNanoseconds = 3.057`，`GoldenMultiProject` convert perf 实测 `convertDurationMs = 4901.549`，任务归档并切换到 `20260411-04-engine-binding-contract`。

## 下一步

- next_action: 激活 `20260411-04-engine-binding-contract`，先审计现有 `Engine*` subject / runtime-core surface，再建立 Phase 3 的 RED 与设计文档。
- owner: codex
- trigger: Phase 2 已闭环，父 roadmap 进入下一依赖阶段。

## 验证

- passed: `python -m pytest tests/unit/run/test_phase2_aot_runtime_completeness.py -v`
- passed: `python -m pytest tests/unit/run/test_phase2_async_await_proof.py -v`
- passed: `python -m pytest tests/unit/run/test_phase2_threading_gc_proof.py -v`
- passed: `python -m pytest tests/unit/run/test_phase2_nested_exception_proof.py -v`
- passed: `python -m pytest tests/unit/run/test_phase2_marshaling_proof.py -v`
- passed: `python -m pytest tests/unit/run/test_phase2_linker_stripping_proof.py -v`
- passed: `python -m pytest tests/unit/run/test_subject_workers_perf.py -v`
- passed: `python -m pytest tests/unit/run/test_phase2_perf_governance.py -v`
- passed: `python -m pytest tests/unit/run/test_subject_manifest_schema.py -v`
- passed: `python -m pytest tests/unit/run/test_subject_planner.py -v`
- passed: `python -m pytest tests/unit/run/test_subject_reporting.py -v`
- passed: `python -m pytest tests/unit/run -v`
- passed: `dotnet build subjects/InterfaceDispatchProof/source/InterfaceDispatchProof.csproj -c Debug`
- passed: `dotnet build subjects/LinkerStrippingProof/source/LinkerStrippingProof.csproj -c Debug`
- passed: `dotnet build solutions/core/windows/chaos-il2cpp-core.sln -c Debug`
- passed: `python build/toolchains/run/run.py --json test subject --id subject/InterfaceDispatchProof --goal perf.profile --validation-profile perf-profile --variant PROFILE`
- passed: `python tests/perf/convert-golden-multi-project/check.py`

## 风险 / 阻塞

### risks

- Batch 5 的完成定义仍严格限制在 analysis 侧 preserve/stripping proof；debug baseline、stack trace recovery 与完整 stripped report 仍属于后续阶段，不可误报为已完成。
- `dispatchNanoseconds` 是 proof-first steady-state 指标，来自 PROFILE 模板中 direct call 与 `invoke_virtual` 的差值模型；它是当前架构下的诚实近似值，不等于完整产品 perf dashboard。
- `GoldenMultiProject` convert perf 仍是 suite 级入口；若后续要统一纳入 subject pipeline，需要单独扩展 `subjects/golden/*` 的发现与路径规则。

### blockers

- 无。
