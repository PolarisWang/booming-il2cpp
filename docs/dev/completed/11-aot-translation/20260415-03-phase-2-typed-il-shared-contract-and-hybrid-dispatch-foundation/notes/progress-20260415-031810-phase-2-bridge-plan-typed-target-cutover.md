# Phase 2 进展记录 - 2026-04-15 03:18:10 +08:00

## 本轮完成

- `BridgePlan` entries 新增 shared typed target identity：
  - `HotUpdateIdentity`
  - `AotIdentity`
  - `EngineIdentity`
  - `TargetIdentity`
- `BridgeGenerator` / `AutoBridgeGenerator` 现在会同时输出：
  - typed target identity
  - 兼容 string `SubjectId`
- `BridgeDispatcher` 优先使用 typed target identity 解析 target；只有兼容读面才回退到 string `SubjectId`。
- `MixedExecutionProofEntry` 的 bridge route 已改成 route table，不再使用 `if/else + string.Equals(bridgeId, ...)`。
- `MixedDelegateFlowProofEntry`、`HotUpdateRoundtripBenchmark`、`BridgeRoundtripProof`、`MixedExecutionProofEntry` 已采用新的 bridge plan typed target contract。

## 验证

- `python -m pytest tests/unit/compatibility/test_phase2_runtime_shared_contract_consumers.py tests/unit/compatibility/test_phase2_typed_il_shared_contracts.py tests/unit/compatibility/test_managed_closure_contract_bundle.py::ManagedClosureContractBundleTests::test_typed_il_methods_expose_semantic_shape_and_capability_contracts tests/unit/compatibility/test_metadata_supplement_bridge_subjects.py::Phase6MetadataSupplementBridgeTests::test_hot_update_sources_define_writer_loader_bridge_generator_and_native_loader_surface -q`
- `dotnet build subjects/MixedExecutionFeaturePack/source/MixedExecutionFeaturePack.csproj -c Release -m:1 -p:BaseOutputPath=artifacts/.tmp-phase2c/mixedexec/`
- `dotnet build tests/fixtures/subjects/BridgeRoundtripProof/source/BridgeRoundtripProof.csproj -c Release -m:1 -p:BaseOutputPath=artifacts/.tmp-phase2c/bridgeroundtrip/`
- `dotnet build tests/fixtures/subjects/AutoBridgeProof/source/AutoBridgeProof.csproj -c Release -m:1 -p:BaseOutputPath=artifacts/.tmp-phase2c/autobridge/`
- `dotnet run --project subjects/MixedExecutionFeaturePack/source/MixedExecutionFeaturePack.csproj -c Release --no-build -p:BaseOutputPath=artifacts/.tmp-phase2c/mixedexec/ --`
- `dotnet run --project tests/fixtures/subjects/BridgeRoundtripProof/source/BridgeRoundtripProof.csproj -c Release --no-build -p:BaseOutputPath=artifacts/.tmp-phase2c/bridgeroundtrip/ --`
- `dotnet run --project tests/fixtures/subjects/AutoBridgeProof/source/AutoBridgeProof.csproj -c Release --no-build -p:BaseOutputPath=artifacts/.tmp-phase2c/autobridge/ --`

## 剩余问题

- `RuntimeManager` / `BridgeDispatcher` / `HotUpdateMethodRegistry` 中仍保留部分 public string compatibility overload。
- 这些 overload 已不再被 retained subjects / fixtures 消费，但是否彻底移除，需要在 Phase 2 completion review 中做明确决策。

## 下一步建议

- 做 Phase 2 completion review：
  - 如果决定删除 compatibility overload，就先补 RED 测试并做最后一次 API 收口
  - 如果决定保留极小兼容读面，就把保留范围写进状态文档，然后归档 Phase 2，进入 Phase 3
