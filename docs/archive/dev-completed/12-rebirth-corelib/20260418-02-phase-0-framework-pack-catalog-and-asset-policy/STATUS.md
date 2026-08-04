---
task_id: 20260418-02-phase-0-framework-pack-catalog-and-asset-policy
title: Phase 0 Framework Pack Catalog And Asset Policy
task_type: plan
lifecycle_status: completed
phase: completed
created_at: 2026-04-18 01:50:00 +08:00
updated_at: 2026-04-18 02:09:22 +08:00
latest_stop_point: Phase 0 completed after importing the net10 10.0.6 repo-frozen snapshot, regenerating catalog/diff artifacts, and passing architecture review plus formal verification
current_dir: docs/dev/completed/20260418-02-phase-0-framework-pack-catalog-and-asset-policy
parent_task_id: 20260418-01-dotnet-foundation-pack-nativeization-roadmap
source_task_id: 20260418-01-dotnet-foundation-pack-nativeization-roadmap
source_relation: roadmap-child-phase-0
design_doc: docs/dev/completed/20260418-02-phase-0-framework-pack-catalog-and-asset-policy/design-v1-01.md
plan_doc: docs/dev/completed/20260418-02-phase-0-framework-pack-catalog-and-asset-policy/plan-v1-01.md
total_tasks: 5
current_task: completed
active: false
---

## 完成结论

- 已把 `.NET 8` foundation baseline 固定为本机官方安装 `8.0.11`。
- 已把 `.NET 10` foundation baseline 固定为仓库冻结快照 `10.0.6`，目录为：
  - `assets/framework-packs/dotnet-foundation/net10/ref`
  - `assets/framework-packs/dotnet-foundation/net10/runtime`
- 已确认 `.NET 10` 快照来自本机官方安装：
  - `C:\Program Files\dotnet\packs\Microsoft.NETCore.App.Ref\10.0.6\ref\net10.0`
  - `C:\Program Files\dotnet\shared\Microsoft.NETCore.App\10.0.6`
- `framework-catalog-v1-01.json` 与 `pack-diff-summary-v1-01.json` 已反映 `net8` / `net10` 双版本 present 状态。
- `Phase 0` 的 formal verification objects 已齐备且通过当前回合核验，可正式归档 `completed`。

## 本轮收口

- 更新 `asset-source-policy-v1-01.md`，把 `.NET 10` 从 `pending-snapshot-import` 收口为已锁定的 `10.0.6` repo-frozen snapshot。
- 保留 `framework-catalog-v1-01.json` 与 `pack-diff-summary-v1-01.json` 作为 Phase 0 machine-readable authority。
- 追加最终 progress note，记录 `.NET 10` 导入、验证与归档依据。

## 架构审视

- `Phase 0` 只负责 source policy、catalog、diff 与 proof matrix，没有把 facade/shim/classification 语义提前混入 catalog，职责边界合理。
- `.NET 8` 继续保留 `machine-global-official-installation`，`.NET 10` 显式固定为 `repo-frozen-snapshot`，两种 source model 没有被错误合并，后续 `FrameworkPackResolver` 契约保持清晰。
- 本阶段不触碰 `System.Private.CoreLib` lowering、consumer validation 或 hotupdate 路径，避免了 Phase 1/2 的职责串层。

## 已验证

- `dotnet --list-sdks`
  - 结果：`8.0.402`、`9.0.101`、`10.0.202`
- `dotnet --list-runtimes`
  - 结果：本机存在 `Microsoft.NETCore.App 8.0.8`、`8.0.11`、`9.0.0`、`10.0.6`
- `Get-ChildItem "$env:ProgramFiles\\dotnet\\packs\\Microsoft.NETCore.App.Ref" -Directory | Sort-Object Name | Select-Object -ExpandProperty Name`
  - 结果：可见 `10.0.6`、`8.0.11`、`8.0.8`、`9.0.0`
- `Get-ChildItem "$env:ProgramFiles\\dotnet\\shared\\Microsoft.NETCore.App" -Directory | Sort-Object Name | Select-Object -ExpandProperty Name`
  - 结果：可见 `10.0.6`、`8.0.11`、`8.0.8`、`9.0.0`
- `(Get-ChildItem assets/framework-packs/dotnet-foundation/net10/ref -File -Filter *.dll | Measure-Object).Count`
  - 结果：`167`
- `(Get-ChildItem assets/framework-packs/dotnet-foundation/net10/runtime -File -Filter *.dll | Measure-Object).Count`
  - 结果：`185`
- `Get-Content docs/dev/completed/20260418-02-phase-0-framework-pack-catalog-and-asset-policy/framework-catalog-v1-01.json | ConvertFrom-Json | Out-Null; Write-Output OK`
  - 结果：`OK`
- `Get-Content docs/dev/completed/20260418-02-phase-0-framework-pack-catalog-and-asset-policy/pack-diff-summary-v1-01.json | ConvertFrom-Json | Out-Null; Write-Output OK`
  - 结果：`OK`
- 说明：本阶段仅涉及资产快照、文档与 JSON 产物核验，不需要执行 `dotnet build` / `dotnet test` / `msbuild`。因此本轮不存在编译崩溃排查链路。

## 风险 / 余留

- `Phase 1` 仍需冻结 facade/shim candidate assemblies、canonical owner 映射与 `contract-complete` gate。
- `framework-catalog-v1-01.json` 目前只承担资产发现 authority，不承担 classification 语义；这部分必须在 `20260418-03` 收口。

## 下一步

- 切入 `20260418-03-phase-1-contract-facade-and-shim-certification-lane`
- 聚焦：
  - 基于 `Phase 0` catalog 派生 facade/shim lane candidate assemblies
  - 冻结 `contract-facade` / `compat-shim` / `implementation` / `runtime-backed` 分类规则
  - 建立 public surface 与 native-binding proof contract

## wiki

- 本阶段暂不直接写入 wiki。
- 原因：`framework-pack` source policy 已稳定，但 classification / certification authority 还要在 `Phase 1` 收口后一起沉淀。
