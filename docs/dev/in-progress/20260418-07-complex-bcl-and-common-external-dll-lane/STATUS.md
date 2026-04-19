---
task_id: 20260418-07-complex-bcl-and-common-external-dll-lane
title: Complex BCL And Common External DLL Lane
task_type: plan
lifecycle_status: completed
phase: completed
created_at: 2026-04-19 00:38:00 +08:00
updated_at: 2026-04-19 00:45:00 +08:00
latest_stop_point: certified `core-bcl-layer-02` at `13/13 finalStatus = ok` and generated layer certification summary
current_dir: docs/dev/in-progress/20260418-07-complex-bcl-and-common-external-dll-lane
parent_task_id: 20260418-01-dotnet-foundation-pack-nativeization-roadmap
source_task_id: 20260418-01-dotnet-foundation-pack-nativeization-roadmap
source_relation: roadmap-child-complex-bcl-and-common-external-dll-lane
design_doc: docs/dev/in-progress/20260418-07-complex-bcl-and-common-external-dll-lane/design-v1-01.md
plan_doc: docs/dev/in-progress/20260418-07-complex-bcl-and-common-external-dll-lane/plan-v1-01.md
total_tasks: 6
current_task: completed
active: false
---

## 2026-04-19 00:38 Start Point

- 已让 `dependency-layer` generator 支持 `--certified-assemblies-path`，用 `20260418-11` 的 `layer-certification-summary-v1-01.json` 作为已认证输入。
- 已重新生成当前 child artifacts：
  - `layer-plan-v1-01.json`
  - `proof-benchmark-lane-v1-01.json`
  - `assembly-nativeization-plan/*.json`
- `net8.0` 与 `net10.0` 的 `firstReadyLayerName` 均已推进为 `core-bcl-layer-02`。
- 合并后的当前 lane 程序集共 `13` 个：
  - `System.Collections.Immutable`
  - `System.Formats.Asn1`
  - `System.IO.Compression.Brotli`
  - `System.IO.Compression.ZipFile`
  - `System.IO.Pipelines`
  - `System.Linq`
  - `System.Net.ServerSentEvents`
  - `System.ObjectModel`
  - `System.Runtime.InteropServices`
  - `System.Runtime.Serialization.Formatters`
  - `System.Security.Principal.Windows`
  - `System.Text.Json`
  - `System.Threading.Tasks.Parallel`

## 当前结论

- `core-bcl-layer-01` 的 `29/29 ok` 结果已经能被后续 layer generator 消费。
- `core-bcl-layer-02` 已完成 proof / benchmark / per-DLL certification closeout。
- 当前 child 收口结果: `13/13 finalStatus = ok`
- 当前 child 仍不包含 hotupdate consumer 验证；hotupdate lane 是后续独立任务。

## 下一步

- 回到父 roadmap，继续使用 `core-bcl-layer-01 + core-bcl-layer-02` 的认证 summary 解锁下一层。

## 风险 / 阻塞

### risks

- `System.Text.Json`、`System.Runtime.InteropServices`、`System.Runtime.Serialization.Formatters` 的 capability 面更复杂，可能暴露 reflection、metadata、interop 或 serialization substrate 缺口。
- `net10.0` 比 `net8.0` 多出 `System.IO.Pipelines` 与 `System.Net.ServerSentEvents`，需要在 report 中保留跨版本差异。

### blockers

- 当前 blocker: `none`

## 验证

- `dotnet build src/tools/Chaos.IL2CPP.Tools.FoundationPack.Analysis/Chaos.IL2CPP.Tools.FoundationPack.Analysis.csproj -c Release`
  - 结果: 通过
- `python -m pytest tests/tooling/foundation/test_dependency_layer_plan.py -q`
  - 结果: `3 passed`
- `dotnet run --project src/tools/Chaos.IL2CPP.Tools.FoundationPack.Analysis/Chaos.IL2CPP.Tools.FoundationPack.Analysis.csproj -- dependency-layer --catalog docs/dev/completed/20260418-02-phase-0-framework-pack-catalog-and-asset-policy/framework-catalog-v1-01.json --phase1-dir docs/dev/completed/20260418-03-phase-1-contract-facade-and-shim-certification-lane --phase2-dir docs/dev/completed/20260418-04-phase-2-system-private-corelib-certification-substrate --phase3-dir docs/dev/completed/20260418-05-phase-3-system-private-corelib-semantic-family-nativeization --output-dir docs/dev/in-progress/20260418-07-complex-bcl-and-common-external-dll-lane --task-id 20260418-07-complex-bcl-and-common-external-dll-lane --certified-assemblies-path docs/dev/in-progress/20260418-11-core-bcl-layer-01-remaining-ready-dll-execution/layer-certification-summary-v1-01.json`
  - 结果: 通过
- `dotnet run --project src/tools/Chaos.IL2CPP.Tools.FoundationPack.Analysis/Chaos.IL2CPP.Tools.FoundationPack.Analysis.csproj -- dependency-layer-certify --assembly-plan-dir docs/dev/in-progress/20260418-07-complex-bcl-and-common-external-dll-lane/assembly-nativeization-plan --proof-summary docs/dev/in-progress/20260418-07-complex-bcl-and-common-external-dll-lane/native-proof-summary-v1-01.json --benchmark-summary docs/dev/in-progress/20260418-07-complex-bcl-and-common-external-dll-lane/nativeization-throughput-benchmark-v1-01.json --output-dir docs/dev/in-progress/20260418-07-complex-bcl-and-common-external-dll-lane/assembly-certification-report --task-id 20260418-07-complex-bcl-and-common-external-dll-lane`
  - 结果: 通过，`13/13 finalStatus = ok`

## wiki

- 当前不写入 wiki
- 原因: 先在当前 child 内验证 layer advancement 机制与 layer-02 执行闭环，再决定是否沉淀长期规则
