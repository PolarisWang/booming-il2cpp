# Legacy Generic Path Purge 执行计划 v1.01

> 面向执行 Agent：必须使用 `dev:executing-plans`、`dev:project-test-governance`、`dev:test-driven-development` 执行本计划。
> 目标：清掉 canonical generic runtime kernel 主线最后一批 legacy generic path，包括 loader / codegen / HotUpdate 中仍暴露 clone/materialization、散落 fallback 与旧 authority 语义的实现与治理契约。

## AOT/IL2CPP/Test Governance Intake

- capabilityFamily: `native-generic-runtime-kernel`
- capabilityItem: `legacy-generic-path-purge`
- ownerSubjectId: `SolutionCorePack`
- proofRequired: `true`
- benchmarkRequired: `true`
- hotupdateImpact: `Proof`
- formalVerificationObjects:
  - `generic matrix`
  - `aot-core-ir generic contracts`
  - `native generic proofs`
  - `hotupdate generic boundary proofs`
  - `generated code review`
  - `generated cpp size/memory benchmark`
- requiredGates:
  - `collector`
  - `registry`
  - `workspace`
  - `native-host`
  - `hotupdate-host`

## 设计与审视入口

- design_doc: `docs/dev/completed/20260421-02-canonical-generic-runtime-kernel-roadmap/design-v1-02.md`
- parent_roadmap: `docs/dev/completed/20260421-02-canonical-generic-runtime-kernel-roadmap/roadmap-v1-02.md`
- arch_review_mode: `critical`

## 已冻结边界

- 不回退到 clone/materialization-driven generic execution
- 不让 `subjectId`、legacy slot string、metadata token 重新成为最终执行 authority
- 不在 loader/codegen/HotUpdate 中保留长期双轨 fallback
- 如需新增 proof / benchmark / formal object，必须回到 parent roadmap 重新 intake；本计划默认不新增 obligation 类型

## 已知残留点

- loader:
  - `LoaderStage.GenericInstantiationProjection.cs`
  - `LoaderStage.GenericInstantiationProjectionSubstitution.cs`
  - `LoaderStage.CrossAssemblyInstantiation.cs`
- tests/governance:
  - `tests/unit/compatibility/il2cpp_codegen_structure_governance_test_support.py`
  - `tests/unit/compatibility/test_il2cpp_codegen_structure_governance_pipeline_contracts.py`
  - `tests/unit/compatibility/test_loader_instantiation_demand_graph_contract.py`
  - `tests/unit/compatibility/test_loader_instantiation_demand_graph_pipeline_contract.py`
  - `tests/unit/compatibility/test_loader_method_identity_generic_arity.py`
- 后续批次候选:
  - `src/managed/Chaos.IL2CPP.CodeGen/**`
  - `src/managed/Chaos.IL2CPP.HotUpdate/**`
  - 相关 docs / manifests / sidecars

## 功能目标 Checklist

- [x] 移除 loader 中显式 `materialization` 命名与“闭包 clone 是最终 authority”暗示
- [x] 收敛 cross-assembly helper 到 `instantiation projection` 语义
- [x] 清理 tests/governance 中仅服务 legacy path 的常量、断言与命名
- [x] 审计 codegen / HotUpdate 中仍暴露 legacy fallback 的路径并分批清理
- [x] 更新文档/状态，保持 roadmap-child 对齐

## 文件审计 Checklist

- [x] `src/managed/Chaos.IL2CPP.Loader/LoaderStage.cs`
- [x] `src/managed/Chaos.IL2CPP.Loader/LoaderStage.AssemblyLoading.cs`
- [x] `src/managed/Chaos.IL2CPP.Loader/LoaderStage.CrossAssemblyInstantiation.cs`
- [x] `src/managed/Chaos.IL2CPP.Loader/LoaderStage.GenericInstantiationProjection.cs`
- [x] `src/managed/Chaos.IL2CPP.Loader/LoaderStage.GenericInstantiationProjectionSubstitution.cs`
- [x] `src/managed/Chaos.IL2CPP.Loader/Models/LoaderModels.cs`
- [x] `tests/unit/compatibility/il2cpp_codegen_structure_governance_test_support.py`
- [x] `tests/unit/compatibility/test_il2cpp_codegen_structure_governance_pipeline_contracts.py`
- [x] `tests/unit/compatibility/test_loader_instantiation_demand_graph_contract.py`
- [x] `tests/unit/compatibility/test_loader_instantiation_demand_graph_pipeline_contract.py`
- [x] `tests/unit/compatibility/test_loader_method_identity_generic_arity.py`

## 执行步骤

- [x] 步骤 1：preflight + RED
  - 把 loader/test governance 中的 legacy 命名断言切换到 `instantiation projection` 语义
  - 显式加入对 `materializedTypes/materializedMethods/materializedFields/materializedProperties`
    与 `MaterializeCrossAssemblyMethodInstantiations` 的禁止断言
  - 运行聚焦 pytest，确认 RED 来自当前实现尚未收敛

- [x] 步骤 2：loader 首批 GREEN
  - 重命名 loader helper 与局部变量，去掉 `materialization` 命名
  - 保持 `GenericInstantiationDemandGraph` 事实链、`GenericInstantiationKey` 与 method identity 行为不变
  - 如需要小型抽象整理，只允许在 loader helper 内完成，不扩散 authority

- [x] 步骤 3：第二批 purge 预审
  - 审计 codegen / HotUpdate 残余 scattered fallback
  - 识别哪些是继续当前 child 的执行问题，哪些会触发新的 authority/boundary 变化
  - 仅把执行级风险写入 STATUS，不在执行阶段重开 design

- [x] 步骤 4：验证与收口
  - 运行本批次聚焦 compatibility tests
  - 运行至少一轮 `dotnet build src/managed/Chaos.IL2CPP.Driver/Chaos.IL2CPP.Driver.csproj -c Release -m:1`
  - 在 STATUS 中记录 arch review、verification 与下一批次入口

## 验证计划

- RED/GREEN 首批:
  - `python -m pytest tests/unit/compatibility/test_il2cpp_codegen_structure_governance_pipeline_contracts.py tests/unit/compatibility/test_loader_instantiation_demand_graph_contract.py tests/unit/compatibility/test_loader_instantiation_demand_graph_pipeline_contract.py tests/unit/compatibility/test_loader_method_identity_generic_arity.py -q`
- C# 构建:
  - `dotnet build src/managed/Chaos.IL2CPP.Driver/Chaos.IL2CPP.Driver.csproj -c Release -m:1`

## 风险与预案

- 风险 1：仅改命名但未真正压掉 legacy authority 暗示
  - 预案：测试同时禁止旧 helper 名称与 `materialized*` 局部变量，避免只做表面换皮
- 风险 2：loader helper 改名影响多个分拆文件
  - 预案：先锁定最小调用链 `LoaderStage.cs -> AssemblyLoading -> CrossAssembly -> GenericInstantiationProjection`
- 风险 3：工作树并行改动导致 patch 冲突
  - 预案：仅改当前 child 覆盖的局部文件，不回滚、不整理无关差异
