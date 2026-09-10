# Full Project Ingestion Progress v1.02

Date: 2026-04-11 16:00:00 +08:00

## 本轮推进

- 新建 `Chaos.IL2CPP.ProjectGraph`，补齐 `ProjectGraphLoader`、`ProjectGraphModel`、`PackageResolver`。
- 新建 `LoadedWorldModel`，将 `LoaderStage`、`PipelinePlan`、`SemanticWorldStage`、`LinkerStage`、`MetadataWriterStage`、`CodeGenStage` 切到多程序集 world。
- `chaos-il2cpp convert` 对 `dotnet-project` 输入新增 `project-graph.json` 输出，并把 `generated/assemblies/<Assembly>/` 作为 Phase 1 的最小 native 目录骨架。
- 修正 `build/toolchains/run/testing/subjects.py` 的 subject discovery 边界，只把 `subjects/<id>/subject.manifest.json` 视为标准 subject，避免 `subjects/golden/*` 干扰 subject registry / planner。

## 验证结果

- `python -m pytest tests/unit/run/test_repo_layout.py -v`
- `python -m pytest tests/unit/run/test_project_graph.py -v`
- `python -m pytest tests/contracts/shared/ tests/unit/run/ tests/tooling/run/ -q`

## 结论

- Phase 1 所需的 golden project ingestion / multi-assembly convert / basic package graph 已闭环。
- 主线下一步切到 `20260411-03-aot-runtime-completeness`。
