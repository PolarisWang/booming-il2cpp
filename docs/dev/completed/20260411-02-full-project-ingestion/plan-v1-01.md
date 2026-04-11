# Full Project Ingestion And Build Graph 实现计划

> 面向执行 Agent：必须使用 `dev:executing-plans` 执行本计划。
> 目标：把 Pipeline 输入从单 PE / 单程序集提升到真实 project graph，支持 `ProjectReference` 闭包、`PackageReference` 基础解析与多程序集 convert。

## Batch 1: ProjectGraph 项目骨架

- [x] **1.1** 创建 `Chaos.IL2CPP.ProjectGraph` 项目（依赖 `Contracts`）
- [x] **1.2** 定义 `ProjectGraphModel`：`ProjectGraph { Projects[], Dependencies[], EntryProject }`
- [x] **1.3** 实现 `ProjectGraphLoader.LoadFromCsproj(path)`：解析单个 `.csproj` 的 `ProjectReference`
- [x] **1.4** 实现 ProjectReference 递归闭包解析（BFS 遍历 project graph）
- [x] **1.5** 实现 `PackageResolver.ResolveFromAssetsFile()`：读取 `obj/project.assets.json` 获取 NuGet DLL 路径

## Batch 2: LoaderStage 多程序集扩展

- [x] **2.1** 定义 `LoadedWorldModel { Assemblies[], EntryAssemblyPath }`
- [x] **2.2** 扩展 `LoaderStage.LoadMultiple(assemblyPaths, entryAssemblyPath)`
- [x] **2.3** 通过 union world + subject id / assembly name 统一索引接通 cross-assembly reference
- [x] **2.4** 扩展 `ManagedClosureRequest` 携带 additional assembly paths

## Batch 3: Pipeline 下游模块扩展

- [x] **3.1** 扩展 SemanticWorld：接受 `LoadedWorldModel`，跨 assembly 分析 capabilities
- [x] **3.2** 扩展 Linker：reachability analysis 跨 assembly 边界追踪
- [x] **3.3** 扩展 MetadataWriter：multi-assembly registration tables
- [x] **3.4** 扩展 CodeGen：按 assembly 分组 `code-registration.json`，并生成最小 `generated/assemblies/<Assembly>/` 目录骨架
- [x] **3.5** 扩展 Pipeline：编接 multi-assembly 流程

## Batch 4: Golden Projects + CLI 接线

- [x] **4.1** 创建 `subjects/golden/GoldenSimpleLib/`：1 个 console app + 1 个 class library
- [x] **4.2** 创建 `subjects/golden/GoldenMultiProject/`：3 个互相引用的 csproj + 1 个 entry
- [x] **4.3** 创建 `subjects/golden/GoldenWithPackage/`：1 个 csproj + Newtonsoft.Json `PackageReference`
- [x] **4.4** 更新 `chaos-il2cpp convert` 支持 ProjectGraph 模式并写出 `project-graph.json`
- [x] **4.5** 编写 `test_project_graph.py` graph snapshot tests

## Batch 5: 验证

- [x] **5.1** `chaos-il2cpp convert subjects/golden/GoldenSimpleLib` 成功
- [x] **5.2** `chaos-il2cpp convert subjects/golden/GoldenMultiProject` 成功
- [x] **5.3** `chaos-il2cpp convert subjects/golden/GoldenWithPackage` 成功
- [x] **5.4** 输出目录满足 Phase 1 最小 `generated/assemblies/<Assembly>/` 结构要求，并写出 `project-graph.json`
- [x] **5.5** `test_project_graph.py` 整体执行时间落在可接受范围内（本次 10.23s，含 3 次 `dotnet run`）
- [x] **5.6** 运行完整 run/tooling/shared-contract 回归：`311 passed, 34 skipped`
