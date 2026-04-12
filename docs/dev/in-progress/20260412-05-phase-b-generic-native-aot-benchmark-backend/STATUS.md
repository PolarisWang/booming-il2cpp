---
task_id: 20260412-05-phase-b-generic-native-aot-benchmark-backend
title: Phase B Generic Native AOT Benchmark Backend
task_type: roadmap
lifecycle_status: in_progress
phase: roadmap
created_at: 2026-04-12 11:42:02 +08:00
updated_at: 2026-04-12 15:15:10 +08:00
current_dir: docs/dev/in-progress/20260412-05-phase-b-generic-native-aot-benchmark-backend
parent_task_id: "20260411-10-post-phase7-development-roadmap"
source_task_id: "20260411-10-post-phase7-development-roadmap"
source_relation: child_of_roadmap_phase_B
active: true
---

## 关键文档

- parent_roadmap: `docs/dev/in-progress/20260411-10-post-phase7-development-roadmap/roadmap-v1-01.md`
- brainstorm: `docs/dev/in-progress/20260412-05-phase-b-generic-native-aot-benchmark-backend/brainstorm-v1-01.md`
- roadmap_or_plan: `docs/dev/in-progress/20260412-05-phase-b-generic-native-aot-benchmark-backend/roadmap-v1-01.md`

## 当前判断

- current_focus: `BenchArithmetic` generic native AOT MVP 已闭环；managed / interpreter / native 三条 benchmark 链路现在都走 workload boundary + 外置 harness/native host，且统一由 subject manifest 的 `validation.perf.harnessIterations` 控制批量迭代。
- why_now: 旧实现里 managed / interpreter 默认 `10000` 次外置迭代会把 `RunWorkload()` 跑到约 97 秒；同时 native 每 sample 只跑 1 次，导致 dashboard 虽然恢复了 AOT 数据，但三种模式的 duration/checksum 口径并不一致。
- done_definition: `BenchArithmetic` 在 managed / interpreter / native 三种模式下都能产出真实 record，计时与批量执行仍然留在 generated code 外部，dashboard 最新值来自同一 workload contract。

## 最近摘要

- 2026-04-12 11:42:02 +08:00: 派生 Phase B 子任务，明确目标是把 benchmark native 从 proof family 特判路线切到 generic native AOT backend。
- 2026-04-12 13:07:54 +08:00: Stage 2 完成，冻结 `workloadEntry` 与 external perf harness 边界，计时逻辑保持在 benchmark source / generated code 之外。
- 2026-04-12 14:31:35 +08:00: Stage 3 完成，`BenchArithmetic` 首次产出真实 native AOT record，dashboard 不再显示 AOT 无数据。
- 2026-04-12 15:15:10 +08:00: 补齐 subject 级 `harnessIterations` override，并让 native host 也复用同一批量迭代口径；三种模式现已对齐到同一 workload / checksum / batch duration 语义。

## 下一步

- next_action: 如果继续推进 Phase B，进入 Stage 4 rollout，把 `generated-native-aot + external harness/native host + harnessIterations` 这一整套 contract 扩到其他 `Bench*` subject。
- trigger: 当前用户提出的 `BenchArithmetic` blocker 已收口，可按需启动 rollout。

## 风险 / 阻塞

### risks

- `BenchArithmetic` 通过 `harnessIterations: 10` 收敛了执行时长，但其他 `Bench*` subject 很可能也需要各自的 override，避免 managed / interpreter 的外置批量运行再次过慢。
- native host 现在输出的是 batch 口径的 `elapsedMilliseconds / opsPerSecond / checksum`；后续 rollout 必须维持与 managed / interpreter 相同的批量语义，不能回退成单次调用口径。
- 历史 records 里仍保留旧口径数据，例如 97 秒的 managed / interpreter 记录和旧 native raw checksum 记录；dashboard 最新值已切到新记录，但历史曲线仍会同时看到旧值与新值。

### blockers

- 当前无外部环境 blocker；`BenchArithmetic` MVP 已闭环。

## 关联任务

### parent

- `20260411-10-post-phase7-development-roadmap`

### source

- `20260411-10-post-phase7-development-roadmap`，`child_of_roadmap_phase_B`

### planned_children

- `pending-phase-b-aot-contract-split`
- `pending-phase-b-workload-entry-boundary`
- `pending-phase-b-bencharithmetic-generic-native-aot-mvp`
- `pending-phase-b-native-rollout`

## 2026-04-12 15:15:10 执行更新

- `subjects/BenchArithmetic/subject.manifest.json` 新增 `validation.perf.harnessIterations: 10`，把 iteration 配置固定在 subject manifest，而不是 benchmark source 或 generated code。
- `build/toolchains/run/testing/subject_workers.py` 新增 subject 级 perf iteration 解析；managed / interpreter 现在优先读取 manifest override；native runtime 也会把 override 传给外置 native host。
- `src/native/benchmark-host/native_aot_main.cpp` 新增 `--iterations` 支持，native host 负责批量调用 `RunNativeAot()`，并输出与 managed / interpreter 对齐的 `elapsedMilliseconds / opsPerSecond / checksum / iterations`。
- 新增或更新验证：
  - `tests/unit/run/test_subject_workers.py`
  - `tests/unit/run/test_subject_workers_perf.py`
  - `tests/unit/run/test_subject_manifest_schema.py`
  - `tests/unit/run/test_benchmark_subject_sources.py`
  - `tests/unit/run/test_benchmark_command.py`
  - `tests/unit/run/test_phase_b_native_aot_workload_entry_bundle.py`
- 单测证据：
  - `python -m pytest tests/unit/run/test_subject_workers.py tests/unit/run/test_subject_workers_perf.py tests/unit/run/test_subject_manifest_schema.py tests/unit/run/test_benchmark_subject_sources.py tests/unit/run/test_benchmark_command.py tests/unit/run/test_phase_b_native_aot_workload_entry_bundle.py -q`
  - 结果：`41 passed, 1 warning`
- 端到端 benchmark 证据：
  - `python build/toolchains/run/run.py benchmark --subject BenchArithmetic --mode managed --record`
  - `python build/toolchains/run/run.py benchmark --subject BenchArithmetic --mode interpreter --record`
  - `python build/toolchains/run/run.py benchmark --subject BenchArithmetic --mode native --record`
- 最新 records：
  - `managed`: `20260412-065556-BenchArithmetic-managed`，`meanDurationMs = 63.484`，`meanChecksum = 7010`
  - `interpreter`: `20260412-065652-BenchArithmetic-interpreter`，`meanDurationMs = 63.640`，`meanChecksum = 7010`
  - `native`: `20260412-071431-BenchArithmetic-native`，`meanDurationMs = 59.961`，`meanChecksum = 7010`
- dashboard 结果：
  - `docs/benchmark/subjects/BenchArithmetic.json` 已切到最新三条 record
  - `docs/benchmark/overview.json` 已更新
  - `nativeSpeedup` 现在是 `1.06x`，不再受批量迭代口径错位影响
- wiki: 本轮暂不单独更新 wiki；长期规则先固化在 manifest contract、native host 与单测中，待 Stage 4 rollout 时再统一沉淀。
