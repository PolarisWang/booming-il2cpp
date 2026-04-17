# Phase 0 Framework Pack Catalog And Asset Policy 实现计划 v1.01

> **面向执行 Agent：** 必须使用 `dev:executing-plans` 执行本计划。步骤使用复选框（`- [ ]`）语法跟踪。

**目标：** 启动 `20260418-01` 的 `Phase 0`，冻结 `.NET 8/.NET 10` foundation pack 资产策略，并落第一版 catalog / diff / monitor 产物。

**架构：** 先把 Phase 0 收成“事实基线 + 策略决策 + 机器可读目录”三件套，再把 `.NET 10` 缺 snapshot 的事实显式记录为 blocker。第一轮不进入 facade/shim classification，也不修改 subject/runner 主线。

**技术栈：** Markdown、JSON、PowerShell、`dotnet` CLI

**AOT/IL2CPP/Test Governance Intake：**
- capabilityFamily: `FrameworkPackNativeization`
- capabilityItem: `Phase0CatalogAndAssetPolicy`
- ownerSubjectId: `DotNetFoundationPack (planned)`
- proofRequired: `true`
- benchmarkRequired: `false`
- hotupdateImpact: `None`
- formalVerificationObjects:
  - `docs/dev/in-progress/20260418-02-phase-0-framework-pack-catalog-and-asset-policy/asset-source-policy-v1-01.md`
  - `docs/dev/in-progress/20260418-02-phase-0-framework-pack-catalog-and-asset-policy/framework-catalog-v1-01.json`
  - `docs/dev/in-progress/20260418-02-phase-0-framework-pack-catalog-and-asset-policy/pack-diff-summary-v1-01.json`
  - `docs/dev/in-progress/20260418-02-phase-0-framework-pack-catalog-and-asset-policy/phase-0-native-proof-and-benchmark-matrix-v1-01.md`
- requiredGates: `n/a (Phase 0 catalog/policy task does not traverse collector -> registry -> workspace yet)`

**设计文档：** `docs/dev/in-progress/20260418-02-phase-0-framework-pack-catalog-and-asset-policy/design-v1-01.md`

**预期知识沉淀：** 按任务决定

**收尾约束：** 执行完成后必须进入“审视架构合理性 -> 测试通过 -> 归档 completed -> 合并&提交”固定链路。

---

Status: in-progress

## 计划清单

- [x] 步骤 1：建立 child task 执行骨架，并把 `ACTIVE.md` 切到 `20260418-02`
- [x] 步骤 2：审计本机 `.NET 8` / `.NET 9` / `.NET 10` foundation pack 可见性，冻结 `.NET 8` 选定版本与 `.NET 10` source policy
- [x] 步骤 3：产出 `framework-catalog-v1-01.json` 与 `pack-diff-summary-v1-01.json`
- [x] 步骤 4：产出 `asset-source-policy-v1-01.md` 与 `phase-0-native-proof-and-benchmark-matrix-v1-01.md`
- [x] 步骤 5：运行针对性校验，并同步 child / parent `STATUS.md`、`docs/dev/ACTIVE.md`、progress note 与索引

## 需要创建或修改的文件

- `docs/dev/in-progress/20260418-02-phase-0-framework-pack-catalog-and-asset-policy/STATUS.md`
- `docs/dev/in-progress/20260418-02-phase-0-framework-pack-catalog-and-asset-policy/design-v1-01.md`
- `docs/dev/in-progress/20260418-02-phase-0-framework-pack-catalog-and-asset-policy/plan-v1-01.md`
- `docs/dev/in-progress/20260418-02-phase-0-framework-pack-catalog-and-asset-policy/asset-source-policy-v1-01.md`
- `docs/dev/in-progress/20260418-02-phase-0-framework-pack-catalog-and-asset-policy/framework-catalog-v1-01.json`
- `docs/dev/in-progress/20260418-02-phase-0-framework-pack-catalog-and-asset-policy/pack-diff-summary-v1-01.json`
- `docs/dev/in-progress/20260418-02-phase-0-framework-pack-catalog-and-asset-policy/phase-0-native-proof-and-benchmark-matrix-v1-01.md`
- `docs/dev/in-progress/20260418-02-phase-0-framework-pack-catalog-and-asset-policy/notes/progress-v1-01.md`
- `docs/dev/ACTIVE.md`
- `docs/dev/INDEX.md`
- `docs/dev/in-progress/INDEX.md`
- `docs/dev/in-progress/20260418-01-dotnet-foundation-pack-nativeization-roadmap/STATUS.md`
- `docs/dev/in-progress/20260418-01-dotnet-foundation-pack-nativeization-roadmap/roadmap-v1-01.md`

## 验证命令

- `dotnet --list-sdks`
- `dotnet --list-runtimes`
- `Get-ChildItem "$env:ProgramFiles\\dotnet\\packs\\Microsoft.NETCore.App.Ref" -Directory | Sort-Object Name`
- `Get-ChildItem "$env:ProgramFiles\\dotnet\\shared\\Microsoft.NETCore.App" -Directory | Sort-Object Name`
- `Get-Content docs/dev/in-progress/20260418-02-phase-0-framework-pack-catalog-and-asset-policy/framework-catalog-v1-01.json | ConvertFrom-Json | Out-Null`
- `Get-Content docs/dev/in-progress/20260418-02-phase-0-framework-pack-catalog-and-asset-policy/pack-diff-summary-v1-01.json | ConvertFrom-Json | Out-Null`

## 状态维护要求

- 执行中由 `docs/dev/ACTIVE.md` 指向本 child task
- 父 roadmap 保持 `in_progress`，但 `active` 切为 `false`
- 父 roadmap 的 `20260418-02` 子任务状态更新为 `in_progress`
- 每完成一个步骤，都同步更新 child / parent `STATUS.md`、`docs/dev/ACTIVE.md`、`notes/progress-v1-01.md` 与索引
- 如果 `.NET 10` snapshot 仍未进入仓库，本任务保持 `in_progress`，不得提前归档 completed
- 即使本轮 checklist 已执行完，只要 `.NET 10` snapshot 资产未入仓，本任务仍保持 `in_progress`
