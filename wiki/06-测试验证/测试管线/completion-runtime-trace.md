objectId: completion-runtime-trace-pipeline-guide
objectType: guide

# completion-runtime-trace

## 目的

- 为支持 trace 导出的宿主平台提供更完整的 completion 类测试管线。
- 在 `completion-runtime-core` 的基础上，把 `trace-export` 正式纳入完成前默认验证对象。

## 测试管线对象

- `pipeline/completion-runtime-trace-windows`
- `pipeline/completion-runtime-trace-macos`
- `system/roadmap-0-windows-reference-gate` 与 `system/roadmap-0-macos-reference-gate` 已并入本管线。

## 分层覆盖

- `code`：`smoke/HelloWorld`、`smoke/GenericEcho`、`smoke/ReflectionLite`、`smoke/PInvokeLite`、`smoke/HostEmbeddingLite`
- `module`：`module/analysis/basic`、`module/managed-smoke/basic`、`module/reflection/basic`、`module/interop/basic`、`module/hosted-runtime/basic`、`module/trace-export/windows`、`module/trace-export/macos`
- `system`：`system/hosted-runtime-smoke`、`system/trace-export-windows-smoke`、`system/trace-export-macos-smoke`

## 隐式验证收口

- `module/analysis/basic` 负责把 `analysis schema/example/snapshot` 契约校验并入 completion 默认模块验证。
- `module/interop/basic` 负责把 `native ABI` 与 `native bridge` compile-only smoke 并入 completion 默认模块验证。
- `module/trace-export/windows|macos` 除 warmup trace compare 外，还负责 `trace schema/snapshot` 契约校验。

## 适用边界

- Linux 继续使用 `pipeline/completion-runtime-core`
- Windows 和 macOS 可使用本页对象，把 trace compare 一并纳入 completion 验证

## 正式入口

- `run test pipeline --id pipeline/completion-runtime-trace-windows`
- `run test pipeline --id pipeline/completion-runtime-trace-macos`
