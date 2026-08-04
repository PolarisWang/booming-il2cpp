# 进展记录 v1-01

## 时间

- 2026-04-11 16:00:23 +08:00

## 本轮完成

- 完成 Batch 1 supplemental metadata template
  - `MetadataWriterStage` 新增 `SupplementalMetadataTemplateArtifact`
  - convert 输出新增 `hot-update/supplemental-metadata-template.json`
  - `closure.manifest.json` 已接入 `supplementalMetadataTemplate`
- 完成 Batch 2 supplemental metadata writer / loader baseline
  - 新增 `SupplementalMetadataWriter`
  - 新增 `SupplementalMetadataLoader`
  - native 侧补齐 `LoadSupplementalMetadataFromPath` / `ReleaseSupplementalMetadataImage` surface
- 完成 Batch 3 bridge generation baseline
  - `BridgeGenerator` 补齐 `AotToHotUpdate`、`HotUpdateToAot`、`HotUpdateToEngine`、`DelegateWrappers`
  - `BridgeDispatcher` 补齐 hot-update / AOT / engine 的最小注册与调用面
- 完成 Batch 4 subject 与验证
  - 新增 `subjects/BridgeRoundtripProof/`
  - 新增 `subjects/GenericSupplementProof/`
  - 新增 `tests/unit/run/test_phase6_metadata_supplement_bridge.py`
  - 同步 `tests/unit/run/test_managed_closure_contract_bundle.py` 与 `wiki/03-功能模块/06-il2cpp核心架构/docs/archive/architecture/roadmap-0/managed-minimal-closure-v0.md`，把 supplemental metadata template 纳入正式 bundle 契约

## 验证结果

- `python -m pytest tests/unit/run/test_phase6_metadata_supplement_bridge.py -v` -> 4 passed
- `python -m pytest tests/unit/run/test_managed_closure_contract_bundle.py -v` -> 6 passed
- `python -m pytest tests/unit/run/test_subject_manifest_schema.py tests/unit/run/test_subject_planner.py tests/unit/run/test_project_graph.py tests/unit/run/test_phase5_hot_update_skeleton.py tests/unit/run/test_repo_layout.py -v` -> 54 passed
- `dotnet run --project subjects/BridgeRoundtripProof/source/BridgeRoundtripProof.csproj --` -> `bridge-aot-to-hot-update=42`, `bridge-hot-update-to-aot=2`, `bridge-hot-update-to-engine=7`, `delegate-roundtrip=42`
- `dotnet run --project subjects/GenericSupplementProof/source/GenericSupplementProof.csproj --` -> `generic-supplement=ok`
- `dotnet build src/managed/Chaos.IL2CPP.HotUpdate/Chaos.IL2CPP.HotUpdate.csproj -c Release` -> succeeded
- `cmake -S . -B artifacts/.tmp-hot-update-reference -G "Visual Studio 17 2022" -DROADMAP0_PRESET_TARGET=windows-x64-reference` -> succeeded
- `cmake --build artifacts/.tmp-hot-update-reference --config Release --target chaos_hot_update` -> succeeded
- `module/managed-closure/basic` formal verification -> `runId=20260411-155954-windows-b697`, `finalStatus=ok`
- `system/runtime-baseline-windows` formal verification -> `runId=20260411-160011-windows-3f75`, `finalStatus=ok`

## 备注

- Phase 6 明确保持 proof-first baseline：template 输出、supplemental metadata writer/loader 与 bridge route 都是可验证最小实现，不提前承担完整 interpreter productization 责任。
- `DriverEntry.WriteJson` 已支持自动创建父目录，使 nested artifact 输出成为正式能力，而不是针对单个产物的特判。
- `managed-closure` contract bundle 现在正式包含 `hot-update/supplemental-metadata-template.json`，后续再改 bundle 结构时必须同步维护该契约。

## Wiki

- 已新增 `wiki/04-工具与集成/metadata-supplement-bridge-baseline-v1.md`
- 原因：supplemental metadata template、bridge baseline surface、proof/verification 入口属于后续 Phase 7/9 也要复用的长期知识。

## 下一步

- 激活 `20260411-08-interpreter-mixed-execution`
- 先为 Batch 1 InterpreterIR 定义写首轮 RED：
  - 新建 `Chaos.IL2CPP.Interpreter` 项目，依赖 `Contracts + HotUpdate`
  - 明确 `InterpreterIR` / `IRMethod` / `IRInstruction` / `IRBasicBlock` 最小 contract
  - 为后续 `IL -> IR lowering` 和 mixed execution proof 铺底
