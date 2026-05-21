# Design v1-03

## Batch 3 最小实现设计

### 目标

在不引入 pipeline 内部依赖的前提下，建立 Phase 3 的 managed binding generation baseline：

- 新建独立 `Chaos.IL2CPP.EngineBinding` 项目
- 冻结 `[EngineExport]` / `[EngineCallback]` 标注
- 用反射式 emitter 从 C# interface 生成 compile-only C++ wrapper stub
- 用 managed smoke 合同 + native compile-only smoke 证明 baseline 可重复验证

### 设计范围

#### 1. `Chaos.IL2CPP.EngineBinding` 项目

- 路径：`src/managed/Chaos.IL2CPP.EngineBinding/`
- 依赖：
  - `Chaos.IL2CPP.Contracts`
- 禁止依赖：
  - `Chaos.IL2CPP.Loader`
  - `Chaos.IL2CPP.SemanticWorld`
  - `Chaos.IL2CPP.Linker`
  - `Chaos.IL2CPP.MetadataWriter`
  - `Chaos.IL2CPP.CodeGen`
  - `Chaos.IL2CPP.Pipeline`

#### 2. managed 标注模型

##### `EngineExportAttribute`

- 用于标注 interface method 是 engine-facing export
- 最小字段：
  - `string ExportName`

##### `EngineCallbackAttribute`

- 用于标注 interface method 是 callback registration 入口
- 最小字段：
  - `string CallbackName`

#### 3. `EngineBindingStubEmitter`

- 公开入口：
  - `string GenerateForInterface(Type interfaceType)`
- 规则：
  - 必须输入 interface
  - 只处理带 `[EngineExport]` / `[EngineCallback]` 的方法
  - v0 baseline 仅支持：
    - `void` 返回值
    - 无参数方法
  - 其他签名直接抛出 `NotSupportedException`
- 输出内容：
  - `#include "engine_bridge.h"`
  - 每个 `[EngineExport]` 方法生成一个 compile-only stub，函数体仅保留占位语义并返回 `ENGINE_STATUS_NOT_SUPPORTED`
  - 每个 `[EngineCallback]` 方法生成一个调用 `engine_register_callback(...)` 的注册 helper

#### 4. smoke baseline

##### managed smoke

- 路径：`tests/contracts/managed/engine-binding-smoke/`
- 内容：
  - `SmokeHostExports.cs`：定义最小 interface
  - `Program.cs`：调用 `EngineBindingStubEmitter.GenerateForInterface(...)`

##### native compile-only smoke

- 路径：`tests/contracts/native/engine-binding/`
- 内容：
  - `CMakeLists.txt`
  - `compile_only_engine_binding_smoke.cpp`
- include：
  - `contracts/engine/v0`
  - `src/native/engine-bridge`
- 目标：
  - 只验证 baseline wrapper 形状可编译，不做运行时链接或行为断言

### RED 测试

- 更新 `tests/unit/run/test_repo_layout.py`
  - 锁定 `Chaos.IL2CPP.EngineBinding` 项目存在且只依赖 `Chaos.IL2CPP.Contracts`
- 新增 `tests/unit/run/test_phase3_engine_binding_codegen_baseline.py`
  - 锁定 managed 项目与关键源文件
  - 锁定 smoke 合同与 native compile-only 目录
  - 锁定 root `CMakeLists.txt` 接入 `tests/contracts/native/engine-binding`

### GREEN 验证

- `python -m pytest tests/unit/run/test_phase3_engine_binding_codegen_baseline.py -v`
- `python -m pytest tests/unit/run/test_repo_layout.py -v`
- `python -m pytest tests/unit/run/test_phase7_engine_binding.py -v`
- `dotnet build src/managed/Chaos.IL2CPP.EngineBinding/Chaos.IL2CPP.EngineBinding.csproj -c Debug`
- `dotnet run --project tests/contracts/managed/engine-binding-smoke/EngineBindingSmoke.csproj`
- `cmake --preset windows-x64-reference`
- `cmake --build artifacts/presets/windows-x64-reference --config Debug --target chaos_engine_binding_compile_only_smoke`

### 非目标

- 本批次不实现完整参数类型映射
- 本批次不实现 managed body 真正调用
- 本批次不创建 `EngineHostProof`
- 本批次不迁移 `HostEmbeddingLite`
