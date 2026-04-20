---
task_id: 20260419-08-scriban-generator-formalization
title: Scriban Generator Formalization
task_type: plan
lifecycle_status: in-progress
phase: implementation
created_at: 2026-04-19 22:45:00 +08:00
updated_at: 2026-04-19 22:58:00 +08:00
latest_stop_point: 已获用户确认执行 `Chaos.IL2CPP.Generator` cutover；下一步按 TDD 先让工程、Python、planning 和 governance 测试切到新名称并验证失败，再实施重命名与 wiki/authority 收口
current_dir: docs/dev/in-progress/20260419-08-scriban-generator-formalization
parent_task_id: 20260417-02-il2cpp-core-structure-and-scriban-governance-roadmap
source_relation: naming-and-cutover-followup
brainstorm_doc: docs/dev/in-progress/20260419-08-scriban-generator-formalization/brainstorm-v1-01.md
design_doc: docs/dev/in-progress/20260419-08-scriban-generator-formalization/design-v1-01.md
plan_doc: docs/dev/in-progress/20260419-08-scriban-generator-formalization/plan-v1-01.md
active: true
---

## Purpose

把当前已经接入并实际使用的 vendored `Scriban` 正式纳入 IL2CPP 核心生成层治理，并将现有 `Chaos.IL2CPP.CodeGen` 原地重命名为 `Chaos.IL2CPP.Generator`。

## Current Conclusion

- `Scriban` 不是“未来再接入”的状态，而是已经通过 `src/managed/Chaos.IL2CPP.CodeGen/Chaos.IL2CPP.CodeGen.csproj` 直接引用 `third_party/scriban/src/Scriban/Scriban.csproj`。
- 当前仓库的真实问题不是“如何引入 Scriban”，而是“如何把已经存在的临时/局部接入正式收口为核心层标准组件”。
- 本任务采用单次 cutover：不新建并行的 `Generator` 壳工程，而是把现有 `Chaos.IL2CPP.CodeGen` 原地 cutover 为 `Chaos.IL2CPP.Generator`。
- canonical 名称统一采用 `Chaos.IL2CPP.Generator`，不引入 `CHaos.*` 分支。

## Governance Intake

- capabilityFamily: n/a
- capabilityItem: scriban-generator-formalization
- ownerSubjectId: n/a
- proofRequired: false
- benchmarkRequired: false
- hotupdateImpact: None
- formalVerificationObjects: []
- requiredGates: collector -> registry -> workspace

## Key Findings

- 源码层依赖点：
  - `Chaos.IL2CPP.Driver`
  - `Chaos.IL2CPP.Pipeline`
  - `DriverEntry.cs` 的 `using Chaos.IL2CPP.CodeGen;`
- 测试层硬编码点：
  - `tests/unit/run/test_repo_layout.py`
  - `tests/unit/planning/test_project_workspace.py`
  - `tests/support.py`
  - 大量 `tests/unit/compatibility/**` 直接引用 `src/managed/Chaos.IL2CPP.CodeGen/**`
- 长期 authority / wiki 硬编码点：
  - `docs/architecture/managed-native-hotupdate-test-pipeline.md`
  - `docs/architecture/runtime-baseline/repo-layout.md`
  - `docs/architecture/runtime-baseline/ownership-map.md`
  - `wiki/04-工具与集成/il2cpp-core-structure-and-scriban-governance.md`

## Next Step

- 按 `plan-v1-01.md` 执行单次 cutover：
  - 先补/修改测试，锁定 `Generator` 为 canonical 名称与 Python 生成入口
  - 原地重命名 `Chaos.IL2CPP.CodeGen` -> `Chaos.IL2CPP.Generator`
  - 保持 vendored `Scriban` 的 `ProjectReference`
  - 同步更新 source/tests/authority docs/wiki
  - 不保留长期 alias 或双轨工程
