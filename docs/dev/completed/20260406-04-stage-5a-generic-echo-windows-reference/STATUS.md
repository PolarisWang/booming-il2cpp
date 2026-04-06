---
task_id: 20260406-04-stage-5a-generic-echo-windows-reference
title: Stage 5A GenericEcho Windows Reference
task_type: plan
lifecycle_status: completed
phase: archived
created_at: 2026-04-06 13:05:16 +08:00
updated_at: 2026-04-06 14:26:02 +08:00
current_dir: docs/dev/completed/20260406-04-stage-5a-generic-echo-windows-reference
parent_task_id: 20260406-03-stage-5-post-proof-split
source_task_id: 20260406-03-stage-5-post-proof-split
source_relation: roadmap-child
active: false
---

## 关键文档

- brainstorm: `docs/dev/completed/20260406-04-stage-5a-generic-echo-windows-reference/brainstorm-v1-01.md`
- design: `docs/dev/completed/20260406-04-stage-5a-generic-echo-windows-reference/design-v1-01.md`
- roadmap_or_plan: `docs/dev/completed/20260406-04-stage-5a-generic-echo-windows-reference/plan-v1-01.md`
- latest_progress: `docs/dev/completed/20260406-04-stage-5a-generic-echo-windows-reference/notes/progress-v1-04.md`

## 当前判断

- current_focus: 本任务已完成归档；Stage 5A 已用最小闭合泛型 proof 打通 managed closure、显式 native lowering plan，以及 Windows reference proof host / run gate。
- why_now: Stage 5A 是 Stage 5 follow-on 路线的第一条语义扩张路径，只有先用最小闭合泛型把 proof spine 跑通，后续的 `ReflectionLite` / `PInvokeLite` 才有稳定基线。
- done_definition: 已满足；`GenericEchoClosedMinimal` 能沿现有 Windows reference spine 产出稳定 managed closure 与 native reference artifacts，真实 proof run 输出 `generic-ok` 且 `exit code` 为 `0`，Stage 4 `HelloWorldObject` 基线继续通过，`src/**` 仍保持 sample-agnostic。

## 最近摘要

- 2026-04-06 12:48:45 +08:00: Stage 5 子 roadmap 已把 follow-on 路线固定为 `GenericEcho -> ReflectionLite -> PInvokeLite -> macOS parity`。
- 2026-04-06 13:05:16 +08:00: 已正式派生 Stage 5A，并把 active 切到新的 `20260406-04-stage-5a-generic-echo-windows-reference`。
- 2026-04-06 13:05:16 +08:00: 当前 scope 明确收窄为最小闭合泛型 proof，而不是直接承接完整 smoke `GenericEcho` 的 tuple / 插值字符串复杂度。
- 2026-04-06 14:07:01 +08:00: 已把 native reference emission 改成“先生成显式 lowering plan contract、再渲染 Scriban 模板”，同时清掉 `src/**` 中与 proof 样例绑定的 `ClosedGenericEcho` / `first-proof` 标签。
- 2026-04-06 14:12:36 +08:00: 已为 `GenericEchoClosedMinimal` 补齐 Windows proof host 目录、root CMake 路由与 run script；当前静态 wiring 与 Stage 3/4/5A 回归均已通过。
- 2026-04-06 14:26:02 +08:00: 本轮 `python -m unittest -v tests.unit.run.test_stage5a_generic_echo_windows_reference` 四个用例全部通过，其中 `test_windows_reference_proof_run_outputs_generic_ok` 已拿到本机构建/运行证据：`stdout` 包含 `generic-ok`，`exit code` 为 `0`。
- 2026-04-06 14:26:02 +08:00: Stage 5A 已完成归档；本任务无需额外 wiki 更新，长期有效约束已沉淀到回归测试、proof host wiring 与 Stage 5 roadmap。

## 下一步

- next_action: 无。若继续 reboot 主线，由父 roadmap `20260406-03-stage-5-post-proof-split` 决定 Stage 5B `ReflectionLite Windows Reference` 的派生时点与边界。
- owner: codex
- trigger: Stage 5A 的 managed closure、native lowering plan、proof host wiring 与真实 proof run 都已拿到验证证据，任务达成完成条件。

## 风险 / 阻塞

### risks

- 如果后续 Stage 5B / Stage 5C 绕过本轮固定的 `native-proof.plan.json`、proof host wiring 与 sample-agnostic 边界，仍可能再次把样例细节泄漏回 `src/**`。
- 如果后续 follow-on 路线不继续把 Stage 4 与 Stage 5A gates 作为固定基线，proof spine 仍可能在新语义扩张中悄悄回退。

### blockers

- 无。

## 关联任务

### parent

- `20260406-03-stage-5-post-proof-split`

### source

- `20260406-03-stage-5-post-proof-split`：roadmap-child

### children

- 无。
