# Full Project Ingestion And Build Graph 实现计划

> **面向执行 Agent：** 必须使用 dev:executing-plans 来执行本计划。步骤使用复选框（`- [ ]`）语法跟踪。

**目标：** 把 Pipeline 输入层从单 PE 程序集升级到完整 project/solution graph，支持 ProjectReference 闭包、PackageReference 解析和多程序集加载。

**架构：** 新建 `Chaos.IL2CPP.ProjectGraph` 项目处理工程图解析，扩展 `LoaderStage` 支持多程序集加载，扩展 Pipeline 下游模块处理 cross-assembly references。通过 3 个 golden project fixture 验证。

**技术栈：** C# (.NET 8), MSBuild evaluation, JSON

**设计文档：** `phase-architecture-v1-01.md` Phase 1 节 + `native-output-structure-v1-01.md`

**预期知识沉淀：** `wiki/` — project graph 接入规范

---

## 要创建的文件

| 文件 | 职责 |
| --- | --- |
| `src/managed/Chaos.IL2CPP.ProjectGraph/Chaos.IL2CPP.ProjectGraph.csproj` | 项目文件 |
| `src/managed/Chaos.IL2CPP.ProjectGraph/ProjectGraphLoader.cs` | .csproj → 依赖图解析 |
| `src/managed/Chaos.IL2CPP.ProjectGraph/ProjectGraphModel.cs` | 数据模型（ProjectGraph, ProjectNode, DependencyEdge） |
| `src/managed/Chaos.IL2CPP.ProjectGraph/PackageResolver.cs` | PackageReference → DLL 路径解析（读 project.assets.json） |
| `src/managed/Chaos.IL2CPP.Contracts/LoadedWorldModel.cs` | 多程序集加载结果模型 |
| `subjects/golden/GoldenSimpleLib/` | 1 csproj + 1 class library |
| `subjects/golden/GoldenMultiProject/` | 3 csproj 互相引用 |
| `subjects/golden/GoldenWithPackage/` | 1 csproj + NuGet PackageReference |
| `tests/unit/run/test_project_graph.py` | project graph snapshot tests |

## 步骤

### Batch 1: ProjectGraph 项目骨架

- [ ] **1.1** 创建 `Chaos.IL2CPP.ProjectGraph` 项目（依赖 Contracts）
- [ ] **1.2** 定义 `ProjectGraphModel`：`ProjectGraph { Projects[], Dependencies[], EntryProject }`
- [ ] **1.3** 实现 `ProjectGraphLoader.LoadFromCsproj(path)`：解析单个 .csproj 的 ProjectReference
- [ ] **1.4** 实现 ProjectReference 递归闭包解析（BFS 遍历 project graph）
- [ ] **1.5** 实现 `PackageResolver.ResolveFromAssetsFile()`：读取 `obj/project.assets.json` 获取 NuGet DLL 路径

### Batch 2: LoaderStage 多程序集扩展

- [ ] **2.1** 定义 `LoadedWorldModel { Assemblies[], CrossReferences[], EntryAssemblyPath }`
- [ ] **2.2** 扩展 `LoaderStage.LoadMultiple(assemblyPaths, entryAssemblyPath)`
- [ ] **2.3** 实现 cross-assembly reference index（assembly A 的 TypeReference → assembly B 的 TypeDefinition）
- [ ] **2.4** 扩展 `ManagedClosureRequest` 或新增 `ProjectGraphRequest`

### Batch 3: Pipeline 下游模块扩展

- [ ] **3.1** 扩展 SemanticWorld：接受 `LoadedWorldModel`，跨 assembly 分析 capabilities
- [ ] **3.2** 扩展 Linker：reachability analysis 跨 assembly 边界追踪
- [ ] **3.3** 扩展 MetadataWriter：multi-assembly registration tables
- [ ] **3.4** 扩展 CodeGen：per-assembly 输出（按 `native-output-structure-v1-01.md` 的目录结构）
- [ ] **3.5** 扩展 Pipeline：编排 multi-assembly 流程

### Batch 4: Golden Projects + CLI 接线

- [ ] **4.1** 创建 `subjects/golden/GoldenSimpleLib/`（1 个 console app + 1 个 class library）
- [ ] **4.2** 创建 `subjects/golden/GoldenMultiProject/`（3 个互相引用的 csproj + 1 个 entry）
- [ ] **4.3** 创建 `subjects/golden/GoldenWithPackage/`（1 个 csproj + Newtonsoft.Json PackageReference）
- [ ] **4.4** 更新 `chaos-il2cpp convert` 支持 ProjectGraph 模式
- [ ] **4.5** 编写 `test_project_graph.py`：graph snapshot tests

### Batch 5: 验证

- [ ] **5.1** `chaos-il2cpp convert subjects/golden/GoldenSimpleLib` 成功
- [ ] **5.2** `chaos-il2cpp convert subjects/golden/GoldenMultiProject` 成功
- [ ] **5.3** `chaos-il2cpp convert subjects/golden/GoldenWithPackage` 成功
- [ ] **5.4** 输出目录结构符合 `native-output-structure-v1-01.md`
- [ ] **5.5** convert 时间 < 10s（GoldenMultiProject）
- [ ] **5.6** 运行完整测试套件，0 regressions
