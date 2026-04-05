# Stage 1B Structure And Test Migration Design

Date: 2026-04-05 22:40:00 +08:00
Status: ready-for-plan

## 设计目标

把 reboot 设计里已经选定的 managed 分层和 tests 角色分层，落成真正的工程入口与目录语义。

## 设计边界

### 纳入

- `src/managed/Chaos.IL2CPP.Contracts`
- `src/managed/Chaos.IL2CPP.Driver`
- `src/managed/Chaos.IL2CPP.Loader`
- `src/managed/Chaos.IL2CPP.SemanticWorld`
- `src/managed/Chaos.IL2CPP.Pipeline`
- `src/managed/Chaos.IL2CPP.Linker`
- `src/managed/Chaos.IL2CPP.MetadataWriter`
- `src/managed/Chaos.IL2CPP.CodeGen`
- `tests/contracts`
- `tests/proof`
- `tests/fixtures`
- tooling / integration 中的 canonical path 切换

### 不纳入

- artifact schema 的语义级修改
- native ABI / bridge 的语义级修改
- `HelloWorld` proof 闭环实现

## 关键设计结论

1. `src/managed` 采用细粒度 `Chaos.IL2CPP.*` 分层，不回退到粗粒度聚合。
2. `tests/contracts` 是唯一 canonical contract family。
3. `tests/contracts/trace` 是 canonical trace contract 路径。
4. `tests/contract/trace` 仅保留短期 compatibility 层。
5. `tests/smoke` 与 `tests/proof` 的角色必须彻底分离。

## 交付落点

- managed skeleton 建立
- tests family 路径落地
- tooling / registry / manifest 的路径迁移计划清晰
- child plan 可在 Stage 1A 关闭后立即执行

