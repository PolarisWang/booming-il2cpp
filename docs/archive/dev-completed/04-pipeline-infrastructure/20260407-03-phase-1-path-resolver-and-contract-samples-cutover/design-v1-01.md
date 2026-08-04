# Phase 1 Path Resolver And Contract Samples Cutover Design v1.01

Date: 2026-04-07
Status: design-frozen-for-implementation

## 1. 设计目标

本阶段冻结两件事：

- 仓库内部消费路径必须先经过统一 resolver，而不是继续散落硬编码
- contract 最小样例的 canonical 命名统一改为 `samples`

## 2. resolver 目标

新增统一 path resolver 模块，负责表达：

- `subjects/<id>` 根
- `subjects/<id>/subject.manifest.json`
- `artifacts/subjects/<id>` 根
- `contracts/artifacts/v0/schemas`
- `contracts/artifacts/v0/samples`
- `contracts/native/v0/samples`
- 兼容 mirror：
  - `contracts/examples/v0/artifacts`
  - `contracts/native/examples/v0`
  - `analysis/contracts/examples`

要求：

- canonical root 与 compatibility root 同时可查询
- 现有 consumer 至少 `contracts.py`、`subjects.py` 接到该 resolver

## 3. contract samples 命名策略

- canonical 样例根改为：
  - `contracts/artifacts/v0/samples`
  - `contracts/native/v0/samples`
- 旧 `examples` 路径暂时保留为 compatibility mirror
- 文档、测试和 code path 优先切到 `samples`

## 4. 兼容策略

- 读取路径时优先 canonical `samples`
- 必要时保留旧 `examples` 文件副本，避免历史入口立即失效
- `analysis/contracts/examples` 只保留镜像语义，不再参与 canonical path 断言
