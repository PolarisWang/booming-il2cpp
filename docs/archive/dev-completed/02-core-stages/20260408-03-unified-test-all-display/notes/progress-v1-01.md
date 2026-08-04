# 进度记录 v1

- 时间：2026-04-08 19:15:45 +08:00
- 阶段：completed

## 本轮动作

- 先补 `tests/tooling/run/test_tui.py` 与 `tests/tooling/run/test_unified_test_command.py` 的 RED 测试，锁定 `test all` preview、progress 与 summary 的目标行为。
- 在 `build/toolchains/run/commands/test.py` 增加 `plannedCounts`、`countsByType`、`countsByFamily`、`failureItems` 和 `activeUnitContext` 聚合数据。
- 在 `build/toolchains/run/testing/reporting.py` 将上述聚合字段写入 `summary.json` 与 `final-summary` event。
- 在 `build/toolchains/run/tui.py` 增加 `test all` 启动前 preview，并增强 progress 页面上的 `Batch / Counts / Current / Failures` 区块。

## 验证结果

- `python -m unittest tests.tooling.run.test_tui tests.tooling.run.test_unified_test_command`
- `python -m unittest tests.tooling.run.test_command_manifest tests.tooling.run.test_registry_command tests.tooling.run.test_tui tests.tooling.run.test_unified_test_command tests.unit.run.test_repo_layout`

## 结果判断

- `test all` 菜单标题已明确为 `suites + subjects`。
- 进入 `test all` 前会展示 host、planned 数量、family 聚合、subject 预览和输出路径。
- 运行中的 watch / progress 页面可以直接看到 batch 规模、按 type 统计、当前 active unit 上下文和 failure rerun 命令。
- `test summary` 已输出 `Overall / Suite Breakdown / Subject Breakdown / Failure Digest`。
