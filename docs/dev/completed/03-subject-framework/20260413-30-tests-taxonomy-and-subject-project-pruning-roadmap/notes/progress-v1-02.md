# Progress v1.02

Date: 2026-04-13 18:44:49 +08:00

## 本轮进展

- 修复了 `build/scripts/verify-runtime-baseline.py` 的真实生产问题：
  - 改为 `from core import tooling as tooling_module`
  - 新增 `resolve_subject_matrix_subject_id(...)`
  - `execute_subject_matrix(...)` 改为走 helper
- 将旧 taxonomy 假设迁出核心回归：
  - planner 测试里的 `benchmark` live subject 假设改为 retained live subject 或 fixture subject
  - engine / mobile 相关 planner 测试改为 fixture subject repo
  - subject command 测试移除 subject-owned xUnit 假设，保留“无 subject-owned validation 时结果为空”的当前行为
  - perf policy / managed closure / interpreter lowering 测试全部对齐 retained solution-slice 与新的 CLI / attribute 入口
- 更新 `managed-minimal-closure-v0.md`，把输入路径明确成 retained solution slice 模式。

## 验证

- `python -m pytest tests/tooling/run/test_verify_roadmap0.py tests/unit/planning/test_subject_planner.py tests/tooling/run/test_subject_command.py tests/unit/performance/test_subject_perf_policy.py tests/unit/compatibility/test_managed_closure_contract_bundle.py tests/unit/compatibility/test_interpreter_mixed_execution_subjects.py -q`
  - 结果：`74 passed, 1 skipped`
- `python -m pytest tests/unit/compatibility tests/unit/planning tests/unit/selection tests/unit/execution tests/unit/performance tests/unit/reporting tests/unit/run tests/tooling/run tests/integration/registry tests/integration/run -q`
  - 结果：`743 passed, 40 skipped`

## 结论

- 该 roadmap 的 Phase 4-5 已完成。
- 任务已满足归档条件，并转入 `docs/dev/completed/`.
