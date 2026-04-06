---
task_id: 20260406-07-stage-5d-macos-reference-parity
title: Stage 5D macOS Reference Parity
task_type: plan
lifecycle_status: completed
phase: archived
created_at: 2026-04-06 16:19:51 +08:00
updated_at: 2026-04-06 16:19:51 +08:00
current_dir: docs/dev/completed/20260406-07-stage-5d-macos-reference-parity
parent_task_id: 20260406-03-stage-5-post-proof-split
source_task_id: 20260406-03-stage-5-post-proof-split
source_relation: roadmap-child
active: false
---

## 关键文档

- brainstorm: `docs/dev/completed/20260406-07-stage-5d-macos-reference-parity/brainstorm-v1-01.md`
- design: `docs/dev/completed/20260406-07-stage-5d-macos-reference-parity/design-v1-01.md`
- roadmap_or_plan: `docs/dev/completed/20260406-07-stage-5d-macos-reference-parity/plan-v1-01.md`
- latest_progress: `docs/dev/completed/20260406-07-stage-5d-macos-reference-parity/notes/progress-v1-01.md`

## 当前判断

- current_focus: 本任务已完成归档；Stage 5D 已把首轮 `macOS` reference parity 的交接边界冻结为一份可执行 handoff 计划。
- why_now: Stage 5A / 5B / 5C 的 Windows trilogy 已经闭环，但当前 Stage 5C proof 绑定 `kernel32.dll` / `MulDiv`，必须先把“哪些样例能进首轮 parity、哪些不能”写清楚，避免把 Windows 专属 interop 假设误写成跨宿主 contract。
- done_definition: 已满足；首轮 `macOS` parity 子集已固定为 Stage 4 + Stage 5A + Stage 5B，当前 Stage 5C 被显式排除，未来 `macOS` 宿主执行入口与验证命令已写入计划。

## 最近摘要

- 2026-04-06 16:19:51 +08:00: 已确认当前 Stage 5C `PInvokeLiteDllImportMinimal` 只证明 Windows import foundation，不具备直接迁移到 `macOS` parity 的条件。
- 2026-04-06 16:19:51 +08:00: 已完成 Stage 5D 的 brainstorm / design / plan 文档，采用“首轮 parity 仅覆盖 Stage 4 + Stage 5A + Stage 5B，Stage 5C 明确排除”的方案。
- 2026-04-06 16:19:51 +08:00: 本任务作为 planning child 直接归档；本轮无额外 wiki 更新需求。

## 下一步

- next_action: 无。若未来切到 `macOS` 宿主执行 parity，以本任务 `plan-v1-01.md` 为起点重新开启执行。
- owner: codex
- trigger: `macOS` parity 的首轮 handoff 已有明确范围、排除项与未来执行入口。

## 风险 / 阻塞

### risks

- 如果未来忽略本任务对 Stage 5C 的显式排除说明，`kernel32.dll` / `MulDiv` 会再次以错误的方式进入跨宿主 parity 范围。
- 如果未来 `macOS` parity 不沿用 Stage 4 / Stage 5A / Stage 5B 的既有 proof gate，而是重新写一套弱化入口，可能损坏 Windows 主基线与 `macOS` parity 的职责边界。

### blockers

- 当前 Windows 宿主不适合执行真实 `macOS` proof run；这正是本任务收窄为 planning handoff 的原因，而不是遗留阻塞。

## 关联任务

### parent

- `20260406-03-stage-5-post-proof-split`

### source

- `20260406-03-stage-5-post-proof-split`：roadmap-child

### children

- 无。
