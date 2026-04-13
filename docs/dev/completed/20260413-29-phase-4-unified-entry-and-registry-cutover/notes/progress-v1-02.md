# 进度记录 v1.02

Date: 2026-04-13 13:26:39 +08:00

## 本次完成

- 完成 `SolutionCorePack` archetype consolidation 收口：
  - 把 `SolutionSimpleLib`、`SolutionMultiProject`、`SolutionPackageReference` 迁入 `subjects/SolutionCorePack/source/Archetypes/`
  - 删除 3 个已迁移的 legacy top-level subject 根
- 更新 `subjects/SolutionCorePack/source/SolutionCorePack.sln`、`subjects/SolutionCorePack/source/Launcher/SolutionCorePack.csproj` 与 `subjects/SolutionCorePack/subject.manifest.json`
- 一次性删除 `build/toolchains/run/testing/path_resolver.py` 中的 compatibility alias 解析，不再接受 `legacySubjectIds` / `subjectAliases` / `aliases`
- 把依赖真实 legacy subject 根的测试切到 synthetic fixture：
  - `tests/fixtures/subjects/solution-simple-lib/subject.manifest.json`
  - `tests/fixtures/subjects/solution-multi-project/subject.manifest.json`
  - `tests/fixtures/subjects/solution-package-reference/subject.manifest.json`
- 跑通定向回归：
  - `python -m pytest tests/unit/planning/test_solution_core_pack_planner.py tests/unit/compatibility/test_solution_core_pack_subject.py tests/unit/planning/test_project_graph.py tests/unit/selection/test_path_resolver.py tests/integration/registry/test_registry_scan.py tests/unit/compatibility/test_metadata_supplement_bridge_subjects.py tests/unit/performance/test_perf_governance.py -q`
  - 结果：`34 passed`
- 跑通扩展回归：
  - `python -m pytest tests/unit/planning/test_subject_planner.py tests/unit/execution/test_subject_executor.py tests/unit/execution/test_subject_workers.py tests/unit/compatibility/test_compiled_subject_catalog.py tests/unit/compatibility/test_subject_manifest_schema.py tests/integration/registry/test_declared_metadata_discovery.py tests/unit/planning/test_solution_core_pack_planner.py tests/unit/compatibility/test_solution_core_pack_subject.py tests/unit/planning/test_project_graph.py tests/unit/selection/test_path_resolver.py tests/integration/registry/test_registry_scan.py tests/unit/compatibility/test_metadata_supplement_bridge_subjects.py tests/unit/performance/test_perf_governance.py -q`
  - 结果：`142 passed`
- 跑通真实 retained subject / archetype 流程：
  - `python build/toolchains/run/run.py test registry refresh`
  - `python build/toolchains/run/run.py test subject --id subject/SolutionCorePack --matrix windows-archetype-simple-lib-managed-output`
  - `python build/toolchains/run/run.py test subject --id subject/SolutionCorePack --matrix windows-archetype-multi-project-managed-output`
  - `python build/toolchains/run/run.py test subject --id subject/SolutionCorePack --matrix windows-archetype-package-reference-managed-output`
  - `python build/toolchains/run/run.py test subject --id subject/SolutionCorePack`
  - `python build/toolchains/run/run.py test subject --id subject/SolutionCorePack --goal perf.release`
  - `python build/toolchains/run/run.py test subject --id subject/HotUpdateHostPack`
  - `python build/toolchains/run/run.py test subject --id subject/HotUpdateHostPack --goal perf.release`
  - `python build/toolchains/run/run.py test subject --id subject/MixedExecutionFeaturePack`
  - `python build/toolchains/run/run.py test subject --id subject/MixedExecutionFeaturePack --goal perf.release`

## 当前结论

- Phase 4 child 的目标不仅已经满足，而且已经连带完成了 Phase 5 verification closure 与 Phase 6 第一批 cleanup closure。
- retained solution-mode subject 的 canonical public path 已稳定，legacy `Solution*` archetype roots 与 compatibility alias 都已不再保留。

## 下一步建议

- 无。本 child task 可以归档。
