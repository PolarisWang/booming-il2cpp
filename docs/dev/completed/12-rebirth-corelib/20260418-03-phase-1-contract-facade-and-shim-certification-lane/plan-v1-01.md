# Phase 1 Contract Facade And Shim Certification Lane 实现计划 v1.01

> **面向执行 Agent：** 必须使用 `dev:executing-plans` 执行本计划。步骤使用复选框（`- [ ]`）语法跟踪。  
> **目标：** 在 `Phase 0` 可重现的 foundation pack 输入层之上，冻结 facade/shim lane 的 candidate assemblies、classification rule、public surface manifest 与 proof/benchmark contract。  
> **架构：** 先以 `framework-catalog-v1-01.json` 为唯一输入，派生 seed assembly set 与 candidate closure，再产出 machine-readable classification/surface artifacts，最后用 `surface-proof + native-binding-proof + manifest-benchmark` 关闭 `Phase 1`。不提前进入 `System.Private.CoreLib` implementation nativeization。  
> **技术栈：** Markdown、JSON、PowerShell、`.NET` framework pack metadata

**AOT/IL2CPP/Test Governance Intake：**
- capabilityFamily: `FrameworkPackNativeization`
- capabilityItem: `Phase1ContractFacadeAndShimCertificationLane`
- ownerSubjectId: `DotNetFoundationPack (planned)`
- proofRequired: `true`
- benchmarkRequired: `true`
- hotupdateImpact: `None`
- formalVerificationObjects:
  - `docs/dev/completed/20260418-03-phase-1-contract-facade-and-shim-certification-lane/assembly-classification-v1-01.json`
  - `docs/dev/completed/20260418-03-phase-1-contract-facade-and-shim-certification-lane/public-surface-manifest-v1-01.json`
  - `docs/dev/completed/20260418-03-phase-1-contract-facade-and-shim-certification-lane/surface-proof-report-v1-01.json`
  - `docs/dev/completed/20260418-03-phase-1-contract-facade-and-shim-certification-lane/native-binding-proof-report-v1-01.json`
  - `docs/dev/completed/20260418-03-phase-1-contract-facade-and-shim-certification-lane/manifest-benchmark-v1-01.json`
- requiredGates: `n/a (Phase 1 contract lane still builds catalog/classification proof objects and does not traverse collector -> registry -> workspace yet)`

**设计文档：** `docs/dev/completed/20260418-03-phase-1-contract-facade-and-shim-certification-lane/design-v1-01.md`

**预期知识沉淀：** 按任务决定  
**收尾约束：** 执行完成后必须进入“审视架构合理性 -> 测试通过 -> 归档 completed -> 合并&提交”固定链路。

---

Status: completed

## 计划清单

- [x] 步骤 1：基于 `Phase 0` catalog 派生 `Phase 1` seed assemblies 与 candidate closure 清单
- [x] 步骤 2：冻结 `contract-facade` / `compat-shim` / `implementation` / `runtime-backed` 分类规则及 canonical owner mapping 约束
- [x] 步骤 3：产出 `assembly-classification-v1-01.json` 与 `public-surface-manifest-v1-01.json`
- [x] 步骤 4：产出 facade/shim `surface-proof-report-v1-01.json`、`native-binding-proof-report-v1-01.json` 与 `manifest-benchmark-v1-01.json`
- [x] 步骤 5：运行针对性校验，并同步 child / parent `STATUS.md`、`docs/dev/ACTIVE.md`、progress note 与索引
- [x] 步骤 6：执行“审视架构合理性 -> 测试通过 -> 归档 completed -> 合并&提交”收口链路；本轮已满足归档条件

## 需要创建或修改的文件

- `docs/dev/completed/20260418-03-phase-1-contract-facade-and-shim-certification-lane/STATUS.md`
- `docs/dev/completed/20260418-03-phase-1-contract-facade-and-shim-certification-lane/design-v1-01.md`
- `docs/dev/completed/20260418-03-phase-1-contract-facade-and-shim-certification-lane/plan-v1-01.md`
- `docs/dev/completed/20260418-03-phase-1-contract-facade-and-shim-certification-lane/assembly-classification-v1-01.json`
- `docs/dev/completed/20260418-03-phase-1-contract-facade-and-shim-certification-lane/public-surface-manifest-v1-01.json`
- `docs/dev/completed/20260418-03-phase-1-contract-facade-and-shim-certification-lane/surface-proof-report-v1-01.json`
- `docs/dev/completed/20260418-03-phase-1-contract-facade-and-shim-certification-lane/native-binding-proof-report-v1-01.json`
- `docs/dev/completed/20260418-03-phase-1-contract-facade-and-shim-certification-lane/manifest-benchmark-v1-01.json`
- `docs/dev/completed/20260418-03-phase-1-contract-facade-and-shim-certification-lane/notes/progress-v1-01.md`
- `docs/dev/completed/20260418-03-phase-1-contract-facade-and-shim-certification-lane/notes/progress-v1-02.md`
- `docs/dev/in-progress/20260418-01-dotnet-foundation-pack-nativeization-roadmap/STATUS.md`
- `docs/dev/in-progress/20260418-01-dotnet-foundation-pack-nativeization-roadmap/roadmap-v1-01.md`
- `docs/dev/ACTIVE.md`
- `docs/dev/INDEX.md`
- `docs/dev/in-progress/INDEX.md`

## 验证命令

- `python -m pytest tests/tooling/foundation/test_phase1_contract_lane.py -q`
- `dotnet run --project src/tools/Chaos.FoundationPack.Analysis/Chaos.FoundationPack.Analysis.csproj -- phase1 --catalog docs/dev/completed/20260418-02-phase-0-framework-pack-catalog-and-asset-policy/framework-catalog-v1-01.json --output-dir docs/dev/completed/20260418-03-phase-1-contract-facade-and-shim-certification-lane --task-id 20260418-03-phase-1-contract-facade-and-shim-certification-lane`
- `Get-Content docs/dev/completed/20260418-03-phase-1-contract-facade-and-shim-certification-lane/assembly-classification-v1-01.json -Raw | ConvertFrom-Json | Out-Null`
- `Get-Content docs/dev/completed/20260418-03-phase-1-contract-facade-and-shim-certification-lane/public-surface-manifest-v1-01.json -Raw | ConvertFrom-Json | Out-Null`
- `Get-Content docs/dev/completed/20260418-03-phase-1-contract-facade-and-shim-certification-lane/surface-proof-report-v1-01.json -Raw | ConvertFrom-Json | Out-Null`
- `Get-Content docs/dev/completed/20260418-03-phase-1-contract-facade-and-shim-certification-lane/native-binding-proof-report-v1-01.json -Raw | ConvertFrom-Json | Out-Null`
- `Get-Content docs/dev/completed/20260418-03-phase-1-contract-facade-and-shim-certification-lane/manifest-benchmark-v1-01.json -Raw | ConvertFrom-Json | Out-Null`

## 状态维护要求

- `20260418-03` 已完成并归档 `completed`
- 父 roadmap 保持 `in_progress`，并把活动入口切到 `20260418-04`
- `20260418-04` 将承接 `System.Private.CoreLib` certification substrate
- 本阶段 formal verification 与架构审视均已完成
