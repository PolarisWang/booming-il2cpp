# Stage 1A Contract Surface Reset Brainstorm

Date: 2026-04-05 22:40:00 +08:00
Status: draft

## 任务意图

这个子任务从 reboot roadmap 派生出来，目的是把 parent 任务里过于集中的 Stage 1 内容，先切出最核心的 contract surface 冻结工作。

## 为什么单独拆出

- 这一部分决定后续所有结构迁移的 canonical 方向。
- 如果不先单独冻结 contract surface，`tests`、tooling、managed skeleton 的迁移会一直反复改路径和语义。
- 它本身就已经包含 schema、subjectId、ABI、bridge、artifact example、doc 入口等多个高耦合点。

## 本任务聚焦

- `contracts/` 单根目录语义
- `typed-il-ir`
- `AOT manifest`
- `code registration`
- `metadata registration`
- canonical `subjectId`
- `runtime ABI`
- `codegen bridge`
- proof-required subset 与 deferred subset

## 明确不做

- `Chaos.IL2CPP.*` 全量 skeleton 迁移
- `tests/proof` 与 `tests/contracts` 的整棵目录重组
- `tests/tooling` 路径迁移
- `HelloWorld` proof 真正实现

## 预期结果

本任务结束后，后续子任务不再需要继续争论：

- canonical contract 根目录是什么
- artifact 主键是什么
- ABI 和 bridge 的 proof-required 面是什么
- direct-call landing 到底是不是 first proof 的正式约束

