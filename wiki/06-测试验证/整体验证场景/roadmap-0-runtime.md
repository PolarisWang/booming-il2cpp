objectId: roadmap-0-runtime-system-guide
objectType: guide

# roadmap-0 runtime

## 目的

- 把 `workflow/roadmap-0-*` 这条项目级验证链路收敛到正式 `system/runtime-baseline-*` 对象，供 harness、skill 和发布前验证统一调用。
- 覆盖 roadmap-0 在本机宿主上的完整运行时验证流程，而不是只停留在手工脚本或历史 `verify` 语义。

## 整体验证对象

- `system/runtime-baseline-windows`
- `system/runtime-baseline-macos`

## 组合边界

- `module/analysis/basic`
- `module/managed-smoke/basic`
- `module/reflection/basic`
- `module/interop/basic`
- `module/hosted-runtime/basic`
- `module/trace-export/windows`
- `module/trace-export/macos`
- `system/windows-reference-gate`
- `system/macos-reference-gate`
- `system/android-startup-gate`
- `system/ios-packaging-gate`
- `system/linux-packaging-gate`

## 兼容入口

- `workflow/roadmap-0-windows`
- `workflow/roadmap-0-macos`

这两个 `workflow` suite 仍保留给公开 `run test workflow ...` 语法使用，但内部已经转发到正式 `system/runtime-baseline-*` 对象；`build/scripts/verify-roadmap-0.*` 只保留为底层实现与调试路径，不再代表正式命令入口。

## 正式入口

- `run test system --id system/runtime-baseline-windows`
- `run test system --id system/runtime-baseline-macos`

## 关联说明

- 详细宿主准备、产物位置和失败排查，继续参考 [`wiki/03-功能模块/06-il2cpp核心架构/docs/archive/architecture/roadmap-0/local-verification.md`](../../../wiki/03-功能模块/06-il2cpp核心架构/docs/archive/architecture/roadmap-0/local-verification.md)。
