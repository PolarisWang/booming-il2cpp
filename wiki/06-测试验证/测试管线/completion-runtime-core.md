objectId: pipeline/completion-runtime-core
objectType: pipeline
primaryModuleId: hosted-runtime

# completion-runtime-core

## 对象

- `pipeline/completion-runtime-core`

## 用途

- completion

## 覆盖模块

- `analysis`
- `managed-smoke`
- `reflection`
- `interop`
- `hosted-runtime`

## 阶段

- `code`：`smoke/HelloWorld`、`smoke/GenericEcho`、`smoke/ReflectionLite`、`smoke/PInvokeLite`、`smoke/HostEmbeddingLite`
- `module`：`module/analysis/basic`、`module/managed-smoke/basic`、`module/reflection/basic`、`module/interop/basic`、`module/hosted-runtime/basic`
- `system`：`system/hosted-runtime-smoke`

## 正式入口

- `run test pipeline --id pipeline/completion-runtime-core`
- `run test pipeline --pipeline completion-runtime-core`

## 补充说明

- 当宿主平台需要把 `trace-export` 一并纳入完成前验证时，使用 `pipeline/completion-runtime-trace-windows` 或 `pipeline/completion-runtime-trace-macos`。
