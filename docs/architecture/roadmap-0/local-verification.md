# Local Verification (Roadmap 0)

## 入口

- Windows host:

```powershell
pwsh ./build/scripts/verify-roadmap-0.ps1 -HostProfile windows
```

- macOS host:

```bash
pwsh ./build/scripts/verify-roadmap-0.ps1 -HostProfile macos
```

- Shell wrapper:

```bash
./build/scripts/verify-roadmap-0.sh -HostProfile macos
```

## `common` 检查

- `cmake --list-presets`
- `analysis/contracts/schemas/*.json`
- `analysis/contracts/examples/*.json`
- `CMakePresets.json`
- `tests/contracts/schema/*.json`
- `tests/contracts/trace/**/*.json`
- native ABI compile-only smoke
- native bridge compile-only smoke
- Windows reference preset smoke
- 5 个 smoke 输入项目 build
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

- 通用: `cmake`、`dotnet`
- Windows host: Visual Studio C++ toolchain
- Android: 如需进入真实 Android toolchain，补 `ANDROID_NDK_ROOT`
- macOS / iOS: `pwsh`、Xcode、可选 `DEVELOPER_DIR`
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

## Gate 记录落点

- `artifacts/verify-roadmap-0/<profile>/cmake-presets.txt`
- `artifacts/verify-roadmap-0/<profile>/*.gate.json`
- `artifacts/verify-roadmap-0/<profile>/*warmup-trace.runtime.json`

## 说明

`Roadmap 0` 对 Android、iOS、Linux 的要求是“矩阵可调度、shell 可落盘、边界可追责”，不是在这个阶段完成完整平台 runtime 语义闭环。
