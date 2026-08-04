# Phase 2 启动

Date: 2026-04-15 01:58:23 +08:00

## 起点结论

- Phase 1 已完成并归档。
- 当前 `typed-il`、解释器、hot update、codegen 还没有共享 method identity / body availability / dispatch 决策契约。
- 本批优先做 shared contract foundation，不直接切大规模 AOT emitter 重构。

## 首批范围

- `ManagedClosureContracts` 增加共享 method identity / enum-coded availability / dispatch kind。
- `SemanticWorldStage` / `CodeGenStage` 负责把这组 contract 发到 typed-il artifact。
- 先用 contract bundle 测试锁定输出，再做实现。
