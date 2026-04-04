objectId: trace-export-smoke-system-guide
objectType: guide

# trace-export smoke

## 目的

- 为 warmup trace 导出与宿主运行时联调提供正式整体验证入口。

## 整体验证对象

- `system/trace-export-windows-smoke`
- `system/trace-export-macos-smoke`

## 组合边界

- `module/hosted-runtime/basic`
- `module/trace-export/windows`
- `module/trace-export/macos`

## 正式入口

- `run test system --id system/trace-export-windows-smoke`
- `run test system --id system/trace-export-macos-smoke`
