# Phase 1 Subject Verification Closure 实现计划

> **面向执行 Agent：** 必须使用 `dev:executing-plans` 执行本计划。步骤使用复选框语法跟踪。

**目标：** 修复 retained subjects 的 benchmark/dashboard 历史 identity 漂移，补齐 Phase 1 所需的 verification closure 基线。
**架构：** 以 Phase 0 冻结的 complete verification checklist 为验收基线，先锁定 dashboard generator 的历史 case 污染缺陷，再以最小实现收口当前 declared benchmark case、subject summary 与 docs 输出。
**技术栈：** Python、`pytest`、`docs/dev` 状态维护、现有 benchmark dashboard generator 与静态 JSON/HTML 产物。
**设计文档：** `docs/dev/in-progress/20260414-12-il2cpp-core-capability-development-roadmap/design-v1-01.md`

**预期知识沉淀：** 先保留在本子任务目录；若 Phase 1 结论稳定，再由父 roadmap 决定是否并入 `docs/architecture`。

---

## 文件与职责

- `docs/dev/in-progress/20260414-14-phase-1-subject-verification-closure/STATUS.md`
  - 记录本子任务状态、风险、摘要和下一步。
- `docs/dev/in-progress/20260414-14-phase-1-subject-verification-closure/plan-v1-01.md`
  - 记录本阶段执行步骤与验收方式。
- `docs/dev/in-progress/20260414-14-phase-1-subject-verification-closure/notes/progress-*.md`
  - 记录本轮执行摘要。
- `tests/unit/performance/test_benchmark_dashboard_generator.py`
  - 补足 Phase 1 的 RED 用例，固定 dashboard 只展示当前 declared benchmark case。
- `build/toolchains/run/testing/benchmark_dashboard_generator.py`
  - 修复 case 收集逻辑，过滤历史已失效 benchmark case，同时保留当前声明但暂缺记录的 case 占位。
- `docs/benchmark/overview.json`
  - 若生成器输出变化，刷新 overview 基线。
- `docs/benchmark/subjects/*.json`
  - 若生成器输出变化，刷新各 retained subject 的 benchmark JSON。
- `docs/benchmark/dashboard.html`
  - 若生成器输出变化，刷新 dashboard 内嵌数据。

## 执行步骤

- [x] Task 1: 建立并激活 Phase 1 child task，更新 `STATUS.md`、`docs/dev/ACTIVE.md` 和索引。
- [x] Task 2: 在 `tests/unit/performance/test_benchmark_dashboard_generator.py` 增加“历史失效 case 不得出现在 dashboard 输出中”的失败测试。
- [x] Task 3: 修复 `benchmark_dashboard_generator.py` 的 case 收集逻辑，只保留当前 declared benchmark cases。
- [x] Task 4: 回归 `tests/unit/performance/test_benchmark_dashboard_generator.py` 与 retained subject command/worker/compatibility 相关测试。
- [x] Task 5: 刷新 `docs/benchmark` 产物，确认 `SolutionCorePack.json` 不再包含历史 `PerformanceFeaturePack/*` case。
- [x] Task 6: 完成 Phase 1 状态回写；若验收通过，更新父 roadmap，将 `20260414-14` 标记为 completed，并将 `20260414-15` 切到 ready。

## 验证

- `python -m pytest -q tests/unit/performance/test_benchmark_dashboard_generator.py`
- `python -m pytest -q tests/tooling/run/test_subject_command.py tests/unit/execution/test_subject_workers.py tests/unit/compatibility/test_solution_core_pack_subject.py tests/unit/compatibility/test_hot_update_skeleton_subject.py tests/unit/compatibility/test_interpreter_mixed_execution_subjects.py`
- `Select-String -Path docs/benchmark/subjects/SolutionCorePack.json -Pattern "PerformanceFeaturePack|CoreRuntimeBenchmarks"`

## 状态维护

- 进入代码修复前，先把 `docs/dev/ACTIVE.md` 指向当前 child task。
- 每完成一个关键节点，更新本任务 `STATUS.md` 的最近摘要和下一步。
- 若 Phase 1 验收通过，回写父 roadmap `STATUS.md`、`roadmap-v1-01.md`、`docs/dev/ACTIVE.md` 与索引。
