# 进度记录 v1.02

Date: 2026-04-13 09:59:03 +08:00

## 本次完成

- 新建 `subjects/SolutionCorePack/subject.manifest.json`
- 新建 `subjects/SolutionCorePack/source/SolutionCorePack.sln`
- 新建 `subjects/SolutionCorePack/source/Launcher/SolutionCorePack.csproj`
- 新建 `subjects/SolutionCorePack/source/Launcher/Program.cs`
- 把 `MainlineFeaturePack`、`PerformanceFeaturePack` 与三个 `Solution*` archetype project 挂进 `SolutionCorePack.sln`
- 通过以下新增验证：
  - `tests/unit/compatibility/test_solution_core_pack_subject.py`
  - `tests/unit/planning/test_solution_core_pack_planner.py`
  - `tests/integration/registry/test_solution_core_pack_registry.py`
- 通过以下相关回归：
  - `tests/unit/compatibility/test_subject_manifest_schema.py`
  - `tests/unit/compatibility/test_compiled_subject_catalog.py`
  - `tests/unit/planning/test_subject_planner.py`
  - `tests/integration/registry/test_registry_scan.py`
  - `tests/integration/registry/test_declared_metadata_discovery.py`

## 当前结论

- `SolutionCorePack` 已经作为真实 retained subject 进入仓库，并能被 subject/planner/registry 识别。
- 本轮完成的是 skeleton 与识别链路，不是最终的真实 proof / benchmark 闭环。
- 执行中确认了一个后续必须处理的技术点：
  - 当前 `host-input -> analysis` 默认围绕 `primaryAssemblyPath`
  - 若 `source.entry` 位于 solution 中的非 primary assembly，真实执行需要补齐多程序集 entry 支持

## 下一步建议

- 进入 `Task 2`，优先承接 `SolutionSimpleLib`、`SolutionMultiProject`、`SolutionPackageReference`
- 在继续吸收 `MainlineFeaturePack` / `PerformanceFeaturePack` 之前，把多程序集 entry 的执行策略定实
