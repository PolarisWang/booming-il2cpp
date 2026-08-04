---
task_id: 20260415-03-phase-2-typed-il-shared-contract-and-hybrid-dispatch-foundation
title: Phase 2 Typed-IL Shared Contract And Hybrid Dispatch Foundation
task_type: plan
lifecycle_status: completed
phase: completed
created_at: 2026-04-15 01:58:23 +08:00
updated_at: 2026-04-15 03:37:41 +08:00
current_dir: docs/dev/completed/20260415-03-phase-2-typed-il-shared-contract-and-hybrid-dispatch-foundation
parent_task_id: 20260414-37-hybridclr-aligned-aot-hotupdate-development-plan
source_task_id: 20260414-37-hybridclr-aligned-aot-hotupdate-development-plan
source_relation: roadmap-child
active: false
---

## 关键文档

- parent_status: `docs/dev/in-progress/20260414-37-hybridclr-aligned-aot-hotupdate-development-plan/STATUS.md`
- parent_roadmap: `docs/dev/in-progress/20260414-37-hybridclr-aligned-aot-hotupdate-development-plan/roadmap-v1-01.md`
- phase_0_identity: `docs/dev/completed/20260415-01-phase-0-contract-freeze-and-aot-translation-ledgers/stable-identity-and-body-availability-v1-01.md`
- phase_1_status: `docs/dev/completed/20260415-02-phase-1-subject-source-reshape-and-test-metadata-cutover/STATUS.md`
- plan: `docs/dev/completed/20260415-03-phase-2-typed-il-shared-contract-and-hybrid-dispatch-foundation/plan-v1-01.md`

## 当前结论

- `typed-il -> semantic world -> codegen -> interpreter IR -> runtime -> bridge plan` 的 shared contract 已经贯通，`ManagedMethodIdentityArtifact`、`BodyAvailabilityCode`、`HybridDispatchKind` 不再只停留在 typed-il 输出侧。
- `RuntimeManager`、`HotUpdateMethodRegistry`、`BridgeDispatcher` 的公共入口已经收口到 typed identity / typed binding set；残留的 public string compatibility overload 已全部删除，不再给后续 consumer 留回退到 string surface 的公共通道。
- retained owner subjects / fixtures 已切到新 contract：
  - `HotUpdateHostPack`
  - `MixedExecutionFeaturePack`
  - `VersionRollbackProof`
  - `BridgeRoundtripProof`
  - `AutoBridgeProof`
- `MixedExecutionProofEntry` 已改成 route table lookup；真实 proof 不再依赖 `if/else + string.Equals(bridgeId, ...)` 的分支硬编码。
- 更宽的 compatibility 套件在本轮 completion verification 中暴露出仓库里误留的 generated artifact `solutions/core/windows/chaos-il2cpp-core.sln`；该静态 legacy solution 已删除，重新对齐“workspace 按需生成、仓库不保留静态 core solution”基线。

## 最近摘要

- 2026-04-15 01:58:23 +08:00: 创建 Phase 2 child task，锁定 shared method identity / body availability / hybrid dispatch 为本阶段主线。
- 2026-04-15 03:09:19 +08:00: 完成 runtime cutover，新增 `ManagedMethodIdentityResolver.Create(string subjectId, string signature, ...)` 与 `HotUpdateMethodBindingSet`，并迁移 retained hot-update / mixed-execution subjects 与 fixtures。
- 2026-04-15 03:18:10 +08:00: 完成 bridge plan cutover，`BridgePlan / BridgeGenerator / AutoBridgeGenerator / BridgeDispatcher` 接入 typed target identity。
- 2026-04-15 03:37:41 +08:00: 完成 public string compatibility surface 清理，删除误留的 legacy core solution 产物，Phase 2 达到退出标准。

## 下一步

- next_action: 返回父 roadmap，创建并执行 `Phase 3: HotUpdate Runtime And Supplemental Metadata Foundation` child task。
- owner: codex
- trigger: Phase 2 已完成，验证通过，无额外阻塞。

## 风险 / 阻塞

### risks

- Phase 3 仍需把 `HotUpdateAssemblyLoader`、`SupplementalMetadataRegistry` 和 package/runtime identity 对齐正式落地，否则 hot update runtime foundation 仍然只有局部闭环。
- `SupplementalMetadataLoader` / `SupplementalMetadataWriter` 目前仍以 `SubjectId` 为主要解析面；Phase 3 需要在不回退 public string protocol 的前提下，把对外主入口提升到 typed-facing surface。

### blockers

- 当前无功能 blocker。
- 本机仍有无法停止的 `MixedExecutionFeaturePack` 遗留进程；涉及可执行项目的 build/run 验证仍需使用唯一 `BaseOutputPath`。
- 之后如执行 core workspace 生成验证，需要记得清理生成出的 `solutions/core/<host>/chaos-il2cpp-core.sln`，避免把 generated artifact 再次留回仓库。

## 验证

1. `python -m pytest tests/unit/compatibility/test_phase2_runtime_shared_contract_consumers.py -q`
   - 结果：`7 passed`
2. `python -m pytest tests/unit/compatibility/test_phase2_runtime_shared_contract_consumers.py tests/unit/compatibility/test_phase2_typed_il_shared_contracts.py tests/unit/compatibility/test_managed_closure_contract_bundle.py::ManagedClosureContractBundleTests::test_typed_il_methods_expose_semantic_shape_and_capability_contracts tests/unit/compatibility/test_metadata_supplement_bridge_subjects.py::Phase6MetadataSupplementBridgeTests::test_hot_update_sources_define_writer_loader_bridge_generator_and_native_loader_surface -q`
   - 结果：`11 passed`
3. `dotnet build src/managed/Chaos.IL2CPP.HotUpdate/Chaos.IL2CPP.HotUpdate.csproj -c Release -m:1`
   - 结果：`build succeeded`
4. `dotnet build subjects/HotUpdateHostPack/source/HotUpdateHostPack.csproj -c Release -m:1 -p:BaseOutputPath=artifacts/.tmp-phase2d/hotupdatehost/`
   - 结果：`build succeeded`
5. `dotnet build subjects/MixedExecutionFeaturePack/source/MixedExecutionFeaturePack.csproj -c Release -m:1 -p:BaseOutputPath=artifacts/.tmp-phase2d/mixedexec/`
   - 结果：`build succeeded`
6. `dotnet build tests/fixtures/subjects/VersionRollbackProof/source/VersionRollbackProof.csproj -c Release -m:1 -p:BaseOutputPath=artifacts/.tmp-phase2d/rollbackfixture/`
   - 结果：`build succeeded`
7. `dotnet build tests/fixtures/subjects/BridgeRoundtripProof/source/BridgeRoundtripProof.csproj -c Release -m:1 -p:BaseOutputPath=artifacts/.tmp-phase2d/bridgeroundtrip/`
   - 结果：`build succeeded`
8. `dotnet build tests/fixtures/subjects/AutoBridgeProof/source/AutoBridgeProof.csproj -c Release -m:1 -p:BaseOutputPath=artifacts/.tmp-phase2d/autobridge/`
   - 结果：`build succeeded`
9. `dotnet run --project subjects/MixedExecutionFeaturePack/source/MixedExecutionFeaturePack.csproj -c Release --no-build -p:BaseOutputPath=artifacts/.tmp-phase2d/mixedexec/ --`
   - 结果：`0 exit code`
10. `dotnet run --project subjects/HotUpdateHostPack/source/HotUpdateHostPack.csproj -c Release --no-build -p:BaseOutputPath=artifacts/.tmp-phase2d/hotupdatehost/ --`
    - 结果：`0 exit code`
11. `dotnet run --project tests/fixtures/subjects/VersionRollbackProof/source/VersionRollbackProof.csproj -c Release --no-build -p:BaseOutputPath=artifacts/.tmp-phase2d/rollbackfixture/ --`
    - 结果：输出包含 `version-rollback-v1=11`、`version-rollback-v2=22`、`version-rollback-back-v1=11`、`version-rollback-back-aot=5`
12. `dotnet run --project tests/fixtures/subjects/BridgeRoundtripProof/source/BridgeRoundtripProof.csproj -c Release --no-build -p:BaseOutputPath=artifacts/.tmp-phase2d/bridgeroundtrip/ --`
    - 结果：输出包含 `bridge-aot-to-hot-update=42`、`delegate-roundtrip=42`
13. `dotnet run --project tests/fixtures/subjects/AutoBridgeProof/source/AutoBridgeProof.csproj -c Release --no-build -p:BaseOutputPath=artifacts/.tmp-phase2d/autobridge/ --`
    - 结果：输出包含 `auto-bridge-int-unary=42`、`auto-bridge-int-binary=42`
14. `python -m pytest tests/unit/compatibility -q`
    - 结果：`173 passed`

## wiki

- 本子任务当前只在 `docs/dev` 记录执行细节。
- Phase 3 如果把 shared runtime / supplemental metadata foundation 收敛稳定，再评估同步到 `wiki/03-功能模块/06-il2cpp核心架构/05-架构参考`。
