# Local Verification (Roadmap 0)

> 项目级测试对象、模块验证、整体验证和测试管线的正式知识入口统一位于 [`wiki/06-测试验证/INDEX.md`](../../../wiki/06-%E6%B5%8B%E8%AF%95%E9%AA%8C%E8%AF%81/INDEX.md)。本页只说明 `Roadmap 0` 本地执行入口。

## 推荐入口

- Windows host:

```powershell
./run.ps1 doctor --json
./run.ps1 bootstrap --yes --json
./run.ps1 prepare workflow roadmap-0 --host windows --json
./run.ps1 test workflow roadmap-0-windows --json
./run.ps1 test pipeline --id pipeline/completion-runtime-core --json
./run.ps1 clean --scope workflow-roadmap0-windows --json
```

- macOS host:

```bash
./run.sh doctor --json
./run.sh bootstrap --yes --json
./run.sh prepare workflow roadmap-0 --host macos --json
./run.sh test workflow roadmap-0-macos --json
./run.sh test pipeline --id pipeline/completion-runtime-core --json
./run.sh clean --scope workflow-roadmap0-macos --json
```

如果只关心整体测试进度或最近一次汇总，可以直接使用：

```bash
./run.sh test watch
./run.sh test summary
```

- 快速 smoke 入口:

```powershell
./run.ps1 prepare smoke --json
./run.ps1 test smoke HelloWorld --json
```

`run.cmd` 只是 Windows 上的薄包装；交互式直接执行 `run` 会进入全屏 TUI 菜单，当前支持上下方向键、`PgUp/PgDn`、`Home/End`、`Enter`、`q` / `Esc`；CI / harness 必须显式调用子命令并带 `--json`。

## Harness 生命周期

推荐统一走 `bootstrap -> prepare -> test -> clean`。

- `bootstrap` 只负责仓库内 Python runtime 缓存。
- `prepare` 负责复用型环境准备，并把当前 scope 写入 `artifacts/run/prepare/*.json`。
- Windows / macOS host 验证 scope 缺失 CMake 时，`prepare` 会优先把缓存版 CMake 安装到 `artifacts/toolchains/cmake/`。
- `test` 默认复用已经准备好的环境；如需单独构建阶段，使用 `run test ... --stage build`。
- `clean --scope ...` 只删除统一入口托管的输出，不碰仓库源码和第三方目录。

## 何时直接调用底层脚本

统一入口默认覆盖本地验证和 harness 生命周期，正式入口应优先使用 `run test workflow ...`、`run test system ...` 或 `run test pipeline ...`。只有在调试底层 `verify-roadmap-0` 脚本本身、绕过 wrapper 做最小重现、或单独复用已有 PowerShell/Shell 流程时，才直接调用 `build/scripts/verify-roadmap-0.*`。

## 入口

- Windows host:

```powershell
pwsh ./build/scripts/verify-roadmap-0.ps1 -HostProfile windows
```

- macOS host:

```bash
python3 ./build/scripts/verify-roadmap-0.py --host-profile macos
```

- Shell wrapper:

```bash
./build/scripts/verify-roadmap-0.sh --host-profile macos
```

## `common` 检查

- `cmake --list-presets`
- `analysis/contracts/schemas/*.json`
- `analysis/contracts/examples/*.json`
- `CMakePresets.json`
- `tests/contracts/schema/*.json`
- `tests/contracts/trace/**/*.json`
- analysis schema/example/snapshot 契约校验
- trace schema/snapshot 契约校验
- native ABI compile-only smoke
- native bridge compile-only smoke
- Windows reference preset smoke
- 5 个 smoke 输入项目 build
- 5 个 managed smoke 运行主线
- Linux cross-compile / packaging preset routing smoke
- Linux cross-compile / packaging shell gate 记录

## `windows` profile 专属

- Windows reference desktop warmup trace compare
- Android startup smoke preset routing smoke
- Android startup smoke shell gate 记录

## `macos` profile 专属

- macOS reference desktop warmup trace compare
- iOS compile/link/packaging preset routing smoke
- iOS compile/link/packaging shell gate 记录

## 前置条件

- 通用: `dotnet`
- `cmake`：可由系统安装提供，也可先运行一次对应的 `run prepare ...` 让统一入口预热 `artifacts/toolchains/cmake/`
- Windows host: Visual Studio C++ toolchain
- Android: 如需进入真实 Android toolchain，补 `ANDROID_NDK_ROOT`
- macOS / iOS: Python 3、Apple Command Line Tools 或 Xcode、可选 `DEVELOPER_DIR`
- Linux cross-compile: 如需进入真实 sysroot，补 `ROADMAP0_LINUX_SYSROOT`

## Trace Compare 产物

- Windows: `artifacts/verify-roadmap-0/windows/windows-warmup-trace.runtime.json`
- macOS: `artifacts/verify-roadmap-0/macos/macos-warmup-trace.runtime.json`

compare script 只冻结以下字段：

- `eventName`
- `phase`
- `subjectKind`
- `subjectId`
- `order`
- `status`

另外，运行期 trace 还必须带 `traceSource=host-embedding-session` 与 `sessionTrace` 主线摘录；compare script 也会校验 `scenario` 文本，以及每条导出 event 都能在 `sessionTrace` 中找到对应的 warmup marker，用来确认对比输入来自刚执行的 `HostEmbeddingLite` session，而不是离线拼装 JSON。

## Gate 记录落点

- `artifacts/verify-roadmap-0/<profile>/cmake-presets.txt`
- `artifacts/verify-roadmap-0/<profile>/*.gate.json`
- `artifacts/verify-roadmap-0/<profile>/*warmup-trace.runtime.json`

## 说明

`Roadmap 0` 对 Android、iOS、Linux 的要求是“矩阵可调度、shell 可落盘、边界可追责”，不是在这个阶段完成完整平台 runtime 语义闭环。
