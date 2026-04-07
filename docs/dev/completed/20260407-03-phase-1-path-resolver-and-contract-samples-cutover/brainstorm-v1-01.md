# Phase 1 Path Resolver And Contract Samples Cutover Brainstorm v1.01

Date: 2026-04-07
Status: execution-approved

## 1. 目标

把父 roadmap 的 `Phase 1` 收窄成可执行任务：

- 抽象统一 path resolver
- 收口 `contracts/*/examples` 到 `samples`
- 保留必要的兼容路径，不在本阶段做大规模清理

## 2. 已确认边界

- 本阶段只处理路径真源与 contract sample 命名，不扩展到 subject manifest 新 schema。
- `analysis/contracts/examples` 不再是长期 canonical source，但可以暂时保留为 compatibility mirror。
- canonical 命名方向已冻结为：
  - `contracts/examples/v0/artifacts` -> `contracts/artifacts/v0/samples`
  - `contracts/native/examples/v0` -> `contracts/native/v0/samples`

## 3. 实施策略

- 先通过测试锁定新的 canonical 路径和 resolver API。
- 再引入统一 path resolver，让 `contracts.py` 与 `subjects.py` 消费它。
- 最后物理增加/迁移 `samples` 目录，并把旧 `examples` 降级为兼容层。

## 4. 非目标

- 不在本阶段处理 `run -> analysis -> variant -> matrix` 的新 artifact 布局。
- 不在本阶段处理 `CHECK / PROFILE / SHIP` 宏定义。
- 不在本阶段写 wiki。
