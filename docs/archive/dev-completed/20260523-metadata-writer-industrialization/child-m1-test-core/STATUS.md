---
task_id: M1-test-core
lifecycle_status: completed
---

# M1-test-core: Write() 核心路径测试 + 契约集成测试

> **子任务:** M1-test-core (Phase 1)
> **状态:** completed

---

## 测试清单

### 新建测试项目

- **项目路径:** `testing/src/managed/Chaos.IL2CPP.Tests.MetadataWriter/`
- **项目文件:** `Chaos.IL2CPP.Tests.MetadataWriter.csproj`
- **测试文件:** `MetadataWriterStageTests.cs`
- **测试框架:** xUnit (沿袭现有模式)
- **测试数量:** 24 (全部通过)

### AotManifest 构建测试 (6 个)

| # | 测试名 | 验证点 |
|---|--------|--------|
| 1 | `Write_EmptyWorld_AotManifestHasNoEntries` | 空 world 输出空 manifest |
| 2 | `Write_SimpleMethod_AotManifestContainsEntry` | method 生成 manifest entry |
| 3 | `Write_EntryPointMethod_HasEntryPointReason` | entrypoint 标记 |
| 4 | `Write_Field_AotManifestContainsFieldEntry` | field 生成 manifest entry |
| 5 | `Write_Dependencies_AppearInAotManifest` | dependency 生成 manifest entry |
| 6 | `Write_MultipleMethods_Dependencies_Fields_AotManifestOrdering` | 多 entry 顺序：methods→fields→dependencies |

### MetadataRegistration 构建测试 (5 个)

| # | 测试名 | 验证点 |
|---|--------|--------|
| 7 | `Write_Assembly_RegistrationHasSlotZero` | assembly slot=0 |
| 8 | `Write_Type_FullRegistrationFields` | type 全字段验证 |
| 9 | `Write_Property_CreatesRegistration` | property 注册 |
| 10 | `Write_Method_FullRegistrationFields` | method 全字段验证 (含 ParameterCount) |
| 11 | `Write_MethodWithParameters_CreatesParameterRegistrations` | parameter 注册 + 顺序 |
| 12 | `Write_ImportedMethod_SetsIsImported` | P/Invoke import 字段 |
| 13 | `Write_FullRegistrationSequence_CorrectSlotOrder` | 完整顺序验证 assembly→type→field→property→method→parameter |

### SupplementalMetadataTemplate 构建测试 (3 个)

| # | 测试名 | 验证点 |
|---|--------|--------|
| 14 | `Write_SupplementalTemplate_HasReservedSlots` | 保留槽位默认值 |
| 15 | `Write_SupplementalTemplate_HasRegisteredTypeEntries` | type template 条目 |
| 16 | `Write_SupplementalTemplate_HasRegisteredMethodEntries` | method template 条目 |

### 泛型 Instantiation 测试 (2 个)

| # | 测试名 | 验证点 |
|---|--------|--------|
| 17 | `Write_GenericType_SupplementalTemplateHasGenericContext` | 泛型 type 的 RuntimeGenericContext 解析 |
| 18 | `Write_DuplicateGenericDemands_FirstWins` | 重复 SubjectId 的 first-wins 语义 |

### 错误/边界处理测试 (4 个)

| # | 测试名 | 验证点 |
|---|--------|--------|
| 19 | `Write_EmptyWorld_ReturnsSuccess` | Write() 返回成功 |
| 20 | `Write_NullGenericDemandGraph_DoesNotThrow` | null demand graph 不抛异常 |
| 21 | `Write_EmptyGenericDemandGraph_DoesNotThrow` | empty demand graph 不抛异常 |

### Artifact 格式契约测试 (2 个)

| # | 测试名 | 验证点 |
|---|--------|--------|
| 22 | `Write_AllArtifacts_HaveExpectedFormatVersion` | FormatVersion = "v0" |
| 23 | `Write_AllArtifacts_HaveExpectedArtifactKind` | ArtifactKind 字符串常量 |

### 契约集成测试 (1 个)

| # | 测试名 | 验证点 |
|---|--------|--------|
| 24 | `Write_Output_IsConsumableByManagedClosureResult` | 3 artifact 可被 ManagedClosureResult 消费 |

### 已覆盖的契约字段

| Artifact | 覆盖字段 |
|----------|---------|
| **AotManifestArtifact** | FormatVersion, ArtifactKind, Entries[].AssemblyName, SubjectKind, SubjectId, Reason |
| **MetadataRegistrationArtifact** | FormatVersion, ArtifactKind, Registrations[].RegistrationKind, Slot, SubjectId, Name, NamespaceName, DisplayName, DefinitionSubjectId, DeclaringTypeSubjectId, DeclaringMethodSubjectId, MemberType, ParameterIndex, ParameterCount, MetadataToken, IsImported, ImportModuleName, ImportEntryPointName |
| **SupplementalMetadataTemplateArtifact** | FormatVersion, ArtifactKind, RegisteredTypes, RegisteredMethods, ReservedSlots; Type/Method entry: AssemblyName, SubjectId, DefinitionSubjectId, RuntimeGenericContext, GenericDiagnostic, MetadataToken, DeclaringTypeSubjectId, ParameterCount |
| **PipelineResult 契约** | IsSuccess, IsFailure, Error.Code, Error.Message |

## 测试结果

```
Total tests: 24
     Passed: 24
     Failed: 0
 Total time: 0.57 Seconds
```

## 关键发现

- `EnsureEquivalentDemand()` 及相关方法（50 行）是死代码，从未被 `BuildGenericDemandLookup()` 调用。重复的 SubjectId 被静默跳过（first-wins 语义）。详见 M1-arch-review。
- LinkedWorldModel 构造需要 15+ 嵌套模型对象，建议未来引入 Builder 模式或共享测试夹具。
