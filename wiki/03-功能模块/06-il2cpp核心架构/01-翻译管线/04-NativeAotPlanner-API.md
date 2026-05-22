# NativeAot Codegen API 文档

> 适用版本: 2026-05  
> 本文档覆盖托管 AOT 代码生成的全套公共 API，包括 `NativeAotEmitter`、`NativeAotLoweringPlanner`、`NativeAotTemplateModel` 以及关键内部方法，供 codegen 管线集成与调试使用。

---

## 1. 架构概览

```
[Managed Closure Artifacts] (JSON on disk)
    ↓ NativeAotEmitter.Generate()
[NativeAotLoweringPlanner.Create()]  ← 核心编排
    ├── ValidateEntryMethod()
    ├── Build method/type indexes (SubjectId → Artifact)
    ├── ComputeAotReachableSubjectIds()  ← call-graph reachability
    ├── Build dispatch slot map
    ├── Emit all sections (ObjectModel, Methods, EH, GC, etc.)
    └── → NativeAotTemplateModel
    ↓
[Scriban Template Rendering]  ← NativeAotTemplateCatalog
    ├── NativeAot.TranslationUnit.cpp.scriban
    ├── NativeAot.Method.cpp.scriban
    ├── NativeAot.ObjectModel.cpp.scriban
    └── ... 40+ template files
    ↓
[NativeAotResult]  → Generated C++ files
```

### 数据流

1. **输入阶段** — 从 closure artifacts（JSON）加载 `AotCoreIrArtifact`、`ManagedClosureManifestArtifact`、`MetadataRegistrationArtifact`、`SupplementalMetadataTemplateArtifact`
2. **规划阶段** — `NativeAotLoweringPlanner.Create()` 构建索引、计算可达性、分配 dispatch slot
3. **发射阶段** — 按领域发射 C++ 代码段（对象模型、方法体、异常处理、GC slot map、泛型注册、模块注册等）
4. **渲染阶段** — `NativeAotEmitter` 将 planner 输出的 `NativeAotTemplateModel` 灌入 Scriban 模板生成完整 .cpp/.h 文件
5. **验证阶段** — `NativeCodegenValidator` 对每个生成文件做 9 条规范校验

---

## 2. NativeAotEmitter（入口）

**文件**: `src/managed/Chaos.IL2CPP.Generator/NativeAotEmitter.cs`

`NativeAotEmitter` 是 codegen 管线的唯一公共入口。调用者通过它触发从 closure artifacts 到 C++ 文件的完整转换。

### 2.1 Generate

```csharp
public NativeAotResult Generate(NativeAotRequest request)
```

**用途**: 从磁盘上的 closure artifacts 触发完整 codegen 管线。

| 参数 | 类型 | 说明 |
|------|------|------|
| `request` | `NativeAotRequest` | 包含 `ManagedClosureRootPath`（artifacts 目录）、`OutputRootPath`（输出目录）、`Mode`（Aot/Reference） |

**返回**: `NativeAotResult` — 包含 `OutputRootPath`、`LoweringPlan`、`Manifest`、`CodegenMetrics`、`GeneratedSources`。

**调用链路**:
1. 从 `ManagedClosureRootPath` 加载 4 个 JSON artifact
2. 调用 `GenerateFromArtifacts()`
3. 返回 `NativeAotResult`

### 2.2 GenerateFromArtifacts

```csharp
public NativeAotResult GenerateFromArtifacts(
    NativeAotLoweringPlanArtifact loweringPlan,
    AotCoreIrArtifact aotCoreIr,
    ManagedClosureManifestArtifact closureManifest,
    MetadataRegistrationArtifact metadataRegistration,
    SupplementalMetadataTemplateArtifact supplementalMetadataTemplate,
    string outputRootPath,
    CodegenMode mode = CodegenMode.Aot)
```

**用途**: 从内存中的 artifacts 直接触发 codegen（无 JSON 序列化开销），供 `FullAssemblyEmitter` 等内部调用者使用。

**调用链路**:
1. `ValidateLoweringPlan()` — 校验 `PlanKind`、`EntrySubjectId`、`WorkloadAbi` 等必填字段
2. `LoadEntryMethod()` — 从 `aotCoreIr.Methods` 找到 entry 方法
3. `planner.Create()` — 创建 `NativeAotTemplateModel`
4. `BuildGeneratedSources()` — 渲染 Scriban 模板生成 C++ 文件
5. `NativeCodegenMetricsBuilder.Build(...)` — 构建 codegen 度量指标
6. `NativeCodegenValidator.ValidateContent()` — 对每个生成文件做规范校验
7. 返回 `NativeAotResult`

---

## 3. NativeAotRequest / NativeAotResult

### 3.1 NativeAotRequest

**文件**: `src/managed/Chaos.IL2CPP.Contracts/ManagedClosureArtifactModels.cs`

```csharp
public sealed record NativeAotRequest(
    string ManagedClosureRootPath,
    string OutputRootPath,
    CodegenMode Mode = CodegenMode.Aot
);
```

### 3.2 NativeAotResult

```csharp
public sealed record NativeAotResult
{
    public required string OutputRootPath { get; init; }
    public required NativeAotLoweringPlanArtifact LoweringPlan { get; init; }
    public required NativeAotManifestArtifact Manifest { get; init; }
    public required NativeCodegenMetricsArtifact CodegenMetrics { get; init; }
    public required IReadOnlyList<NativeAotGeneratedSource> GeneratedSources { get; init; }
}
```

---

## 4. NativeAotLoweringPlanner（核心编排器）

**文件**: `src/managed/Chaos.IL2CPP.Generator/NativeAotLoweringPlanner.cs` (partial class, 20+ 文件)

以 C# partial class 分布在 **25 个文件**中（3 区 + 根目录），见 [NativeAotLoweringPlanner 文件布局](04-NativeAotLoweringPlanner文件布局.md)。

### 4.1 Create — 入口方法

```csharp
public NativeAotTemplateModel Create(
    NativeAotLoweringPlanArtifact loweringPlan,
    AotCoreIrArtifact aotCoreIr,
    AotCoreIrMethodArtifact entryMethod,
    ManagedClosureManifestArtifact closureManifest,
    MetadataRegistrationArtifact metadataRegistration,
    SupplementalMetadataTemplateArtifact supplementalMetadataTemplate,
    bool fullAssemblyMode = false,
    CodegenMode mode = CodegenMode.Aot)
```

**用途**: 创建包含所有生成 C++ 代码段的 `NativeAotTemplateModel`。是 planner 的唯一公共方法。

| 参数 | 说明 |
|------|------|
| `loweringPlan` | lowering 计划，包含 `EntrySubjectId`、`AssemblyName`、`WorkloadAbi`、TU paging 信息 |
| `aotCoreIr` | AOT Core IR 产物，包含 `Methods[]`（每个方法有主体 + SEH clauses） |
| `entryMethod` | entry point 方法（`fullAssemblyMode=true` 时可为 null） |
| `closureManifest` | 闭包清单，包含所有引用的 assembly、type、method |
| `metadataRegistration` | 元数据注册，包含 `methodsBySubjectId`、`typesBySubjectId` |
| `supplementalMetadataTemplate` | 补充元数据模板 |
| `fullAssemblyMode` | 全 assembly 模式 — 不设单 entry point，所有方法都是 AOT-reachable |
| `mode` | `Aot` 或 `Reference` |

**执行阶段**（按顺序）:

1. **Phase 0: 数据准备**
   - 构建 `_methodsBySubjectId` 索引
   - 收集 instantiated type（newobj/box/constrained.）
   - 构建 module symbol table（same-module 方法 → 直接 C++ 调用）
   - 构建 devirtualization hints

2. **Phase 1: 索引构建**
   - `BuildAttributeStorageFieldIndex()`
   - `CollectReferenceTypeBaseSubjectIds()` / `CollectReferenceTypeImplementedInterfaceSubjectIds()`
   - `CollectValueTypeSubjectIds()` / `CollectSealedTypeSubjectIds()` / `CollectInterfaceTypeSubjectIds()`
   - `BuildGenericSharingCanonicalMap()`
   - `CollectComInterfaceVtableData()`

3. **Phase 2: 方法排序 + 可达性**
   - `CollectReachableMethods()` / `CollectAllMethods()`
   - 按 numeric sort key 排序（确保 `Subject_10` 在 `Subject_1` 之后）
   - 补充 COM interface 实现方法（interface dispatch 可能不被静态 call-graph 发现）
   - `ComputeAotReachableSubjectIds()` — BFS 从 entry 出发的 call-graph 可达性
   - `BuildDispatchSlotMap()` — 构建 `NativeSymbol → slot number` 映射
   - `CollectStringLiterals()`

4. **Phase 3: C++ 代码发射**
   - `EmitObjectModelDeclarations()` — TypeInfoV0 / VTable / IfaceMap
   - `EmitMethodDeclarations()` — 函数前向声明
   - `EmitMethodBodies()` — 遍历每个方法，调用 `EmitMethod()` 生成函数体
   - `EmitDispatchEntryCode()` — RunNativeAot dispatch table
   - `BuildGcSlotMapSection()` — GC slot map segment
   - `BuildExternalRuntimeDispatchTable()` — 外部运行时 helper 表
   - `EmitStringIdTable()` / `EmitObjectEqualityCode()` / `EmitReflectionObjects()`
   - `EmitRuntimePrelude()` / `EmitCodeRegistration()`
   - `BuildGeneratedModuleHeader()` / `BuildGeneratedModuleSource()`（A1+A2 typed dispatch table）
   - `EmitGenericRegistration()` / `EmitModuleRegistration()`
   - `BuildDispatchEntryCode()` — verification dispatch manifest (JSON)

### 4.2 类型映射方法

以下方法用于在 IL 类型和 C++ 类型之间做映射：

| 方法 | 签名 | 用途 |
|------|------|------|
| `GetPrimitiveType` | `(PrimitiveTypeCode typeCode) → string` | 将 ELEMENT_TYPE_* 映射到 C++ 类型名（如 `int32_t`、`double`） |
| `GetSystemType` | `() → string` | 返回 `System.Type` 的 C++ 类型名 |
| `GetSZArrayType` | `(string elementType) → string` | 返回单维数组的 C++ 类型名 |
| `GetTypeFromDefinition` | `(MetadataReader, TypeDefinitionHandle, byte) → string` | 从 TypeDef 解析完整类型名 |
| `GetTypeFromReference` | `(MetadataReader, TypeReferenceHandle, byte) → string` | 从 TypeRef 解析完整类型名 |
| `GetTypeFromSerializedName` | `(string name) → string` | 从序列化名解析类型 |
| `GetUnderlyingEnumType` | `(string type) → PrimitiveTypeCode` | 获取枚举的底层整数类型 |
| `GetTypeName` | `(MetadataReader, TypeDefinitionHandle/TypeReferenceHandle) → string` (static) | 从 metadata handle 获取类型名 |
| `IsSystemType` | `(string type) → bool` | 判断是否为 `System.Type` |

### 4.3 度量字段

| 字段 | 类型 | 说明 |
|------|------|------|
| `StructuredMethodCount` | `int` | 成功结构化恢复的方法数 |
| `StructuredExceptionBodyCount` | `int` | 成功结构化恢复的 EH 方法数 |
| `FlatFallbackCount` | `int` | 回退到 flat goto 发射的方法数 |
| `TotalMethodCount` | `int` | 总方法数 |
| `AotReachableMethodCount` | `int` | AOT call-graph 可达的方法数 |
| `AotUnreachableMethodCount` | `int` | AOT call-graph 不可达的方法数 |

---

## 5. NativeAotTemplateModel（输出数据模型）

**文件**: `src/managed/Chaos.IL2CPP.Generator/NativeAotLoweringPlanner.cs`（末尾）

`NativeAotTemplateModel` 是 planner 的输出，封装了所有需要渲染到 Scriban 模板的 C++ 代码段和数据。

| 属性 | 类型 | 说明 |
|------|------|------|
| `Includes` | `IReadOnlyList<string>` | C++ #include 列表 |
| `ObjectModelCode` | `string` | TypeInfoV0 内联定义 |
| `MethodDeclarations` | `IReadOnlyList<string>` | 函数前向声明 |
| `Methods` | `IReadOnlyList<NativeAotMethodTemplateModel>` | 方法体列表（含 SubjectId + MethodSource） |
| `EntrySubjectId` | `string` | Entry point 的 subjectId |
| `EntrySymbol` | `string` | Entry point 的调度符号 |
| `EntryNativeSymbol` | `string` | Entry point 的 C++ native symbol |
| `NativeEntryFunctionName` | `string` | C++ native entry 函数名 |
| `EntryBridgeArguments` | `string` | Entry bridge 参数声明 |
| `ShapeDispatchHeaderContent` | `string` | Shape dispatch header（.h 文件） |
| `EnumMetadataHeaderContent` | `string` | Enum metadata header（空串表示无 enum 类型） |
| `TypeDeclarationsCode` | `string` | 跨 TU TypeInfoV0 extern 声明（仅 TU paging 模式） |
| `WorkloadAbi` | `string` | Workload ABI 签名（如 `"int(int32)"`） |
| `GenericRegistrationCode` | `string` | 泛型注册代码段 |
| `ModuleRegistrationCode` | `string` | 模块注册代码段 |
| `GlobalDeclarations` | `string` | 文件作用域全局变量 |
| `ManifestJson` | `string` | 验证 dispatch 用 JSON manifest |
| `CodegenNamespace` | `string` | C++ namespace（如 `chaos::il2cpp::codegen::*`） |
| `GeneratedModuleHeaderContent` | `string` | A1 typed dispatch table header（空串表示未激活） |
| `GeneratedModuleSourceContent` | `string` | A2 dispatch wiring source（空串表示未激活） |

---

## 6. 关键内部方法索引

以下各区域的 internal 方法供 codegen 维护者参考。

### 6.1 Emission 区域

#### MethodEmission.cs — 方法体发射入口

| 方法 | 说明 |
|------|------|
| `EmitMethodBodies(...)` | 遍历所有方法，调用 `EmitMethod()` |
| `EmitMethod(...)` | 单方法发射：CFG 构建 → 结构化恢复 → 指令 lowering |
| `LowerInstruction(...)` | 单条 AotCoreIr 指令 → C++ 代码段 |
| `EmitLinearCall(...)` | 线性方法调用发射（直接调用 / devirtualized 调用 / extern table） |

#### StructuredIR.cs — 结构化 IR 恢复

| 方法 | 说明 |
|------|------|
| `TryBuildStructuredExceptionMethodBody(...)` | 尝试 EH 方法结构化恢复，通过 5 种 shape detector 链 |
| `EmitIRExceptionRegion(...)` | 发射 `CHAOS_EH_TRY/CATCH/END` 等宏，处理 TryCatch/TryFinally/TryFilter 三种 IRKind |
| `BuildStructuredMethodBody(...)` | 非 EH 方法的结构化恢复 |
| `AllocatePushTarget()` | 分配新的结构化 push target |
| `PopValue()` / `PeekValue()` / `Discard()` / `RestoreDepth()` | 结构化 IR 的栈深度管理 |

#### ExceptionEmission.cs — 异常处理发射

| 方法 | 说明 |
|------|------|
| (Internal EH emission helpers) | 5 种 EH shape 的 flat goto 备份路径发射 |
| (InlineShapeDescriptor folding) | 调用折叠（如 `box` + `ToString()` → `ChaosEnumToStringRaw`） |
| (enum.ToString 常量折叠) | `ldsfld` enum 字段 → 常量字符串折叠 |

#### StructuredControlFlow.cs — CFG 构建

| 方法 | 说明 |
|------|------|
| (CFG builder internals) | 基本块分析、控制流图构建、结构化恢复算法 |

#### GcSlotMap.cs — GC Slot Map 发射

| 方法 | 说明 |
|------|------|
| `BuildGcSlotMapSection(...)` | 构建 GC slot map section（packed struct 数组），包含 frame_size、slot 偏移列表 |

#### GeneratedModule.cs — A1+A2 Typed Dispatch Table

| 方法 | 说明 |
|------|------|
| `BuildGeneratedModuleHeader(...)` | 生成 `chaos_generated_module.h`（typed function pointer 数组） |
| `BuildGeneratedModuleSource(...)` | 生成 `chaos_generated_module.cpp`（extern wiring + ChaosRuntimeHost class） |

#### 其他 Emission 文件

| 文件 | 职责 |
|------|------|
| `ObjectModelEmission.cs` | TypeInfo / VTable / IfaceMap 对象模型发射 |
| `ObjectModelUtilities.cs` | SubjectId 工具函数、stable_id 计算、类型名解析 |
| `ObjectEqualityEmission.cs` | 对象相等性比较发射 |
| `StringIdEmission.cs` | 字符串表发射 |
| `ReflectionObjectEmission.cs` | 反射对象发射（FieldInfo / MethodInfo / PropertyInfo） |
| `GenericRegistration.cs` | 泛型注册表发射（Gen-2 Handle scheme） |
| `ModuleRegistration.cs` | ModuleRegistrationV0 发射 |
| `GenericSharing.cs` | 泛型共享 canonical map 构建 |

### 6.2 Planning 区域

| 文件 | 方法 | 说明 |
|------|------|------|
| `InvocationPlanning.cs` | (invocation planning internals) | 方法调用 lowering 决策：直接调用、虚调用、接口调用、去虚化分析 |
| `MetadataSupport.cs` | (metadata support internals) | metadata 初始化发射决策 |
| `StaticInitializationPlanning.cs` | (static init planning internals) | 静态初始化规划 |

### 6.3 RuntimeSupport 区域

| 文件 | 说明 |
|------|------|
| `RuntimeSupport.cs` | Runtime support 注册入口 |
| `RuntimePrelude.cs` | 前置代码生成（#includes、前向声明） |
| `StaticInitializationEmission.cs` | 静态初始化代码发射 |
| `ExternalRuntimeHelpers.cs` | 外部运行时 helper 注册入口 |
| `ExternalRuntimeHelpers.TypeResolution.cs` | 类型解析 helper 注册 |
| `ExternalRuntimeHelpers.Assertions.cs` | 断言 helper 注册 |
| `ExternalRuntimeHelpers.StringAndPlatform.cs` | 字符串与平台 helper 注册 |
| `ExternalRuntimeHelpers.InvocationAbi.cs` | 调用 ABI helper 注册 |
| `ExternalRuntimeHelpers.CollectionAndReflection.cs` | 集合与反射 helper 注册 |

---

## 7. NativeAotTemplateCatalog（Scriban 模板目录）

**文件**: `src/managed/Chaos.IL2CPP.Generator/Emission/NativeAot/NativeAotTemplateCatalog.cs`

提供 40+ 个 Scriban 模板的加载入口。每个模板对应一个 `.scriban` 文件，位于 `Templates/` 目录。

### 核心模板

| 方法 | 模板路径 | 说明 |
|------|---------|------|
| `GetTranslationUnitTemplate()` | `NativeAot.TranslationUnit.cpp.scriban` | 翻译单元主模板 |
| `GetObjectModelTemplate()` | `NativeAot.ObjectModel.cpp.scriban` | 对象模型子模板 |
| `GetMethodTemplate()` | `NativeAot.Method.cpp.scriban` | 方法体子模板 |
| `GetRuntimePreludeTemplate()` | `NativeAot.RuntimePrelude.cpp.scriban` | 前置代码子模板 |

### 功能模板

| 方法 | 说明 |
|------|------|
| `GetStaticInitializationDefinitionTemplate()` | 静态初始化定义 |
| `GetStaticInitializationActionTemplate()` | 静态初始化动作 |
| `GetStaticInitializationCallTemplate()` | 静态初始化调用 |
| `GetMethodReturnTemplate()` | 方法返回 ABI |
| `GetAbiReturnPushTemplate()` | ABI 返回值 push |
| `GetAbiArgumentInitializationTemplate()` | ABI 参数初始化 |
| `GetStringJoinInt32EnumerableTemplate()` / `GetStringJoinStringEnumerableTemplate()` | string.Join 展开 |
| `GetSimpleExternalRuntimeHelperTemplate()` | 简单外部 runtime helper |
| `GetReflectionGetObjectTypeTemplate()` / `GetReflectionCreateInstanceTemplate()` | 反射操作 |
| `GetRuntimeHelperShapeDispatchHeaderTemplate()` | Shape dispatch header |
| `GetTypeDeclarationsTemplate()` | 跨 TU 类型声明 |
| `GetStringIdTableTemplate()` | 字符串 ID 表 |
| `GetObjectEqualityTemplate()` | 对象相等性 |
| `GetGenericRegistrationTemplate()` / `GetGenericAotRegistrationTemplate()` / `GetGenericRegistrationHelperTemplate()` | 泛型注册 |
| `GetModuleRegistrationTemplate()` | 模块注册 |
| `GetDispatchEntryCodeTemplate()` | Dispatch entry 代码 |
| `GetCodeRegistrationTemplate()` | CodeRegistration 结构体 |
| `GetAbiManifestTemplate()` | ABI manifest |
| `GetExternalRuntimeDispatchTableTemplate()` | 外部 runtime dispatch 表 |
| `GetHotpatchTableTemplate()` | Hotpatch 表 |
| `GetReflectionQueryImageTemplate()` | 反射查询映像 |
| `GetMethodTableInitializationTemplate()` | MethodTable 初始化 |
| `GetDelegateRuntimeSupportTemplate()` / `GetDelegateThunksTemplate()` | 委托运行时支持 |
| `GetStructMarshallingDescriptorsTemplate()` | 结构体封送描述符 |
| `GetGeneratedModuleHeaderTemplate()` / `GetGeneratedModuleSourceTemplate()` | A1+A2 typed dispatch table |
| `GetInterfaceTypeIdTemplate()` / `GetIfaceMapArrayTemplate()` | Interface 映射 |
| `GetTypeInfoTemplate()` / `GetVTableArrayTemplate()` | TypeInfo/VTable |

---

## 8. 使用示例

### 8.1 标准调用（从磁盘 artifacts）

```csharp
var emitter = new NativeAotEmitter();
var result = emitter.Generate(new NativeAotRequest(
    ManagedClosureRootPath: "/path/to/closure/artifacts",
    OutputRootPath: "/path/to/output",
    Mode: CodegenMode.Aot
));

// 生成的 C++ 文件
foreach (var source in result.GeneratedSources)
{
    Console.WriteLine($"{source.RelativePath}: {source.Contents.Length} chars");
}

// Codegen 度量指标
var metrics = result.CodegenMetrics;
Console.WriteLine($"Structured recovery rate: {metrics.StructuredRecoveryRate}");
Console.WriteLine($"Flat fallbacks: {metrics.FlatFallbackCount}");
```

### 8.2 内存 artifacts 调用（无 JSON 序列化）

```csharp
var emitter = new NativeAotEmitter();
var result = emitter.GenerateFromArtifacts(
    loweringPlan, aotCoreIr, closureManifest,
    metadataRegistration, supplementalMetadataTemplate,
    outputRootPath: "/path/to/output",
    mode: CodegenMode.Aot
);
```

### 8.3 快照测试中调用

```csharp
var planner = new NativeAotLoweringPlanner();
var templateModel = planner.Create(
    loweringPlan, aotCoreIr, entryMethod,
    closureManifest, metadataRegistration,
    supplementalMetadataTemplate);

// 验证特定方法的 C++ 输出
var methodModel = templateModel.Methods
    .First(m => m.SubjectId.Contains("TargetMethod"));
Assert.Contains("expected_pattern", methodModel.MethodSource);
```

---

## 9. NativeCodegenValidator（代码规范校验）

**文件**: `src/managed/Chaos.IL2CPP.Generator/Validation/NativeCodegenValidator.cs`

9 条校验规则，在 `GenerateFromArtifacts()` 末尾自动执行：

| # | 规则 | 严重性 |
|---|------|--------|
| 1 | 禁止 `using namespace std;` 或 `using namespace chaos;` | Error |
| 2 | 禁止 `using std::...` 或 `using chaos::...` | Error |
| 3 | 禁止 `NULL` → 用 `nullptr` | Error |
| 4 | 优先使用 `<cstdint>` 类型 | Warning |
| 5 | 头文件必须有 `CHAOS_IL2CPP_*_H_` include guard | Warning |
| 6 | 禁止 `chaos_` 前缀的 ChaosIl2cpp::Common 符号 | Error |
| 7 | .cpp 文件必须包含 `<chaos/common.h>` | Error |
| 8 | 禁止原始 `std::` 类型 → 用 `CHAOS_IL2CPP_*` 宏 | Error |
| 9 | 禁止原始 `<cstdint>` 类型 → 用宏 | Error |

---

## 10. 扩展指南

### 10.1 添加新的 Scriban 模板

1. 在 `Templates/` 目录下创建 `.scriban` 文件
2. 在 `NativeAotTemplateCatalog.cs` 中添加常量路径 + `Get*Template()` 方法
3. 在对应的 Emission 文件中添加构建 `ScriptObject` 和渲染的代码

### 10.2 添加新的 Emission 文件

1. 在 `Emission/` 目录下创建 `NativeAotLoweringPlanner.<Subsystem>.cs`
2. 声明 `public sealed partial class NativeAotLoweringPlanner`
3. 在 `NativeAotLoweringPlanner.Create()` 的 Phase 3 中插入调用

### 10.3 添加新的 validator 规则

1. 在 `NativeCodegenValidator.cs` 的 `ValidateContent()` 中添加检查逻辑
2. 如果规则定义为 Error，违反会导致 build warning 但不会阻止输出
