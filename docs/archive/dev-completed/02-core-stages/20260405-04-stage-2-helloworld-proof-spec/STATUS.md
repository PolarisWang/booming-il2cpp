---
task_id: 20260405-04-stage-2-helloworld-proof-spec
title: Stage 2 HelloWorld Proof Spec
task_type: plan
lifecycle_status: completed
phase: archived
created_at: 2026-04-05 23:50:13 +08:00
updated_at: 2026-04-06 00:04:02 +08:00
current_dir: docs/dev/completed/20260405-04-stage-2-helloworld-proof-spec
parent_task_id: 20260405-01-il2cpp-reboot-after-abandoned-roadmap
source_task_id: 20260405-01-il2cpp-reboot-after-abandoned-roadmap
source_relation: roadmap-child
active: false
---

## 关键文档
- brainstorm: `docs/dev/completed/20260405-04-stage-2-helloworld-proof-spec/brainstorm-v1-01.md`
- design: `docs/dev/completed/20260405-04-stage-2-helloworld-proof-spec/design-v1-01.md`
- roadmap_or_plan: `docs/dev/completed/20260405-04-stage-2-helloworld-proof-spec/plan-v1-01.md`
- latest_progress: `docs/dev/completed/20260405-04-stage-2-helloworld-proof-spec/notes/progress-v1-02.md`

## 当前判断
- current_focus: 本任务已完成归档；Stage 2 已固定 object-based `HelloWorld` 的 canonical proof spec，并消除了 source / artifact / snapshot 之间的关键分叉。
- why_now: Stage 1A / Stage 1B 已铺完 proof-facing contract 与 `tests/proof` 路径，Stage 2 必须先把 canonical truth 收口，才能安全进入 Stage 3 / Stage 4。
- done_definition: 后续子任务能够直接复用本任务产出的 canonical spec、`Program.cs` 真源、artifact snapshots 与 gate 归属，而不再重新争论 `HelloWorld` 形状。

## 最近摘要
- 2026-04-05 23:50:13 +08:00: 从 reboot roadmap 派生为 Stage 2 active 子任务，承接 `HelloWorld` proof spec freeze。
- 2026-04-05 23:50:13 +08:00: 已确认当前 proof sample source 与 canonical artifact / native direct-call landing 存在偏差：`Program.cs` 仍偏向 args-driven `object-world` / `void Main`，而 contract 侧已冻结为 `Hello, World!` / `return 0` / generated direct-call landing。
- 2026-04-05 23:50:13 +08:00: 当前默认按 parent design、`contracts/examples/v0/artifacts/*.json` 与 `contracts/native/examples/v0/bridge/direct-call-landing.cpp` 共同指向的 object-based `HelloWorld` 形状作为 Stage 2 推荐 canonical truth。
- 2026-04-06 00:04:02 +08:00: 已新增 `docs/archive/architecture/roadmap-0/hello-world-proof-spec-v0.md`，把 canonical output、artifact 真源、shared `subjectId`、direct-call landing 与 gate 归属写成唯一长期规格。
- 2026-04-06 00:04:02 +08:00: 已将 `tests/proof/input/HelloWorldObject/Program.cs` 对齐到 `Hello, World!` / `return 0`，并补 unit test 锁定 `Program.Main`、proof spec 入口与 typed-il example。
- 2026-04-06 00:04:02 +08:00: 已修复 `typed-il-ir` example / snapshot 中 `Greeter.BuildMessage()` 丢失结尾 `!` 的隐藏分叉；当前 canonical example、compatibility mirror 与 snapshots 已同步。

## 下一步
- next_action: 无。若继续 reboot 主线，下一步由父 roadmap 决定 Stage 3 `Managed Minimal Closure` 与 Stage 4 `Native Bootstrap And Reference Proof` 的派生节奏。
- owner: codex
- trigger: Stage 2 proof spec freeze 已完成，等待父 roadmap 继续派生。

## 风险/阻塞
### risks
- 后续 Stage 3 / Stage 4 如果绕开本任务的 canonical spec，仍可能重新引入 sample truth 分叉。
- `tests/proof/hello-world-object/suite.manifest.json` 当前仍只覆盖 `build`；进入真实 runtime proof 前，必须继续维持 gate 归属说明的一致性。

### blockers
- 无。

## 关联任务
### parent
- `20260405-01-il2cpp-reboot-after-abandoned-roadmap`

### source
- `20260405-01-il2cpp-reboot-after-abandoned-roadmap`：roadmap-child

### children
- 无。
