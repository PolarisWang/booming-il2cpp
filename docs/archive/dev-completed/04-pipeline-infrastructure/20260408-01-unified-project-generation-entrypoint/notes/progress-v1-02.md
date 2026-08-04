# 进展记录 v1.02

## 时间

- 2026-04-08 13:13:54 +08:00

## 本轮完成

- 已落地统一入口的 `generate project subject|core`、`build project subject|core` 与 `deploy core`。
- 已冻结 `artifacts/projects/**` 调试工作区与 `deploy/core/**` 正式产品输出的边界。
- 已把 `subject-exec` 规则固定为仅 `generate project subject --refresh-generated` 显式刷新。
- 已补齐 `project workspace` / `deploy core` 自动化测试，并同步 wiki 与 `docs/dev` 状态。

## 验证

- `python -m unittest tests.tooling.run.test_command_manifest tests.tooling.run.test_project_command tests.unit.run.test_project_workspace tests.unit.run.test_deploy_core tests.tooling.run.test_registry_command tests.tooling.run.test_tui tests.tooling.run.test_build tests.tooling.run.test_operation_reporting`
- 结果：`Ran 70 tests in 3.150s`，`OK (skipped=25)`。

## Wiki 决策

- 已新增 `wiki/04-工具与集成/unified-project-workspaces-and-deploy-core.md` 并更新 `wiki/04-工具与集成/INDEX.md`、`wiki/INDEX.md`。
- 已清理 `wiki/04-工具与集成/统一入口脚本.md` 中误留的 `# TEMP`。
