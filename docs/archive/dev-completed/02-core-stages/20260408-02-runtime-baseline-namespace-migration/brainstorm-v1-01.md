# runtime-baseline 命名空间迁移 brainstorm

## 背景

当前 live 结构里仍有大量 `roadmap-0` 命名：

- `prepare/test workflow roadmap-0-*`
- `system/roadmap-0-*`
- `artifacts/verify-roadmap-0/**`
- `build/scripts/verify-roadmap-0.*`
- `wiki/03-功能模块/06-il2cpp核心架构/docs/archive/architecture/roadmap-0/`

这些名字在历史上成立，但在当前仓库语义里已经退化成“runtime baseline + platform gates”的验证命名空间，不再适合作为正式顶层概念继续扩张。

## 目标

把 live canonical surface 从 `roadmap-0` 迁到更准确的名字，同时不打断已有兼容入口。

## 方案对比

### 方案 A：完全保留 `roadmap-0`

- 优点：零迁移成本。
- 缺点：继续放大语义错位，后续新能力还会继续挂到历史名下。

### 方案 B：只改文档，不改代码

- 优点：风险低。
- 缺点：CLI、registry、artifact、脚本和文档语义继续分裂，用户层认知不会真正收口。

### 方案 C：live canonical 全量迁移，旧入口保兼容

- 优点：正式结构收口，同时避免直接打断旧脚本和旧命令。
- 缺点：需要同步改命令、registry、文档、脚本路径和测试。

## 结论

采用方案 C。

## 已确认边界

- canonical 模块名改为 `runtime-baseline`
- runtime guide 改为 `runtime-baseline`
- gate guide 改为 `platform-gates`
- 旧 `roadmap-0` 命令和脚本保留为 compatibility alias / wrapper
- 历史讨论、已归档任务和历史记录不做全量重写，只处理 live surface
