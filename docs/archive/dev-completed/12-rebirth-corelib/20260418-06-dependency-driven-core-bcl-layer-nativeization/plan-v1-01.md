# Dependency-Driven Core BCL Layer Nativeization 实施计划 v1.01

> 面向执行 Agent: 必须使用 `dev:executing-plans` 执行本计划。步骤使用复选框语法跟踪。
> 目标: 基于 foundation pack 依赖图构建核心 `System.*` DLL 的分层 nativeization 计划，并为首层 DLL 准备可执行的 proof / benchmark / monitor contract。
> 架构: 先冻结层次与候选 DLL，再建立 per-layer proof / benchmark authority，最后把首层 DLL 收敛成可执行 execution plan。
> 技术栈: Markdown、JSON、PowerShell、foundation pack artifacts、registry-driven proof orchestration

**AOT/IL2CPP/Test Governance Intake**
- capabilityFamily: `FrameworkPackNativeization`
- capabilityItem: `DependencyDrivenCoreBclLayerNativeization`
- ownerSubjectId: `SolutionCorePack`
- proofRequired: `true`
- benchmarkRequired: `true`
- hotupdateImpact: `None`
- formalVerificationObjects:
  - `docs/dev/completed/20260418-06-dependency-driven-core-bcl-layer-nativeization/layer-plan-v1-01.json`
  - `docs/dev/completed/20260418-06-dependency-driven-core-bcl-layer-nativeization/proof-benchmark-lane-v1-01.json`
  - `docs/dev/completed/20260418-06-dependency-driven-core-bcl-layer-nativeization/assembly-nativeization-plan/`
- requiredGates: `collector -> registry -> workspace -> dependency-layer-proof -> benchmark`

**设计文档**: `docs/dev/completed/20260418-06-dependency-driven-core-bcl-layer-nativeization/design-v1-01.md`

**预期知识沉淀**: 冻结 layer plan 与首层 DLL authority 稳定性结论
**收尾约束**: 执行完成后必须进入“审视架构合理性 -> 测试通过 -> 归档 completed -> 合并&提交”的固定链路。

---

Status: completed

## 计划清单

- [x] 步骤 1: 冻结 dependency-layer 范围、命名和输入 authority
- [x] 步骤 2: 生成核心 `System.*` DLL 的 layer plan 与 blocked / ready 分类
- [x] 步骤 3: 建立 per-layer proof / benchmark / monitor contract
- [x] 步骤 4: 选择首层 DLL，生成 assembly-nativeization plans
- [x] 步骤 5: 同步 child / parent `STATUS.md`、`docs/dev/ACTIVE.md`、progress note 与索引
- [x] 步骤 6: 执行“审视架构合理性 -> 测试通过 -> 归档 completed -> 合并&提交”；首层 DLL 已具备正式 execution plan，并满足 completed 条件

## 需要创建或修改的文件

- `docs/dev/completed/20260418-06-dependency-driven-core-bcl-layer-nativeization/STATUS.md`
- `docs/dev/completed/20260418-06-dependency-driven-core-bcl-layer-nativeization/design-v1-01.md`
- `docs/dev/completed/20260418-06-dependency-driven-core-bcl-layer-nativeization/plan-v1-01.md`
- `docs/dev/completed/20260418-06-dependency-driven-core-bcl-layer-nativeization/layer-plan-v1-01.json`
- `docs/dev/completed/20260418-06-dependency-driven-core-bcl-layer-nativeization/proof-benchmark-lane-v1-01.json`
- `docs/dev/completed/20260418-06-dependency-driven-core-bcl-layer-nativeization/assembly-nativeization-plan/`
- `docs/dev/completed/20260418-06-dependency-driven-core-bcl-layer-nativeization/notes/progress-v1-01.md`
- `docs/dev/in-progress/20260418-01-dotnet-foundation-pack-nativeization-roadmap/STATUS.md`
- `docs/dev/INDEX.md`
- `docs/dev/in-progress/INDEX.md`
- `docs/dev/completed/INDEX.md`

## 预期验证命令

- `python -m pytest tests/tooling/foundation/test_dependency_layer_plan.py tests/tooling/foundation/test_phase1_contract_lane.py tests/tooling/foundation/test_phase2_corelib_substrate.py tests/tooling/foundation/test_phase3_corelib_family_plan.py tests/tooling/foundation/test_corelib_family_execution_entry.py -q`
- `python -m pytest tests/unit/compatibility/test_solution_core_pack_subject.py -q`
- `Get-Content docs/dev/completed/20260418-06-dependency-driven-core-bcl-layer-nativeization/layer-plan-v1-01.json -Raw | ConvertFrom-Json | Out-Null`
- `Get-Content docs/dev/completed/20260418-06-dependency-driven-core-bcl-layer-nativeization/proof-benchmark-lane-v1-01.json -Raw | ConvertFrom-Json | Out-Null`

## 状态维护要求

- child 归档后删除 `docs/dev/ACTIVE.md`
- 父 roadmap 保持 `in_progress` 且 `active = false`
- `20260418-05-phase-3-system-private-corelib-semantic-family-nativeization` 与 `20260418-06-dependency-driven-core-bcl-layer-nativeization` 均已归档 `completed`
- 如验证阶段出现 `dotnet build` / `dotnet test` / `msbuild` 崩溃，必须立即进入根因排查，未修复前不得归档
