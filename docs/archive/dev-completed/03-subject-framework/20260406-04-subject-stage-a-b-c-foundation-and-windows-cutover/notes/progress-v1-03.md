# 执行进度 v1.03

Date: 2026-04-06 18:50:00 +08:00

## 本轮进展

- 已把 `tests/tooling/run/test_verify_roadmap0.py` 收敛到新的 subject-based 真相：
  - 不再依赖已删除的 Stage 4 helper。
  - 改为校验 Python `execute_subject_matrix()` 路由、PowerShell forwarder，以及 proof artifact 兼容校验。
- 已在 `build/scripts/verify-roadmap-0.py` 增加 `resolve_stage4_runtime_root()`，让 proof artifact 校验同时兼容：
  - 新的 runtime bucket 根目录。
  - legacy 根目录下的 `run/` 子目录。
- 已补长期知识沉淀：
  - `wiki/03-功能模块/06-il2cpp核心架构/05-架构参考/INDEX.md`
  - `docs/archive/architecture/subject-test-framework-v1/INDEX.md`
  - `docs/archive/architecture/subject-test-framework-v1/foundation-and-windows-cutover-v1.md`

## 验证

- 已通过：
  - `python -m unittest tests.tooling.run.test_verify_roadmap0`
  - `python -m unittest tests.unit.run.test_subject_manifest_schema tests.unit.run.test_subject_contracts_source_cutover tests.unit.run.test_subject_planner tests.unit.run.test_subject_executor tests.unit.run.test_helloworld_subject_windows_cutover tests.tooling.run.test_contract_paths tests.tooling.run.test_contract_check tests.unit.run.test_stage4_windows_reference_wiring tests.tooling.run.test_verify_roadmap0 tests.unit.run.test_stage4_bootstrap_support_skeleton tests.unit.run.test_stage4_native_reference_proof`
- 已尝试执行：
  - `python build/scripts/verify-roadmap-0.py windows`
- 实际结果：
  - 该命令在 `cmake --list-presets=all` 前即失败，宿主报 `FileNotFoundError: [WinError 2]`，根因是当前机器 PATH 中不存在 `cmake`。
  - 这是环境缺口，不是本轮 subject cutover 的代码 contract 失败。

## 当前判断

- `subjects/HelloWorldObject/`、contract cutover、planner/executor/worker skeleton、Windows 首批 matrix cutover 与 `verify-roadmap-0` compatibility gate 退化边界都已完成代码收口。
- 条件性实机 gate 缺口已经显式记录，不再阻塞本任务归档。

## 后续动作

- 将本任务转入 `completed`。
- 父 roadmap 只保留后续 `Stage D + Stage E` 与 `Stage F` 的派生入口。
