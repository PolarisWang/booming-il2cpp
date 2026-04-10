# Progress V1 03

- 2026-04-10 09:10:28 +08:00: `generate/build project subject|core` 已从 `Build` 菜单拆出并迁入新的 `Project And IDE` / `工程 / IDE` 入口，文本菜单与全屏/内联 TUI 现已保持一致。
- 2026-04-10 09:10:28 +08:00: 工程工作区根已从 `artifacts/projects/**` 迁移到仓库根下的 `solutions/**`；`generate project all` 现会写出 `solutions/manifest.json` 与 `solutions/all/generation.report.json`。
- 2026-04-10 09:10:28 +08:00: Windows 宿主下的 subject solution 现会引用同次生成的 native `.vcxproj`；TUI 新增了 `project-menu`、参数收集流和底部说明区。
- 2026-04-10 09:10:28 +08:00: 已完成计划内回归：`python -m unittest tests.tooling.run.test_command_manifest tests.tooling.run.test_tui tests.tooling.run.test_tui_routing tests.tooling.run.test_project_command tests.unit.run.test_project_workspace`。
