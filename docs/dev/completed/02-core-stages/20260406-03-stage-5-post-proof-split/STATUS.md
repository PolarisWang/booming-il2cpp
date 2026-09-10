---
task_id: 20260406-03-stage-5-post-proof-split
title: Stage 5 Post-Proof Split
task_type: roadmap
lifecycle_status: completed
phase: archived
created_at: 2026-04-06 12:48:45 +08:00
updated_at: 2026-04-06 16:19:51 +08:00
current_dir: docs/dev/completed/20260406-03-stage-5-post-proof-split
parent_task_id: 20260405-01-il2cpp-reboot-after-abandoned-roadmap
source_task_id: 20260405-01-il2cpp-reboot-after-abandoned-roadmap
source_relation: roadmap-child
active: false
---

## 关键文档

- brainstorm: `docs/dev/completed/20260406-03-stage-5-post-proof-split/brainstorm-v1-01.md`
- design: `docs/dev/completed/20260406-03-stage-5-post-proof-split/design-v1-01.md`
- roadmap_or_plan: `docs/dev/completed/20260406-03-stage-5-post-proof-split/roadmap-v1-01.md`
- latest_progress: `docs/dev/completed/20260406-03-stage-5-post-proof-split/notes/progress-v1-06.md`

## 当前判断

- current_focus: 本任务已完成归档；Stage 5 已把 post-proof follow-on 收敛为 `GenericEcho -> ReflectionLite -> PInvokeLite` 的 Windows 主线，并把 `Stage 5D macOS Reference Parity` 明确收窄为独立 parity handoff 计划。
- why_now: Stage 4 已拿到真实 Windows native proof，后续必须把“样例扩张”和“平台扩张”拆开推进，否则会重新落回旧路线那种 semantic/runtime/platform 三轴混调。
- done_definition: 已满足；Stage 5A / Stage 5B / Stage 5C 均已完成归档，Stage 5D 的首轮 parity 子集与执行交接也已冻结到独立 plan 任务中，Stage 5 的 child mapping、阶段顺序与风险边界已经稳定。

## 最近摘要

- 2026-04-06 12:48:45 +08:00: 已从 reboot 父 roadmap 派生 Stage 5 子任务，并把当前建议收敛为 `GenericEcho -> ReflectionLite -> PInvokeLite -> macOS parity`。
- 2026-04-06 14:26:02 +08:00: Stage 5A 已完成归档，真实 Windows reference proof 已验证 `generic-ok` / `exit code 0`。
- 2026-04-06 15:35:12 +08:00: Stage 5B 已完成归档，真实 Windows reference proof 已验证 `reflection-ok|...`，且 Stage 4 / Stage 5A 回归保持通过。
- 2026-04-06 16:05:00 +08:00: 已正式派生 `20260406-06-stage-5c-pinvoke-lite-windows-reference`，并把第一轮 scope 收窄为最小 `DllImport` 整数 import proof。
- 2026-04-06 16:19:51 +08:00: Stage 5C 已完成归档，真实 Windows reference proof 输出 `pinvoke-ok|14`；同时已确认当前 `kernel32.dll` / `MulDiv` proof 是 Windows 专属样例，不能原样纳入首轮 `macOS` parity。
- 2026-04-06 16:19:51 +08:00: 已派生并归档 `20260406-07-stage-5d-macos-reference-parity`，把首轮 `macOS` parity 收窄为 Stage 4 + Stage 5A + Stage 5B，并把 Stage 5C 明确留给后续跨平台 interop 样例重设计。

## 下一步

- next_action: 无。若未来切到 `macOS` 宿主推进 parity，实现入口以 `20260406-07-stage-5d-macos-reference-parity` 的 `plan-v1-01.md` 为准。
- owner: codex
- trigger: Stage 5 的 Windows trilogy 与 `macOS` parity handoff 都已拿到书面边界和归档证据。

## 风险 / 阻塞

### risks

- 如果后续强行把当前 Stage 5C 的 `kernel32.dll` / `MulDiv` proof 当作 `macOS` parity 输入，会把 Windows 专属 interop 假设误当成跨宿主 contract。
- 如果未来 `macOS` parity 实现绕开 Stage 4 / Stage 5A / Stage 5B 的既有 proof gate，或者重新把样例判断写回 `src/**`，仍会破坏本轮固定下来的边界。

### blockers

- 无。

## 关联任务

### parent

- `20260405-01-il2cpp-reboot-after-abandoned-roadmap`

### source

- `20260405-01-il2cpp-reboot-after-abandoned-roadmap`：roadmap-child

### children

- `20260406-04-stage-5a-generic-echo-windows-reference`
- `20260406-05-stage-5b-reflection-lite-windows-reference`
- `20260406-06-stage-5c-pinvoke-lite-windows-reference`
- `20260406-07-stage-5d-macos-reference-parity`
