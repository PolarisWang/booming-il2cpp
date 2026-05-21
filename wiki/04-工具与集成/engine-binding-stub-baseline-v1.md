# engine binding stub baseline v1

## 适用范围

本文记录 Phase 3 当前稳定使用的 engine binding baseline：

- `src/managed/Chaos.IL2CPP.EngineBinding/`
- managed smoke 合同：`tests/contracts/managed/engine-binding-smoke/`
- native compile-only smoke：`tests/contracts/native/engine-binding/`

这里沉淀的是长期有效的接入规则和验证入口，不记录单次运行结果。

## 1. 项目边界

### managed 项目

- 项目路径：
  - `src/managed/Chaos.IL2CPP.EngineBinding/Chaos.IL2CPP.EngineBinding.csproj`
- 当前依赖：
  - `Chaos.IL2CPP.Contracts`
- 当前明确禁止依赖：
  - `Chaos.IL2CPP.Loader`
  - `Chaos.IL2CPP.SemanticWorld`
  - `Chaos.IL2CPP.Linker`
  - `Chaos.IL2CPP.MetadataWriter`
  - `Chaos.IL2CPP.Generator`
  - `Chaos.IL2CPP.Pipeline`

这条隔离规则的目标是让 engine-facing binding 入口不反向耦合 pipeline 内部实现。

## 2. managed 标注入口

### `[EngineExport]`

- 用于标注 interface method 是 engine-facing export
- 最小字段：
  - `ExportName`

### `[EngineCallback]`

- 用于标注 interface method 是 callback registration 入口
- 最小字段：
  - `CallbackName`

## 3. `EngineBindingStubEmitter` 当前能力

### 入口

- `EngineBindingStubEmitter.GenerateForInterface(Type interfaceType)`

### 输出约定

- 生成结果包含：
  - `#include "engine_bridge.h"`
  - `[EngineExport]` 方法对应的 compile-only stub
  - `[EngineCallback]` 方法对应的 `engine_register_callback(...)` helper

### v0 限制

当前 baseline 故意只支持最小签名：

- 必须是 interface method
- 必须是 `void` 返回值
- 必须无参数

超过这条边界的 richer signature 不在当前 baseline 内，后续需要新增专门 proof 或扩展 emitter。

### 事实边界

当前 stub baseline 只证明三件事：

- managed 入口名称已经冻结
- 生成的 C++ wrapper 形状稳定
- wrapper 形状可以通过 native compile-only 验证

它不等于：

- 完整 managed body 调用闭环
- 完整参数类型映射
- 完整 host proof

## 4. smoke baseline 入口

### managed smoke

- 合同：
  - `tests/contracts/managed/engine-binding-smoke/SmokeHostExports.cs`
- 执行器：
  - `tests/contracts/managed/engine-binding-smoke/Program.cs`

### baseline 比对命令

```powershell
dotnet run --project tests/contracts/managed/engine-binding-smoke/EngineBindingSmoke.csproj -- --check-baseline tests/contracts/native/engine-binding/compile_only_engine_binding_smoke.cpp
```

用途：

- 用当前 emitter 重新生成 smoke interface 的 wrapper
- 与仓库内沉淀的 native baseline 做逐字比对

## 5. native compile-only 入口

### 目录

- `tests/contracts/native/engine-binding/`

### root CMake 接线

- `CMakeLists.txt` 在 `windows-x64-reference` 下接入：
  - `add_subdirectory(tests/contracts/native/engine-binding)`

### 构建目标

- `chaos_engine_binding_compile_only_smoke`

### 验证命令

```powershell
cmake --preset windows-x64-reference
cmake --build artifacts/presets/windows-x64-reference --config Debug --target chaos_engine_binding_compile_only_smoke
```

用途：

- 证明当前 smoke baseline 不是“文本上看起来像 C++”，而是真实可编译的 wrapper 形状


## 6. 与 host proof 的边界

- 本文只覆盖 Chaos.IL2CPP.EngineBinding 的 managed 标注入口与 compile-only stub baseline。
- 真正的 host lifecycle / ownership / callback 闭环请看：
  - wiki/04-工具与集成/engine-host-proof-baseline-v1.md`r
