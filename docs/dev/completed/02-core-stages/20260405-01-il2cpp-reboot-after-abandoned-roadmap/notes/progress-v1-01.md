# 进度记录 v1-01

## 时间

- 2026-04-05 20:51:20 +08:00

## 已完成

- 创建新的 reboot brainstorm 任务。
- 明确本任务不是沿用旧 roadmap，而是先完成旧方案全过程复盘。
- 产出首版 `brainstorm-v1-01.md` 与 `design-v1-01.md`。
- 用户已确认新的顺序不是直接 proof-first，而是“先重整执行入口与工程骨架，再进入 `HelloWorld` proof”。 
- 用户进一步确认 Stage 1 采用重量版，当前需要把 proof 相关 contract reset 的边界压实。
- 用户已同意将 `AOT manifest` 与 `code/metadata registration` 一并纳入 Stage 1。
- 已细化 `Pack A` 与 `Pack B` 的冻结建议。
- 已细化 `Pack C / D / E`，并补充了建议目录结构与对应技术路线。
- 已补充 contract 分层、managed 命名和 tests 结构的方案对比与推荐。
- 用户已确认结构组合：contracts 方案 2、managed 命名方案 1、tests 方案 2。
- 已创建 `plan-v1-01.md`，准备进入 Stage 1 执行。
- 已根据流程语义修正当前状态：plan 已预写，但任务仍处于 design 阶段。
- 已完成 design 审查的 4 个关键收口：
  - direct-call landing 显式纳入 first proof
  - `typed-il-ir` 显式纳入 canonical `subjectId`
  - Stage 1 设计边界保持重量版 A，若实施过大再分期推进
  - `tests/contracts/trace` 固定为 canonical trace contract 路径
- 已完成更新后一致性复核，当前没有新的 design 阻塞，任务可从 design 收口进入 planning。
- 已将父任务从单一大 plan 升级为轻量 roadmap。
- 已正式拆出两个 roadmap-child 子任务：
  - `20260405-02-stage-1a-contract-surface-reset`
  - `20260405-03-stage-1b-structure-and-test-migration`

## 当前状态

- 任务状态：`in_progress`
- 当前阶段：`roadmap`

## 下一步

- 维护父 roadmap 的阶段边界与子任务映射。
- 当前 active 执行入口切换到 `20260405-02-stage-1a-contract-surface-reset`。
