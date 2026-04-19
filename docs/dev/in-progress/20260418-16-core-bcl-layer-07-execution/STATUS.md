---
task_id: 20260418-16-core-bcl-layer-07-execution
title: Core BCL Layer 07 Execution
task_type: plan
lifecycle_status: completed
phase: completed
created_at: 2026-04-19 00:54:00 +08:00
updated_at: 2026-04-19 10:00:00 +08:00
latest_stop_point: certified `core-bcl-layer-07` at `5/5 finalStatus = ok`
current_dir: docs/dev/in-progress/20260418-16-core-bcl-layer-07-execution
parent_task_id: 20260418-01-dotnet-foundation-pack-nativeization-roadmap
source_task_id: 20260418-01-dotnet-foundation-pack-nativeization-roadmap
source_relation: roadmap-child-core-bcl-layer-07-execution
active: false
---

## 2026-04-19 10:00 Closeout

- 已解锁并认证 `core-bcl-layer-07`。
- 当前 layer 覆盖 `5` 个程序集：`System.Net.Http.Json`、`System.Net.Mail`、`System.Net.Requests`、`System.Net.WebClient`、`System.Net.WebSockets.Client`。
- 已生成 `assembly-certification-report/*.json` 与 `layer-certification-summary-v1-01.json`。
- 收口结果: `5/5 finalStatus = ok`。
- 累计 certified 输入已去重为 `84` 个唯一程序集；原始 summary 输入中 `System.Net.Requests`、`System.Reflection.Emit`、`System.Text.Json` 重复出现。

## 下一步

- 使用 `cumulative-certified-assemblies-v1-01.json` 解锁下一层 dependency-layer。

## 风险 / 阻塞

### risks

- 当前 layer 继续复用既有 union proof / benchmark evidence 投影，属于共享证据投影，不等同于每层重新执行全部 native proof / benchmark。
- 当前 child 不包含 hotupdate consumer 验证；`managed -> native -> hotupdate` 全链路仍需后续独立 lane 验证。

### blockers

- 当前 blocker: `none`

## 验证

- `dotnet run --project src/tools/Chaos.IL2CPP.Tools.FoundationPack.Analysis/Chaos.IL2CPP.Tools.FoundationPack.Analysis.csproj -- dependency-layer-summarize --source-scope docs/dev/in-progress/20260418-16-core-bcl-layer-07-execution/core-bcl-layer-07-scope-v1-01.json --proof-summary docs/dev/in-progress/20260418-16-core-bcl-layer-07-execution/native-proof-summary-v1-01.json --benchmark-summary docs/dev/in-progress/20260418-16-core-bcl-layer-07-execution/nativeization-throughput-benchmark-v1-01.json --report-dir docs/dev/in-progress/20260418-16-core-bcl-layer-07-execution/assembly-certification-report --output-path docs/dev/in-progress/20260418-16-core-bcl-layer-07-execution/layer-certification-summary-v1-01.json --task-id 20260418-16-core-bcl-layer-07-execution`
  - 结果: 生成 summary，`layerStatus.finalStatus = ok`

## wiki

- 当前不写入 wiki。
- 原因: 本轮记录为 roadmap/task 级执行收口，不是新的长期 authority 规则。
