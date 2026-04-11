# Project Graph Ingestion v1

## 目标

- 让 `chaos-il2cpp convert` 不再假定输入只有单个 DLL，而是能从真实 `.csproj` 入口解析 project graph。
- 把 `ProjectReference` 闭包、direct `PackageReference` 解析结果和 entry project 信息写成稳定的 `project-graph.json`。

## 当前规则

- `src/managed/Chaos.IL2CPP.ProjectGraph/` 是 project graph 解析的唯一入口。
- `ProjectGraphLoader` 负责：
  - 从 entry `.csproj` 出发做 `ProjectReference` BFS 闭包。
  - 为每个 project 生成 `assemblyName`、`projectPath`、`outputAssemblyPath`。
- `PackageResolver` 负责：
  - 读取 `obj/project.assets.json`。
  - 只解析 direct package 的首个 runtime/compile `.dll` 作为 Phase 1 的 package assembly 证据。
- Driver 在 `dotnet-project` convert 中会：
  - 先执行 `dotnet build`。
  - 再写出 `<output>/project-graph.json`。
  - 用 entry assembly + additional project assemblies 驱动多程序集 pipeline。

## Pipeline 约束

- `LoaderStage.LoadMultiple(...)` 会把 entry assembly 与 project-reference assemblies 合成单个 loaded world。
- `SemanticWorld` / `Linker` / `MetadataWriter` / `CodeGen` 都必须把“内部程序集集合”视为同一个 managed world，而不是把非入口程序集误判成 external dependency。
- `code-registration.json` 按程序集分 module 输出。
- `generated/assemblies/<Assembly>/` 是 Phase 1 冻结的最小 native 目录骨架；更细的 per-type production emitter 仍可后续扩展。

## Subject 边界

- `subjects/<id>/subject.manifest.json` 才是 run/testing 体系中的标准 subject。
- `subjects/golden/*` 只用于 Phase 1 project graph fixture，不参与通用 subject registry / planner / reporting discovery。

## 验证基线

- `tests/unit/run/test_project_graph.py`
- `tests/unit/run/test_repo_layout.py`
- `python -m pytest tests/contracts/shared/ tests/unit/run/ tests/tooling/run/ -q`
