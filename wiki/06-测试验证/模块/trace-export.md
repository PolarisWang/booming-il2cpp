objectId: trace-export-module-guide
objectType: guide

# trace-export

## 目的

- 为 trace 导出与 warmup trace 快照比对提供项目级模块验证入口。

## 模块对象

- `module/trace-export/windows`
- `module/trace-export/macos`

## 上层对象

- `system/trace-export-windows-smoke`
- `system/trace-export-macos-smoke`
- `system/runtime-baseline-windows`
- `system/runtime-baseline-macos`

## 覆盖

- `contract/trace-schema`
- `contract/trace-compare-windows`
- `contract/trace-compare-macos`

## 正式入口

- `run test module --id module/trace-export/windows`
- `run test module --id module/trace-export/macos`
- `run test system --id system/trace-export-windows-smoke`
- `run test system --id system/trace-export-macos-smoke`
- `run test system --id system/runtime-baseline-windows`
- `run test system --id system/runtime-baseline-macos`
