# Stage 1A Contract Surface Reset Plan

Date: 2026-04-05 22:40:00 +08:00
Status: ready

## 目标

冻结 first proof 直接依赖的 contract surface，并为 Stage 1B 的结构与 tests 迁移提供稳定上游。

## Chunk 1：统一 contract 根目录语义

- [ ] 建立 `contracts/artifacts/v0/`、`contracts/native/v0/`、`contracts/docs/v0/`、`contracts/examples/v0/` 的目标语义
- [ ] 评估 `analysis/contracts` 的迁移方式或 compatibility copy 方式
- [ ] 在总入口文档中写清 artifact/native/shared identity 的对应关系

## Chunk 2：冻结 Pack B + Pack D artifact 契约

- [ ] 为 `typed-il-ir` 补齐 canonical `subjectId`
- [ ] 收敛 `AOT manifest` proof-required retained world
- [ ] 收敛 `code registration` / `metadata registration` 的最小 proof 面
- [ ] 补 proof 样例 example / snapshot

## Chunk 3：冻结 Pack C runtime / bridge 边界

- [ ] 审视 `runtime_abi.h` proof-required subset
- [ ] 审视 `codegen_bridge.h` proof-required subset
- [ ] 把 direct-call landing 写成 first proof 的正式约束
- [ ] 标出 deferred helpers

## Chunk 4：验证与状态同步

- [ ] 基础 schema/example 可解析性检查
- [ ] compile-only contract smoke 的验证入口检查
- [ ] 同步本子任务与 parent roadmap 的状态文件

## 完成标准

- `contracts/` 成为 proof-facing contract 的统一语义根
- `subjectId` 规则完成 Pack B / D 贯通
- ABI / bridge 边界完成 first-proof 级冻结
- Stage 1B 不再需要重新争论 canonical contract path

