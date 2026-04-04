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
- `pipeline/completion-runtime-trace-windows`
- `pipeline/completion-runtime-trace-macos`
- `pipeline/trace-export-windows-runtime`
- `pipeline/trace-export-macos-runtime`

## 覆盖

- `contract/trace-schema`
- `contract/trace-compare-windows`
- `contract/trace-compare-macos`

## 正式入口

- `run test module --id module/trace-export/windows`
- `run test module --id module/trace-export/macos`
- `run test system --id system/trace-export-windows-smoke`
- `run test system --id system/trace-export-macos-smoke`
- `run test pipeline --id pipeline/completion-runtime-trace-windows`
- `run test pipeline --id pipeline/completion-runtime-trace-macos`
- `run test pipeline --id pipeline/trace-export-windows-runtime`
- `run test pipeline --id pipeline/trace-export-macos-runtime`
