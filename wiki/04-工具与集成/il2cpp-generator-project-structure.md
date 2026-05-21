# Chaos.IL2CPP.Generator — 项目结构与规范

## 1. 概述

`Chaos.IL2CPP.Generator` 是 IL2CPP 管线的正式代码生成项目。它包含 planner（语义分析、ABI/dispatch/EH 规划）、emitter（消费 plan 生成 C++ 代码）和 Scriban 模板渲染三层职责。

## 2. 项目边界

### 2.1 项目表

| 项目 | 路径 | 职责 | 依赖 |
|------|------|------|------|
| `Chaos.IL2CPP.Generator` | `src/managed/Chaos.IL2CPP.Generator/` | 核心 codegen：planner / emitter / template | Contracts, Loader, MetadataWriter, Scriban |
| `Chaos.IL2CPP.Generator.Generators` | `src/managed/Chaos.IL2CPP.Generator.Generators/` | Roslyn source generator（analyzer） | （通过 OutputItemType=Analyzer 引用） |

### 2.2 命名空间

所有 Generator 代码使用以下命名空间约定：

```
namespace Chaos.IL2CPP.Generator;                           // 核心 codegen
namespace Chaos.IL2CPP.Generator.Templates;                  // Scriban 模板模型
namespace Chaos.IL2CPP.Generator.Emission;                   // emitter / lowering planner
namespace Chaos.IL2CPP.Generator.ReferenceProof;             // native reference proof catalog
namespace Chaos.IL2CPP.Generator.Generators;                 // Roslyn source generators
```

### 2.3 上游消费者

```
Chaos.IL2CPP.Driver          → ProjectReference Generator     （管线入口）
Chaos.IL2CPP.Pipeline        → ProjectReference Generator     （管线编排）
Chaos.IL2CPP.Generator       → ProjectReference Generator.Generators (Analyzer)
```

## 3. 目录结构

```
src/managed/Chaos.IL2CPP.Generator/
├── Chaos.IL2CPP.Generator.csproj
├── Emission/                  # Planner / emitter 实现
│   ├── NativeAotLoweringPlanner.*.cs
│   ├── NativeAotModuleRegistration*.cs
│   └── SourceGenerators/
├── Templating/                # Scriban 渲染基础设施
│   ├── ScribanTemplateRenderer.cs
│   └── TemplateFactory.cs
├── Templates/                 # Scriban .scriban 模板文件
│   ├── NativeAot.*.cpp.scriban
│   ├── NativeAot.*.h.scriban
│   └── ...
├── ReferenceProof/            # Native reference proof
│   └── NativeReferenceProofCatalog.cs
└── GeneratorUtilities.cs      # 辅助工具
```

## 4. 结构治理约束

### 4.1 Planner / Emitter / Template 分离

Generator 内部严格遵守三层分离：

1. **Planner**（`Emission/`）：负责语义分析、ABI dispatch 规划、EH 策略、runtime-support 注册规划。输出 typed plan。
2. **Emitter**（`Emission/`）：消费 plan，不重新做业务决策。将 plan 映射为 render model 并调用 Scriban 渲染。
3. **Template**（`Templates/`）：Scriban `.scriban` 文件。只做结构化文本输出，不包含语义级 lowering 规则。

禁止：
- Emitter 中重新做 planner 已确定的 ABI/dispatch/EH 决策
- 模板中硬编码语义级 lowering 规则
- 新增长期 `StringBuilder.AppendLine(...)` 发射路径替代 Scriban 模板

详见 [`scriban-usage-and-codegen-rules.md`](./scriban-usage-and-codegen-rules.md)。

### 4.2 Managed / Native 桥接

Generator 产出的 C++ 代码通过以下方式与 native 侧对接：

- `NativeAot.AbiManifest.cpp.scriban` → 生成 ABI manifest
- `NativeAot.CodeRegistration.cpp.scriban` → 生成 method pointer 注册表
- `NativeAot.DispatchEntryCode.cpp.scriban` → 生成 dispatch entry
- `NativeAot.GenericAotRegistration.cpp.scriban` → 泛型 AOT 注册
- `NativeAot.DelegateRuntimeSupport.cpp.scriban` → delegate 运行时支持
- `NativeAot.HotpatchTable.cpp.scriban` → hotpatch 表
- `NativeAot.ExternalRuntimeDispatchTable.cpp.scriban` → 外部运行时 dispatch
- `NativeAot.MethodTableInitialization.cpp.scriban` → method table 初始化
- `NativeAot.ReflectionQueryImage.cpp.scriban` → reflection 查询镜像
- `NativeAot.StructMarshallingDescriptors.cpp.scriban` → 结构体 marshalling

## 5. 项目历史

- 初始项目名：`Chaos.IL2CPP.CodeGen`
- 2026-04 原地重命名为 `Chaos.IL2CPP.Generator`，以反映该项目已不限于"代码生成"，同时包含 Scriban 模板渲染、Roslyn source generator 和 planner/emitter 三层职责。
- 伴随重命名：`Chaos.IL2CPP.CodeGen.Generators` → `Chaos.IL2CPP.Generator.Generators`
- 旧命名空间 `Chaos.IL2CPP.CodeGen` 已无残留。

## 6. 相关文档

- [`scriban-usage-and-codegen-rules.md`](./scriban-usage-and-codegen-rules.md) — Scriban 模板使用规范与扩展顺序
- [`il2cpp-core-structure-and-scriban-governance.md`](./il2cpp-core-structure-and-scriban-governance.md) — IL2CPP 核心代码层结构治理
- [`../06-测试验证/CodeGen快照测试规范.md`](../06-%E6%B5%8B%E8%AF%95%E9%AA%8C%E8%AF%81/CodeGen%E5%BF%AB%E7%85%A7%E6%B5%8B%E8%AF%95%E8%A7%84%E8%8C%83.md) — CodeGen 快照测试规范
