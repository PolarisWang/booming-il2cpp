# Reference Assembly 架构

## 问题背景

在 Native AOT 管线中，entrypoint 程序集的方法经常调用 `System.Private.CoreLib.dll` 中的方法（如 `System.Array::Copy`、`System.Threading.Thread::get_CurrentThread`）。如果 CoreLib 以正常程序集方式加载，其所有方法都会被拉入管线处理，导致：

1. **BFS reachability 扩散** — `ExpandReachableMethods` 将 CoreLib 方法引用的其他 CoreLib 方法也拉入可达集
2. **Generic instantiation projection 冲突** — 跨程序集泛型特化的 `DefinitionSubjectId` 与 entry assembly 产生冲突
3. **IsDefined 崩溃** — `CollectMemberInfoIsDefinedAttributeTypeSubjectIds` 对非常量参数的 `HasCustomAttribute` 调用直接 throw
4. **不必要的 IL body 解码** — CoreLib 方法大部分不需要生成 C++ 代码

## Reference Assembly 概念

Reference Assembly 是一种 "metadata only" 加载模式：程序集只用于 callee 类型/方法签名解析，不参与 codegen 管线。

```
manifest.json:
  assemblies:           [entrypoint.dll]        ← 正常加载（IL body + codegen）
  referenceAssemblies:  [System.Private.CoreLib.dll]  ← metadata only
```

## 关键数据流

```
batch_native_aot_runner.py
  → manifest.json (referenceAssemblies: [corlib])
  → DriverEntry.cs 解析 referenceAssemblies
  → ManagedClosureRequest.ReferenceAssemblyPaths
  → LoaderStage.LoadMultiple()
      → 加载 entry DLL（正常：有 IL body、generic projection、demand graph）
      → 加载 reference assemblies（metadata only：无 IL body、无 generic projection、无 demand graph）
      → 合并到 LoadedWorldModel，附带 ReferenceAssemblyNames
  → SemanticWorldStage.Build()
      → 透传 ReferenceAssemblyNames
      → BodyAvailabilityResolver.Resolve() → ExternalRuntime（无 IL body）
  → LinkerStage.Link()
      → BFS 不展开 reference assembly 方法
      → reference assembly 方法不在 reachable closure 中
  → CodeGenStage.Generate()
      → typedIl: 排除 reference methods（无 IL body）
      → codeRegistration: 不包含 reference methods
      → targetSymbols: 不包含 reference methods
  → AotCoreIrLowering.Create()
      → managedMethods: 包含所有方法（含 reference，供 callee 解析）
      → AotCoreIr.Methods: 不包含 reference methods
  → NativeAotLoweringPlanner
      → InvocationPlanning: TryResolveDirectInvocationTarget()
          → callee 在 methodsBySubjectId（含 reference）→ resolved
          → TargetSymbol → 用 CreateMethodSymbol fallback
          → codegen → ExternalRuntimeHelper
```

## 合约字段

| 合约 | 字段 | 说明 |
|------|------|------|
| `ManagedClosureRequest` | `ReferenceAssemblyPaths` | 路径列表，传给 Loader |
| `LoadedAssemblyModel` | `IsReferenceAssembly` | 标记该加载是否 reference 模式 |
| `SemanticWorldModel` | `ReferenceAssemblyNames` | 程序集名列表，用于快速过滤 |
| `LinkedWorldModel` | `ReferenceAssemblyNames` | 同上，透传到 Linker |

## Loader 层修改

### LoadAssembly 新增 `isReferenceAssembly` 参数

当 `isReferenceAssembly == true` 时：

- **跳过 IL body 解码**：`LoadMethods` 内部不调用 `DecodeMethodBody`，直接构造空 body
- **跳过 generic instantiation projection**：不调用 `BuildGenericInstantiationProjection`
- **跳过 demand graph**：不传 `GenericInstantiationDemandGraph`
- **保留 type/field/method 定义**：TypeDef、FieldDef、MethodDef 解析 + signature 正常进行（供 callee 解析使用）

### LoadMultiple 加载顺序

```
1. 加载 entry assembly（正常模式）
2. 加载 additional assemblies（正常模式）
3. 加载 reference assemblies（metadata only）
4. 合并到 LoadedWorldModel
```

## Linker 层过滤

`ExpandReachableMethods` BFS 展开时跳过 reference assembly 方法：

```csharp
if (IsInternalAssembly(semanticWorld, callee.AssemblyName) &&
    !IsReferenceAssembly(semanticWorld, callee.AssemblyName))  // 新增
{
    pendingMethods.Enqueue(callee);
}
```

## CodeGen 层过滤

- `typedIl` 构建时排除 reference assembly 方法（无 IL body）
- `codeRegistration` 天然不命中 reference assembly（只注册 entry assembly 方法）
- `targetSymbols` 不包含 reference assembly 方法

## Callee 解析路径

CoreLib 方法虽不参与 codegen，但保留在 `_methodsBySubjectId` 字典中。InvocationPlanning 遇到 CoreLib 调用目标时：

1. **直接调用** → `TryResolveDirectInvocationTarget` 匹配已知调用目标（如 `chaos_thread_get_current`、`chaos_delegate_combine` 等）
2. **函数指针** → `GetRequiredFunctionPointerTargetSymbol` 生成 `&ManagedNaming::CreateMethodSymbol` 引用，由 native 链接器解析
3. **ExternalRuntimeHelper** → 通过 `chaos_external_runtime_` 前缀符号路由到运行时帮助函数

## 验证方式

1. `batch_native_aot_runner.py` 全部 32 个 family 通过
2. AOT Core IR 中 CoreLib methods 数为 0
3. 生成的 C++ 代码中没有 CoreLib 方法体（仅有 type_id 常量、struct 定义、类型转换引用）
4. Code Registration 中没有 CoreLib 符号
5. IsDefined 崩溃不再出现
6. Generic instantiation demand 无冲突

## 涉及文件

| 文件 | 位置 |
|------|------|
| `ManagedClosureContracts.cs` | `ReferenceAssemblyPaths` 字段 |
| `ManagedClosureModels.cs` | `IsReferenceAssembly` 属性 |
| `ManagedSemanticWorldContracts.cs` | `ReferenceAssemblyNames` 字段 |
| `DriverEntry.cs` | manifest 解析逻辑 |
| `LoaderStage.cs` | reference assembly 加载循环 |
| `LoaderStage.AssemblyLoading.cs` | `LoadAssembly` 新增参数 |
| `SemanticWorldStage.cs` | 透传 |
| `LinkerStage.cs` | 透传 |
| `LinkerStage.Reachability.cs` | BFS 跳过 |
| `CodeGenStage.cs` | typedIl 排除 |
| `AotCoreIrLowering.cs` | 保留 managedMethods 但排除 AotCoreIr.Methods |
| `batch_native_aot_runner.py` | manifest 写入 `referenceAssemblies` |

## 与 HotUpdate 关系

Reference Assembly 的加载对 HotUpdate 透明：
- HotUpdate 只替换 entry assembly 的方法实现
- CoreLib 始终通过 runtime 提供，不参与 hotupdate 替换
- `ReferenceAssemblyNames` 在 hotupdate 场景下保持不变
