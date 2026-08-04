---
task_id: 20260405-02-stage-1a-contract-surface-reset
title: Stage 1A Contract Surface Reset
task_type: plan
lifecycle_status: completed
phase: archived
created_at: 2026-04-05 22:40:00 +08:00
updated_at: 2026-04-05 23:07:20 +08:00
current_dir: docs/dev/completed/20260405-02-stage-1a-contract-surface-reset
parent_task_id: 20260405-01-il2cpp-reboot-after-abandoned-roadmap
source_task_id: 20260405-01-il2cpp-reboot-after-abandoned-roadmap
source_relation: roadmap-child
active: false
---

## 关键文档
- brainstorm: `docs/dev/completed/20260405-02-stage-1a-contract-surface-reset/brainstorm-v1-01.md`
- design: `docs/dev/completed/20260405-02-stage-1a-contract-surface-reset/design-v1-01.md`
- roadmap_or_plan: `docs/dev/completed/20260405-02-stage-1a-contract-surface-reset/plan-v1-01.md`
- latest_progress: `docs/dev/completed/20260405-02-stage-1a-contract-surface-reset/notes/progress-v1-03.md`

## 当前判断
- current_focus: 本任务已完成归档；Stage 1A 已冻结 proof-facing contract surface，后续如继续 reboot 主线，应转入 Stage 1B 处理结构与 tests 迁移。
- why_now: parent reboot 任务已升级为 roadmap；当前最小可执行子任务应先锁 contract surface，避免后续结构迁移反复变更 canonical path。
- done_definition: Stage 1B 与后续 proof-spec 子任务都能以本任务产出的 contract 边界为稳定上游。

## 最近摘要
- 2026-04-05 22:40:00 +08:00: 从 reboot roadmap 派生为第一个 active 子任务，承接 Stage 1A。
- 2026-04-05 23:00:12 +08:00: 已建立 `contracts/artifacts/v0/schemas`、`contracts/examples/v0/artifacts` 与 `contracts/docs/v0`，明确 `analysis/contracts` 继续作为 compatibility mirror，并补 `.gitignore` 例外规则以纳入新的 canonical contract 路径。
- 2026-04-05 23:00:12 +08:00: 已将 `typed-il-ir`、`aot-manifest`、`code-registration`、`metadata-registration` 收敛到 first-proof `HelloWorldObject` 主线；`typed-il-ir` 显式携带 canonical `subjectId`，`aot-manifest` 补齐 `field` subjectKind。
- 2026-04-05 23:00:12 +08:00: 已补充 runtime/bridge 的 proof-boundary 文档与 generated direct-call landing 示例，并把 native pack-local 文档挂上 boundary 补充入口。
- 2026-04-05 23:00:12 +08:00: 已通过 analysis compatibility 路径 schema/example/snapshot 校验、canonical `contracts/` JSON 解析检查，以及 canonical/compatibility mirror 一致性检查。
- 2026-04-05 23:07:20 +08:00: 已使用 MSVC `cl.exe` 在 VS 开发命令环境下完成 `tests/contracts/native/abi` 与 `tests/contracts/native/bridge` 的 compile-only smoke，native proof-facing contract 验证闭环完成。
- 2026-04-05 23:07:20 +08:00: 本任务无新增 wiki 沉淀；长期有效知识已直接写入 `contracts/docs/v0`、`contracts/native/docs/v0` 与 `wiki/03-功能模块/06-il2cpp核心架构/05-架构参考/roadmap-0/schema-pack-v0.md`。

## 下一步
- next_action: 无。若继续 reboot 主线，下一步激活 `20260405-03-stage-1b-structure-and-test-migration`。
- owner: codex
- trigger: Stage 1A 已完成，等待主线切换到 Stage 1B。

## 风险/阻塞
### risks
- 如果 contract 冻结和结构迁移同时大面积推进，canonical path 与 compatibility path 容易打架。
- 如果 `subjectId` 规则在这一层没有写死，Stage 2 以后会重新引入隐式映射。
- 如果 canonical 路径与 compatibility mirror 后续没有持续同步，Stage 1B 迁移时容易再次出现“文档和工具各说各话”。

### blockers
- 无。

## 关联任务
### parent
- `20260405-01-il2cpp-reboot-after-abandoned-roadmap`

### source
- `20260405-01-il2cpp-reboot-after-abandoned-roadmap`：roadmap-child

### children
- 无。

