---
task_id: 20260418-01-dotnet-foundation-pack-nativeization-roadmap
title: DotNet Foundation Pack Nativeization Roadmap
task_type: roadmap
lifecycle_status: completed
phase: completed
created_at: 2026-04-18 01:19:47 +08:00
updated_at: 2026-04-19 10:22:00 +08:00
latest_stop_point: completed dependency-layer terminal certification, HotUpdate consumer validation, and .NET 10 delta closure
current_dir: docs/dev/in-progress/20260418-01-dotnet-foundation-pack-nativeization-roadmap
parent_task_id:
source_task_id: 20260416-01-aot-core-ir-gap-audit-and-next-expansion-roadmap
source_relation: architectural-redesign-from-entry-driven-aot-to-framework-pack-nativeization
active: false
---

## 2026-04-19 10:22 Closeout

- `dependency-layer` 主线已推进到终态：`core-bcl-layer-01` 到 `core-bcl-layer-07` 的 layer summary 全部 `finalStatus = ok`。
- `20260418-17-core-bcl-layer-08-execution` 作为终态检查确认：
  - `net10.0`: `7` 层全部 `certified`，`firstReadyLayerName = null`
  - `net8.0`: `8` 层全部 `certified`，`firstReadyLayerName = null`
  - `assembly-nativeization-plan` 为空，无剩余 ready DLL。
- `20260418-08-hotupdate-consumer-validation` 已完成：
  - `HotUpdateHostPack` declared proof: `7/7 ok`
  - managed consumer benchmark: `4/4 ok`
  - engineering validation / workload: `8/8 ok`
- `20260418-09-dotnet10-delta-closure-and-release-gates` 已完成：
  - `.NET 10` managed ref delta `4/4` certified
  - `net10.0.firstReadyLayerName = null`

## 关键修复

- `dependency-layer` generator 支持 `--certified-assemblies-path`，并能消费 `certifiedAssemblies` 或 `certificationReports[finalStatus=ok]`。
- `dependency-layer` generator 现在能处理全候选集已认证的终态，不再在无 `ready` layer 时抛 `Sequence contains no matching element`。
- 新增 `dependency-layer-summarize` 命令，统一从 per-DLL report 生成 layer summary。
- `PatchCallbackFlowProofEntry` 与 `HotUpdateDispatchBenchmarkEntry` 已修复为真实 `LoadPackage -> RuntimeMode.Mixed -> DispatchInt32Unary` 路径，避免只测 fallback。
- `ldftn` MSVC overloaded function ambiguity 修复仍在 `NativeAotLoweringPlanner.MethodEmission.cs` 中生效。

## 边界说明

- 当前可以声明 `managed -> native -> hotupdate` 相关 roadmap lane 已分别验证：managed/native foundation DLL certification 通过 per-DLL/layer summary 证明，hotupdate consumer lane 通过 `HotUpdateHostPack` proof/benchmark/engineering gate 证明。
- HotUpdate consumer benchmark 是 managed consumer profile，不是 native benchmark proof。
- layer-02 之后的 certification 多数复用 union proof / benchmark evidence 投影；这是当前 `dependency-layer-certify` 语义下的共享证据投影，不等同于每层重新执行全部 native proof / benchmark。

## 验证

- JSON parse:
  - `docs/dev/in-progress/20260418-*/**/*.json`
  - 结果: `199` 个 JSON 可解析，`0` 失败
- layer summary:
  - `7` 个 `layer-certification-summary-v1-01.json`
  - 结果: 全部 `finalStatus = ok` 且 `total = ok`
- `dotnet build src/tools/Chaos.IL2CPP.Tools.FoundationPack.Analysis/Chaos.IL2CPP.Tools.FoundationPack.Analysis.csproj -c Release`
  - 结果: build succeeded
- `dotnet build src/managed/Chaos.IL2CPP.CodeGen/Chaos.IL2CPP.CodeGen.csproj -c Release`
  - 结果: build succeeded
- `dotnet build subjects/HotUpdateHostPack/source/HotUpdateHostPack.csproj -c Release -m:1`
  - 结果: build succeeded
- `python -m pytest tests/tooling/foundation/test_dependency_layer_plan.py tests/tooling/foundation/test_dependency_layer_certification_report.py tests/unit/compatibility/test_hot_update_e2e_flows.py::Phase9HotUpdateE2ETests::test_patch_callback_flow_and_dispatch_benchmark_use_loaded_mixed_runtime_path -q`
  - 结果: `7 passed`
- `python build/toolchains/run/run.py test registry refresh --json`
  - 结果: `status = ok`, `historyPath = artifacts/tests/registry/history/20260419-102109/index.json`

## wiki

- 当前不写入 wiki。
- 原因: 本轮主要是 roadmap 执行、工具终态健壮性和正式验证对象修复；长期 authority 未新增。
