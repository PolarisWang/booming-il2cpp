# Progress v1.05

Date: 2026-04-06 23:45:00 +08:00

## 本次完成

- 按 TDD 扩展并转绿以下 public cutover 测试：
  - `tests/integration/run/test_registry_scan.py`
  - `tests/tooling/run/test_registry_command.py`
  - `tests/tooling/run/test_command_manifest.py`
  - `tests/tooling/run/test_subject_command.py`
  - `tests/tooling/run/test_tui.py`
- 已让 `subject` 成为统一 registry/public surface 的一等对象：
  - registry 新增 `subject` collection 与 `subject/<subject-id>` object id
  - `canonicalCommand` 固定为 `run test subject --id subject/<subject-id>`
  - command manifest / CLI parser 已支持 `test subject`
- 已让 `test subject` 直连 `subject_planner + subject_executor`：
  - public dispatch 不再回退到 legacy suite session
  - matrix report 固定写到 `artifacts/subjects/<subject-id>/matrices/<matrix-id>/report.json`
  - subject summary 固定写到 `artifacts/subjects/<subject-id>/subject-report/summary.json`
  - session `summary.json` / `final-summary` 继续以 `subjectResults` / `subjectStatusCounts` 暴露 subject 结果
- 已完成 TUI 最小 cutover：
  - test menu 新增 `subject` selector
  - `test watch` / TUI progress screen 现在会显示 `Subjects:`
  - highlight 区域会直接导航到 `Subject summary`
- 已写入长期知识：
  - `wiki/03-功能模块/06-il2cpp核心架构/docs/archive/architecture/subject-test-framework-v1/reporting-and-public-cutover-v1.md`
  - `wiki/06-测试验证/subject-public-entry-and-reporting-cutover.md`
- 已通过组合验证：
  - `python -m unittest tests.unit.run.test_subject_manifest_schema tests.unit.run.test_subject_planner tests.unit.run.test_subject_reporting tests.unit.run.test_events_schema tests.unit.run.test_subject_executor tests.unit.run.test_helloworld_subject_windows_cutover tests.integration.run.test_reporting_layout tests.integration.run.test_registry_scan tests.tooling.run.test_command_manifest tests.tooling.run.test_registry_command tests.tooling.run.test_subject_command tests.tooling.run.test_test_watch_summary tests.tooling.run.test_tui`

## 当前判断

- `Chunk 1-6` 的代码与定向测试已经落地，public subject entry/reporting/TUI cutover 现有回归保护。
- 当前仍不能声称 Windows compatibility gate 通过，因为 `python build/scripts/verify-roadmap-0.py windows` 在本机缺少 `cmake` 时会于第一步退出。
- 本轮不是代码缺陷阻塞，而是环境缺件阻塞；因此任务保持 `in_progress/executing`，等待环境补齐后复跑最后一道门。

## 下一步

- 安装或恢复 `cmake` 到 PATH。
- 重跑 `python build/scripts/verify-roadmap-0.py windows`。
- 若通过，则将本任务归档为 `completed`。