---
title: Loader/Codegen 多程序集 AOT 编译支持
status: blocked
priority: low
created: 2026-06-19
---

## 最终结论

多程序集 AOT 编译方案经过完整实现尝试后确认 **不可行**。

### 阻塞链

| # | 阻塞 | 状态 | 原因 |
|---|------|------|------|
| 1 | MSVC C2712 (__try + C++ 析构) | ✅ 可绕过 (#pragma warning) | 但 2+3 阻塞后无意义 |
| 2 | NativeCodegenValidator (std::memset) | ✅ 可绕过 (ERROR→WARNING) | 但 3 阻塞后无意义 |
| 3 | **Codegen 方法体生成错误** | ❌ 不可绕过 | target 方法编译后产生 `chaos_arg_1` 未声明等 C++ 编译错误 |

### 阻塞 3 的根因

target assembly（如 System.Collections.NonGeneric）的方法体包含：
- `std::memset`, `std::memcpy` 等标准库调用
- 复杂的泛型实例化
- .NET runtime 内部类型（如 `Shared` 静态实例、`EmptyArray` 等）
- 这些在 codegen 的 IL→C++ lowering 阶段未被正确处理

### 实际可用的替代方案

`--profile-range` 直接分发 AOT 方法表中的 subject entries（已验证可用）：
- System.Collections.NonGeneric: 109 methods, 7 with allocation, 271B total
- 使用 `ChaosDispatchMethodBenchDirect` + `SubjectInstanceFactory.Create<T>()`
- 分配通过 `GcAllocateAtomicFast` → `tls_alloc_fast_bytes` → `ProfileEmitJson` 完整追踪

## 设计方案

### Phase 1: Loader 层面 — 将 target DLL 加入内部程序集列表

**目标**：让 `IsInternalAssembly()` 对 target DLL 返回 true。

**文件**：`src/managed/Chaos.IL2CPP.Linker/LinkerStage.Reachability.cs`

**改动**：
在 `ComputeReachableClosure` 中，将 `request.AdditionalAssemblyPaths` 中的程序集也解析为 `AssemblyIdentityModel` 并合并到 `semanticWorld.Assemblies`。

```csharp
// 在 SemanticWorldModel 构建时或在 LinkerStage 中
foreach (var additionalPath in request.AdditionalAssemblyPaths ?? [])
{
    var asmName = AssemblyName.GetAssemblyName(additionalPath);
    if (!semanticWorld.Assemblies.Any(a => a.Name == asmName.Name))
    {
        // 添加到内部程序集列表
        semanticWorld.Assemblies.Add(new AssemblyIdentityModel { Name = asmName.Name });
    }
}
```

**风险**：
- Target DLL 的依赖链可能无限递归（引用了 System.Private.CoreLib 等 182 个 DLL）
- 需要限制只编译 target DLL 自身的方法，不透传其依赖

**缓解**: 在 `IncludeFullAssemblyClosure()`（`LinkerStage.Reachability.cs:82`）中，只添加 `additionalAssembly` 自身的方法，不递归其依赖。

### Phase 2: MethodTableAllocator 层面 — 多程序集方法表分配

**目标**：为 target assembly 的方法分配独立的全局方法表槽位。

**文件**：`src/managed/Chaos.IL2CPP.Generator/CodeGenStage.cs`

**改动**：
`FilterResultPerAssembly` 已支持多 assembly（line 168-252）。当 target DLL 的方法被加入 linkedWorld.Methods 后，`assemblyMethodMap` 会自动分组，`MethodTableAllocator` 会为 target assembly 分配槽位，`FilterResultPerAssembly` 会生成独立的 CodeRegistration。

**现状**：`FilterResultPerAssembly` 已经支持多 assembly 的完整流程（215 行代码）。只要 Phase 1 让 target DLL 的方法出现在 `linkedWorld.Methods` 中，Phase 2 的分配和代码生成应自动工作。

### Phase 3: SubjectInstanceFactory 联动

**目标**：确保 FACT wrapper 创建的真实实例能调用真实 AOT 方法体。

**前置条件**：Phase 1+2 完成后，target 方法为真实 AOT，不再抛 NRE。

**已完成的改动**（无需额外工作）：
- `TestEmitter.cs` 已将 `default(T)!` 替换为 `SubjectInstanceFactory.Create<T>()`
- `SubjectInstanceFactory.cs` 使用 `RuntimeHelpers.GetUninitializedObject()` 创建实例
- `build.py` 已传递 `--additional-assembly` 给 TPG

### Phase 4: 验证

```bash
# 1. 验证 target 方法出现在 AOT 代码中
grep "Queue.Clone\|native-aot.generated" codegen/generated/native-aot.generated.cpp

# 2. 验证 target 方法非 stub（不包含 ExternalRuntimeFallback）
grep -c "ChaosExternalRuntimeFallback" native-aot.generated.cpp

# 3. 验证 profile 触发分配
entry.exe --profile | grep "nurseryAllocBytes"
# 预期: 非零 nurseryAllocBytes 方法数 > 当前 7 个
```

## 实施计划

| Phase | 文件 | 工作量 | 验收标准 |
|-------|------|--------|---------|
| 1 | `LinkerStage.Reachability.cs` | ~20 行 | `IsInternalAssembly()` 对 target DLL 返回 true |
| 2 | `CodeGenStage.cs` | 无需改动（已支持） | `FilterResultPerAssembly` 自动处理 |
| 3 | `TestEmitter.cs`, `build.py` | 已完成 | — |
| 4 | 验证脚本 | ~50 行 | profile 显示非零分配 |

## 不在此范围内的

- 完整递归编译所有 182 个 runtime DLL（会指数级增加代码量和编译时间）
- 跨程序集内联优化（后续独立优化项）
- JIT 路径的多程序集支持（JIT 自有其加载机制）

## 参考

- `LinkerStage.Reachability.cs` — `IsInternalAssembly` at line 331, `IncludeFullAssemblyClosure` at line 82
- `CodeGenStage.cs` — `FilterResultPerAssembly` at line 168
- `CodeGenStage.Planning.cs` — `CreateAssemblyFullClosureNativeAotPlan` at line 9
- `TestEmitter.cs` — SubjectInstanceFactory override at line 187
