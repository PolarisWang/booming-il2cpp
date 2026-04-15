# Phase 2 完成记录

时间：2026-04-15 03:37:41 +08:00

## 本轮完成

- 删除 `HotUpdateMethodRegistry`、`RuntimeManager`、`BridgeDispatcher` 残留的 public string compatibility overload，只保留 typed identity / typed binding set 公共入口。
- 扩展并跑通 Phase 2 兼容测试，确保 string surface 不会重新暴露到 public API。
- 在更宽的 `tests/unit/compatibility` 回归中发现并清理误留的 generated artifact `solutions/core/windows/chaos-il2cpp-core.sln`。

## 验证

- `python -m pytest tests/unit/compatibility/test_phase2_runtime_shared_contract_consumers.py -q`
- `python -m pytest tests/unit/compatibility/test_phase2_runtime_shared_contract_consumers.py tests/unit/compatibility/test_phase2_typed_il_shared_contracts.py tests/unit/compatibility/test_managed_closure_contract_bundle.py::ManagedClosureContractBundleTests::test_typed_il_methods_expose_semantic_shape_and_capability_contracts tests/unit/compatibility/test_metadata_supplement_bridge_subjects.py::Phase6MetadataSupplementBridgeTests::test_hot_update_sources_define_writer_loader_bridge_generator_and_native_loader_surface -q`
- `dotnet build src/managed/Chaos.IL2CPP.HotUpdate/Chaos.IL2CPP.HotUpdate.csproj -c Release -m:1`
- `dotnet build subjects/HotUpdateHostPack/source/HotUpdateHostPack.csproj -c Release -m:1 -p:BaseOutputPath=artifacts/.tmp-phase2d/hotupdatehost/`
- `dotnet build subjects/MixedExecutionFeaturePack/source/MixedExecutionFeaturePack.csproj -c Release -m:1 -p:BaseOutputPath=artifacts/.tmp-phase2d/mixedexec/`
- `dotnet build tests/fixtures/subjects/VersionRollbackProof/source/VersionRollbackProof.csproj -c Release -m:1 -p:BaseOutputPath=artifacts/.tmp-phase2d/rollbackfixture/`
- `dotnet build tests/fixtures/subjects/BridgeRoundtripProof/source/BridgeRoundtripProof.csproj -c Release -m:1 -p:BaseOutputPath=artifacts/.tmp-phase2d/bridgeroundtrip/`
- `dotnet build tests/fixtures/subjects/AutoBridgeProof/source/AutoBridgeProof.csproj -c Release -m:1 -p:BaseOutputPath=artifacts/.tmp-phase2d/autobridge/`
- `dotnet run --project subjects/MixedExecutionFeaturePack/source/MixedExecutionFeaturePack.csproj -c Release --no-build -p:BaseOutputPath=artifacts/.tmp-phase2d/mixedexec/ --`
- `dotnet run --project subjects/HotUpdateHostPack/source/HotUpdateHostPack.csproj -c Release --no-build -p:BaseOutputPath=artifacts/.tmp-phase2d/hotupdatehost/ --`
- `dotnet run --project tests/fixtures/subjects/VersionRollbackProof/source/VersionRollbackProof.csproj -c Release --no-build -p:BaseOutputPath=artifacts/.tmp-phase2d/rollbackfixture/ --`
- `dotnet run --project tests/fixtures/subjects/BridgeRoundtripProof/source/BridgeRoundtripProof.csproj -c Release --no-build -p:BaseOutputPath=artifacts/.tmp-phase2d/bridgeroundtrip/ --`
- `dotnet run --project tests/fixtures/subjects/AutoBridgeProof/source/AutoBridgeProof.csproj -c Release --no-build -p:BaseOutputPath=artifacts/.tmp-phase2d/autobridge/ --`
- `python -m pytest tests/unit/compatibility -q`

## 交接

- Phase 3 入口切到 hot update runtime foundation。
- 后续如果运行 core workspace 生成链，记得不要把生成出的 `solutions/core/<host>/chaos-il2cpp-core.sln` 留回仓库。
