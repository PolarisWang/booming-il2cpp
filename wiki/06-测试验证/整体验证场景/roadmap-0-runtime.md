objectId: roadmap-0-runtime-system-guide
objectType: guide

# roadmap-0 runtime

## 目的

- 把 `workflow/roadmap-0-*` 这条项目级验证链路收敛为正式 `system` 对象，供 harness、skill 和发布前验证统一调用。
- 覆盖 roadmap-0 在本机宿主上的完整运行时验证流程，而不是只停留在手工脚本或历史 `verify` 语义。

## 整体验证对象

- `system/roadmap-0-windows`
- `system/roadmap-0-macos`

## 组合边界

- `pipeline/completion-runtime-trace-windows`
- `pipeline/completion-runtime-trace-macos`
- `system/roadmap-0-android-startup-gate`
- `system/roadmap-0-ios-packaging-gate`
- `system/roadmap-0-linux-packaging-gate`

## 兼容入口

- `workflow/roadmap-0-windows`
- `workflow/roadmap-0-macos`

这两个 `workflow` suite 仍保留给公开 `run test workflow ...` 语法使用，但内部已经转发到正式 `system/roadmap-0-*` 对象；`build/scripts/verify-roadmap-0.*` 只保留为底层实现与调试路径，不再代表正式命令入口。

## 正式入口

- `run test system --id system/roadmap-0-windows`
- `run test system --id system/roadmap-0-macos`

## 关联说明

- 详细宿主准备、产物位置和失败排查，继续参考 [`docs/architecture/roadmap-0/local-verification.md`](../../../docs/architecture/roadmap-0/local-verification.md)。
