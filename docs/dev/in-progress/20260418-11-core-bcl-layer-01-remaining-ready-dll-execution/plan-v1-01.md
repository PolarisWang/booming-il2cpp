# Core BCL Layer 01 Remaining Ready DLL Execution 实施计划 v1.01

> 面向执行 Agent: 必须使用 `dev:executing-plans` 执行本计划。步骤使用复选框语法跟踪。
> 目标: 直接覆盖 `core-bcl-layer-01` 中 representative batch 之外的剩余 `21` 个 ready DLL，执行聚合后的 formal proof / benchmark / engineering gate，并完成逐 DLL `full certified` closeout；在整层 closeout 之前，先给出 `gameplay-core-ready` 阶段性验收结论。
> 架构: 先冻结 remaining batch 的 machine-readable scope，再按逐程序集 plan 聚合出的 unique canonical commands 执行 proof、engineering validation 与 benchmark，最后把 shared run evidence 回填为逐 DLL `assembly-certification-report`，再做整层收口判断。
> 技术栈: Markdown、JSON、PowerShell、dependency-layer artifacts、SolutionCorePack registry canonical commands

**AOT/IL2CPP/Test Governance Intake**
- capabilityFamily: `FrameworkPackNativeization`
- capabilityItem: `CoreBclLayer01RemainingReadyDllExecution`
- ownerSubjectId: `SolutionCorePack`
- proofRequired: `true`
- benchmarkRequired: `true`
- hotupdateImpact: `None`
- formalVerificationObjects:
  - `docs/dev/in-progress/20260418-11-core-bcl-layer-01-remaining-ready-dll-execution/remaining-ready-scope-v1-01.json`
  - `declared-unit-test/SolutionCorePack::CoreRuntimeFeatures::CoreRuntimeFeatures.AbiWideningProofEntry::Run()`
  - `declared-unit-test/SolutionCorePack::CoreRuntimeFeatures::CoreRuntimeFeatures.AotClosureValidationProofEntry::Run()`
  - `declared-unit-test/SolutionCorePack::CoreRuntimeFeatures::CoreRuntimeFeatures.ConversionOpsProofEntry::Run()`
  - `declared-unit-test/SolutionCorePack::CoreRuntimeFeatures::CoreRuntimeFeatures.DirectCallVirtProofEntry::Run()`
  - `declared-unit-test/SolutionCorePack::CoreRuntimeFeatures::CoreRuntimeFeatures.FunctionPointerProofEntry::Run()`
  - `declared-unit-test/SolutionCorePack::CoreRuntimeFeatures::CoreRuntimeFeatures.MemberMetadataLookupProofEntry::Run()`
  - `declared-unit-test/SolutionCorePack::CoreRuntimeFeatures::CoreRuntimeFeatures.MetadataSupplementProofEntry::Run()`
  - `declared-unit-test/SolutionCorePack::CoreRuntimeFeatures::CoreRuntimeFeatures.ReflectionInteropClosureEntry::Run()`
  - `declared-unit-test/SolutionCorePack::CoreRuntimeFeatures::CoreRuntimeFeatures.RequiredInstantiationClosureProofEntry::Run()`
  - `declared-unit-test/SolutionCorePack::CoreRuntimeFeatures::CoreRuntimeFeatures.SpanAndMemoryProofEntry::Run()`
  - `declared-unit-test/SolutionCorePack::CoreRuntimeFeatures::CoreRuntimeFeatures.TaskAndValueTaskFlowProofEntry::Run()`
  - `declared-benchmark/SolutionCorePack::CoreRuntimeBenchmarks::CoreRuntimeBenchmarks.AllocationBenchmarkEntry::RunWorkload()`
  - `declared-benchmark/SolutionCorePack::CoreRuntimeBenchmarks::CoreRuntimeBenchmarks.FunctionPointerBenchmarkEntry::RunWorkload()`
  - `declared-benchmark/SolutionCorePack::CoreRuntimeBenchmarks::CoreRuntimeBenchmarks.MemberMetadataLookupBenchmarkEntry::RunWorkload()`
  - `declared-benchmark/SolutionCorePack::CoreRuntimeBenchmarks::CoreRuntimeBenchmarks.RequiredInstantiationClosureBenchmarkEntry::RunWorkload()`
  - `declared-benchmark/SolutionCorePack::CoreRuntimeBenchmarks::CoreRuntimeBenchmarks.SpanAndMemoryBenchmarkEntry::RunWorkload()`
  - `declared-benchmark/SolutionCorePack::CoreRuntimeBenchmarks::CoreRuntimeBenchmarks.StringAndUtf8MarshalingBenchmarkEntry::RunWorkload()`
  - `declared-benchmark/SolutionCorePack::CoreRuntimeBenchmarks::CoreRuntimeBenchmarks.TaskAndValueTaskFlowBenchmarkEntry::RunWorkload()`
  - `engineering-validation/SolutionCorePack/managed-build`
  - `engineering-validation/SolutionCorePack/managed-runtime-output`
  - `engineering-validation/SolutionCorePack/native-executable-smoke`
  - `engineering-validation/SolutionCorePack/project-graph`
- requiredGates: `collector -> registry -> workspace -> dependency-layer-proof -> benchmark`

**设计文档**: `docs/dev/in-progress/20260418-11-core-bcl-layer-01-remaining-ready-dll-execution/design-v1-01.md`
**验收文档**: `docs/dev/in-progress/20260418-11-core-bcl-layer-01-remaining-ready-dll-execution/dll-full-certified-acceptance-path-v1-01.md`

**预期知识沉淀**: 沉淀剩余 ready DLL 的整批执行证据、逐 DLL certification closeout 结果，并判断 `core-bcl-layer-01` 是否达到 full certification 前置条件
**收尾约束**: 执行完成后必须进入“审视架构合理性 -> 测试通过 -> 归档 completed -> 合并&提交”的固定链路。

---

Status: in-progress

## 计划清单

- [ ] 步骤 1: 冻结剩余 `21` 个 ready DLL 的 scope、程序集清单、`gameplay-core-ready` milestone 与 formal verification objects
- [ ] 步骤 2: 校对逐程序集 plan 聚合出的 unique proof / benchmark / engineering gate 映射，并把“完整 native 化四层判定”接到逐 DLL closeout criteria
- [ ] 步骤 3: 对接 `20260418-10` 代表性批次的 `8` 个基线 DLL 证据，明确 `gameplay-core-ready` 的 baseline report 输入
- [ ] 步骤 4: 执行 `11` 个 unique native proofs，并记录 run evidence
- [ ] 步骤 5: 执行 `4` 个 engineering validations 与 `7` 个 benchmarks，并记录 throughput / gate 结果
- [ ] 步骤 6: 先按优先顺序回填当前 `7` 个核心 DLL 的 shared run evidence，并与基线 `8` 个 DLL 一起评估 `gameplay-core-ready`
- [ ] 步骤 7: 再补齐剩余 tail DLL 的 `assembly-certification-report/<assembly>.json`，生成 `layer-certification-summary-v1-01.json`
- [ ] 步骤 8: 同步 child / parent `STATUS.md`、`docs/dev/ACTIVE.md`、progress note 与索引，并根据 `gameplay-core-ready` 与整层逐 DLL report 判断是否继续收口 `core-bcl-layer-01`

## 需要创建或修改的文件

- `docs/dev/in-progress/20260418-11-core-bcl-layer-01-remaining-ready-dll-execution/STATUS.md`
- `docs/dev/in-progress/20260418-11-core-bcl-layer-01-remaining-ready-dll-execution/design-v1-01.md`
- `docs/dev/in-progress/20260418-11-core-bcl-layer-01-remaining-ready-dll-execution/plan-v1-01.md`
- `docs/dev/in-progress/20260418-11-core-bcl-layer-01-remaining-ready-dll-execution/remaining-ready-scope-v1-01.json`
- `docs/dev/in-progress/20260418-11-core-bcl-layer-01-remaining-ready-dll-execution/native-proof-summary-v1-01.json`
- `docs/dev/in-progress/20260418-11-core-bcl-layer-01-remaining-ready-dll-execution/nativeization-throughput-benchmark-v1-01.json`
- `docs/dev/in-progress/20260418-11-core-bcl-layer-01-remaining-ready-dll-execution/dll-full-certified-acceptance-path-v1-01.md`
- `docs/dev/in-progress/20260418-11-core-bcl-layer-01-remaining-ready-dll-execution/assembly-certification-report/`
- `docs/dev/in-progress/20260418-11-core-bcl-layer-01-remaining-ready-dll-execution/layer-certification-summary-v1-01.json`
- `docs/dev/in-progress/20260418-11-core-bcl-layer-01-remaining-ready-dll-execution/notes/progress-v1-01.md`
- `docs/dev/in-progress/20260418-01-dotnet-foundation-pack-nativeization-roadmap/STATUS.md`
- `docs/dev/ACTIVE.md`
- `docs/dev/INDEX.md`
- `docs/dev/in-progress/INDEX.md`

## 预期验证命令

- `Get-Content docs/dev/in-progress/20260418-11-core-bcl-layer-01-remaining-ready-dll-execution/remaining-ready-scope-v1-01.json -Raw | ConvertFrom-Json | Out-Null`
- `python build/toolchains/run/run.py test registry refresh --json`
- `python build/toolchains/run/run.py test declared-unit-test --id declared-unit-test/SolutionCorePack::CoreRuntimeFeatures::CoreRuntimeFeatures.AbiWideningProofEntry::Run() --json`
- `python build/toolchains/run/run.py test declared-unit-test --id declared-unit-test/SolutionCorePack::CoreRuntimeFeatures::CoreRuntimeFeatures.AotClosureValidationProofEntry::Run() --json`
- `python build/toolchains/run/run.py test declared-unit-test --id declared-unit-test/SolutionCorePack::CoreRuntimeFeatures::CoreRuntimeFeatures.ConversionOpsProofEntry::Run() --json`
- `python build/toolchains/run/run.py test declared-unit-test --id declared-unit-test/SolutionCorePack::CoreRuntimeFeatures::CoreRuntimeFeatures.DirectCallVirtProofEntry::Run() --json`
- `python build/toolchains/run/run.py test declared-unit-test --id declared-unit-test/SolutionCorePack::CoreRuntimeFeatures::CoreRuntimeFeatures.FunctionPointerProofEntry::Run() --json`
- `python build/toolchains/run/run.py test declared-unit-test --id declared-unit-test/SolutionCorePack::CoreRuntimeFeatures::CoreRuntimeFeatures.MemberMetadataLookupProofEntry::Run() --json`
- `python build/toolchains/run/run.py test declared-unit-test --id declared-unit-test/SolutionCorePack::CoreRuntimeFeatures::CoreRuntimeFeatures.MetadataSupplementProofEntry::Run() --json`
- `python build/toolchains/run/run.py test declared-unit-test --id declared-unit-test/SolutionCorePack::CoreRuntimeFeatures::CoreRuntimeFeatures.ReflectionInteropClosureEntry::Run() --json`
- `python build/toolchains/run/run.py test declared-unit-test --id declared-unit-test/SolutionCorePack::CoreRuntimeFeatures::CoreRuntimeFeatures.RequiredInstantiationClosureProofEntry::Run() --json`
- `python build/toolchains/run/run.py test declared-unit-test --id declared-unit-test/SolutionCorePack::CoreRuntimeFeatures::CoreRuntimeFeatures.SpanAndMemoryProofEntry::Run() --json`
- `python build/toolchains/run/run.py test declared-unit-test --id declared-unit-test/SolutionCorePack::CoreRuntimeFeatures::CoreRuntimeFeatures.TaskAndValueTaskFlowProofEntry::Run() --json`
- `python build/toolchains/run/run.py test engineering-validation --id engineering-validation/SolutionCorePack/managed-build --json`
- `python build/toolchains/run/run.py test engineering-validation --id engineering-validation/SolutionCorePack/managed-runtime-output --json`
- `python build/toolchains/run/run.py test engineering-validation --id engineering-validation/SolutionCorePack/native-executable-smoke --json`
- `python build/toolchains/run/run.py test engineering-validation --id engineering-validation/SolutionCorePack/project-graph --json`
- `python build/toolchains/run/run.py test declared-benchmark --id declared-benchmark/SolutionCorePack::CoreRuntimeBenchmarks::CoreRuntimeBenchmarks.AllocationBenchmarkEntry::RunWorkload() --json`
- `python build/toolchains/run/run.py test declared-benchmark --id declared-benchmark/SolutionCorePack::CoreRuntimeBenchmarks::CoreRuntimeBenchmarks.FunctionPointerBenchmarkEntry::RunWorkload() --json`
- `python build/toolchains/run/run.py test declared-benchmark --id declared-benchmark/SolutionCorePack::CoreRuntimeBenchmarks::CoreRuntimeBenchmarks.MemberMetadataLookupBenchmarkEntry::RunWorkload() --json`
- `python build/toolchains/run/run.py test declared-benchmark --id declared-benchmark/SolutionCorePack::CoreRuntimeBenchmarks::CoreRuntimeBenchmarks.RequiredInstantiationClosureBenchmarkEntry::RunWorkload() --json`
- `python build/toolchains/run/run.py test declared-benchmark --id declared-benchmark/SolutionCorePack::CoreRuntimeBenchmarks::CoreRuntimeBenchmarks.SpanAndMemoryBenchmarkEntry::RunWorkload() --json`
- `python build/toolchains/run/run.py test declared-benchmark --id declared-benchmark/SolutionCorePack::CoreRuntimeBenchmarks::CoreRuntimeBenchmarks.StringAndUtf8MarshalingBenchmarkEntry::RunWorkload() --json`
- `python build/toolchains/run/run.py test declared-benchmark --id declared-benchmark/SolutionCorePack::CoreRuntimeBenchmarks::CoreRuntimeBenchmarks.TaskAndValueTaskFlowBenchmarkEntry::RunWorkload() --json`
- `Get-Content docs/dev/in-progress/20260418-11-core-bcl-layer-01-remaining-ready-dll-execution/dll-full-certified-acceptance-path-v1-01.md -Raw | Out-Null`
- `Get-ChildItem docs/dev/in-progress/20260418-11-core-bcl-layer-01-remaining-ready-dll-execution/assembly-certification-report/*.json | ForEach-Object { Get-Content $_.FullName -Raw | ConvertFrom-Json | Out-Null }`
- `Get-Content docs/dev/in-progress/20260418-11-core-bcl-layer-01-remaining-ready-dll-execution/layer-certification-summary-v1-01.json -Raw | ConvertFrom-Json | Out-Null`

## 状态维护要求

- `docs/dev/ACTIVE.md` 指向当前 child task
- 父 roadmap 保持 `in_progress` 且 `active = false`
- `20260418-10-core-bcl-layer-01-priority-batch-execution` 已归档 `completed`
- 如验证阶段出现 `dotnet build` / `dotnet test` / `msbuild` 崩溃，必须立即进入根因排查，未修复前不得归档
