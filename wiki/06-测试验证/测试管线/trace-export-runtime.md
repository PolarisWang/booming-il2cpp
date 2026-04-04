objectId: trace-export-runtime-pipeline-guide
objectType: guide

# trace-export runtime

## 目的

- 为 trace 导出链路提供 `code -> module -> system` 分层测试管线。

## 测试管线对象

- `pipeline/trace-export-windows-runtime`
- `pipeline/trace-export-macos-runtime`

## 分层覆盖

- `code`：`smoke/HostEmbeddingLite`
- `module`：`module/hosted-runtime/basic`、`module/trace-export/windows`、`module/trace-export/macos`
- `system`：`system/trace-export-windows-smoke`、`system/trace-export-macos-smoke`

## 模块补充

- `module/trace-export/windows|macos` 同时覆盖 `contract/trace-schema` 与各自平台的 `contract/trace-compare-*`。

## 正式入口

- `run test pipeline --id pipeline/trace-export-windows-runtime`
- `run test pipeline --id pipeline/trace-export-macos-runtime`
