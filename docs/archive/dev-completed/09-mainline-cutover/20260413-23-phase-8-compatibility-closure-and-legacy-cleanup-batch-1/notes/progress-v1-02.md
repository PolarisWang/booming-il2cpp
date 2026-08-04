# Progress v1.02

日期：2026-04-13 07:24:06 +08:00

## 本轮完成

- `tests/unit/planning/test_project_graph.py` 已去除对 `subjects/golden/GoldenMultiProject` shell 的依赖，改为直接验证 `SolutionMultiProject` 的 canonical convert 行为，同时保留 `GoldenMultiProject.*` assembly name 合同断言。
- `tests/unit/planning/test_subject_planner.py` 中剩余 legacy shell 规划用例已全部切换到 canonical packs：
  - `PerformanceFeaturePack`
  - `HotUpdateHostPack`
  - `MixedExecutionFeaturePack`
  - `MainlineFeaturePack`
- hot-update proof / benchmark 与 mixed-execution proof / benchmark 相关 planner case 现在显式断言 canonical source path、source entry、workload entry，而不再通过已删除 shell subject id 间接表达。
- interpreter arithmetic support project 的约束已改为：
  - planner 选择 canonical `MixedExecutionFeaturePack.csproj`
  - nested support project `InterpreterArithmeticProof.csproj` 继续存在于 canonical source tree
- mainline proof slice 的 planner 断言已统一改成 canonical `MainlineFeaturePack + matrix + validation_profile_id` 选择模型。

## 验证结果

- `python -m pytest tests/unit/planning/test_project_graph.py tests/unit/planning/test_subject_planner.py -q`
  - 结果：`44 passed`
- `python -m pytest -q`
  - 结果：`781 passed, 40 skipped`

## 结论

- Phase 8 Batch 1 已满足完成条件。
- 本批无额外 wiki 更新要求；长期有效的结构边界已经沉淀在 `design-v1-03.md`、`roadmap-v1-03.md` 与归档任务状态中。
