---
task_id: 20260418-05-phase-3-system-private-corelib-semantic-family-nativeization
title: System.Private.CoreLib Semantic Family Nativeization
task_type: plan
lifecycle_status: completed
phase: completed
created_at: 2026-04-18 03:14:08 +08:00
updated_at: 2026-04-18 11:21:56 +08:00
latest_stop_point: completed after fixing the stage-scoped host-selection regression, executing the full first semantic-family batch, and passing architecture review plus formal verification
current_dir: docs/dev/completed/20260418-05-phase-3-system-private-corelib-semantic-family-nativeization
parent_task_id: 20260418-01-dotnet-foundation-pack-nativeization-roadmap
source_task_id: 20260418-01-dotnet-foundation-pack-nativeization-roadmap
source_relation: roadmap-child-corelib-semantic-family-nativeization
design_doc: docs/dev/completed/20260418-05-phase-3-system-private-corelib-semantic-family-nativeization/design-v1-01.md
plan_doc: docs/dev/completed/20260418-05-phase-3-system-private-corelib-semantic-family-nativeization/plan-v1-01.md
total_tasks: 6
current_task: completed
active: false
---

## 完成结论

- 首批 semantic-family batch `managed-cil`、`internal-call`、`field-metadata` 已完成从 execution entry 到真实 proof / conformance / engineering workload / benchmark 的闭环实跑。
- `semantic-family-execution-entry-v1-01.json` 绑定的 27 条 canonical commands 已全部通过，覆盖：
  - `14` 条 `declared-unit-test` native proof
  - `4` 条 `engineering-validation`
  - `3` 条 `engineering-workload`
  - `6` 条 `declared-benchmark`
- 本轮补齐了 `selection.source.entrySelection` 的阶段边界：subject-level managed entry 只在 native AOT 生成/构建/运行链路使用 workspace host，不再污染 `managed-runtime-output` 这类 source app 路径。

## 本轮收口

- 在 [subject_workers.py](/D:/agent/booming-il2cpp/build/toolchains/run/testing/subject_workers.py) 中把 subject-entry host 选择拆成“通用 managed host”与“native AOT 专用 host”两层解析。
- 保持 native AOT 路径继续支持：
  - declared unit test -> `proof-host`
  - subject entry selection -> `benchmark-host`
- 恢复 `managed-runtime-output` 的 source-primary 行为，避免 host-input 被误导向 `DeclaredBenchmarkHost`。
- 新增回归测试 [test_subject_workers.py](/D:/agent/booming-il2cpp/tests/unit/execution/test_subject_workers.py)，锁定 `subject entrySelection` 在 managed-output 下必须继续构建 source project。

## 架构审视

- `selection.source.entrySelection` 是 lowering 语义，不是全局 managed host 选择 authority；把它直接并入 `_selection_workspace_host_kind()` 会让 host-input、managed-output、native AOT 三类阶段共享错误判断。这次已将该泄漏收回。
- native AOT 仍然通过 `_windows_subject_build()` 集中消费 host contract，dispatch manifest 只在 `benchmark-host` 时生成，`proof-host` 不再带无意义的 benchmark 侧产物，职责边界更清晰。
- workspace manifest 依旧是 host / native test project contract 的唯一 authority；本轮没有引入新的跨层 schema，也没有把 native 特例继续扩散到 managed runtime stage。
- 结论：当前实现符合本任务“semantic-family execution entry -> real proof/conformance/benchmark run”的设计目标，架构合理，可归档 `completed`。

## 已验证

- `python -m pytest tests/unit/execution/test_subject_workers.py -q -k "test_host_input_build_keeps_source_project_for_subject_entry_selection_managed_output or test_windows_build_target_routes_subject_entry_selection_native_aot_through_benchmark_host_contract or test_runtime_observe_executes_native_aot_benchmark_host_output_for_subject_entry_selection or test_native_proof_emitter_routes_subject_entry_selection_through_native_aot or test_host_input_build_uses_workspace_managed_benchmark_host_for_declared_benchmark or test_host_input_build_uses_workspace_managed_proof_host_for_declared_unit_test"`
  - 结果: `6 passed`
- `python build/toolchains/run/run.py test engineering-validation --id "engineering-validation/SolutionCorePack/managed-runtime-output" --json`
  - 结果: `runId = 20260418-105927-windows-0ea8`，恢复为 source app 路径并成功运行 `GoldenSimpleLib.App.dll`
  - summary: [summary.json](/D:/agent/booming-il2cpp/artifacts/subjects/SolutionCorePack/runs/20260418-105927-windows-0ea8/run-report/summary.json)
  - events: [events.jsonl](/D:/agent/booming-il2cpp/artifacts/subjects/SolutionCorePack/runs/20260418-105927-windows-0ea8/run-report/events.jsonl)
  - console: [console.log](/D:/agent/booming-il2cpp/artifacts/subjects/SolutionCorePack/runs/20260418-105927-windows-0ea8/run-report/console.log)
- `semantic-family-execution-entry-v1-01.json` 首批 canonical commands 全量实跑
  - 结果: `20260418-110105-windows-c5c3` 到 `20260418-111909-windows-aea9` 共 `27` 个 run 全部 `finalStatus = ok`
  - 证据目录规则: 每个 run 均产出 `artifacts/subjects/SolutionCorePack/runs/<runId>/run-report/{summary.json,events.jsonl,console.log}`
  - 覆盖对象:
    - proof: `ArithmeticOpsProofEntry`、`AsyncAwaitProofEntry`、`CustomAttributeLookupProofEntry`、`DelegateProofEntry`、`ExceptionProofEntry`、`InterfaceDispatchProofEntry`、`MemberMetadataLookupProofEntry`、`MetadataSupplementProofEntry`、`MonitorAndLockingProofEntry`、`ReflectionInteropClosureEntry`、`RuntimeHelpersProofEntry`、`TaskAndValueTaskFlowProofEntry`、`TaskSchedulingProofEntry`、`ThreadingProofEntry`
    - conformance: `managed-build`、`managed-runtime-output`、`native-executable-smoke`、`project-graph`
    - engineering workload: `codegen`、`convert`、`native-link`
    - benchmark: `ArithmeticBenchmarkEntry`、`DispatchBenchmarkEntry`、`MemberMetadataLookupBenchmarkEntry`、`MonitorAndLockingBenchmarkEntry`、`TaskAndValueTaskFlowBenchmarkEntry`、`TaskSchedulingBenchmarkEntry`
- 说明:
  - 本轮验证期间未出现 `dotnet build` / `dotnet test` / `msbuild` 崩溃
  - 所有 real-command 验证均返回 `exitCode = 0`

## 风险 / 余留

- 本任务完成的是 `System.Private.CoreLib` 首批 semantic-family execution entry 与 representative proof/benchmark 闭环，不等于整个 foundation-pack roadmap 已完成。
- 下一阶段仍需把已稳定的 corelib family 能力继续向 dependency-driven 核心 `System.*` DLL 分层扩展，再进入复杂 BCL、外部 DLL、hotupdate consumer 与 `.NET 10` delta closure。
- 现有 task_id / 目录中保留历史 `phase-*` 命名；后续新增任务应继续使用语义化命名，不再扩散 `Phase{x}` 风格。

## 下一步

- 回到父 roadmap，激活 dependency-layer child task，开始推进核心 `System.*` DLL 的依赖驱动分层 nativeization。

## wiki

- 本轮不写入 wiki
- 原因: 本轮新增的是 execution entry 实跑结果与阶段化 host 路由修复，仍属于当前 roadmap 主线内部的执行经验；待 dependency-layer 稳定后再决定是否上升为长期 authority
