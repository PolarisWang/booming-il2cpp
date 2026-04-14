# Follow-up Progress 2026-04-14 19:54:36 +08:00

- 完成 planning tests 三层拆分：
  - `test_subject_planner.py` 仅保留 generic synthetic manifest contract tests。
  - `test_fixture_subject_planner.py` 独立承载 fixture repo smoke。
  - `test_solution_core_pack_planner.py` 集中承载 retained real-subject planning 回归。
- 整理 `test_fixture_subject_planner.py` 的 `shutil` 导入，移除局部 `finally` import。
- 修正 3 个 stale compatibility tests，对齐到“legacy static core solution 已移除”的新契约，避免继续硬编码已删除的 `solutions/core/windows/chaos-il2cpp-core.sln`。
- 验证通过：
  - `python -m pytest tests/unit/planning/test_subject_planner.py`
  - `python -m pytest tests/unit/planning/test_fixture_subject_planner.py`
  - `python -m pytest tests/unit/planning/test_solution_core_pack_planner.py`
  - `python -m pytest tests/unit/planning`
  - `python -m pytest`
