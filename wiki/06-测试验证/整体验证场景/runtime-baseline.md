objectId: runtime-baseline-system-guide
objectType: guide

# runtime-baseline runtime

## 目的

- 将当前项目级 runtime 主链路统一收口到 `runtime-baseline` 命名空间。
- 保留旧的 `roadmap-0` 命令和 system id 作为兼容入口，但不再作为 canonical surface。

## 正式对象

- `system/runtime-baseline-windows`
- `system/runtime-baseline-macos`

## 组合边界

- `pipeline/completion-runtime-trace-windows`
- `pipeline/completion-runtime-trace-macos`
- `system/android-startup-gate`
- `system/ios-packaging-gate`
- `system/linux-packaging-gate`

## 兼容入口

- `test workflow roadmap-0-windows`
- `test workflow roadmap-0-macos`
- `test system --id system/roadmap-0-windows`
- `test system --id system/roadmap-0-macos`

## 正式入口

- `run test system --id system/runtime-baseline-windows`
- `run test system --id system/runtime-baseline-macos`

## 关联说明

- 本地准备、产物路径和排障说明见 [`docs/architecture/runtime-baseline/local-verification.md`](../../../docs/architecture/runtime-baseline/local-verification.md)。
