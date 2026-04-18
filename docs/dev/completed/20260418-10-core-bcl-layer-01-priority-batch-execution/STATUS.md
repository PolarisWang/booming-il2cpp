---
task_id: 20260418-10-core-bcl-layer-01-priority-batch-execution
title: Core BCL Layer 01 Priority Batch Execution
task_type: plan
lifecycle_status: completed
phase: completed
created_at: 2026-04-18 13:17:03 +08:00
updated_at: 2026-04-18 14:27:06 +08:00
latest_stop_point: completed after rerunning registry refresh, six representative native proofs, four engineering validations, four benchmarks, generating machine-readable summaries, and archiving the child task
current_dir: docs/dev/completed/20260418-10-core-bcl-layer-01-priority-batch-execution
parent_task_id: 20260418-01-dotnet-foundation-pack-nativeization-roadmap
source_task_id: 20260418-01-dotnet-foundation-pack-nativeization-roadmap
source_relation: roadmap-child-core-bcl-layer-01-priority-batch-execution
design_doc: docs/dev/completed/20260418-10-core-bcl-layer-01-priority-batch-execution/design-v1-01.md
plan_doc: docs/dev/completed/20260418-10-core-bcl-layer-01-priority-batch-execution/plan-v1-01.md
total_tasks: 6
current_task: completed
active: false
---

## 完成结论

- `core-bcl-layer-01` representative priority batch 已完成真实闭环：`6` 个 native proof、`4` 个 engineering validation、`4` 个 benchmark 全部返回 `finalStatus = ok`。
- 本轮生成 `native-proof-summary-v1-01.json` 与 `nativeization-throughput-benchmark-v1-01.json`，把 run id、summary/events/console 路径、benchmark 吞吐指标与 gate 结果落成 machine-readable artifact。
- representative batch 覆盖 collections / memory / threading / console 四个方向，证明 `SolutionCorePack` 现有 registry canonical commands 可以作为 layer-01 execution 模板继续扩展。
- 当前结论只覆盖 `8 / 29` 个 ready DLL 的代表切片，不等价于整个 `core-bcl-layer-01` 已完成 full certification。

## 本轮收口

- 重跑 `collector -> registry` 入口：`python build/toolchains/run/run.py test registry refresh --json` 成功，刷新 `artifacts/tests/registry/current/index.json` 并生成历史快照 `artifacts/tests/registry/history/20260418-141046/index.json`。
- 重跑 `dependency-layer-proof`：
  - `aot-closure-validation-proof` -> `20260418-141336-windows-1a51`
  - `generic-collection-proof` -> `20260418-141411-windows-21a5`
  - `span-memory-proof` -> `20260418-141448-windows-fe56`
  - `threading-proof` -> `20260418-141524-windows-5381`
  - `monitor-locking-proof` -> `20260418-141604-windows-41cc`
  - `required-instantiation-closure-proof` -> `20260418-141641-windows-cf1c`
- 重跑 `workspace` / engineering gate：
  - `managed-build` -> `20260418-141843-windows-fc17`
  - `managed-runtime-output` -> `20260418-141852-windows-3789`
  - `native-executable-smoke` -> `20260418-141900-windows-f6e3`
  - `project-graph` -> `20260418-141958-windows-a576`
- 重跑 `benchmark`：
  - `generic-bench` -> `20260418-142116-windows-75c2`
  - `span-memory-bench` -> `20260418-142132-windows-8aab`
  - `task-scheduling-bench` -> `20260418-142151-windows-33f6`
  - `required-instantiation-closure-bench` -> `20260418-142209-windows-2118`
- 将任务目录归档到 `docs/dev/completed/20260418-10-core-bcl-layer-01-priority-batch-execution/`，同步父 roadmap 与 `docs/dev` 索引，并删除过期的 `docs/dev/ACTIVE.md`。

## 架构审视

- 本轮完全复用 `20260418-06` 冻结的 dependency-layer authority 与 `SolutionCorePack` registry canonical commands，没有引入新的 test-flow 或 owner subject 旁路。
- representative batch 同时覆盖 collection、memory、threading、console 四类能力，足以验证 layer-01 execution 模板是否自洽，而不会误把单一 proof 成功当作整层完成。
- `requiredGates = collector -> registry -> workspace -> dependency-layer-proof -> benchmark` 已逐项消费；formal verification object 的数量和 scope file 保持一致。
- `managed-build` / `managed-runtime-output` / `native-executable-smoke` / `project-graph` 与全部 benchmark / proof 运行期间未观察到 `dotnet build` / `dotnet test` / `msbuild` 崩溃。
- 结论：该 child 的职责边界清晰、证据完整，可以归档为 `completed`。

## 已验证

- `python build/toolchains/run/run.py test registry refresh --json`
  - 结果: `status = ok`，`currentPath = artifacts/tests/registry/current/index.json`，`historyPath = artifacts/tests/registry/history/20260418-141046/index.json`
- `python build/toolchains/run/run.py test declared-unit-test --id declared-unit-test/SolutionCorePack::CoreRuntimeFeatures::CoreRuntimeFeatures.AotClosureValidationProofEntry::Run() --json`
  - 结果: `runId = 20260418-141336-windows-1a51`，`finalStatus = ok`
- `python build/toolchains/run/run.py test declared-unit-test --id declared-unit-test/SolutionCorePack::CoreRuntimeFeatures::CoreRuntimeFeatures.GenericCollectionProofEntry::Run() --json`
  - 结果: `runId = 20260418-141411-windows-21a5`，`finalStatus = ok`
- `python build/toolchains/run/run.py test declared-unit-test --id declared-unit-test/SolutionCorePack::CoreRuntimeFeatures::CoreRuntimeFeatures.SpanAndMemoryProofEntry::Run() --json`
  - 结果: `runId = 20260418-141448-windows-fe56`，`finalStatus = ok`
- `python build/toolchains/run/run.py test declared-unit-test --id declared-unit-test/SolutionCorePack::CoreRuntimeFeatures::CoreRuntimeFeatures.ThreadingProofEntry::Run() --json`
  - 结果: `runId = 20260418-141524-windows-5381`，`finalStatus = ok`
- `python build/toolchains/run/run.py test declared-unit-test --id declared-unit-test/SolutionCorePack::CoreRuntimeFeatures::CoreRuntimeFeatures.MonitorAndLockingProofEntry::Run() --json`
  - 结果: `runId = 20260418-141604-windows-41cc`，`finalStatus = ok`
- `python build/toolchains/run/run.py test declared-unit-test --id declared-unit-test/SolutionCorePack::CoreRuntimeFeatures::CoreRuntimeFeatures.RequiredInstantiationClosureProofEntry::Run() --json`
  - 结果: `runId = 20260418-141641-windows-cf1c`，`finalStatus = ok`
- `python build/toolchains/run/run.py test engineering-validation --id engineering-validation/SolutionCorePack/managed-build --json`
  - 结果: `runId = 20260418-141843-windows-fc17`，`finalStatus = ok`
- `python build/toolchains/run/run.py test engineering-validation --id engineering-validation/SolutionCorePack/managed-runtime-output --json`
  - 结果: `runId = 20260418-141852-windows-3789`，`finalStatus = ok`
- `python build/toolchains/run/run.py test engineering-validation --id engineering-validation/SolutionCorePack/native-executable-smoke --json`
  - 结果: `runId = 20260418-141900-windows-f6e3`，`finalStatus = ok`
- `python build/toolchains/run/run.py test engineering-validation --id engineering-validation/SolutionCorePack/project-graph --json`
  - 结果: `runId = 20260418-141958-windows-a576`，`finalStatus = ok`
- `python build/toolchains/run/run.py test declared-benchmark --id declared-benchmark/SolutionCorePack::CoreRuntimeBenchmarks::CoreRuntimeBenchmarks.GenericBenchmarkEntry::RunWorkload() --json`
  - 结果: `runId = 20260418-142116-windows-75c2`，`finalStatus = ok`，`meanOpsPerSecond = 509.229`
- `python build/toolchains/run/run.py test declared-benchmark --id declared-benchmark/SolutionCorePack::CoreRuntimeBenchmarks::CoreRuntimeBenchmarks.SpanAndMemoryBenchmarkEntry::RunWorkload() --json`
  - 结果: `runId = 20260418-142132-windows-8aab`，`finalStatus = ok`，`meanOpsPerSecond = 5686.031`
- `python build/toolchains/run/run.py test declared-benchmark --id declared-benchmark/SolutionCorePack::CoreRuntimeBenchmarks::CoreRuntimeBenchmarks.TaskSchedulingBenchmarkEntry::RunWorkload() --json`
  - 结果: `runId = 20260418-142151-windows-33f6`，`finalStatus = ok`，`meanOpsPerSecond = 1658.744`
- `python build/toolchains/run/run.py test declared-benchmark --id declared-benchmark/SolutionCorePack::CoreRuntimeBenchmarks::CoreRuntimeBenchmarks.RequiredInstantiationClosureBenchmarkEntry::RunWorkload() --json`
  - 结果: `runId = 20260418-142209-windows-2118`，`finalStatus = ok`，`meanOpsPerSecond = 9565.572`
- `Get-Content docs/dev/completed/20260418-10-core-bcl-layer-01-priority-batch-execution/native-proof-summary-v1-01.json -Raw | ConvertFrom-Json | Out-Null`
  - 结果: 通过
- `Get-Content docs/dev/completed/20260418-10-core-bcl-layer-01-priority-batch-execution/nativeization-throughput-benchmark-v1-01.json -Raw | ConvertFrom-Json | Out-Null`
  - 结果: 通过
- 说明:
  - 本轮验证期间未出现 `dotnet build` / `dotnet test` / `msbuild` 崩溃
  - `native-proof-summary-v1-01.json` 与 `nativeization-throughput-benchmark-v1-01.json` 已落盘且可解析

## 风险 / 余留

- 当前只完成 representative batch；`core-bcl-layer-01` 仍有 `21` 个 ready DLL 尚未进入 full execution child。
- 四个 benchmark 当前全部报告 `regressionStatus = no-baseline`，因此本轮结论是吞吐证据，而不是相对历史基线的回归认证。
- 更高层 dependency layer、复杂 BCL、外部 DLL 与 `.NET 10` delta closure 仍未开始，后续 scope 明显大于本 child。

## 下一步

- 回到父 roadmap，基于已验证的 representative execution 模板，创建下一条 child 把 `core-bcl-layer-01` 的剩余 `21` 个 ready DLL 继续推进到真实 proof / benchmark 闭环。

## wiki

- 本轮不写入 wiki
- 原因: 当前新增的是执行证据与 representative batch 结论，没有形成新的跨任务长期 authority
