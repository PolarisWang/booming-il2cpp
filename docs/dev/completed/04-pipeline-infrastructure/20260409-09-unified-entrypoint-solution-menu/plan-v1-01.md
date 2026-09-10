# 统一入口工程 / IDE 菜单改造实现计划

> **面向执行 Agent：** 必须使用 `dev:executing-plans` 或 `dev:subagent-driven-development` 执行本计划。步骤使用复选框（`- [ ]`）语法跟踪。

**目标：** 把现有 `generate/build project subject|core` 能力提升为统一入口中的一等“工程 / IDE”菜单体验；把生成结果统一迁移到仓库根下的 `solutions/`；新增“生成所有 Solution（当前宿主）”入口；并让 `subject` solution 引用同次生成出的 native project。

**架构：** 在保留现有 `generate/build project subject|core` 兼容性的前提下，扩展 `project_workspace.py` 的输出根与 solution 组成；新增 `generate project all` 聚合命令；文本菜单和全屏 TUI 同步调整，避免两套菜单继续分裂。

**技术栈：** Python、`unittest`、统一入口 manifest/TUI、subject manifest 解析、solution writer

**设计文档：** `docs/dev/in-progress/20260409-09-unified-entrypoint-solution-menu/design-v1-01.md`

**预期知识沉淀：** `wiki/04-工具与集成/unified-project-workspaces-and-deploy-core.md`

---

## 目标文件与职责

- `build/toolchains/run/run_manifest.json`
  - 新增 `Project And IDE` 分组
  - 调整 5 个 project command 的 group / title / examples 展示元数据
- `build/toolchains/run/project_workspace.py`
  - 把工作区根迁移到 `solutions/**`
  - 生成根级聚合 manifest/report
  - 让 subject solution 引用 native project
- `build/toolchains/run/commands/project.py`
  - 接入 `generate project all`
- `build/toolchains/run/menu.py`
  - 文本菜单组名与中文标题别名
- `build/toolchains/run/tui.py`
  - 新增 `project-menu` 一级入口
  - 新增 `工程 / IDE` 子菜单
  - 新增 `generate-project-all`
  - 为 subject/core generate/build 增加参数选择流
  - 在菜单页底部增加选中项说明区
- `tests/tooling/run/test_command_manifest.py`
  - 锁定新分组与命令可见性
- `tests/tooling/run/test_tui.py`
  - 锁定一级菜单、`project-menu` 子菜单、说明区和参数选择路由
- `tests/tooling/run/test_tui_routing.py`
  - 锁定新菜单路由不会破坏现有 bare run / fullscreen / inline 行为
- `tests/tooling/run/test_project_command.py`
  - 锁定菜单生成的 argv 仍与既有 CLI handler 兼容
- `tests/unit/run/test_project_workspace.py`
  - 锁定 `solutions/**` 输出布局、聚合 manifest 与 subject solution/native project 引用
- `wiki/04-工具与集成/unified-project-workspaces-and-deploy-core.md`
  - 补充 `solutions/**` 根目录布局、“工程 / IDE”入口层说明和“生成所有 Solution”的菜单语义

## 执行任务

- [ ] Task 1：冻结新的根目录布局与命令元数据
  - 文件：
    - `build/toolchains/run/run_manifest.json`
    - `build/toolchains/run/project_workspace.py`
    - `build/toolchains/run/menu.py`
    - `tests/tooling/run/test_command_manifest.py`
    - `tests/unit/run/test_project_workspace.py`
  - 责任：
    - 新增 `Project And IDE` 分组
    - 新增 `generate-project-all`
    - 把 `generate/build project subject|core` 迁入该分组
    - 冻结输出根为 `solutions/**`
    - 锁定中文菜单标题与“全部支持目标”表述
  - 验证：
    - `python -m unittest tests.tooling.run.test_command_manifest tests.unit.run.test_project_workspace`

- [ ] Task 2：迁移 solution/workspace 输出根并补聚合生成入口
  - 文件：
    - `build/toolchains/run/project_workspace.py`
    - `build/toolchains/run/commands/project.py`
    - `tests/unit/run/test_project_workspace.py`
    - `tests/tooling/run/test_project_command.py`
  - 责任：
    - 把 subject/core solution 与 workspace 根从 `artifacts/projects/**` 迁移到 `solutions/**`
    - 新增 `generate project all`
    - 为聚合命令生成 `solutions/manifest.json` 与 `solutions/all/generation.report.json`
    - 保持现有 `generate/build project subject|core` CLI 兼容
  - 验证：
    - `python -m unittest tests.unit.run.test_project_workspace tests.tooling.run.test_project_command`

- [ ] Task 3：让 subject solution 引用同次生成出的 native project
  - 文件：
    - `build/toolchains/run/project_workspace.py`
    - `tests/unit/run/test_project_workspace.py`
  - 责任：
    - subject 每个选中 matrix 生成一个 native project/configure root
    - solution writer 支持把 native `.vcxproj` 与 `.csproj` 一起写入 subject `.sln`
    - 默认目标、指定目标、全部支持目标三种模式都保证 solution 引用集合正确
  - 验证：
    - `python -m unittest tests.unit.run.test_project_workspace`

- [ ] Task 4：新增 TUI 一级 `project-menu`、聚合入口与参数选择流
  - 文件：
    - `build/toolchains/run/tui.py`
    - `tests/tooling/run/test_tui.py`
    - `tests/tooling/run/test_tui_routing.py`
    - `tests/tooling/run/test_project_command.py`
  - 责任：
    - 在一级菜单中把 `project-menu` 放在 `prepare-menu` 后、`build-menu` 前
    - 新增 5 个菜单项：
      - `generate-project-all`
      - `generate-project-subject`
      - `generate-project-core`
      - `build-project-subject`
      - `build-project-core`
    - `generate-project-all` 提供“是否刷新全部 generated”的确认流
    - subject/core 子入口继续提供 `subject / target / all-targets / variant / refresh-generated` 选择流
    - 保持 `deploy-core` 留在 `Build`
  - 验证：
    - `python -m unittest tests.tooling.run.test_tui tests.tooling.run.test_tui_routing tests.tooling.run.test_project_command`

- [ ] Task 5：增加说明区与文档沉淀
  - 文件：
    - `build/toolchains/run/tui.py`
    - `build/toolchains/run/menu.py`
    - `wiki/04-工具与集成/unified-project-workspaces-and-deploy-core.md`
  - 责任：
    - 在菜单页底部增加选中项说明区
    - 固定展示：
      - 简介
      - 输出
      - 目标范围
      - 当前宿主支持
      - 执行命令
      - 备注
    - 统一声明：
      - 所有生成根统一在 `solutions/**`
      - subject solution 会引用 native project
      - native 侧输出仍是 workspace / native project，不与 managed `.sln` 混称
  - 验证：
    - `python -m unittest tests.tooling.run.test_tui`

- [ ] Task 6：运行回归并收口任务状态
  - 文件：
    - `docs/dev/in-progress/20260409-09-unified-entrypoint-solution-menu/STATUS.md`
    - `docs/dev/in-progress/20260409-09-unified-entrypoint-solution-menu/notes/progress-v1-01.md`
    - `wiki/04-工具与集成/unified-project-workspaces-and-deploy-core.md`
  - 验证：
    - `python -m unittest tests.tooling.run.test_command_manifest tests.tooling.run.test_tui tests.tooling.run.test_tui_routing tests.tooling.run.test_project_command tests.unit.run.test_project_workspace`

## 实施边界

- 不新增脱离 `project.dispatch` 的第二套生成入口。
- 不把 `deploy core` 混入 `工程 / IDE` 一级菜单。
- 不在本次直接实现 “打开 VS / 打开 Xcode”。

## 完成标准

- 用户在文本菜单中能直接看见“工程 / IDE”组。
- 用户在全屏 TUI 中能进入 `project-menu`。
- 用户在菜单中能直接选择“生成所有 Solution（当前宿主）”。
- 用户不需要离开菜单即可补齐 `subject` / `target` / `all-targets` / `variant` 等必填参数。
- 生成结果统一落在仓库根下的 `solutions/**`。
- `subject` solution 能引用同次生成的 native project。
- 菜单说明区能够解释 `.sln`、native project 与 workspace 的区别。
