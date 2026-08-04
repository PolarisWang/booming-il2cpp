# Core BCL Layer 01 Priority Batch Execution 实施计划 v1.01

> 面向执行 Agent: 必须使用 `dev:executing-plans` 执行本计划。步骤使用复选框语法跟踪。
> 目标: 围绕 `core-bcl-layer-01` 的 representative priority batch 执行真实 native proof / benchmark / engineering gate，并为剩余 layer-01 DLL 沉淀可复用 execution 模板。
> 架构: 先冻结 machine-readable batch scope，再按 registry canonical commands 执行 representative proofs、benchmarks 与 engineering validations，最后沉淀 summary 与后续扩展决策。
> 技术栈: Markdown、JSON、PowerShell、dependency-layer artifacts、SolutionCorePack registry canonical commands

**AOT/IL2CPP/Test Governance Intake**
- capabilityFamily: `FrameworkPackNativeization`
- capabilityItem: `CoreBclLayer01PriorityBatchExecution`
- ownerSubjectId: `SolutionCorePack`
- proofRequired: `true`
- benchmarkRequired: `true`
- hotupdateImpact: `None`
- formalVerificationObjects:
  - `docs/dev/completed/20260418-10-core-bcl-layer-01-priority-batch-execution/priority-batch-scope-v1-01.json`
  - `declared-unit-test/SolutionCorePack::CoreRuntimeFeatures::CoreRuntimeFeatures.AotClosureValidationProofEntry::Run()`
  - `declared-unit-test/SolutionCorePack::CoreRuntimeFeatures::CoreRuntimeFeatures.GenericCollectionProofEntry::Run()`
  - `declared-unit-test/SolutionCorePack::CoreRuntimeFeatures::CoreRuntimeFeatures.SpanAndMemoryProofEntry::Run()`
  - `declared-unit-test/SolutionCorePack::CoreRuntimeFeatures::CoreRuntimeFeatures.ThreadingProofEntry::Run()`
  - `declared-unit-test/SolutionCorePack::CoreRuntimeFeatures::CoreRuntimeFeatures.MonitorAndLockingProofEntry::Run()`
  - `declared-unit-test/SolutionCorePack::CoreRuntimeFeatures::CoreRuntimeFeatures.RequiredInstantiationClosureProofEntry::Run()`
  - `declared-benchmark/SolutionCorePack::CoreRuntimeBenchmarks::CoreRuntimeBenchmarks.GenericBenchmarkEntry::RunWorkload()`
  - `declared-benchmark/SolutionCorePack::CoreRuntimeBenchmarks::CoreRuntimeBenchmarks.SpanAndMemoryBenchmarkEntry::RunWorkload()`
  - `declared-benchmark/SolutionCorePack::CoreRuntimeBenchmarks::CoreRuntimeBenchmarks.TaskSchedulingBenchmarkEntry::RunWorkload()`
  - `declared-benchmark/SolutionCorePack::CoreRuntimeBenchmarks::CoreRuntimeBenchmarks.RequiredInstantiationClosureBenchmarkEntry::RunWorkload()`
  - `engineering-validation/SolutionCorePack/managed-build`
  - `engineering-validation/SolutionCorePack/managed-runtime-output`
  - `engineering-validation/SolutionCorePack/native-executable-smoke`
  - `engineering-validation/SolutionCorePack/project-graph`
- requiredGates: `collector -> registry -> workspace -> dependency-layer-proof -> benchmark`

**设计文档**: `docs/dev/completed/20260418-10-core-bcl-layer-01-priority-batch-execution/design-v1-01.md`

**预期知识沉淀**: 按任务决定；优先沉淀 representative dependency-layer execution 模板是否值得升级为长期 authority
**收尾约束**: 执行完成后必须进入“审视架构合理性 -> 测试通过 -> 归档 completed -> 合并&提交”的固定链路。

---

Status: completed

## 计划清单

- [x] 步骤 1: 冻结 priority batch 范围、程序集清单与 formal verification objects
- [x] 步骤 2: 校对每个程序集与 proof / benchmark / engineering gate 的 authority 映射
- [x] 步骤 3: 执行 representative native proofs，并记录 per-assembly / per-capability 结果
- [x] 步骤 4: 执行 benchmarks 与 engineering validations，记录 throughput 与 gate 结果
- [x] 步骤 5: 生成 `native-proof-summary-v1-01.json` 与 `nativeization-throughput-benchmark-v1-01.json`
- [x] 步骤 6: 同步 child / parent `STATUS.md`、`docs/dev/ACTIVE.md`、progress note 与索引，并决定是否扩展剩余 layer-01 DLL；当前 child 已归档 `completed`

## 需要创建或修改的文件

- `docs/dev/completed/20260418-10-core-bcl-layer-01-priority-batch-execution/STATUS.md`
- `docs/dev/completed/20260418-10-core-bcl-layer-01-priority-batch-execution/design-v1-01.md`
- `docs/dev/completed/20260418-10-core-bcl-layer-01-priority-batch-execution/plan-v1-01.md`
- `docs/dev/completed/20260418-10-core-bcl-layer-01-priority-batch-execution/priority-batch-scope-v1-01.json`
- `docs/dev/completed/20260418-10-core-bcl-layer-01-priority-batch-execution/native-proof-summary-v1-01.json`
- `docs/dev/completed/20260418-10-core-bcl-layer-01-priority-batch-execution/nativeization-throughput-benchmark-v1-01.json`
- `docs/dev/completed/20260418-10-core-bcl-layer-01-priority-batch-execution/notes/progress-v1-01.md`
- `docs/dev/in-progress/20260418-01-dotnet-foundation-pack-nativeization-roadmap/STATUS.md`
- `docs/dev/in-progress/20260418-01-dotnet-foundation-pack-nativeization-roadmap/roadmap-v1-01.md`
- `docs/dev/INDEX.md`
- `docs/dev/in-progress/INDEX.md`
- `docs/dev/completed/INDEX.md`

## 预期验证命令

- `Get-Content docs/dev/completed/20260418-10-core-bcl-layer-01-priority-batch-execution/priority-batch-scope-v1-01.json -Raw | ConvertFrom-Json | Out-Null`
- `python build/toolchains/run/run.py test registry refresh --json`
- `python build/toolchains/run/run.py test declared-unit-test --id declared-unit-test/SolutionCorePack::CoreRuntimeFeatures::CoreRuntimeFeatures.AotClosureValidationProofEntry::Run() --json`
- `python build/toolchains/run/run.py test declared-unit-test --id declared-unit-test/SolutionCorePack::CoreRuntimeFeatures::CoreRuntimeFeatures.GenericCollectionProofEntry::Run() --json`
- `python build/toolchains/run/run.py test declared-unit-test --id declared-unit-test/SolutionCorePack::CoreRuntimeFeatures::CoreRuntimeFeatures.SpanAndMemoryProofEntry::Run() --json`
- `python build/toolchains/run/run.py test declared-unit-test --id declared-unit-test/SolutionCorePack::CoreRuntimeFeatures::CoreRuntimeFeatures.ThreadingProofEntry::Run() --json`
- `python build/toolchains/run/run.py test declared-unit-test --id declared-unit-test/SolutionCorePack::CoreRuntimeFeatures::CoreRuntimeFeatures.MonitorAndLockingProofEntry::Run() --json`
- `python build/toolchains/run/run.py test declared-unit-test --id declared-unit-test/SolutionCorePack::CoreRuntimeFeatures::CoreRuntimeFeatures.RequiredInstantiationClosureProofEntry::Run() --json`
- `python build/toolchains/run/run.py test engineering-validation --id engineering-validation/SolutionCorePack/managed-build --json`
- `python build/toolchains/run/run.py test engineering-validation --id engineering-validation/SolutionCorePack/managed-runtime-output --json`
- `python build/toolchains/run/run.py test engineering-validation --id engineering-validation/SolutionCorePack/native-executable-smoke --json`
- `python build/toolchains/run/run.py test engineering-validation --id engineering-validation/SolutionCorePack/project-graph --json`
- `python build/toolchains/run/run.py test declared-benchmark --id declared-benchmark/SolutionCorePack::CoreRuntimeBenchmarks::CoreRuntimeBenchmarks.GenericBenchmarkEntry::RunWorkload() --json`
- `python build/toolchains/run/run.py test declared-benchmark --id declared-benchmark/SolutionCorePack::CoreRuntimeBenchmarks::CoreRuntimeBenchmarks.SpanAndMemoryBenchmarkEntry::RunWorkload() --json`
- `python build/toolchains/run/run.py test declared-benchmark --id declared-benchmark/SolutionCorePack::CoreRuntimeBenchmarks::CoreRuntimeBenchmarks.TaskSchedulingBenchmarkEntry::RunWorkload() --json`
- `python build/toolchains/run/run.py test declared-benchmark --id declared-benchmark/SolutionCorePack::CoreRuntimeBenchmarks::CoreRuntimeBenchmarks.RequiredInstantiationClosureBenchmarkEntry::RunWorkload() --json`
- `Get-Content docs/dev/completed/20260418-10-core-bcl-layer-01-priority-batch-execution/native-proof-summary-v1-01.json -Raw | ConvertFrom-Json | Out-Null`
- `Get-Content docs/dev/completed/20260418-10-core-bcl-layer-01-priority-batch-execution/nativeization-throughput-benchmark-v1-01.json -Raw | ConvertFrom-Json | Out-Null`

## 状态维护要求

- child 归档后删除 `docs/dev/ACTIVE.md`
- 父 roadmap 保持 `in_progress` 且 `active = false`
- `20260418-06-dependency-driven-core-bcl-layer-nativeization` 与 `20260418-10-core-bcl-layer-01-priority-batch-execution` 均已归档 `completed`
- 如验证阶段出现 `dotnet build` / `dotnet test` / `msbuild` 崩溃，必须立即进入根因排查，未修复前不得归档
