# Stage 1B Structure And Test Migration Brainstorm

Date: 2026-04-05 22:40:00 +08:00
Status: draft

## 任务意图

这个子任务负责把 parent reboot 设计中已经确认的结构方向真正落到工程骨架和测试语义上。

## 为什么单独拆出

- 它依赖 Stage 1A 的 canonical contract 路径和 shared identity 规则。
- 但它本身涉及 managed skeleton、tests family、tooling path、compatibility 层，已经是另一类工作。
- 如果和 1A 混在一个 plan 里，review 和风险收口会非常差。

## 本任务聚焦

- `src/managed/Chaos.IL2CPP.*` skeleton
- `tests/contracts`
- `tests/proof`
- `tests/fixtures`
- `tests/integration` / `tests/tooling` 的 canonical path 切换
- `tests/contract` 的 compatibility 收口

## 明确不做

- schema 字段本身的重新冻结
- ABI / bridge helper 范围再讨论
- `HelloWorld` proof 真正闭环实现

## 预期结果

本任务结束后，repo 的目录和测试角色语义应与 reboot 设计保持一致，而不是继续停留在旧路径与新路径并存的状态。

