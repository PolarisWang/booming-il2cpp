# Roadmap 0：Contract Freeze And Skeleton 设计索引

**日期：** 2026-04-01 03:23:47 +08:00

**状态：** 已根据 brainstorm 确认结果拆分为入口文件与分段设计文件

## 设计目标

本设计用于收敛 `Roadmap 0` 的正式范围、目录布局、contract pack、smoke 输入项目、平台验证矩阵与 `verify-roadmap-0` gate。

本设计不是实现计划。
它的目标是把已经逐条确认过的方案固定成可继续转写为 implementation plan 的规格基线。

## 设计结构

1. [第 1 段：范围、交付物与验证总原则](./design-2026-04-01-03-01-scope-and-principles-roadmap-0-contract-freeze-and-skeleton.md)
2. [第 2 段：仓库骨架、目录责任与落盘位置](./design-2026-04-01-03-02-repo-layout-roadmap-0-contract-freeze-and-skeleton.md)
3. [第 3 段：Native Contract Pack（ABI/Bridge）](./design-2026-04-01-03-03-native-contract-pack-roadmap-0-contract-freeze-and-skeleton.md)
4. [第 4 段：Smoke 输入项目与平台矩阵映射](./design-2026-04-01-03-04-smoke-projects-and-platform-matrix-roadmap-0-contract-freeze-and-skeleton.md)
5. [第 5 段：Verify、Trace Contract 与验证资产组织](./design-2026-04-01-03-05-verification-and-trace-roadmap-0-contract-freeze-and-skeleton.md)
6. [第 6 段：阶段切分、建议执行顺序与边界](./design-2026-04-01-03-06-sequencing-and-boundaries-roadmap-0-contract-freeze-and-skeleton.md)

## 当前结论

- `Roadmap 0` 采用大包版范围，不只是轻脚手架，而是一次性冻结偏重的 contract-and-skeleton 基线。
- 顶层仓库骨架、schema pack、native contract pack、5 个真实 smoke 输入、平台矩阵 gate、trace contract 都属于本阶段正式交付物。
- 设计已经逐段确认通过，可以作为下一步 implementation plan 的输入。

## 关联文档

- Brainstorm 记录：[`brainstorm-2026-04-01-00-roadmap-0-contract-freeze-and-skeleton.md`](./brainstorm-2026-04-01-00-roadmap-0-contract-freeze-and-skeleton.md)
- Finding 记录：[`finding-2026-03-31-23-roadmap-0-contract-freeze-and-skeleton.md`](./finding-2026-03-31-23-roadmap-0-contract-freeze-and-skeleton.md)
- 历史讨论记录：[`plan-discuss-2026-03-31-23-roadmap-0-contract-freeze-and-skeleton.md`](./plan-discuss-2026-03-31-23-roadmap-0-contract-freeze-and-skeleton.md)
