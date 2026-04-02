# 统一入口脚本设计

**日期：** 2026-04-03 03:06 +08:00

## 背景

当前仓库已经有一批真实可执行能力，但入口是分散的：

- `build/scripts/verify-roadmap-0.ps1` 与 `build/scripts/verify-roadmap-0.sh` 提供 `Roadmap 0` 总体验证入口。
- `tests/smoke/input/*` 提供 5 个 managed smoke 输入项目。
- `tests/contracts/native/*` 提供 native ABI / bridge compile-only smoke。
- `CMakePresets.json` 与 `build/toolchains/*.cmake` 提供 Windows、macOS、Android、iOS、Linux 的 preset / platform gate 路径。

这些能力可以执行，但还没有统一的跨平台入口来承担以下职责：

- 为本地开发者提供单一启动入口。
- 为 harness 测试阶段提供稳定的环境拉起协议。
- 为 CI / 非交互环境提供稳定、可脚本化的非交互命令面。
- 为后续新增能力提供统一扩展点，而不是继续堆散落脚本。

## 目标

建立一套跨平台统一入口机制，使仓库能够通过 `run.ps1` / `run.sh` / `run.cmd`：

- 在 Windows、macOS、Linux 上以一致方式启动。
- 在机器未预装 Python 时，自举下载并缓存仓库内 Python runtime。
- 同时支持交互菜单与非交互 CLI。
- 只暴露当前仓库中真实可执行的能力，不提前暴露未来占位能力。
- 支持 harness 通过统一命令拉起、复用、清理测试环境。
- 为命令输出提供稳定的退出码与 `--json` 机器可读摘要。

## 非目标

本次设计不包含以下内容：

- 不在当前阶段实现新的产品能力；只统一现有 build / test / verify 入口。
- 不在当前阶段把发布 / 部署产物能力伪造进菜单；没有真实 handler 的能力不显示。
- 不自动安装 Visual Studio、Xcode 等平台级重依赖；这些依赖只检查不安装。
- 不把 Python runtime vendored 进仓库；运行时按需下载和缓存。
- 不要求 harness 直接理解底层脚本路径或 CMake 细节。

## 已确认约束

以下约束已在讨论中确认：

- 统一入口采用双模：交互菜单 + 子命令。
- 只暴露当前真实可执行能力，不显示 `planned` / `not implemented` 命令。
- 统一实现采用 Python。
- `run.ps1` / `run.sh` / `run.cmd` 只做薄包装。
- Python runtime 不 vendored 源码；首次按平台下载 / 解包。
- Python runtime 缓存位于 `artifacts/toolchains/python/<platform>/`。
- 通用依赖（如 `cmake`、`dotnet`）允许可选自动安装；VS / Xcode 只检查。
- 裸运行 `run` 时：交互终端进菜单；非交互环境输出帮助并退出。
- 首次 bootstrap：交互环境询问确认；CI / 非交互环境必须显式执行 `run bootstrap`。
- 对外同时支持动作优先与平台优先 alias；内部只维护一套主语法。
- 命令颗粒度采用“完整暴露当前真实能力”的方式。
- Python runtime 下载源统一使用 `python-build-standalone`。
- Python 版本策略固定到明确补丁号。
- harness 测试环境允许 `prepare` 一次后复用，并支持显式 `clean`。
- harness 结果协议至少包含退出码 + `--json` 摘要。
- `prepare` 同时支持全局入口与目标化入口；内部以目标化执行为主。
- `Quick Start` 不包含 `verify roadmap-0`。
- 当前阶段不显示 `Package And Deploy` 菜单分组。

## 顶层架构

统一入口分成 4 层：

### 1. Wrapper 层

仓库根目录提供：

- `run.ps1`
- `run.sh`
- `run.cmd`

职责：

- 识别当前 host 平台。
- 检测是否已有可用的仓库内 Python runtime。
- 在缺失时处理 bootstrap 流程。
- 将参数原样转发给 Python 主入口。

Wrapper 不承载任何业务命令逻辑。

### 2. Python Runtime 自举层

Python runtime 通过 manifest 固定版本、下载源、校验和与解释器路径。

职责：

- 下载对应平台的 Python runtime 归档。
- 校验 SHA-256。
- 解包到 `artifacts/toolchains/python/<platform>/<version>/`。
- 将“当前启用的解释器”记录到状态文件。

### 3. Command 层

Python 主入口负责：

- 解析 CLI 子命令。
- 渲染交互菜单。
- 输出 help、capability list 与 `--json` 摘要。
- 做平台可见性过滤。
- 做前置条件检查与环境复用判断。

### 4. Handler 层

每个真实能力都由对应 handler 执行：

- build handler
- test handler
- verify handler
- environment handler
- inspect handler

handler 只负责一个能力本身，不负责菜单、alias 或 platform filtering。

## 目录布局

统一入口相关文件采用如下布局：

```text
run.ps1
run.sh
run.cmd

build/
  toolchains/
    run/
      run.py
      run_manifest.json
      runtime_manifest.json
      commands/
        __init__.py
        bootstrap.py
        doctor.py
        prepare.py
        clean.py
        build.py
        test.py
        verify.py
        inspect.py

artifacts/
  toolchains/
    python/
      <platform>/
        <version>/
      current.json
```

说明：

- 按用户要求，统一入口的 Python 实现脚本放在 `build/toolchains/run/` 子目录。
- `build/toolchains/run/` 承载“如何拉起统一入口”的工具链逻辑。
- `artifacts/toolchains/python/` 承载按平台缓存的 Python runtime。

## Python Runtime 策略

### 下载源

统一采用 `python-build-standalone` 作为 Windows / macOS / Linux 的 runtime 供应方。

这样可以避免：

- Windows 用 embeddable zip、Linux 自己编译、macOS 再单独找包的三套逻辑。
- wrapper 为不同平台写三套 bootstrap 流程。

### 固定版本

第一版设计建议固定为：

- provider release：`python-build-standalone` `20260303`
- CPython version：`3.12.13`

这是基于当前可验证来源做出的推断：

- `python-build-standalone` 的 `20260303` release 将 `CPython 3.12.12 -> 3.12.13`。[推断基于当前 release 信息]
- Python.org 的 `3.12.13` 已是 source-only，因此若要求跨平台可下载运行时，统一走 `python-build-standalone` 更稳。[推断基于当前 release 信息]

### Manifest 结构

`runtime_manifest.json` 至少包含：

- `provider`
- `provider_release`
- `python_version`
- `platforms`

每个平台项包含：

- `platform_id`
- `download_url`
- `sha256`
- `archive_type`
- `extract_dir`
- `python_relpath`

### Bootstrap 规则

- 交互环境执行 `run` 且未发现 Python runtime 时，先询问是否 bootstrap。
- 非交互环境下，wrapper 不自动下载；必须显式执行 `run bootstrap`。
- `run bootstrap --yes` 可用于 CI / harness。
- 已存在有效 runtime 时，wrapper 不重复下载。

## 依赖策略

### Python

- 统一入口自带 bootstrap。
- 用户机器不要求预装 Python。

### 通用依赖

`cmake`、`dotnet` 等通用依赖采用“可选自动安装”策略：

- `doctor` 检查是否存在。
- 若支持自动安装，则通过显式参数或交互确认执行。
- 默认不在普通命令执行过程中静默安装。

### 平台重依赖

Visual Studio C++ toolchain、Xcode、Apple 平台工具链等采用“只检查，不安装”策略：

- `doctor` 明确报告是否存在。
- `prepare` / `build` / `verify` 若依赖缺失则返回失败摘要。

## 命令模型

### 主语法

内部唯一主语法采用动作优先：

```text
run <verb> <noun> [subtarget] [options]
```

例如：

- `run build smoke HelloWorld`
- `run build preset windows-x64-reference`
- `run test trace-compare windows`
- `run verify roadmap-0 --host windows`

### 平台优先 alias

对外允许平台优先 alias，但内部统一转换回主语法：

- `run windows build reference`
- `run macos verify roadmap-0`
- `run linux build packaging`

### 交互模式

- 交互终端下直接执行 `run`：进入菜单。
- 非交互终端下直接执行 `run`：输出帮助并退出。

## 菜单分组

第一版交互菜单只显示以下分组：

### Quick Start

- `doctor`
- `prepare`
- `menu`
- `list`

### Build

- native contract smoke build
- smoke project build
- preset build
- platform gate build

### Test And Verify

- managed smoke run
- trace compare
- `verify roadmap-0`

### Environment

- `bootstrap`
- `doctor`
- `prepare`
- `clean`

### Inspect

- `list`
- `list --json`
- `help`
- `capability <id>`

当前阶段不显示 `Package And Deploy` 分组，因为仓库里还没有真实 deploy/package handler。

## 当前真实能力映射

### Build

- `build native-contract abi`
- `build native-contract bridge`
- `build smoke HelloWorld`
- `build smoke GenericEcho`
- `build smoke ReflectionLite`
- `build smoke PInvokeLite`
- `build smoke HostEmbeddingLite`
- `build preset windows-x64-reference`
- `build preset macos-reference`
- `build platform android-arm64-smoke`
- `build platform ios-arm64-packaging`
- `build platform linux-x64-packaging`

### Test And Verify

- `test smoke HelloWorld`
- `test smoke GenericEcho`
- `test smoke ReflectionLite`
- `test smoke PInvokeLite`
- `test smoke HostEmbeddingLite`
- `test trace-compare windows`
- `test trace-compare macos`
- `verify roadmap-0 --host windows`
- `verify roadmap-0 --host macos`

### Environment

- `bootstrap`
- `doctor`
- `prepare`
- `prepare smoke`
- `prepare verify roadmap-0 --host windows`
- `prepare verify roadmap-0 --host macos`
- `clean`
- `clean --scope smoke`
- `clean --scope verify-roadmap0-windows`
- `clean --scope verify-roadmap0-macos`

### Inspect

- `list`
- `list --json`
- `help`
- `capability <id>`

## Manifest 驱动模型

`run_manifest.json` 用于声明：

- 命令 id
- 标题
- 分组
- alias
- 平台可见性
- 前置依赖
- `prepare_scope`
- handler 名称
- CLI 示例
- 是否在交互菜单中显示

统一从这份 manifest 生成：

- CLI help
- 交互菜单
- `list`
- `list --json`
- alias 映射
- capability 说明

这避免出现“改了命令但忘了改菜单 / help”的双份维护问题。

## Harness 集成

统一入口不是开发便利脚本，而是 harness 的统一 orchestration 面。

### Harness 使用规则

harness 只调用非交互命令，不调用菜单，不依赖 alias：

- `run bootstrap --yes --json`
- `run prepare verify roadmap-0 --host windows --json`
- `run verify roadmap-0 --host windows --json`
- `run clean --scope verify-roadmap0-windows --json`

### 生命周期

环境生命周期采用“prepare 后复用”的模式：

1. `bootstrap`
2. `prepare`
3. `build` / `test` / `verify`
4. `clean`

其中：

- `prepare` 支持全局入口与目标化入口。
- 内部以目标化 prepare scope 为主。
- `clean` 支持按 scope 清理。
- `build` / `test` / `verify` 默认复用已准备好的环境。

### 结果协议

所有非交互命令必须提供：

- 稳定退出码
- 控制台输出
- `--json` 机器可读摘要

`--json` 摘要至少包含：

- `command`
- `status`
- `hostPlatform`
- `target`
- `durationMs`
- `artifacts`
- `checks`
- `errors`
- `nextActions`

## 平台规则

### Windows host

支持当前真实能力：

- `verify roadmap-0 --host windows`
- `build preset windows-x64-reference`
- `build platform android-arm64-smoke`
- common smoke / contract build 与 test

### macOS host

支持当前真实能力：

- `verify roadmap-0 --host macos`
- `build preset macos-reference`
- `build platform ios-arm64-packaging`
- common smoke / contract build 与 test

### Linux host

第一版统一入口允许：

- `doctor`
- `bootstrap`
- `prepare`
- common inspect 能力
- 与当前 Linux host 上真实可执行能力等价的 build / test

是否直接开放 `verify roadmap-0 --host linux` 取决于仓库未来是否真的提供该 host profile；当前设计不预留假命令。

## 与现有仓库规则的关系

- `build/` 保持原名，继续表示构建系统源码目录。
- 统一入口实现脚本属于构建工具链逻辑，因此进入 `build/toolchains/run/`。
- Python runtime 缓存属于可再生产物，因此进入 `artifacts/toolchains/python/`。
- 最终部署产物仍应遵守现有边界，进入 `deploy/`，而不是本设计新增别的目录。

## 风险与取舍

### 优点

- 统一了本地开发、harness、CI 三种使用面。
- 没有引入虚假能力或占位命令。
- 通过 manifest 避免菜单 / help / alias 漂移。
- 通过仓库内 Python runtime 降低环境前置要求。

### 风险

- bootstrap 引入了下载、校验和缓存治理逻辑。
- 通用依赖“可选自动安装”会让 `doctor` 与 `bootstrap` 之间存在额外状态管理。
- `build/toolchains/run/` 虽然满足当前目录诉求，但未来如果工具扩展过多，可能需要再拆分。

### 取舍

- 当前优先保证统一入口可用，而不是一开始就做成完整工作流引擎。
- 当前优先稳定 harness 协议，而不是过早引入 junit / xml 等多种输出格式。

## 成功标准

当以下条件全部满足时，本设计算落地成功：

- 用户可以在 Windows、macOS、Linux 上通过 `run.ps1` / `run.sh` / `run.cmd` 进入统一入口。
- 未预装 Python 的机器可以通过 bootstrap 获得仓库内 Python runtime。
- 交互模式能展示当前真实能力菜单，且不出现假命令。
- 非交互模式能稳定执行 build / test / verify 命令。
- harness 能通过 `bootstrap`、`prepare`、`verify`、`clean` 形成统一协议。
- 所有非交互命令都支持 `--json` 摘要。

## 后续计划入口

本设计的下一步不是直接实现，而是编写 implementation plan，拆分：

1. wrapper 与 bootstrap
2. Python runtime manifest 与缓存治理
3. command / manifest 框架
4. 现有真实能力接线
5. harness prepare / clean / `--json` 协议
