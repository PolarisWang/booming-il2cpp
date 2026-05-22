# NativeAotLoweringPlanner 文件布局

`NativeAotLoweringPlanner` 是 CodeGen 层的中央编排器，以 C# partial class 分布在 **25 个文件**（3 区域 + 模型常量）：

- **Planning/** — 3 个文件，负责 lowering 决策
- **Emission/** — 10 个文件，负责 C++ 代码生成
- **RuntimeSupport/** — 9 个文件，负责运行时支持注册
- **根目录** — 2 个辅助文件（Models + Constants）

## 根目录

| 文件 | 职责 |
|------|------|
| `NativeAotLoweringPlanner.cs` | 主入口、plan 装配、LoweringPlan 构建 |
| `NativeAotLoweringPlanner.Models.cs` | LoweringPlan 数据结构、PlanNode 定义 |
| `NativeAotLoweringPlanner.Constants.cs` | 字符串 ID、opcode 常量 |

## Planning 区域（3 个文件）

决策层：分析 IL 语义，选择 lowering 策略。**当前阶段只有 3 个文件**，其余降级策略（计算、控制流、数组、装箱、异常、委托等）直接实现在 Emission 区域内。

| 文件 | 职责 |
|------|------|
| `Planning/NativeAotLoweringPlanner.InvocationPlanning.cs` | 方法调用 lowering 决策（直接调用、虚调用、接口调用、去虚化分析） |
| `Planning/NativeAotLoweringPlanner.MetadataSupport.cs` | metadata 初始化发射决策 |
| `Planning/NativeAotLoweringPlanner.StaticInitializationPlanning.cs` | 静态初始化规划 |

## Emission 区域（11 个文件）

按功能领域划分，而非按 IL opcode 一对一映射：

| 文件 | 职责 |
|------|------|
| `Emission/NativeAotLoweringPlanner.MethodEmission.cs` | 方法体发射入口、指令派发、结构化控制流发射、虚方法/接口方法 dispatch |
| `Emission/NativeAotLoweringPlanner.ExceptionEmission.cs` | EH C++ 生成（5种 shape），InlineShapeDescriptor 调用折叠、enum.ToString 常量折叠、box+ToString fusion→ChaosEnumToStringRaw |
| `Emission/NativeAotLoweringPlanner.StructuredControlFlow.cs` | CFG 构建、基本块分析、结构化恢复算法 |
| `Emission/NativeAotLoweringPlanner.ObjectModelEmission.cs` | 对象模型发射（TypeInfo、VTable 数组、iface_map、对象创建） |
| `Emission/NativeAotLoweringPlanner.ObjectModelUtilities.cs` | SubjectId 工具函数、stable_id 计算、类型名解析 |
| `Emission/NativeAotLoweringPlanner.ObjectEqualityEmission.cs` | 对象相等性比较发射 |
| `Emission/NativeAotLoweringPlanner.StringIdEmission.cs` | 字符串表发射 |
| `Emission/NativeAotLoweringPlanner.ReflectionObjectEmission.cs` | 反射对象发射 |
| `Emission/NativeAotLoweringPlanner.GenericRegistration.cs` | 泛型注册表发射（Gen-2 Handle scheme） |
| `Emission/NativeAotLoweringPlanner.ModuleRegistration.cs` | ModuleRegistrationV0 发射 |
| `Emission/NativeAotLoweringPlanner.GeneratedModule.cs` | A1+A2 生成：typed dispatch table header (chaos_generated_module.h) + dispatch wiring source (chaos_generated_module.cpp) |

## RuntimeSupport 区域（9 个文件）

运行时支持注册 — 将 C# helper 方法的地址注册到 C++ 运行时侧：

| 文件 | 职责 |
|------|------|
| `RuntimeSupport/NativeAotLoweringPlanner.RuntimeSupport.cs` | Runtime support 注册入口 |
| `RuntimeSupport/NativeAotLoweringPlanner.RuntimePrelude.cs` | 前置代码生成（includes、前向声明） |
| `RuntimeSupport/NativeAotLoweringPlanner.StaticInitializationEmission.cs` | 静态初始化代码发射 |
| `RuntimeSupport/NativeAotLoweringPlanner.ExternalRuntimeHelpers.cs` | 外部运行时 helper 注册入口 |
| `RuntimeSupport/NativeAotLoweringPlanner.ExternalRuntimeHelpers.TypeResolution.cs` | 类型解析 helper 注册 |
| `RuntimeSupport/EnumMetadataExtractor.cs` | Enum metadata C++ 头生成：constexpr field arrays、FNV-24 dispatch table（排序数组+二分查找）、静态 registrar（ChaosEnumRegisterDispatchTable） |
| `RuntimeSupport/NativeAotLoweringPlanner.ExternalRuntimeHelpers.Assertions.cs` | 断言 helper 注册 |
| `RuntimeSupport/NativeAotLoweringPlanner.ExternalRuntimeHelpers.StringAndPlatform.cs` | 字符串与平台 helper 注册 |
| `RuntimeSupport/NativeAotLoweringPlanner.ExternalRuntimeHelpers.InvocationAbi.cs` | 调用 ABI helper 注册 |
| `RuntimeSupport/NativeAotLoweringPlanner.ExternalRuntimeHelpers.CollectionAndReflection.cs` | 集合与反射 helper 注册 |

## 文件分布变迁说明

Wiki 此前列出的文件清单（如 `Computation.cs`、`Emit_MethodCall.cs`、`RuntimeSupport.String.cs` 等）对应的是**重构前的规划结构**。目前的实际架构按功能领域划分：

- **Planning 精简**：降低策略集中到 `InvocationPlanning.cs`，其余 IL opcode 的 lowering 决策直接内联在 Emission 中完成
- **Emission 聚合**：不再按 `Emit_*` 一对一映射 IL opcode，而是按概念领域（对象模型、字符串、异常、反射、泛型等）
- **RuntimeSupport 拆分**：`ExternalRuntimeHelpers.*` 按功能子域（类型解析、断言、字符串、调用 ABI、集合反射）拆分为 5 个文件
- **新增模块**：`GenericRegistration.cs`、`ModuleRegistration.cs`、`ObjectModelUtilities.cs` 等为 gen-2 架构新增

如需将部分 Emission 逻辑重构回 Planning 层（如 `Emit_Computation.cs` → `Planning/Computation.cs`），这是架构演进的方向，但当前阶段 Emission 仍为本体。
