---
task_id: 20260418-02-phase-0-framework-pack-catalog-and-asset-policy
title: Phase 0 Framework Pack Catalog And Asset Policy
task_type: plan
lifecycle_status: in_progress
phase: executing
created_at: 2026-04-18 01:50:00 +08:00
updated_at: 2026-04-18 01:56:05 +08:00
latest_stop_point: phase-0 baseline artifacts generated; net8 8.0.11 selected, net10 kept blocked on missing repo-frozen snapshot
current_dir: docs/dev/in-progress/20260418-02-phase-0-framework-pack-catalog-and-asset-policy
parent_task_id: 20260418-01-dotnet-foundation-pack-nativeization-roadmap
source_task_id: 20260418-01-dotnet-foundation-pack-nativeization-roadmap
source_relation: roadmap-child-phase-0
design_doc: docs/dev/in-progress/20260418-02-phase-0-framework-pack-catalog-and-asset-policy/design-v1-01.md
plan_doc: docs/dev/in-progress/20260418-02-phase-0-framework-pack-catalog-and-asset-policy/plan-v1-01.md
total_tasks: 5
current_task: step-5-sync-status-and-keep-phase0-blocked-on-net10-snapshot
active: true
---

## 关键文档

- parent_status: `docs/dev/in-progress/20260418-01-dotnet-foundation-pack-nativeization-roadmap/STATUS.md`
- parent_roadmap: `docs/dev/in-progress/20260418-01-dotnet-foundation-pack-nativeization-roadmap/roadmap-v1-01.md`
- design: `docs/dev/in-progress/20260418-02-phase-0-framework-pack-catalog-and-asset-policy/design-v1-01.md`
- plan: `docs/dev/in-progress/20260418-02-phase-0-framework-pack-catalog-and-asset-policy/plan-v1-01.md`
- asset_source_policy: `docs/dev/in-progress/20260418-02-phase-0-framework-pack-catalog-and-asset-policy/asset-source-policy-v1-01.md`
- framework_catalog: `docs/dev/in-progress/20260418-02-phase-0-framework-pack-catalog-and-asset-policy/framework-catalog-v1-01.json`
- pack_diff_summary: `docs/dev/in-progress/20260418-02-phase-0-framework-pack-catalog-and-asset-policy/pack-diff-summary-v1-01.json`
- proof_matrix: `docs/dev/in-progress/20260418-02-phase-0-framework-pack-catalog-and-asset-policy/phase-0-native-proof-and-benchmark-matrix-v1-01.md`

## 当前判断

- current_focus: 已生成 `Phase 0` 首批 source policy / catalog / diff / proof-matrix 产物，当前保持任务为 `in_progress`，等待 `.NET 10` snapshot 冻结后再完成 Phase 0。
- why_now: 用户已要求正式启动 `20260418-01`；父 roadmap 的第一批 ready child 就是 `20260418-02`。
- done_definition: 只有当 `.NET 8` 基线、`.NET 10` 资产策略、catalog/diff/matrix 产物都落盘并通过基本校验，且 `.NET 10` snapshot 已进入仓库或 source model 被用户改写后，才允许进入后续 Phase 1。

## 最近摘要

- 2026-04-18 01:56:05 +08:00: 已生成 `asset-source-policy-v1-01.md`、`framework-catalog-v1-01.json`、`pack-diff-summary-v1-01.json` 与 `phase-0-native-proof-and-benchmark-matrix-v1-01.md`。
- 2026-04-18 01:56:05 +08:00: 已将 `.NET 8` 开发基线固定为本机官方安装 `8.0.11`；`.NET 10` 固定为 `assets/framework-packs/dotnet-foundation/net10/{ref,runtime}` repo-frozen snapshot 策略，但当前仍缺资产。
- 2026-04-18 01:50:00 +08:00: 已从父 roadmap 派生 `20260418-02`，并把任务切到执行态。

## 下一步

- next_action: 决定并导入 `.NET 10` repo-frozen snapshot，或由用户改写 source model；在此之前继续保持 `Phase 0` block 状态，不进入 `Phase 1`。
- owner: codex
- trigger: 用户要求继续执行 foundation-pack nativeization 主线。

## 风险 / 阻塞

### risks

- 如果 Phase 0 不把 `.NET 8` 与 `.NET 10` 的 source model 区分开，后续 resolver contract 会失真。
- 如果 catalog 过早混入 classification 语义，Phase 1 会发生返工。
- `framework-catalog-v1-01.json` 当前记录的是 `.NET 8` 真实基线与 `.NET 10` 缺口状态，不能被误读成“已具备双版本可比性”。

### blockers

- 当前 blocker：仓库内尚不存在 `.NET 10` foundation snapshot。
- 说明：本阶段已经冻结策略并产出 `.NET 8` catalog baseline，但在 `.NET 10` snapshot 到位前不得 completed。

## 验证

- `dotnet --list-sdks`
  - 结果：`8.0.402`、`9.0.101`
- `dotnet --list-runtimes`
  - 结果：本机有 `Microsoft.NETCore.App 8.0.8`、`8.0.11`、`9.0.0`，无 `.NET 10`
- `Get-ChildItem "$env:ProgramFiles\\dotnet\\packs\\Microsoft.NETCore.App.Ref" -Directory | Sort-Object Name`
  - 结果：`8.0.8`、`8.0.11`、`9.0.0`
- `Get-ChildItem "$env:ProgramFiles\\dotnet\\shared\\Microsoft.NETCore.App" -Directory | Sort-Object Name`
  - 结果：`8.0.8`、`8.0.11`、`9.0.0`
- `Get-Content docs/dev/in-progress/20260418-02-phase-0-framework-pack-catalog-and-asset-policy/framework-catalog-v1-01.json | ConvertFrom-Json | Out-Null`
  - 结果：`OK`
- `Get-Content docs/dev/in-progress/20260418-02-phase-0-framework-pack-catalog-and-asset-policy/pack-diff-summary-v1-01.json | ConvertFrom-Json | Out-Null`
  - 结果：`OK`

## wiki

- 本阶段暂不写 wiki；待 source policy 与 catalog contract 在后续 phase 中稳定后，再决定是否沉淀。
