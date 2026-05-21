# 进展记录 v1-03

## 时间

- 2026-04-11 03:54:56 +08:00

## 本轮完成

- 完成 Batch 1 interface dispatch 最小闭环：
  - `ManagedTypeModel` 新增 `IsInterface`
  - `LoaderStage` 写入接口类型标记并在泛型实例化时继承
  - `SemanticWorldStage` 新增 `uses-interface-call-site`
  - `LinkerStage` 新增 `callvirt.interface-runtime-helper`
  - `NativeReferenceLoweringPlanner` / `NativeReferenceProofEmitter` 新增 `managed-dispatch.interface-message.minimal`
  - 新增 `NativeReferenceProof.InterfaceDispatchMessage.cpp.scriban`
  - 新增 `subjects/InterfaceDispatchProof/` proof subject
- 为 `InterfaceDispatchProof` 补上 `Program.Main()`，使其既能保留 proof entry，又能作为 `Exe` 真实编译。

## 验证结果

- `python -m pytest tests/unit/run/test_phase2_aot_runtime_completeness.py -v` -> 3 passed
- `python -m pytest tests/unit/run/test_repo_layout.py -v` -> 18 passed
- `python -m pytest tests/unit/run/test_project_graph.py -v` -> 4 passed
- `python -m pytest tests/unit/run/test_phase2_core_contract_cutover.py tests/unit/run/test_phase3_capability_driven_lowering.py tests/unit/run/test_phase4_mainline_feature_pack_onboarding.py tests/unit/run/test_phase5_capability_batch_a.py tests/unit/run/test_phase6_capability_batch_b.py tests/unit/run/test_phase7_engine_binding.py -v` -> 17 passed
- `dotnet build solutions/core/windows/chaos-il2cpp-core.sln -c Debug` -> succeeded
- `dotnet build subjects/InterfaceDispatchProof/source/InterfaceDispatchProof.csproj -c Debug` -> succeeded

## Wiki

- 本轮先不写入 wiki。
- 原因：Batch 2 async/await 即将开始，待 Phase 2 能力矩阵再沉淀一轮后统一整理 runtime completeness 规则更合适。

## 下一步

- 审计 async/await 现状并建立 Batch 2 RED 测试，锁定 state machine 识别、reachability 与 lowering 入口。
