# 统一入口工程生成、核心宿主与部署输出实现计划

> 面向执行 Agent：本计划已完成归档，保留为本任务执行真源。

**目标：** 为统一入口补齐 `generate project subject|core`、`build project subject|core`、`deploy core` 三类正式能力；其中 `artifacts/projects/**` 只承载调试工作区，`deploy/core/**` 承载正式 core 产物，并让桌面 reference 目标交付 `chaos-il2cpp(.exe)` 入口。

**设计文档：** `docs/dev/completed/20260408-01-unified-project-generation-entrypoint/design-v1-01.md`

## 目标文件与职责

- `build/toolchains/run/commands/project.py`
  - 负责 `generate project subject|core` 的工作区生成。
- `build/toolchains/run/commands/deploy.py`
  - 负责 `deploy core` 的正式输出。
- `build/toolchains/run/run.py`
  - 接入 `project` / `deploy` 分发。
- `build/toolchains/run/run_manifest.json`
  - 注册新命令元数据。
- `build/toolchains/run/project_workspace.py`
  - 抽取 subject/core 工作区布局、manifest 与 build helper。
- `build/toolchains/run/deploy_layout.py`
  - 抽取 `deploy/core/**` 布局与 deploy helper。
- `build/toolchains/run/commands/build.py`
  - 增加 `build project subject|core` 分支。
- `tests/tooling/run/test_command_manifest.py`
  - 锁新命令可见性与 CLI parse。
- `tests/tooling/run/test_project_command.py`
  - 锁 `generate/build project` 与 `deploy core` handler 行为。
- `tests/unit/run/test_project_workspace.py`
  - 锁 workspace / manifest / 路径布局。
- `tests/unit/run/test_deploy_core.py`
  - 锁 `deploy/core/**` 的 direct-run host / package-root 语义。
- `wiki/04-工具与集成/unified-project-workspaces-and-deploy-core.md`
  - 沉淀长期有效的命令与产物边界规则。

## 执行原则

- [x] `artifacts/projects/**` 与 `deploy/core/**` 严格分层。
- [x] `subject-exec` 只允许显式刷新，不自动跟随每次 `run test subject`。
- [x] 首轮实现先冻结命令面、manifest 与验证闭环，再逐步补强 deeper native 细节。
- [x] 桌面 reference 目标交付 `chaos-il2cpp(.exe)`，Android / iOS / Linux 至少交付 `package-root/`。
- [x] 按 TDD 落地新命令和工作区/部署逻辑。

## Chunk 1：任务真源与命令面 RED

- [x] 创建 `docs/dev/ACTIVE.md`、`plan-v1-01.md` 与首份 `notes/progress-v1-01.md`。
- [x] 更新 `STATUS.md`，把任务切到执行态并补 `total_tasks / current_task / latest_progress`。
- [x] 扩 `tests/tooling/run/test_command_manifest.py`，锁：
  - `generate-project-subject`
  - `generate-project-core`
  - `build-project-subject`
  - `build-project-core`
  - `deploy-core`
  - 命令仍归入现有 group
- [x] 锁 CLI parse：
  - `run generate project subject --id subject/HelloWorldObject`
  - `run build project core --host windows --target windows-x64-reference`
  - `run deploy core --host windows --all-targets`

## Chunk 2：统一入口命令面 GREEN

- [x] 更新 `run_manifest.json` 注册新命令。
- [x] 更新 `run.py` 接入 `project` / `deploy` 分发。
- [x] 新增 `commands/project.py` 与 `commands/deploy.py`。
- [x] 让 `tests/tooling/run/test_command_manifest.py` 转绿。

## Chunk 3：workspace manifest 与 subject project

- [x] 新增 `tests/unit/run/test_project_workspace.py` 与 `tests/tooling/run/test_project_command.py`。
- [x] 实现 `generate project subject`：
  - 生成 `workspace.manifest.json`
  - 只在 `--refresh-generated` 时刷新 stable `subject-exec`
  - 生成 managed solution 与 native configure root
- [x] 实现 `build project subject`：
  - 读取 workspace manifest
  - 触发 managed/native 构建

## Chunk 4：core project 与 `deploy core`

- [x] 扩 RED 测试覆盖 `generate project core`、`build project core`、`deploy core` 与 `deploy.manifest.json`。
- [x] 实现 core workspace 生成：
  - managed core solution
  - native target workspace manifest
- [x] 实现 `build project core`。
- [x] 实现 `deploy core`：
  - 输出 `deploy/core/**`
  - 为 direct-run host 导出 `bin/chaos-il2cpp(.exe)`
  - 为其他目标导出 `package-root/`

## Chunk 5：回归、任务同步与收尾

- [x] 更新 `STATUS.md`、`notes/progress-*.md` 与 `docs/dev` 索引。
- [x] 新增 wiki 页面并更新索引。
- [x] 运行 focused 回归：
  - `tests.tooling.run.test_command_manifest`
  - `tests.tooling.run.test_project_command`
  - `tests.unit.run.test_project_workspace`
  - `tests.unit.run.test_deploy_core`
  - `tests.tooling.run.test_registry_command`
  - `tests.tooling.run.test_tui`
  - `tests.tooling.run.test_build`
  - `tests.tooling.run.test_operation_reporting`

## 完成标准

- [x] 统一入口可执行 `generate project subject|core`
- [x] 统一入口可执行 `build project subject|core`
- [x] 统一入口可执行 `deploy core`
- [x] `artifacts/projects/**` 与 `deploy/core/**` 的边界被代码、测试与 wiki 共同冻结
- [x] `subject-exec` 只在显式刷新时更新
- [x] 当前 host/target 矩阵都有 formal deploy 语义
- [x] 相关自动化验证通过
