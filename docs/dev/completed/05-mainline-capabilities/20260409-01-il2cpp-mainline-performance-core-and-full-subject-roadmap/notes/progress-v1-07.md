# Progress v1.07

Date: 2026-04-09 11:54:10 +08:00

## 本轮更新

- `Phase 2` child `20260409-04-phase-2-core-contract-cutover` 已完成并归档，核心主线已经显式 threaded `canonical subjects / semantic shapes / capability bundles / optimization facts`。
- 用户已确认 `contract` 与 `subject` 的边界清理不在本轮插入；subject-bearing contract 清理将作为后续单独 follow-up task 处理。
- 已正式派生 `20260409-05-phase-3-capability-driven-lowering-and-arbitration`，当前进入 lowering family 选择与 emitter boundary cutover。

## 当前建议

- 先用 RED 测试锁定 `CodeGen` 必须成为 lowering family 与 managed lowering plan 的真源。
- 再把 emitter 收缩为 plan consumer，只保留 plan completeness / backend invariant validation。
