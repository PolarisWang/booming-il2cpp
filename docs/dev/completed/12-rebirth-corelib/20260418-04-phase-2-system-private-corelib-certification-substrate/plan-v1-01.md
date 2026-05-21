# Phase 2 System.Private.CoreLib Certification Substrate 实现计划 v1.01

> **面向执行 Agent：** 必须使用 `dev:executing-plans` 执行本计划。步骤使用复选框（`- [ ]`）语法跟踪。  
> **目标：** 固化 `System.Private.CoreLib` 的 public surface、semantic ledger、runtime helper contracts 与 proof/benchmark substrate，为 `Phase 3` 的 per-family nativeization 建立可信底座。  
> **架构：** 复用 `Phase 0` catalog 与 `Phase 1` classification authority，扩展 `Chaos.FoundationPack.Analysis` 的 `phase2` 命令来导出 corelib machine-readable artifacts。  
> **技术栈：** Markdown、JSON、PowerShell、`.NET` metadata analysis、IL feature inventory

**AOT/IL2CPP/Test Governance Intake：**
- capabilityFamily: `FrameworkPackNativeization`
- capabilityItem: `Phase2SystemPrivateCoreLibCertificationSubstrate`
- ownerSubjectId: `DotNetFoundationPack (planned)`
- proofRequired: `true`
- benchmarkRequired: `true`
- hotupdateImpact: `None`
- formalVerificationObjects:
  - `docs/dev/completed/20260418-04-phase-2-system-private-corelib-certification-substrate/assembly-semantic-ir-system-private-corelib-v1-01.json`
  - `docs/dev/completed/20260418-04-phase-2-system-private-corelib-certification-substrate/runtime-helper-contracts-v1-01.json`
  - `docs/dev/completed/20260418-04-phase-2-system-private-corelib-certification-substrate/corelib-surface-proof-report-v1-01.json`
  - `docs/dev/completed/20260418-04-phase-2-system-private-corelib-certification-substrate/corelib-semantic-proof-report-v1-01.json`
  - `docs/dev/completed/20260418-04-phase-2-system-private-corelib-certification-substrate/corelib-substrate-benchmark-v1-01.json`
- requiredGates: `n/a (Phase 2 still builds formal substrate artifacts and does not traverse collector -> registry -> workspace yet)`

**设计文档：** `docs/dev/completed/20260418-04-phase-2-system-private-corelib-certification-substrate/design-v1-01.md`

**预期知识沉淀：** 视执行结果决定  
**收尾约束：** 执行完成后必须进入“审视架构合理性 -> 测试通过 -> 归档 completed -> 合并&提交”固定链路。

---

Status: completed

## 计划清单

- [x] 步骤 1：冻结 `Phase 2` 的 corelib source selection、artifact schema 与 semantic ledger contract
- [x] 步骤 2：先补失败测试，锁定 `phase2` 生成器命令与 formal artifact 契约
- [x] 步骤 3：扩展 `Chaos.FoundationPack.Analysis`，实现 `phase2` 产物生成
- [x] 步骤 4：生成正式 corelib substrate artifacts，并完成针对性 proof/benchmark 校验
- [x] 步骤 5：同步 child / parent `STATUS.md`、`docs/dev/ACTIVE.md`、progress note 与索引
- [x] 步骤 6：执行“审视架构合理性 -> 测试通过 -> 归档 completed -> 合并&提交”收口链路；本轮已满足归档条件

## 需要创建或修改的文件

- `docs/dev/completed/20260418-04-phase-2-system-private-corelib-certification-substrate/STATUS.md`
- `docs/dev/completed/20260418-04-phase-2-system-private-corelib-certification-substrate/design-v1-01.md`
- `docs/dev/completed/20260418-04-phase-2-system-private-corelib-certification-substrate/plan-v1-01.md`
- `docs/dev/completed/20260418-04-phase-2-system-private-corelib-certification-substrate/assembly-semantic-ir-system-private-corelib-v1-01.json`
- `docs/dev/completed/20260418-04-phase-2-system-private-corelib-certification-substrate/runtime-helper-contracts-v1-01.json`
- `docs/dev/completed/20260418-04-phase-2-system-private-corelib-certification-substrate/corelib-surface-proof-report-v1-01.json`
- `docs/dev/completed/20260418-04-phase-2-system-private-corelib-certification-substrate/corelib-semantic-proof-report-v1-01.json`
- `docs/dev/completed/20260418-04-phase-2-system-private-corelib-certification-substrate/corelib-substrate-benchmark-v1-01.json`
- `docs/dev/completed/20260418-04-phase-2-system-private-corelib-certification-substrate/notes/progress-v1-01.md`
- `docs/dev/completed/20260418-04-phase-2-system-private-corelib-certification-substrate/notes/progress-v1-02.md`
- `tests/tooling/foundation/test_phase2_corelib_substrate.py`
- `src/tools/Chaos.FoundationPack.Analysis/Program.cs`
- `docs/dev/in-progress/20260418-01-dotnet-foundation-pack-nativeization-roadmap/STATUS.md`
- `docs/dev/in-progress/20260418-01-dotnet-foundation-pack-nativeization-roadmap/roadmap-v1-01.md`
- `docs/dev/ACTIVE.md`
- `docs/dev/INDEX.md`
- `docs/dev/in-progress/INDEX.md`

## 验证命令

- `python -m pytest tests/tooling/foundation/test_phase2_corelib_substrate.py -q`
- `python -m pytest tests/tooling/foundation/test_phase1_contract_lane.py tests/tooling/foundation/test_phase2_corelib_substrate.py -q`
- `dotnet run --project src/tools/Chaos.FoundationPack.Analysis/Chaos.FoundationPack.Analysis.csproj -- phase2 --catalog docs/dev/completed/20260418-02-phase-0-framework-pack-catalog-and-asset-policy/framework-catalog-v1-01.json --phase1-dir docs/dev/completed/20260418-03-phase-1-contract-facade-and-shim-certification-lane --output-dir docs/dev/completed/20260418-04-phase-2-system-private-corelib-certification-substrate --task-id 20260418-04-phase-2-system-private-corelib-certification-substrate`
- `Get-Content docs/dev/completed/20260418-04-phase-2-system-private-corelib-certification-substrate/assembly-semantic-ir-system-private-corelib-v1-01.json -Raw | ConvertFrom-Json | Out-Null`
- `Get-Content docs/dev/completed/20260418-04-phase-2-system-private-corelib-certification-substrate/runtime-helper-contracts-v1-01.json -Raw | ConvertFrom-Json | Out-Null`
- `Get-Content docs/dev/completed/20260418-04-phase-2-system-private-corelib-certification-substrate/corelib-surface-proof-report-v1-01.json -Raw | ConvertFrom-Json | Out-Null`
- `Get-Content docs/dev/completed/20260418-04-phase-2-system-private-corelib-certification-substrate/corelib-semantic-proof-report-v1-01.json -Raw | ConvertFrom-Json | Out-Null`
- `Get-Content docs/dev/completed/20260418-04-phase-2-system-private-corelib-certification-substrate/corelib-substrate-benchmark-v1-01.json -Raw | ConvertFrom-Json | Out-Null`

## 状态维护要求

- `20260418-04` 已完成并归档 `completed`
- 父 roadmap 保持 `in_progress`，并把活动入口切到 `20260418-05`
- `20260418-05` 将承接 corelib semantic family nativeization
- 本阶段 formal verification 与架构审视均已完成
