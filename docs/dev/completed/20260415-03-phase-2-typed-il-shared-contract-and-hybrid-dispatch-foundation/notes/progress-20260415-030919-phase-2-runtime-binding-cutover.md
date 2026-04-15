# Phase 2 进展记录 - 2026-04-15 03:09:19 +08:00

## 本轮完成

- 在 `Chaos.IL2CPP.Contracts` 中新增 `ManagedMethodIdentityResolver.Create(string subjectId, string signature, string? definitionSubjectId = null)`。
- 在 `Chaos.IL2CPP.HotUpdate` 中新增：
  - `HotUpdateConstantInt32Binding`
  - `HotUpdateInt32UnaryBinding`
  - `HotUpdateGenericMethodBinding`
  - `HotUpdateMethodBindingSet`
- 在 `RuntimeManager` 中新增 typed `LoadPackage(..., HotUpdateMethodBindingSet bindings)` 入口，并保留旧字典入口作为兼容读面。
- 迁移保留 consumer：
  - `HotUpdateHostPack`
  - `MixedExecutionFeaturePack`
  - `VersionRollbackProof`
  - `BridgeRoundtripProof`
- 统一让 retained source 优先通过 `ManagedMethodIdentityResolver.Create(...)` 构造 typed identity，而不是在 subject 内继续扩散 string `subjectId` 路由。

## 验证

- compatibility
  - `python -m pytest tests/unit/compatibility/test_phase2_runtime_shared_contract_consumers.py tests/unit/compatibility/test_phase2_typed_il_shared_contracts.py tests/unit/compatibility/test_managed_closure_contract_bundle.py::ManagedClosureContractBundleTests::test_typed_il_methods_expose_semantic_shape_and_capability_contracts -q`
- build
  - `dotnet build src/managed/Chaos.IL2CPP.HotUpdate/Chaos.IL2CPP.HotUpdate.csproj -c Release -m:1`
  - `dotnet build subjects/HotUpdateHostPack/source/HotUpdateHostPack.csproj -c Release -m:1 -p:BaseOutputPath=artifacts/.tmp-phase2/hotupdatehost/`
  - `dotnet build subjects/MixedExecutionFeaturePack/source/MixedExecutionFeaturePack.csproj -c Release -m:1 -p:BaseOutputPath=artifacts/.tmp-phase2/mixedexec/`
  - `dotnet build tests/fixtures/subjects/VersionRollbackProof/source/VersionRollbackProof.csproj -c Release -m:1 -p:BaseOutputPath=artifacts/.tmp-phase2/rollbackfixture/`
  - `dotnet build tests/fixtures/subjects/BridgeRoundtripProof/source/BridgeRoundtripProof.csproj -c Release -m:1 -p:BaseOutputPath=artifacts/.tmp-phase2/bridgeroundtrip/`
  - `dotnet build tests/fixtures/subjects/AutoBridgeProof/source/AutoBridgeProof.csproj -c Release -m:1 -p:BaseOutputPath=artifacts/.tmp-phase2/autobridge/`
- real run
  - `dotnet run --project subjects/MixedExecutionFeaturePack/source/MixedExecutionFeaturePack.csproj -c Release --no-build -p:BaseOutputPath=artifacts/.tmp-phase2/mixedexec/ --`
  - `dotnet run --project subjects/HotUpdateHostPack/source/HotUpdateHostPack.csproj -c Release --no-build -p:BaseOutputPath=artifacts/.tmp-phase2/hotupdatehost/ --`
  - `dotnet run --project tests/fixtures/subjects/VersionRollbackProof/source/VersionRollbackProof.csproj -c Release --no-build -p:BaseOutputPath=artifacts/.tmp-phase2/rollbackfixture/ --`
  - `dotnet run --project tests/fixtures/subjects/BridgeRoundtripProof/source/BridgeRoundtripProof.csproj -c Release --no-build -p:BaseOutputPath=artifacts/.tmp-phase2/bridgeroundtrip/ --`
  - `dotnet run --project tests/fixtures/subjects/AutoBridgeProof/source/AutoBridgeProof.csproj -c Release --no-build -p:BaseOutputPath=artifacts/.tmp-phase2/autobridge/ --`

## 剩余差距

- `BridgePlan` / `BridgeGenerator` / `AutoBridgeGenerator` 仍把 target contract 固定在 string `SubjectId`。
- `BridgeDispatcher` 的 plan lookup 仍围绕 string bridge id / target subject id。
- mixed execution proof 中针对 bridge id 的运行时路由仍然是显式字符串判断。

## 下一步建议

- 继续做 Phase 2 最后一批 bridge plan typed identity cutover：
  - 为 `BridgePlan` entries 增加 shared identity target
  - 让 `BridgeGenerator` / `AutoBridgeGenerator` 直接输出 typed target contract
  - 把 mixed execution bridge executor 中残留的 string target routing 压缩到最小边界
