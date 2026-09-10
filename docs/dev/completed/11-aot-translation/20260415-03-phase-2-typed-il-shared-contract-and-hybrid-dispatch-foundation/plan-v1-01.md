# Phase 2 Typed-IL Shared Contract And Hybrid Dispatch Foundation 实现计划

> **面向执行 Agent：** 必须使用 `dev:executing-plans` 与 `dev:test-driven-development` 执行本计划。先补失败测试，再完成最小实现。
> **目标：** 建立 typed-il、解释器、hot update、codegen 共享的最小 method identity / body availability / hybrid dispatch 契约，并让 typed-il artifact 成为这组契约的首个稳定出口。
> **架构：** 本阶段不直接重构 `NativeAotEmitter`，先把共享 contract 和 resolver 放到 `Chaos.IL2CPP.Contracts`，再把 `SemanticWorldStage` / `CodeGenStage` 接上，最后让解释器 / mixed execution 后续阶段可以消费同一套信息。

**技术栈：** .NET 8、`Chaos.IL2CPP.Contracts`、`Chaos.IL2CPP.SemanticWorld`、`Chaos.IL2CPP.CodeGen`、managed closure contract tests

**设计文档：**

- `docs/dev/in-progress/20260414-37-hybridclr-aligned-aot-hotupdate-development-plan/roadmap-v1-01.md`
- `docs/dev/completed/20260415-01-phase-0-contract-freeze-and-aot-translation-ledgers/stable-identity-and-body-availability-v1-01.md`
- `docs/dev/completed/20260415-02-phase-1-subject-source-reshape-and-test-metadata-cutover/STATUS.md`

**预期知识沉淀：** 如 shared contract 稳定，后续同步到 `docs/architecture`。

---

## 预期改动面

- `src/managed/Chaos.IL2CPP.Contracts/ManagedClosureContracts.cs`
- `src/managed/Chaos.IL2CPP.SemanticWorld/SemanticWorldStage.cs`
- `src/managed/Chaos.IL2CPP.CodeGen/CodeGenStage.cs`
- 视实现需要补充的 shared contract / resolver 新文件
- `tests/unit/compatibility/test_managed_closure_contract_bundle.py`
- 新增 Phase 2 shared contract 兼容测试

## 执行步骤

- [ ] Task 1: 补 RED 测试，锁定以下 contract 输出：
  - typed-il method identity 结构
  - enum-coded body availability
  - enum-coded hybrid dispatch kind
- [ ] Task 2: 在 `Chaos.IL2CPP.Contracts` 引入 shared contract：
  - `ManagedMethodIdentity`
  - `BodyAvailabilityCode`
  - `HybridDispatchKind`
  - 对应 resolver
- [ ] Task 3: 接线 `SemanticWorldStage` 与 `CodeGenStage`，让 typed-il artifact 发出新 contract 字段。
- [ ] Task 4: 跑通 managed closure contract bundle 与新增 shared contract 测试。
- [ ] Task 5: 更新父 roadmap / Phase 2 status / progress notes。

## 验收

- `Chaos.IL2CPP.Contracts` 中存在可复用的 shared contract / resolver，而不是继续把逻辑散在 `SemanticWorldStage` / `ILToIRLowering` / `RuntimeManager` 各处。
- `typed-il-ir.json` 的每个 method 至少带出：
  - 结构化 method identity
  - enum-coded body availability
  - call 指令上的 enum-coded dispatch kind
- 现有 contract bundle 不回归；如保留老字符串字段，仅作为兼容读面，不作为后续新实现的唯一 typed source。

## 验证命令

- `python -m pytest tests/unit/compatibility/test_phase2_typed_il_shared_contracts.py tests/unit/compatibility/test_managed_closure_contract_bundle.py -k "phase2 or typed_il_methods_expose_semantic_shape_and_capability_contracts"`
- `dotnet build src/managed/Chaos.IL2CPP.Driver/Chaos.IL2CPP.Driver.csproj -c Release -m:1`
