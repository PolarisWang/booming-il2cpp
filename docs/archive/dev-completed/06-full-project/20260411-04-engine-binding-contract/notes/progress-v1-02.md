# 进展记录 v1-02

## 时间

- 2026-04-11 13:21:37 +08:00

## 本轮完成

- 完成 Batch 2 `src/native/engine-bridge/` 最小落地与真实编译验证
  - 新增 `src/native/engine-bridge/CMakeLists.txt`
  - 新增 `src/native/engine-bridge/engine_bridge.h`
  - 新增 `src/native/engine-bridge/engine_bridge.cpp`
  - root `CMakeLists.txt` 已接入 `add_subdirectory(src/native/engine-bridge)`
- 为 Batch 3 新增设计文档
  - `brainstorm-v1-03.md`
  - `design-v1-03.md`
- 按 TDD 新增 RED 并转绿
  - 更新 `tests/unit/run/test_repo_layout.py`
  - 新增 `tests/unit/run/test_phase3_engine_binding_codegen_baseline.py`
- 建立 `Chaos.IL2CPP.EngineBinding` managed 项目
  - 新增 `src/managed/Chaos.IL2CPP.EngineBinding/Chaos.IL2CPP.EngineBinding.csproj`
  - 新增 `EngineExportAttribute.cs`
  - 新增 `EngineCallbackAttribute.cs`
  - 新增 `EngineBindingStubEmitter.cs`
  - `solutions/core/windows/chaos-il2cpp-core.sln` 已接入该项目
- 建立 managed smoke 合同与 native compile-only baseline
  - 新增 `tests/contracts/managed/engine-binding-smoke/EngineBindingSmoke.csproj`
  - 新增 `tests/contracts/managed/engine-binding-smoke/SmokeHostExports.cs`
  - 新增 `tests/contracts/managed/engine-binding-smoke/Program.cs`
  - 新增 `tests/contracts/native/engine-binding/CMakeLists.txt`
  - 新增 `tests/contracts/native/engine-binding/compile_only_engine_binding_smoke.cpp`
  - root `CMakeLists.txt` 已接入 `add_subdirectory(tests/contracts/native/engine-binding)`

## 验证结果

- `python -m pytest tests/unit/run/test_phase3_engine_binding_codegen_baseline.py -v` -> 3 passed
- `python -m pytest tests/unit/run/test_phase3_engine_bridge_minimal.py -v` -> 3 passed
- `python -m pytest tests/unit/run/test_phase3_engine_contract_freeze.py -v` -> 3 passed
- `python -m pytest tests/unit/run/test_phase7_engine_binding.py -v` -> 3 passed
- `python -m pytest tests/unit/run/test_repo_layout.py -v` -> 18 passed
- `python -m pytest tests/unit/run/test_phase3_engine_binding_codegen_baseline.py tests/unit/run/test_phase3_engine_bridge_minimal.py tests/unit/run/test_phase3_engine_contract_freeze.py tests/unit/run/test_phase7_engine_binding.py tests/unit/run/test_repo_layout.py -v` -> 30 passed
- `dotnet build solutions/core/windows/chaos-il2cpp-core.sln -c Debug` -> succeeded
- `dotnet run --project tests/contracts/managed/engine-binding-smoke/EngineBindingSmoke.csproj -- --check-baseline tests/contracts/native/engine-binding/compile_only_engine_binding_smoke.cpp` -> baseline matches
- `cmake --preset windows-x64-reference` -> succeeded
- `cmake --build artifacts/presets/windows-x64-reference --config Debug --target chaos_engine_binding_compile_only_smoke` -> succeeded

## 备注

- Batch 3 当前交付的是 binding generation baseline，不是完整 managed/native 双向调用闭环。
- `EngineBindingStubEmitter` 当前只支持 v0 最小签名：
  - interface method
  - `void` 返回值
  - 无参数
- 这条边界是故意收窄的，目的是先把 managed 标注入口、生成产物形状和 native compile-only 验证固定下来。

## Wiki

- 已新增 `wiki/04-工具与集成/engine-binding-stub-baseline-v1.md`
- 原因：`Chaos.IL2CPP.EngineBinding` 项目隔离规则、smoke baseline 入口与 compile-only 验证命令属于长期有效知识。

## 下一步

- 进入 Batch 4 `EngineHostProof`
- 先收敛最小 host 验证边界：
  - engine mock host
  - lifecycle init/register/dispatch/shutdown
  - 是否在本批次纳入 `HostEmbeddingLite` ownership 协议迁移
