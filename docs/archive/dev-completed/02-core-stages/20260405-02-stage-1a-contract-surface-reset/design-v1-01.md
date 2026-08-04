# Stage 1A Contract Surface Reset Design

Date: 2026-04-05 22:40:00 +08:00
Status: ready-for-plan

## 设计目标

先把 first proof 直接依赖的 contract surface 冻结住，再让结构迁移和 tests 重组基于这套冻结结果推进。

## 设计边界

### 纳入

- `contracts/artifacts/v0/` 语义
- `contracts/native/v0/` 语义
- `contracts/docs/v0/` 总入口
- canonical `subjectId`
- Pack B / Pack C / Pack D 对应的 schema、header、example、doc 方向
- first proof 的 generated direct-call landing 约束

### 不纳入

- `src/managed/Chaos.IL2CPP.*` project skeleton
- `tests/proof` 输入项目
- `tests/tooling` 与 registry path 全量迁移
- wiki 全量沉淀

## 关键设计结论

1. contract 统一收敛到 `contracts/` 根目录。
2. `typed-il-ir` 必须显式携带 canonical `subjectId`。
3. `AOT manifest`、`code registration`、`metadata registration` 必须共享同一套 `subjectId`。
4. `runtime ABI` 负责 lifecycle / allocation / field access / minimal query / managed failure。
5. `codegen bridge` 负责 registration / bootstrap / token resolve / direct-call landing / minimal generated helper。
6. first proof 的 `Program.Main`、`Greeter..ctor(string)`、`Greeter.BuildMessage()` 采用 generated direct-call landing。

## 交付落点

- contract 根目录与版本化文档落点明确
- proof-facing schema/example/snapshot 冻结范围明确
- ABI / bridge proof-required subset 与 deferred subset 明确
- child plan 可直接进入执行

