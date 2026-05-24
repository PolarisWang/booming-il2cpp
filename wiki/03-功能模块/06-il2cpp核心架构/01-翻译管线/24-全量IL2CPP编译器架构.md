# 全量 IL2CPP 编译器架构

> chaos-il2cpp convert-to-cpp 从验证管道到产品级编译器的架构升级方案。

## 概述

当前 `convert-to-cpp` 使用真正的 IL2CPP 翻译通道（PipelinePlan → NativeAotEmitter），经过 Loader → SemanticWorld → Linker → MetadataWriter → CodeGen 完整阶段，但仍处于**验证管道形态**。本方案设计从当前状态到完整 IL2CPP 编译器的架构升级路径。

### 架构目标

| 优先级 | 维度 | 要求 |
|--------|------|------|
| P1（最高） | 性能最优 | 同程序集 direct call、跨程序集 method table dispatch |
| P2（次高） | 架构完美 | 每程序集独立翻译单元、明确跨程序集接口 |
| P3（第三） | HotUpdate 适配 | D3 dispatch table 覆盖所有方法、interpreter 回退 |

---

## CLI 模型（Unity IL2CPP 风格）

```bash
chaos-il2cpp convert-to-cpp \
  --assembly GameLogic.dll \
  --assembly ScriptRuntime.dll \
  --assembly-dir deps/ \
  --output gen/ \
  [--entry-point GameLogic.Program::Main] \
  [--full-closure]
```

- `--assembly` 可重复，指定要编译的程序集
- `--entry-point` 可选入口点（不指定则 full closure 模式）
- `--full-closure` 编译所有可达方法

## 数据流

```
N 个 --assembly DLLs
  │
  ├─ LoaderStage.LoadMultiple()
  │   一次加载所有程序集到统一的 LoadedWorldModel
  │
  ├─ SemanticWorldStage.Build()
  │   统一的语义世界（跨程序集类型/方法/字段）
  │
  ├─ LinkerStage.Link()
  │   跨程序集可达性分析（从 entry point BFS 或 full-closure）
  │
  ├─ MetadataWriterStage.Write()
  │   全局元数据注册（每程序集分配 slot 范围）
  │
  ├─ CodeGenStage.FilterResultPerAssembly()
  │   按程序集过滤方法/类型/字段，生成每程序集 artifact
  │
  └─ NativeAotEmitter.GeneratePerAssembly()
       │
       ├─ {Assembly1}.cpp + .module-desc.h + .name-index.h
       ├─ {Assembly2}.cpp + .module-desc.h + .name-index.h
       ├─ ...
       ├─ global-registration.cpp     # 聚合 CodeRegistration
       ├─ runtime-entry.cpp           # 运行时入口点
       └─ CMakeLists.txt              # 每程序集 OBJECT 库 → 链接
```

## 每程序集输出结构

```
gen/
  GameLogic.cpp              # 方法实现 + TypeInfo + vtable + string table
  GameLogic.module-desc.h    # ModuleDescriptor 常量数据
  GameLogic.name-index.h     # Hotpatch 名称索引 + dispatch table
  GameLogic.abi-manifest.h   # ChaosAbiManifestV0
  ScriptRuntime.cpp
  ScriptRuntime.module-desc.h
  ...
  global-registration.cpp    # 聚合的 RegisterModule 调用
  runtime-entry.cpp          # main() 入口
  CMakeLists.txt             # OBJECT 库 → add_executable
```

## 调用约定

| 场景 | 方式 | 性能 |
|------|------|------|
| 同程序集调用 | 直接 `callee(args)` | 最快 |
| 跨程序集调用 | 方法表 `ResolveMethodTable(index)` | 一次间接 |
| HotPatch | Hotpatch dispatch entry 切换 interrupt_ptr | 已优化 |
| 泛型运行时实例化 | RuntimeInstantiationBridgeV0 | 解释执行 |

## 跨程序集调用决议

`NativeAotLoweringPlanner.TryGetMethodTableIndex()` 已实现跨模块检测：

1. 从 callee SubjectId 提取程序集名称（`"AssemblyName/Type::Method"` 格式）
2. 与当前 `_assemblyName` 比较
3. 相同程序集 → 返回 false（直接调用）
4. 不同程序集 → 分配方法表索引，记录 `WriteMethodTable()` 初始化条目

`MethodTableAllocator`（`src/.../Planning/MethodTableAllocator.cs`）提供全局方法表索引分配，上限 65536 条目。

## 构建系统集成

`CmakeGenerator`（`src/.../BuildSystem/CmakeGenerator.cs`）生成：

```cmake
add_library(chaos_gen_AssemblyA OBJECT AssemblyA.cpp)
target_link_libraries(chaos_gen_AssemblyA PUBLIC chaos_runtime_core)
target_include_directories(chaos_gen_AssemblyA PUBLIC "${CMAKE_CURRENT_LIST_DIR}")

add_executable(chaos_app
    $<TARGET_OBJECTS:chaos_gen_AssemblyA>
    $<TARGET_OBJECTS:chaos_gen_AssemblyB>
    runtime-entry.cpp)
target_link_libraries(chaos_app chaos_runtime_core)
```

构建命令：
```bash
cmake -S gen/ -B gen/build
cmake --build gen/build
```

## 关键 API

### NativeAotLoweringPlanner.CreateForAssembly()

```csharp
public NativeAotTemplateModel CreateForAssembly(
    string assemblyName,
    string assemblyPath,
    AotCoreIrArtifact aotCoreIr,
    NativeAotLoweringPlanArtifact loweringPlan,
    ManagedClosureManifestArtifact closureManifest,
    MetadataRegistrationArtifact metadataRegistration,
    SupplementalMetadataTemplateArtifact supplementalMetadataTemplate)
```

- 按程序集过滤方法（SubjectId 前缀匹配）
- 独立调用 `CollectModuleTypeData(assemblyPath)` 读取该程序集 PE metadata
- 每程序集生成独立的 TypeInfo、ModuleDescriptor、NameIndex、AbiManifest
- 跨程序集类型引用使用 `extern const TypeInfo` 声明

### NativeAotEmitter.GeneratePerAssembly()

```csharp
public IReadOnlyList<NativeAotResult> GeneratePerAssembly(
    MultiAssemblyClosureRequest request,
    ManagedClosureResult fullResult,
    string outputRoot)
```

- 遍历每个输入程序集
- 为每个程序集调用 `CreateForAssembly()` 生成 template model
- 渲染 Scriban templates → 每程序集 `{AssemblyName}.cpp`
- 返回每程序集的 NativeAotResult

## 关键文件

### 新建文件

| 文件 | 职责 |
|------|------|
| `src/.../CodeGen/Planning/MethodTableAllocator.cs` | 全局方法表索引分配 |
| `src/.../CodeGen/BuildSystem/CmakeGenerator.cs` | CMakeLists.txt 生成 |
| `src/.../CodeGen/Emission/NativeAotLoweringPlanner.MultiAssembly.cs` | CreateForAssembly() 实现 |
| `src/.../CodeGen/Templates/RuntimeEntry.cpp.scriban` | 运行时入口模板 |

### 修改的文件

| 文件 | 变更 |
|------|------|
| `src/.../Driver/ConvertToCpp/ConvertToCppConfig.cs` | 多 `--assembly`、`--entry-point`、`--full-closure` |
| `src/.../Driver/ConvertToCpp/ConvertToCppHandler.cs` | 多程序集编排 + CMake + runtime entry 生成 |
| `src/.../Pipeline/PipelinePlan.cs` | ExecuteMulti() 多程序集管线 |
| `src/.../CodeGen/CodeGenStage.cs` | FilterResultPerAssembly() |
| `src/.../Contracts/ManagedClosureContracts.cs` | MultiAssemblyClosureRequest |
| `src/.../CodeGen/NativeAotEmitter.cs` | GeneratePerAssembly() |

## 阶段路线图（Phase 0-5）

| 阶段 | 目标 | 状态 |
|------|------|------|
| **Phase 0** | 多程序集管道基础：CLI 支持多个 `--assembly`、ExecuteMulti()、FilterResultPerAssembly() | ✅ 完成 |
| **Phase 1** | 每程序集 NativeAot Emission：CreateForAssembly()、GeneratePerAssembly()、每程序集 ModuleDescriptor | ✅ 完成 |
| **Phase 2** | 跨程序集调用决议：MethodTableAllocator、同程序集 direct call / 跨程序集 method table | ✅ 完成 |
| **Phase 3** | 构建系统 + 运行时集成：CMakeLists.txt 生成、runtime-entry.cpp、TypeInfo extern 声明 | ✅ 完成 |
| **Phase 4** | HotUpdate 完整性：每方法 Hotpatch dispatch entry、interpreter 回退、PatchLoader 多模块 | ✅ 已完成 |
| **Phase 5** | 性能优化：内联、去虚拟化、LTO、大程序集分页 | ✅ 已完成 |

### Phase 4 交付摘要

| 子项 | 交付内容 |
|------|----------|
| p4-hotpatch | dispatch slot 覆盖全部 IL 方法（含无 ECMA token 方法），HotpatchNameRegistry 支持按名称查找 |
| p4-multimodule | PatchData 格式扩展（v1 增加 `dependency_count`/`PatchDataDependency`），跨模块 ResolveToken 实现 |
| p4-interpreter | EH/泛型/PInvoke 三条 interpreter 回退路径压力验证（`eh-stress`/`generics-stress`/`pinvoke-stress` 家族各 3-4 个方法，无 CHAOS_IL2CPP_FAIL） |

### Phase 5 交付摘要

| 子项 | 交付内容 |
|------|----------|
| p5-inline | 方法内联优化（热点小函数的内联翻译路径选择） |
| p5-devirt | 去虚拟化优化（CallVirt → direct Call 的静态决议） |
| p5-lto | LTO（Link-Time Optimization）集成适配 |
| p5-paging | 大程序集分页输出（page-*.cpp + module.cpp 分段编译避免单文件编译 OOM） |

---

## 关联文档

- [`01-总体流程与五层职责.md`](./01-%E6%80%BB%E4%BD%93%E6%B5%81%E7%A8%8B%E4%B8%8E%E4%BA%94%E5%B1%82%E8%81%8C%E8%B4%A3.md) — PipelinePlan 五层架构
- [`04-NativeAotLoweringPlanner文件布局.md`](./04-NativeAotLoweringPlanner%E6%96%87%E4%BB%B6%E5%B8%83%E5%B1%80.md) — LoweringPlanner 20+ partial 文件分布
- [`18-热更新架构.md`](./18-%E7%83%AD%E6%9B%B4%E6%96%B0%E6%9E%B6%E6%9E%84.md) — Hotpatch dispatch + PatchLoader
- [`15-泛型上下文运行时.md`](./15-%E6%B3%9B%E5%9E%8B%E4%B8%8A%E4%B8%8B%E6%96%87%E8%BF%90%E8%A1%8C%E6%97%B6.md) — RuntimeInstantiationBridgeV0
- [`27-T4原生代码生成器架构.md`](./27-T4%E5%8E%9F%E7%94%9F%E4%BB%A3%E7%A0%81%E7%94%9F%E6%88%90%E5%99%A8%E6%9E%B6%E6%9E%84.md) — JIT T4 编译体系
- [`29-模式选择决策树.md`](./29-%E6%A8%A1%E5%BC%8F%E9%80%89%E6%8B%A9%E5%86%B3%E7%AD%96%E6%A0%91.md) — 模式选择场景指南
- `wiki/04-历史决策/` — 方案 C、HybridCLR 对齐等历史决策
