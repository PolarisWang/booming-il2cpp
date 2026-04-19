# 进展记录 v1.01

## 时间

- 2026-04-18 14:56:35 +08:00
- 2026-04-18 19:41:19 +08:00
- 2026-04-18 22:47:46 +08:00
- 2026-04-18 23:16:14 +08:00
- 2026-04-19 00:18:00 +08:00

## 本轮完成

- 冻结 `remaining-ready-scope-v1-01.json`，明确剩余 `21` 个 ready DLL、`gameplay-core-ready` 里程碑与 formal verification intake。
- 补齐并执行当前 child 所需 proof / benchmark / engineering validation，修复 `function-pointer-bench` 暴露出的 native codegen `ldftn` 重载取址问题。
- 明确 `TaskAndValueTaskFlowBenchmarkEntry::RunWorkload()` 为 managed-only benchmark，不把它当作 native benchmark 通过依据。
- 生成当前 child 的 proof / benchmark machine-readable summary：
  - `native-proof-summary-v1-01.json`
  - `nativeization-throughput-benchmark-v1-01.json`
- 首轮把 remaining-ready `21` 个 DLL 投影为 `assembly-certification-report/*.json`。
- 发现 baseline `8` 个 DLL 仍依赖代表批次中的 `generic/threading` 旧证据后，合并 `20260418-10` 的缺失 proof / benchmark，生成 layer-ready union summary：
  - `layer-ready-proof-summary-v1-01.json`
  - `layer-ready-throughput-benchmark-v1-01.json`
- 使用 union summary 重跑全量 certify，完成 `29` 个 ready DLL 的逐 DLL report 收口，并生成 `layer-certification-summary-v1-01.json`。

## 收口结果

- `gameplay-core-ready`: `15/15 finalStatus = ok`
- `core-bcl-layer-01` ready set: `29/29 finalStatus = ok`
- 当前 child 已完成，可从父 roadmap 视角把 `dependency-layer` 阶段标记为已收口

## 验证

- `python build/toolchains/run/run.py test registry refresh --json`
  - 结果: `status = ok`, `historyPath = artifacts/tests/registry/history/20260419-000228/index.json`
- `dotnet run --project src/tools/Chaos.IL2CPP.Tools.FoundationPack.Analysis/Chaos.IL2CPP.Tools.FoundationPack.Analysis.csproj -- dependency-layer-certify --assembly-plan-dir docs/dev/completed/20260418-06-dependency-driven-core-bcl-layer-nativeization/assembly-nativeization-plan --proof-summary docs/dev/in-progress/20260418-11-core-bcl-layer-01-remaining-ready-dll-execution/layer-ready-proof-summary-v1-01.json --benchmark-summary docs/dev/in-progress/20260418-11-core-bcl-layer-01-remaining-ready-dll-execution/layer-ready-throughput-benchmark-v1-01.json --output-dir docs/dev/in-progress/20260418-11-core-bcl-layer-01-remaining-ready-dll-execution/assembly-certification-report --task-id 20260418-11-core-bcl-layer-01-remaining-ready-dll-execution`
  - 结果: 通过
- `Get-ChildItem docs/dev/in-progress/20260418-11-core-bcl-layer-01-remaining-ready-dll-execution/assembly-certification-report/*.json | ForEach-Object { Get-Content $_.FullName -Raw | ConvertFrom-Json | Out-Null }`
  - 结果: `29` 份 report 全部可解析
- `Get-Content docs/dev/in-progress/20260418-11-core-bcl-layer-01-remaining-ready-dll-execution/layer-certification-summary-v1-01.json -Raw | ConvertFrom-Json | Out-Null`
  - 结果: 通过

## 说明

- 本轮没有验证 `managed -> native -> hotupdate` 全链路。
- 本轮没有修改长期 wiki authority；新增内容仅是当前 child 的执行证据与收口结果。
