# 统一入口脚本实现计划

> **For agentic workers:** REQUIRED: Use dev:subagent-driven-development (if subagents available) or dev:executing-plans to implement this plan. Steps use checkbox (`- [ ]`) syntax for tracking.

**目标：** 为仓库建立跨平台统一入口 `run.ps1` / `run.sh` / `run.cmd`，由 Python 主程序统一承载 bootstrap、doctor、prepare、build、test、verify、inspect，并为 harness 提供可复用环境与 `--json` 结果协议。

**架构：** 采用四层结构推进：wrapper 层负责跨平台拉起与 Python runtime 自举，runtime manifest 层负责版本锁定与缓存治理，command/manifest 层负责命令树与菜单生成，handler 层负责接入当前仓库已有的 build / test / verify 能力。实现按 5 个 chunk 推进，先打通 bootstrap 与最小命令框架，再接线现有能力，最后补齐 harness 生命周期、文档和验收。

**技术栈：** PowerShell、Shell、CMD、Python 3.12.13、`python-build-standalone`、JSON manifest、`cmake`、`.NET SDK`、现有 `verify-roadmap-0` 脚本。

**设计文档：** `docs/booming/2026-04-03-unified-run-entrypoint/design-2026-04-03-03-unified-run-entrypoint.md`

**预期知识沉淀：** `wiki/04-工具与集成/统一入口脚本.md`，并更新 `wiki/04-工具与集成/INDEX.md`

---

## 全局约束

- 不把未来能力写成占位命令；只暴露当前真实可执行能力。
- `Quick Start` 不显示 `verify roadmap-0`。
- 当前阶段不显示 `Package And Deploy` 分组。
- `build/` 保持原名；入口 Python 源码进入 `build/toolchains/run/`。
- Python runtime 不 vendored 源码；统一由 `python-build-standalone` 按需下载。
- Python runtime 缓存固定进入 `artifacts/toolchains/python/<platform>/<version>/`。
- 交互终端直接执行 `run` 进入菜单；非交互终端直接执行 `run` 输出帮助并退出。
- 交互环境首次 bootstrap 需要确认；CI / harness 必须显式 `run bootstrap --yes`。
- harness 只调用非交互主语法命令，不依赖菜单，不依赖 alias。
- 所有非交互命令必须返回稳定退出码，并支持 `--json`。
- 不在本计划中实现新的 deploy/package 业务逻辑，只为将来预留可接线框架。

## 文件结构总览

本计划预计创建或修改如下文件：

- 创建：`run.ps1`
- 创建：`run.sh`
- 创建：`run.cmd`
- 创建：`build/toolchains/run/run.py`
- 创建：`build/toolchains/run/run_manifest.json`
- 创建：`build/toolchains/run/runtime_manifest.json`
- 创建：`build/toolchains/run/__init__.py`
- 创建：`build/toolchains/run/common.py`
- 创建：`build/toolchains/run/runtime.py`
- 创建：`build/toolchains/run/menu.py`
- 创建：`build/toolchains/run/manifest.py`
- 创建：`build/toolchains/run/result.py`
- 创建：`build/toolchains/run/commands/__init__.py`
- 创建：`build/toolchains/run/commands/bootstrap.py`
- 创建：`build/toolchains/run/commands/doctor.py`
- 创建：`build/toolchains/run/commands/prepare.py`
- 创建：`build/toolchains/run/commands/clean.py`
- 创建：`build/toolchains/run/commands/build.py`
- 创建：`build/toolchains/run/commands/test.py`
- 创建：`build/toolchains/run/commands/verify.py`
- 创建：`build/toolchains/run/commands/inspect.py`
- 创建：`tests/tooling/run/test_runtime_manifest.py`
- 创建：`tests/tooling/run/test_command_manifest.py`
- 创建：`tests/tooling/run/test_json_output.py`
- 创建：`tests/tooling/run/test_prepare_scopes.py`
- 创建：`tests/tooling/run/test_wrapper_bootstrap.ps1`
- 创建：`tests/tooling/run/test_wrapper_bootstrap.sh`
- 创建：`tests/tooling/run/test_run_cli.ps1`
- 创建：`tests/tooling/run/test_run_cli.sh`
- 修改：`docs/archive/architecture/roadmap-0/local-verification.md`
- 修改：`wiki/04-工具与集成/INDEX.md`
- 创建：`wiki/04-工具与集成/统一入口脚本.md`

其中职责边界如下：

- 根目录 wrapper 只负责“拉起 Python 主程序”。
- `runtime.py` 只负责 runtime manifest、下载、校验、缓存定位。
- `manifest.py` 只负责命令能力注册、alias 解析与平台可见性。
- `menu.py` 只负责交互视图。
- `result.py` 只负责统一结果对象与 `--json` 输出。
- 各 `commands/*.py` 只负责具体命令逻辑。

## Chunk 1: Wrapper 与 Python Runtime Bootstrap

### 任务 1：建立 wrapper 入口与 runtime manifest

**文件：**
- 创建：`run.ps1`
- 创建：`run.sh`
- 创建：`run.cmd`
- 创建：`build/toolchains/run/runtime_manifest.json`
- 创建：`build/toolchains/run/runtime.py`
- 创建：`build/toolchains/run/common.py`
- 创建：`tests/tooling/run/test_wrapper_bootstrap.ps1`
- 创建：`tests/tooling/run/test_wrapper_bootstrap.sh`

**知识沉淀：**
- 目标：`无`
- 原因：本任务先固定 runtime 自举与 wrapper 行为，最终以整体验收阶段沉淀到统一 wiki 页面

- [ ] **步骤 1：编写 runtime manifest 的失败测试**

断言至少覆盖：
- 固定 provider 为 `python-build-standalone`
- 固定 Python 版本为 `3.12.13`
- 每个平台项都包含 `download_url`、`sha256`、`archive_type`、`python_relpath`
- 缓存根目录固定映射到 `artifacts/toolchains/python`

- [ ] **步骤 2：编写 wrapper bootstrap 行为失败测试**

断言至少覆盖：
- 检测不到 Python runtime 时，交互环境会进入确认分支
- 非交互环境不自动下载，要求显式 `bootstrap`
- `bootstrap --yes` 跳过确认
- 已存在有效 runtime 时，不重复下载

- [ ] **步骤 3：实现 `runtime_manifest.json` 与 `runtime.py` 最小可用版本**

内容至少包括：
- 平台识别
- runtime 路径计算
- checksum 校验
- 解包目录约定
- `current.json` 写入

- [ ] **步骤 4：实现三套 wrapper 最小版本**

要求：
- 参数透传
- 定位仓库根
- 定位/拉起缓存中的 Python
- 缺失时触发 bootstrap 分支

- [ ] **步骤 5：运行 wrapper 与 manifest 测试**

运行：
```powershell
python -m unittest tests.tooling.run.test_runtime_manifest
powershell -NonInteractive -File tests/tooling/run/test_wrapper_bootstrap.ps1
```

预期：
- manifest 断言通过
- wrapper bootstrap 流程满足交互/非交互边界

## Chunk 2: 命令框架、菜单与能力注册表

### 任务 2：建立 `run.py` 主入口与统一结果对象

**文件：**
- 创建：`build/toolchains/run/run.py`
- 创建：`build/toolchains/run/result.py`
- 创建：`build/toolchains/run/menu.py`
- 创建：`build/toolchains/run/manifest.py`
- 创建：`build/toolchains/run/run_manifest.json`
- 创建：`build/toolchains/run/commands/inspect.py`
- 创建：`tests/tooling/run/test_command_manifest.py`
- 创建：`tests/tooling/run/test_json_output.py`
- 创建：`tests/tooling/run/test_run_cli.ps1`
- 创建：`tests/tooling/run/test_run_cli.sh`

**知识沉淀：**
- 目标：`无`
- 原因：框架细节属于实现阶段知识，最终统一沉淀到“统一入口脚本”页面

- [ ] **步骤 1：编写命令 manifest 失败测试**

断言至少覆盖：
- 分组只包含 `Quick Start`、`Build`、`Test And Verify`、`Environment`、`Inspect`
- `Quick Start` 不包含 `verify roadmap-0`
- 当前不出现 `Package And Deploy`
- 每个命令都有 `id`、`group`、`handler`、`platforms`

- [ ] **步骤 2：编写 `--json` 结果协议失败测试**

断言至少覆盖：
- 非交互命令支持 `--json`
- JSON 摘要至少包含 `command`、`status`、`hostPlatform`、`target`、`durationMs`、`checks`、`errors`
- 命令失败时仍输出稳定结构

- [ ] **步骤 3：实现 `result.py` 与 `manifest.py`**

要求：
- 统一结果对象
- 主语法命令解析
- alias 映射到主语法
- 平台可见性过滤

- [ ] **步骤 4：实现 `run.py` 与 `menu.py` 最小交互框架**

要求：
- 交互终端直接执行 `run` 进入菜单
- 非交互终端直接执行 `run` 输出帮助
- `list` / `list --json` / `help` / `capability <id>` 可用

- [ ] **步骤 5：运行框架测试**

运行：
```powershell
python -m unittest tests.tooling.run.test_command_manifest
python -m unittest tests.tooling.run.test_json_output
powershell -NonInteractive -File tests/tooling/run/test_run_cli.ps1
```

预期：
- 交互与非交互分流正确
- 菜单分组与命令元数据符合设计

## Chunk 3: 接线当前真实 build / test / verify 能力

### 任务 3：实现 Build / Test / Verify handler

**文件：**
- 创建：`build/toolchains/run/commands/build.py`
- 创建：`build/toolchains/run/commands/test.py`
- 创建：`build/toolchains/run/commands/verify.py`
- 修改：`build/toolchains/run/run_manifest.json`
- 测试：`tests/tooling/run/test_command_manifest.py`
- 测试：`tests/tooling/run/test_json_output.py`

**知识沉淀：**
- 目标：`无`
- 原因：能力接线细节要到实现完成后再沉淀为稳定使用文档

- [ ] **步骤 1：在 manifest 中登记全部当前真实能力**

至少登记：
- `build native-contract abi|bridge`
- `build smoke <5 个项目>`
- `build preset windows-x64-reference|macos-reference`
- `build platform android-arm64-smoke|ios-arm64-packaging|linux-x64-packaging`
- `test smoke <5 个项目>`
- `test trace-compare windows|macos`
- `verify roadmap-0 --host windows|macos`

- [ ] **步骤 2：实现 `build.py`**

要求：
- 调用现有 `dotnet build`、`cmake -S/-B`、`cmake --build`
- 统一结果对象
- 对应 artifacts 路径进入结果摘要

- [ ] **步骤 3：实现 `test.py`**

要求：
- managed smoke 调用现有 smoke DLL 运行路径
- trace compare 调用现有 compare 脚本
- 不复制底层验证逻辑，只做统一调度

- [ ] **步骤 4：实现 `verify.py`**

要求：
- 统一调度现有 `build/scripts/verify-roadmap-0.ps1`
- 在 Windows/macOS 上做 host profile 映射
- Linux host 不暴露假 `verify roadmap-0 --host linux`

- [ ] **步骤 5：运行接线测试并做最小 smoke 自证**

运行：
```powershell
./run.ps1 list --json
./run.ps1 build smoke HelloWorld --json
./run.ps1 verify roadmap-0 --host windows --json
```

预期：
- `list --json` 只列出真实能力
- 单项 build 可执行
- verify 命令能正确转发到底层验证脚本

## Chunk 4: Harness 生命周期与环境复用

### 任务 4：实现 `bootstrap` / `doctor` / `prepare` / `clean`

**文件：**
- 创建：`build/toolchains/run/commands/bootstrap.py`
- 创建：`build/toolchains/run/commands/doctor.py`
- 创建：`build/toolchains/run/commands/prepare.py`
- 创建：`build/toolchains/run/commands/clean.py`
- 创建：`tests/tooling/run/test_prepare_scopes.py`
- 测试：`tests/tooling/run/test_json_output.py`
- 修改：`build/toolchains/run/run_manifest.json`

**知识沉淀：**
- 目标：`wiki/04-工具与集成/统一入口脚本.md`
- 原因：prepare / clean / doctor / bootstrap 会形成长期稳定的使用协议

- [ ] **步骤 1：编写 `prepare_scope` 失败测试**

断言至少覆盖：
- `prepare` 支持全局入口
- `prepare verify roadmap-0 --host windows`
- `prepare verify roadmap-0 --host macos`
- `prepare smoke`
- `clean --scope ...` 只清对应范围

- [ ] **步骤 2：实现 `bootstrap.py` 与 `doctor.py`**

要求：
- `bootstrap` 调 runtime 下载与缓存逻辑
- `doctor` 检查 Python runtime、`cmake`、`dotnet`、VS/Xcode 等依赖
- 通用依赖允许提供“可选自动安装”框架，但平台重依赖只检查

- [ ] **步骤 3：实现 `prepare.py`**

要求：
- 支持全局与目标化 prepare
- 以目标化 scope 为内部实现主线
- 默认复用已准备环境，不重复昂贵准备

- [ ] **步骤 4：实现 `clean.py`**

要求：
- 支持全局 clean
- 支持 scope clean
- 不误删非本入口管理的权威文件

- [ ] **步骤 5：运行 harness 生命周期测试**

运行：
```powershell
python -m unittest tests.tooling.run.test_prepare_scopes
./run.ps1 bootstrap --yes --json
./run.ps1 prepare verify roadmap-0 --host windows --json
./run.ps1 clean --scope verify-roadmap0-windows --json
```

预期：
- bootstrap / prepare / clean 流水线稳定
- `--json` 输出可供 harness 消费

## Chunk 5: 文档、知识沉淀与最终验收

### 任务 5：补齐使用文档与项目知识沉淀

**文件：**
- 修改：`docs/archive/architecture/roadmap-0/local-verification.md`
- 修改：`wiki/04-工具与集成/INDEX.md`
- 创建：`wiki/04-工具与集成/统一入口脚本.md`
- 如有必要修改：`docs/booming/2026-04-03-unified-run-entrypoint/design-2026-04-03-03-unified-run-entrypoint.md`

**知识沉淀：**
- 目标：`wiki/04-工具与集成/统一入口脚本.md`
- 原因：统一入口是长期稳定的项目级工具能力，适合沉淀到 wiki

- [ ] **步骤 1：更新架构 / 使用文档**

补充点：
- `run.ps1` / `run.sh` / `run.cmd` 的入口说明
- `verify-roadmap-0` 何时直接用，何时通过 `run verify ...` 用
- 非交互 / harness 推荐调用方式

- [ ] **步骤 2：编写 wiki 页面**

页面至少包含：
- 入口分层
- bootstrap 规则
- 常用命令
- harness 接入建议
- `--json` 协议摘要

- [ ] **步骤 3：更新 wiki 索引**

要求：
- `wiki/04-工具与集成/INDEX.md` 索引新页面
- 最近更新条目回填

- [ ] **步骤 4：执行端到端验收**

运行：
```powershell
./run.ps1 doctor --json
./run.ps1 list --json
./run.ps1 prepare smoke --json
./run.ps1 test smoke HelloWorld --json
./run.ps1 verify roadmap-0 --host windows --json
```

在 macOS host 上额外运行：
```bash
./run.sh verify roadmap-0 --host macos --json
```

预期：
- wrapper、bootstrap、命令解析、能力接线、`--json`、prepare/clean 全链路可用

- [ ] **步骤 5：完成前自检与分支收尾**

要求：
- 使用 `dev:verification-before-completion`
- 核对所有非交互命令的退出码与 JSON 结构
- 核对菜单中没有假能力或错误分组

## 完成标准

满足以下条件时，本计划才算完成：

- 三套 wrapper 都能跨平台拉起统一入口
- Python runtime 能按 manifest 下载、校验、解包并缓存
- 命令框架支持主语法、alias、菜单、`list --json`
- 当前真实 build / test / verify 能力全部接线完成
- harness 可通过 `bootstrap`、`prepare`、`verify`、`clean` 复用环境
- 所有非交互命令都支持 `--json`
- wiki 与本地验证文档同步更新

## 执行交接

计划已保存到当前目录，可直接进入执行阶段。执行时应优先按 Chunk 1 -> Chunk 5 顺序推进，不要跳过 bootstrap / runtime 基础层直接接线业务命令。
