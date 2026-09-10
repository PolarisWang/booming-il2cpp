# Planning Tests Genericization Follow-up 实现计划

> **面向执行 Agent：** 必须使用 `dev:executing-plans` 执行本计划。步骤使用复选框语法跟踪。

**目标：** 把 planning 层测试按 generic contract、fixture repo smoke、retained real-subject integration 三层重新分离，并保持 planner 行为不回退。

**架构：** 以测试数据来源为分层边界，而不是以断言数量为边界。`test_subject_planner.py` 回归纯 synthetic manifest contract；fixture repo smoke 新拆文件；真实 `SolutionCorePack` planning 行为集中到 dedicated retained suite。

**技术栈：** Python `pytest` / `unittest`、`tests.support` fixture helper、`build/toolchains/run/testing/subject_planner.py`

**设计文档：** `docs/dev/in-progress/20260414-36-planning-tests-genericization-followup/design-v1-01.md`

**预期知识沉淀：** 暂不更新 `wiki/`；以 `docs/dev` 记录本次测试分层规则。

---

## 涉及文件与职责

- `tests/unit/planning/test_subject_planner.py`
  - 收缩为纯 generic synthetic-manifest contract tests。
- `tests/unit/planning/test_fixture_subject_planner.py`
  - 新建，承载 fixture subject repo smoke。
- `tests/unit/planning/test_solution_core_pack_planner.py`
  - 扩充，吸收真实 `SolutionCorePack` / declared entry / archetype / thin manifest 回归。
- `tests/support.py`
  - 仅在需要去重 helper 时最小调整。
- `build/toolchains/run/testing/subject_planner.py`
  - 仅在测试暴露真实 contract 缺口时做最小修复。
- `docs/dev/in-progress/20260414-36-planning-tests-genericization-followup/STATUS.md`
  - 持续更新阶段、摘要、下一步。
- `docs/dev/ACTIVE.md`
  - 维护当前活动任务指针。
- `docs/dev/in-progress/20260414-36-planning-tests-genericization-followup/notes/progress-*.md`
  - 记录实施进展与验证结果。

## 执行步骤

- [ ] 盘点 `test_subject_planner.py` 中所有真实 retained subject 断言，列出迁移清单。
- [ ] 新建 `test_fixture_subject_planner.py`，把 `clone_fixture_subject_repo(...)` 驱动的 fixture smoke 从 generic suite 迁出。
- [ ] 扩充 `test_solution_core_pack_planner.py`，吸收 `SolutionCorePack` 的 declared-unit-test、declared-benchmark、archetype、thin-manifest、native perf 默认项断言。
- [ ] 收缩 `test_subject_planner.py`，移除所有真实 retained subject 与 fixture smoke，仅保留 synthetic manifest contract tests。
- [ ] 如测试迁移暴露真正 helper 重复，再最小补 `tests/support.py` 或提炼 planning helper；否则不动。
- [ ] 跑局部验证：
  - `python -m pytest tests/unit/planning/test_subject_planner.py`
  - `python -m pytest tests/unit/planning/test_fixture_subject_planner.py`
  - `python -m pytest tests/unit/planning/test_solution_core_pack_planner.py`
- [ ] 跑汇总验证：
  - `python -m pytest tests/unit/planning`
  - `python -m pytest`
- [ ] 回写 `STATUS.md` / `ACTIVE.md` / `notes/progress-*.md`，如已闭环则归档或等待用户下一个 follow-up。

## 验收标准

- `tests/unit/planning/test_subject_planner.py` 不再直接引用真实 `SolutionCorePack`、真实 `subjects/*` 路径或 retained stable ID。
- fixture repo smoke 有独立文件承载，不再冒充 generic contract suite。
- `test_solution_core_pack_planner.py` 成为真实 retained planning 行为的集中回归入口。
- `python -m pytest tests/unit/planning`
- `python -m pytest`

## 风险控制

- 如果迁移后 `test_solution_core_pack_planner.py` 覆盖不足，优先补 retained suite，不把断言退回 generic suite。
- 如果 planner 生产代码无需修改，则不主动调整其实现，避免“为了整理测试而改行为”。
- 如果全量验证失败，按最小问题面逐个修复，不在同一轮混入额外结构重构。
