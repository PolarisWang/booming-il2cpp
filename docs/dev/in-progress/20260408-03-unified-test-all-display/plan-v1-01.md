# 统一入口 test all 展示增强实现计划

> **面向执行 Agent：** 使用 `dev:executing-plans` 按批次执行本计划。步骤使用复选框跟踪。

**目标：** 让统一入口 `test all` 在启动前、运行中、完成后三个阶段都能展示更完整的批次信息、状态摘要和失败定位信息。

**架构：** 先补测试，再补 `summary/final event` 的聚合字段，然后升级 `summary/progress/menu` 渲染层。保持 `test all` 的执行语义不变，只增强聚合数据和显示结果。

**技术栈：** Python、`unittest`、统一入口 TUI、测试汇总 reporting

**设计文档：** `docs/dev/in-progress/20260408-03-unified-test-all-display/design-v1-01.md`

**预期知识沉淀：** 按任务决定

---

- [ ] Task 1: 先补失败测试，覆盖 `test all` 菜单标题、启动前预览、progress 汇总和 summary 摘要
  - 文件：
    - `tests/tooling/run/test_tui.py`
    - `tests/tooling/run/test_unified_test_command.py`
  - 验证：
    - `python -m unittest tests.tooling.run.test_tui tests.tooling.run.test_unified_test_command`

- [ ] Task 2: 补 `test all` 的聚合字段与失败摘要数据
  - 文件：
    - `build/toolchains/run/commands/test.py`
    - `build/toolchains/run/testing/reporting.py`
  - 责任：
    - 生成 `plannedCounts`
    - 生成 `countsByType`
    - 生成 `countsByFamily`
    - 生成 `failureItems`
    - 在 progress 可用的事件里补 `activeUnitContext`

- [ ] Task 3: 升级 TUI 渲染
  - 文件：
    - `build/toolchains/run/tui.py`
  - 责任：
    - 升级 `test all` 菜单标题
    - 为 `test all` 增加启动前预览
    - 升级 progress 页面布局
    - 升级 summary 文本渲染

- [ ] Task 4: 运行回归并收口任务状态
  - 文件：
    - `docs/dev/in-progress/20260408-03-unified-test-all-display/STATUS.md`
    - `docs/dev/in-progress/20260408-03-unified-test-all-display/notes/progress-v1-01.md`
    - `docs/dev/ACTIVE.md`
  - 验证：
    - `python -m unittest tests.tooling.run.test_tui tests.tooling.run.test_unified_test_command`
    - `python -m unittest tests.tooling.run.test_command_manifest tests.tooling.run.test_registry_command tests.tooling.run.test_tui tests.tooling.run.test_unified_test_command tests.unit.run.test_repo_layout`
