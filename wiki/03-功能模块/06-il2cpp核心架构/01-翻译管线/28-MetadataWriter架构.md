# 28-MetadataWriter 元数据写入器

> MetadataWriterStage 是 Chaos IL2CPP 翻译管线的第四阶段（Loader → SemanticWorld → Linker → **MetadataWriter** → CodeGen），负责将 LinkedWorldModel 中的语义信息转换为结构化 artifact，供 codegen 阶段消费。

## 职责概述

MetadataWriterStage 接收 Linker 产出的 `LinkedWorldModel`，产出三个 artifact：

| Artifact | 用途 |
|----------|------|
| `AotManifestArtifact` | 声明哪些 method/field/dependency 需要 AOT 编译，附带原因分类 |
| `MetadataRegistrationArtifact` | 按 slot 编号注册 assembly/type/field/property/method/parameter，生成启动引导用的元数据表 |
| `SupplementalMetadataTemplateArtifact` | 为 type/method 解析泛型实例化上下文（RuntimeGenericContext + GenericDiagnostic），补充元数据模板 |

MetadataWriterStage 本身不涉及 Scriban 模板渲染，不涉及 C++ 代码生成——它是**纯数据映射层**，所有输出都是 C# 内存对象。

## 管线位置

```
Loader ──▶ SemanticWorld ──▶ Linker ──▶ MetadataWriter ──▶ CodeGen
                                              │
                                    ┌─────────┼─────────┐
                                    ▼         ▼         ▼
                              AotManifest  Metadata   Supplemental
                                            Registration MetadataTemplate
```

`MetadataWriterOutput` 被直接传入 `CodeGenStage.Generate()`：

```
CodeGenStage.Generate(request, linkedWorld, metadataWriterOutput)
```

详见 `PipelinePlan.cs`。

## 输入：LinkedWorldModel

`LinkedWorldModel` 包含：

- `Methods` / `Fields` / `Properties` / `Types` / `Assemblies` — 已链接的类型系统
- `Dependencies` — 跨 assembly 依赖声明
- `GenericInstantiationDemandGraph` — 泛型实例化需求图（可选）
- `CanonicalSubjects` / `SemanticShapes` / `CapabilityBundles` — 语义分析结果
- `OptimizationFacts` / `PreserveDescriptor` — 优化与保留描述符

## 输出 Artifact 格式

### AotManifestArtifact

```csharp
sealed record AotManifestArtifact {
    string FormatVersion;  // "v0"
    string ArtifactKind;   // "aotManifest"
    IReadOnlyList<AotManifestEntry> Entries;
}

sealed record AotManifestEntry {
    string AssemblyName;
    string SubjectKind;    // "method" | "field" | (from dependency)
    string SubjectId;
    string Reason;         // "entrypoint" | "generated-direct-call" |
                           // "instance-field-state" | (from dependency)
}
```

**字段说明：**

- `Reason = "entrypoint"` — 标记入口方法（与 `LinkedWorldModel.EntryPointSubjectId` 匹配的方法）
- `Reason = "generated-direct-call"` — 所有非 entrypoint 的方法
- `Reason = "instance-field-state"` — 所有字段（AOT 需要实例状态）
- 依赖项的 Reason 来自 `LinkedDependencyModel.Reason`（由上游 Linker 指定）

### MetadataRegistrationArtifact

```csharp
sealed record MetadataRegistrationArtifact {
    string FormatVersion;     // "v0"
    string ArtifactKind;      // "metadataRegistration"
    IReadOnlyList<MetadataRegistrationEntry> Registrations;
}

sealed record MetadataRegistrationEntry {
    string RegistrationKind;  // "assembly" | "type" | "field" |
                              // "property" | "method" | "parameter"
    int Slot;                 // 0-based sequential slot number
    string SubjectId;
    string? Name;
    // ... 其他字段按 RegistrationKind 有条件填充
}
```

**Slot 分配顺序：（重要 — 消费端依赖此顺序）**

1. 所有 Assemblies（按 `linkedWorld.Assemblies` 迭代顺序）
2. 所有 Types
3. 所有 Fields
4. 所有 Properties
5. 所有 Methods
6. 所有 Parameters（按方法分组，每个方法内按参数索引顺序）

Slot 从 0 开始递增，**全局连续编号**。

### SupplementalMetadataTemplateArtifact

```csharp
sealed record SupplementalMetadataTemplateArtifact {
    string FormatVersion;              // "v0"
    string ArtifactKind;               // "supplementalMetadataTemplate"
    List<SupplementalMetadataTypeTemplateEntry> RegisteredTypes;
    List<SupplementalMetadataMethodTemplateEntry> RegisteredMethods;
    SupplementalMetadataReservedSlots ReservedSlots;
}
```

**泛型上下文解析：** 当 `GenericInstantiationDemandGraph` 非空时，为每个 type/method 查找对应的 `GenericInstantiationDemandModel`，填充 `RuntimeGenericContext`（InstantiationKey、SharedGenericBodyId、InstantiationStubId、SupportKindCode、SpecializationKindCode）和 `GenericDiagnostic`。

**ReservedSlots 默认值：**
- `TypeCount = 256`
- `MethodCount = 1024`
- `GenericInstantiationCount = 256`

## 关键设计决策

1. **纯数据映射** — MetadataWriterStage 不做语义分析、不做 IL 解析、不做 CFG 构建。所有语义决策在 Loader/SemanticWorld/Linker 中完成。MetadataWriter 只做数据格式转换。
2. **单文件架构** — 当前 339 行（重构后），超过 500 行时需考虑拆分 helper 类。已提取三个 builder 方法（`BuildAotManifest`、`BuildMetadataRegistration`、`BuildSupplementalTemplate`）。
3. **错误码体系** — 三种错误分类：
   - `METADATA_WRITER_DEMAND_CONFLICT` — 泛型实例化需求冲突
   - `METADATA_WRITER_INVALID_INPUT` — 输入数据为空/无效
   - `METADATA_WRITER_INTERNAL_ERROR` — 其他内部错误

### 架构审视发现（2026-05-23）

- `EnsureEquivalentDemand()` 方法（~50 行）为死代码——从未在任何路径中被调用。`BuildGenericDemandLookup` 在遇到重复 SubjectId 时直接走 first-wins 语义，不调用此验证方法。
- 推荐操作：在下次重构时移除该方法和关联常量 `METADATA_WRITER_DEMAND_CONFLICT`。

## 测试覆盖

### 工业化前（2026-05-23 前）
- **MetadataWriterStageTests.cs**（22 tests）— 私有辅助方法测试（SequenceEqual、AreEquivalentInstantiationKeys、BuildGenericDemandLookup、ResolveRuntimeGenericContext、EnsureEquivalentDemand）
- **MetadataWriterStageCoreTests.cs**（16 tests）— `Write()` 核心路径测试：artifact 格式验证、字段映射验证、slot 分配验证、参数注册验证、P/Invoke 导入验证、泛型上下文解析验证、错误处理验证

### 工业化新增（2026-05-23）
- **Chaos.IL2CPP.Tests.MetadataWriter**（24 tests）— 独立测试项目，覆盖：
  - AotManifest 构建：空 world、简单方法、入口点、字段、依赖项、多条目排序（6 tests）
  - MetadataRegistration 构建：assembly、type 全字段、property、method 全字段、parameter 注册、imported method、slot 顺序（7 tests）
  - SupplementalMetadataTemplate 构建：reserved slots、type 条目、method 条目（3 tests）
  - 泛型实例化解析（2 tests）
  - 错误/边界处理（4 tests）
  - 格式契约（2 tests）
  - CodeGen 契约集成（1 test）

### CI 门禁
- 集成到 `codegen-regression.yml`：PR/push 修改 `test/src/managed/**` 时自动触发
- 管线步骤：restore → build → test → coverlet 覆盖率收集（cobertura 格式）
- 覆盖率阈值：line coverage ≥ 70%（通过 `coverlet.runsettings` 配置）
- 覆盖率报告作为 CI artifact 上传

## 故障诊断

### Write() 返回 METADATA_WRITER_DEMAND_CONFLICT

**症状：** MetadataWriter 抛出 `InvalidOperationException`，错误信息包含 "conflicting generic instantiation demand entries detected"。

**原因：** `LinkedWorldModel.GenericInstantiationDemandGraph` 中存在两条相同 SubjectId 但 DefinitionSubjectId/SupportKind/SpecializationKind/InstantiationKey 不一致的 demand 条目。

**排查：**
1. 检查上游 Linker 的泛型需求图构建逻辑
2. 检查是否有多个 loader 路径对同一方法产生了不同的泛型需求
3. 在 Linker 阶段增加 demand 防冲突检查

### MetadataRegistration Slot 偏移

**症状：** 运行时发现 metadata 解析到的 type/method 信息不匹配。

**原因：** Slot 分配顺序变更但消费端未同步更新。Slot 顺序是 MetadataWriter 和 BootstrapRuntime 之间的隐式契约。

**排查：**
1. 确认装配 MetadataWriterStageCoreTests 中的 slot 分配测试通过
2. 检查 CodeGenStage 消费 MetadataRegistration 的方式——是按 slot 索引还是按 SubjectId 查找
3. 如果按 slot 索引，任何迭代顺序变更（如新增 registration kind）都会破坏下游

### AotManifest 缺少预期条目

**症状：** 运行时代码报 MissingMethodException 或 NullReferenceException，但该 method/field 确实在 IL 中存在。

**原因：** 上游 Linker 的 LinkedWorldModel 中没有包含该条目。

**排查：**
1. 确认 LinkedWorldModel.Methods/Fields/Dependencies 包含了预期条目
2. MetadataWriter 对所有输入做 **全量转发**，不做过滤——不会"丢失"条目
3. 问题必定在上游，不在 MetadataWriter

### 泛型上下文为空

**症状：** SupplementalMetadataTemplate 中某些 type/method 的 RuntimeGenericContext 为 null。

**原因：** `GenericInstantiationDemandGraph.Demands` 中没有该 SubjectId 对应的 demand。

**排查：**
1. 检查 `GenericInstantiationDemandGraph` 是否为空
2. 检查 Linker 阶段是否对该 type/method 产生了 demand
3. 如果 type/method 不是泛型或没有泛型实例化需求，RuntimeGenericContext 为 null 是正常行为

### CI 覆盖率门禁失败

**症状：** CI（codegen-regression.yml）中 MetadataWriter Coverage Collection 步骤失败，报 "coverlet threshold not met"。

**原因：** 新增代码降低了总体 line coverage，低于 70% 阈值。

**排查：**
1. 检查覆盖率报告 artifact 中的各模块行覆盖明细
2. 如果新增了 MetadataWriterStage.cs 的逻辑但未加对应测试，补充测试
3. 如果新增代码在被排除的目录中（如 `**/obj/**`），确认排除配置正确
4. 阈值可在 `tests/unit/managed/Chaos.IL2CPP.Tests.MetadataWriter/coverlet.runsettings` 中调整

---

## 相关文档

- [01-总体流程与五层职责.md](./01-总体流程与五层职责.md) — 管线整体流程
- [24-全量IL2CPP编译器架构.md](./24-全量IL2CPP编译器架构.md) — 多程序集编译架构（MetadataWriter 在其中的位置）
- [08-泛型执行模型.md](./08-泛型执行模型.md) — 泛型上下文运行时
- [17-反射查询模型.md](./17-反射查询模型.md) — 反射元数据关联
