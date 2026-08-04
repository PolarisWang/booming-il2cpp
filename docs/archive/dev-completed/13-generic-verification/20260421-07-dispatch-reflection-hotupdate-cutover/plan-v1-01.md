# Dispatch / Reflection / HotUpdate Cutover 实现计划

> **面向执行 Agent：** 必须使用 `dev:executing-plans`、`dev:project-test-governance`、`dev:test-driven-development` 执行本计划。当前任务是 roadmap child；完成时必须先归档当前子任务目录，再回写父任务 `docs/dev/in-progress/20260421-02-canonical-generic-runtime-kernel-roadmap/STATUS.md`。
> **目标：** 把 dispatch、reflection 与 HotUpdate 主线从 `subjectId` / legacy slot string authority 切到 `InstantiationStubId + generation`，并补齐 `PackageFormatVersion` / `KernelArtifactVersion` 的版本拓扑与验证入口。

**架构：** 本轮先建立 HotUpdate runtime authority foundation：引入统一的 authority key / handle / reason-code / version contract，让 host registry、patch registry、bridge dispatcher 与 package validator 都消费同一套 canonical generic authority。reflection 与 advanced carrier 规则在同一 authority 基础上推进，不再额外保留平行字符串路由。

**技术栈：** C# (`Chaos.IL2CPP.Contracts`, `Chaos.IL2CPP.HotUpdate`)、Python unit compatibility tests、subject fixtures / retained proofs、`dotnet build`

**AOT/IL2CPP/Test Governance Intake：**
- capabilityFamily: `native-generic-runtime-kernel`
- capabilityItem: `dispatch-reflection-hotupdate-cutover`
- ownerSubjectId: `SolutionCorePack`
- proofRequired: `true`
- benchmarkRequired: `true`
- hotupdateImpact: `Proof`
- formalVerificationObjects:
  - `dispatch proof`
  - `reflection construction/invoke proof`
  - `hotupdate host->patch proof`
  - `hotupdate patch->host proof`
  - `stale handle proof`
  - `version mismatch proof`
  - `advanced carrier proof`
  - `generated code review`
  - `generated cpp size/memory benchmark`
- requiredGates:
  - `collector`
  - `registry`
  - `workspace`
  - `native-host`
  - `hotupdate-host`

**设计文档：** `docs/dev/in-progress/20260421-02-canonical-generic-runtime-kernel-roadmap/design-v1-02.md`

**预期知识沉淀：** `docs/archive/architecture/managed-native-hotupdate-test-pipeline.md`、`wiki/06-测试验证/AOT新Feature接入自测规范.md`、`wiki/06-测试验证/INDEX.md`

**收尾约束：** 执行完成后必须进入“审视架构合理性 -> 测试通过 -> 归档 completed -> 合并&提交”固定链路；未完成 formal verification gate 前不得宣称完成。

---

## 文件审计范围

- `src/managed/Chaos.IL2CPP.Contracts/ManagedMethodIdentityContracts.cs`
- `src/managed/Chaos.IL2CPP.HotUpdate/HotUpdatePackage.cs`
- `src/managed/Chaos.IL2CPP.HotUpdate/HotUpdateMethodBindings.cs`
- `src/managed/Chaos.IL2CPP.HotUpdate/HotUpdateMethodRegistry.cs`
- `src/managed/Chaos.IL2CPP.HotUpdate/RuntimeManager.cs`
- `src/managed/Chaos.IL2CPP.HotUpdate/BridgeDispatcher.cs`
- `src/managed/Chaos.IL2CPP.HotUpdate/BridgeGenerator.cs`
- `src/managed/Chaos.IL2CPP.HotUpdate/AutoBridgeGenerator.cs`
- `src/managed/Chaos.IL2CPP.HotUpdate/PackageReader.cs`
- `src/managed/Chaos.IL2CPP.HotUpdate/PackageValidator.cs`
- `src/managed/Chaos.IL2CPP.HotUpdate/SupplementalMetadataRegistry.cs`
- `subjects/HotUpdateHostPack/source/Patch/HotUpdatePackageSupport.cs`
- `tests/unit/compatibility/test_phase2_runtime_shared_contract_consumers.py`
- `tests/unit/compatibility/test_phase3_hotupdate_runtime_foundation.py`
- `tests/unit/compatibility/test_hot_update_e2e_flows.py`

## 执行步骤

### 1. intake 与文档接管

- [x] 创建 `20260421-07-dispatch-reflection-hotupdate-cutover`
- [x] 把 `docs/dev/ACTIVE.md` 切到当前子任务
- [x] 在 `docs/dev/in-progress/INDEX.md` 登记当前子任务
- [x] 在父 roadmap 中把 `20260421-07` 标记为 `in-progress`

### 2. RED：authority / version contract

- [x] 为 `HotUpdateMethodRegistry` / `RuntimeManager` / `BridgeDispatcher` 补失败测试
- [x] 为 package manifest / validator 补失败测试
- [x] 明确 `InstantiationStubId + generation` authority、stale handle reason code、`PackageFormatVersion` / `KernelArtifactVersion`

### 3. GREEN：runtime authority foundation

- [x] 引入 canonical hotupdate authority key / handle / reason-code helper
- [x] 让 registry、runtime manager、dispatcher 以 authority key 为主索引
- [x] 保留 `ManagedMethodIdentityArtifact` 作为 diagnostic / authoring surface，而不是最终执行 key
- [x] 让 `SupplementalMetadataRegistry` 能消费 canonical instantiation / stub authority

### 4. GREEN：package / bridge version topology

- [x] 将 manifest 从 `FormatVersion` 升级到 `PackageFormatVersion`
- [x] 新增 `KernelArtifactVersion`
- [x] 让 `PackageReader` / `PackageValidator` 显式校验 package format 与 execution ABI
- [x] 更新 retained subjects / fixtures 的 package 生成入口

### 5. 验证与收口

- [x] 运行 HotUpdate runtime / contract / e2e 相关 Python tests
- [x] 运行 `dotnet build src/managed/Chaos.IL2CPP.Driver/Chaos.IL2CPP.Driver.csproj -c Release -m:1`
- [x] 记录验证结果到 `STATUS.md` 与 `notes/progress-*.md`
- [ ] 视长期规则变更同步更新 architecture / wiki authority 文档
