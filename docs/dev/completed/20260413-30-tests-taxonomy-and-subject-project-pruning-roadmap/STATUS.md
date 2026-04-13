---
task_id: 20260413-30-tests-taxonomy-and-subject-project-pruning-roadmap
title: Tests Taxonomy And Subject Project Pruning Roadmap
task_type: roadmap
lifecycle_status: completed
phase: archived
created_at: 2026-04-13 14:28:02 +08:00
updated_at: 2026-04-13 18:44:49 +08:00
current_dir: docs/dev/completed/20260413-30-tests-taxonomy-and-subject-project-pruning-roadmap
parent_task_id:
source_task_id: 20260413-24-solution-core-pack-subject-consolidation
source_relation: follow-up-roadmap-after-retained-subject-cutover
active: false
---

## 关键文档

- reference_previous_roadmap: `docs/dev/completed/20260413-24-solution-core-pack-subject-consolidation/roadmap-v1-01.md`
- brainstorm: `docs/dev/completed/20260413-30-tests-taxonomy-and-subject-project-pruning-roadmap/brainstorm-v1-01.md`
- design: `docs/dev/completed/20260413-30-tests-taxonomy-and-subject-project-pruning-roadmap/design-v1-01.md`
- roadmap_or_plan: `docs/dev/completed/20260413-30-tests-taxonomy-and-subject-project-pruning-roadmap/roadmap-v1-02.md`
- latest_progress: `docs/dev/completed/20260413-30-tests-taxonomy-and-subject-project-pruning-roadmap/notes/progress-v1-02.md`

## 完成判定

- 顶层 `subjects/` 已收口到 3 个 retained subject：
  - `SolutionCorePack`
  - `HotUpdateHostPack`
  - `MixedExecutionFeaturePack`
- compatibility alias 已一次性删除。
- legacy 顶层 subject 已迁移或删除，不再恢复。
- `tests/` 中最强的旧 taxonomy 假设已迁到 generic fixture 或 retained smoke。
- `build/scripts/verify-runtime-baseline.py` 已修复为当前 `core.tooling` 导入路径，并补齐 matrix -> subject 解析 helper。
- retained subject 的 proof / benchmark 真跑证据已保留，且后续回归全部通过。

## 最近摘要

- 2026-04-13 18:44:49 +08:00: 完成 Phase 4-5 收尾。修复 `verify-runtime-baseline.py` 的真实导入缺陷；将 planner / subject command / compatibility / performance 测试对齐 retained subject 与 fixture 机制；更新 managed closure 文档到 solution-slice 路径；完成最小回归与 broader suite 验证；任务归档。

## 验证结果

- retained subject 实跑证据
  - `python build/toolchains/run/run.py test registry refresh --json`
  - `python build/toolchains/run/run.py test subject --id subject/SolutionCorePack --json`
    - proof runId: `20260413-175456-windows-458e`
  - `python build/toolchains/run/run.py test subject --id subject/SolutionCorePack --goal perf.release --validation-profile perf-profile --variant PROFILE --json`
    - benchmark runId: `20260413-175603-windows-5044`
  - `python build/toolchains/run/run.py test subject --id subject/HotUpdateHostPack --json`
    - proof runId: `20260413-175641-windows-7a4f`
  - `python build/toolchains/run/run.py test subject --id subject/HotUpdateHostPack --goal perf.release --validation-profile perf-profile --variant PROFILE --json`
    - benchmark runId: `20260413-175657-windows-7df6`
  - `python build/toolchains/run/run.py test subject --id subject/MixedExecutionFeaturePack --json`
    - proof runId: `20260413-175719-windows-2247`
  - `python build/toolchains/run/run.py test subject --id subject/MixedExecutionFeaturePack --goal perf.release --validation-profile perf-profile --variant PROFILE --json`
    - managed benchmark runId: `20260413-175736-windows-b2f0`
  - `python build/toolchains/run/run.py test subject --id subject/MixedExecutionFeaturePack --goal perf.release --matrix windows-native-perf --validation-profile perf-profile --variant PROFILE --json`
    - native benchmark runId: `20260413-175802-windows-a0e2`
  - `python build/toolchains/run/run.py test subject --id subject/MixedExecutionFeaturePack --goal perf.release --matrix windows-interpreter-perf --validation-profile perf-profile --variant PROFILE --json`
    - interpreter benchmark runId: `20260413-175836-windows-7046`
- 本轮收尾回归
  - `python -m pytest tests/tooling/run/test_verify_roadmap0.py tests/unit/planning/test_subject_planner.py tests/tooling/run/test_subject_command.py tests/unit/performance/test_subject_perf_policy.py tests/unit/compatibility/test_managed_closure_contract_bundle.py tests/unit/compatibility/test_interpreter_mixed_execution_subjects.py -q`
    - `74 passed, 1 skipped`
  - `python -m pytest tests/unit/compatibility tests/unit/planning tests/unit/selection tests/unit/execution tests/unit/performance tests/unit/reporting tests/unit/run tests/tooling/run tests/integration/registry tests/integration/run -q`
    - `743 passed, 40 skipped`

## 残留观察

- `tests/` 中仍保留部分 `select_subject_record(...)` 用法，但已不再依赖旧 benchmark/xUnit/csharp-perf-harness taxonomy；剩余用法主要用于 retained smoke / registry / TUI 级联验证。
- 用户之前指出的“`tests/unit/planning/test_subject_planner.py` 以及其他 `tests/` 里仍有 subject 关联逻辑”已从旧 subject-heavy 假设收缩到 retained smoke 边界；如果后续要继续做更彻底的 generic 化，可从当前归档任务另起 follow-up。

## Wiki

- 本轮未新增 wiki。当前结论主要属于任务执行与收尾记录，保留在本任务归档目录即可。
