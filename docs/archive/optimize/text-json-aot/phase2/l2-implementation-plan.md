# L2 Implementation Plan: BridgeMethodResolver

## Architecture

```
NativeAotLoweringPlanner.Create()
  │
  ├── CollectExternalRuntimeDispatchEntries()
  ├── CollectBridgeImportThunks()
  │       │
  │       └── bridgeImportThunks: { SubjectId → BridgeImportThunkDefinition }
  │
  ├── ResolveBridgedMethods()     ← L2 新增
  │       │
  │       └── BridgeMethodResolver
  │               ├── 对每个桥接方法：
  │               │   ├── 解析 assembly DLL 路径
  │               │   ├── 打开 PEReader + MetadataReader
  │               │   ├── 匹配 SubjectId → MethodDefinitionHandle
  │               │   ├── 加载 IL body
  │               │   ├── 构建 ManagedMethodModel
  │               │   ├── 构建 TypedIlMethodArtifact
  │               │   ├── 调用 TryCreateMethod → AotCoreIrMethodArtifact
  │               │   └── 成功 → 加入 compiledMethods
  │               │        失败 → L3 fallback（保留桥接）
  │               │
  │               └── 返回 compiledMethods
  │
  ├── methodsForLowering += compiledMethods
  │       │
  │       └── codegen 编译新增方法，生成 extern "C" 函数
  │
  ├── 【自动生效】dispatch table 检测到 compiled methods
  │       → hasHelper = true
  │       → 替换桥接为编译体
  │
  └── 【L3 自然兜底】未编译的方法保留桥接 → 解释器
```

## File Layout

```
src/managed/Chaos.IL2CPP.Generator/RuntimeSupport/
├── NativeAotLoweringPlanner.BridgeMethodResolver.cs   ← 新增
└── NativeAotLoweringPlanner.ExternalRuntimeHelpers.cs  ← 修改 (添加调用点)
```

## Key Design

**SubjectId → MethodDefinitionHandle matching**:
```csharp
// SubjectId: "System.Text.Json/JsonDocument::Parse:JsonDocument(System.String,JsonDocumentOptions)"
// → assemblyName: "System.Text.Json"
// → After extracting, find the method in the DLL's MethodDef table
//   by matching declaring type + method name + parameter count

foreach (var methodHandle in metadataReader.MethodDefinitions)
{
    var method = metadataReader.GetMethodDefinition(methodHandle);
    var declaringType = metadataReader.GetTypeDefinition(method.GetDeclaringType());
    var typeName = GetTypeFullName(metadataReader, declaringType);
    var methodName = metadataReader.GetString(method.Name);
    if ($"{typeName}::{methodName}" matches SubjectId)
        → found!
}
```

**Method body loading**: Use `method.GetBody()` → `MethodBodyBlock` → IL instructions

**AOT IR creation**: Use existing `AotCoreIrLowering.TryCreateMethod()` after building `ManagedMethodModel`

## Deliverables

| Sub-phase | Content | Files |
|-----------|---------|-------|
| L2a | BridgeMethodResolver skeleton + assembly path resolution | BridgeMethodResolver.cs |
| L2b | SubjectId → MethodDefinitionHandle matching | BridgeMethodResolver.cs |
| L2c | IL body loading + ManagedMethodModel construction | BridgeMethodResolver.cs |
| L2d | AOT IR injection (TryCreateMethod call + methodsForLowering append) | BridgeMethodResolver.cs + NativeAotLoweringPlanner.cs |
| L2e | Verification: benchmark bridge thunk reduction | — |
