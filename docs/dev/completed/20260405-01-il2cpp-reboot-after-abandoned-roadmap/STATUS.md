---
task_id: 20260405-01-il2cpp-reboot-after-abandoned-roadmap
title: IL2CPP-Like C# to C++ 重新 Brainstorm（废弃路线复盘）
task_type: roadmap
lifecycle_status: completed
phase: archived
created_at: 2026-04-05 20:51:20 +08:00
updated_at: 2026-04-06 16:19:51 +08:00
current_dir: docs/dev/completed/20260405-01-il2cpp-reboot-after-abandoned-roadmap
parent_task_id:
source_task_id: 20260329-01-il2cpp-like-cs-to-cpp
source_relation: reboot-after-abandonment
active: false
---

## 关键文档
- brainstorm: `docs/dev/completed/20260405-01-il2cpp-reboot-after-abandoned-roadmap/brainstorm-v1-01.md`
- design: `docs/dev/completed/20260405-01-il2cpp-reboot-after-abandoned-roadmap/design-v1-01.md`
- roadmap_or_plan: `docs/dev/completed/20260405-01-il2cpp-reboot-after-abandoned-roadmap/roadmap-v1-01.md`
- latest_progress: `docs/dev/completed/20260405-01-il2cpp-reboot-after-abandoned-roadmap/notes/progress-v1-12.md`
- finding: `docs/dev/completed/20260405-01-il2cpp-reboot-after-abandoned-roadmap/finding-2026-04-05-01-il2cpp-reboot-after-abandoned-roadmap.md`

## 当前判断
- current_focus: 本任务已完成归档；reboot 主线已经从旧父 roadmap 的废弃状态，走完了 Stage 1A / 1B / 2 / 3 / 4 / 5 的 proof-first 重建，并把后续 `macOS` parity 的执行入口冻结到独立 handoff 计划。
- why_now: 用户明确要求废弃旧路线、完整复盘并重建一条新的 proof-first 主线，而且这条主线必须能落到真实 native proof 与后续 follow-on 交接。
- done_definition: 已满足；reboot roadmap 的阶段顺序、依赖、child mapping 和推进边界都已稳定，所有 Stage 1A-Stage 5 子任务均已完成归档。

## 最近摘要
- 2026-04-05 22:40:00 +08:00: 已将 reboot 主线从单一大 plan 升级为轻量 roadmap，并拆出 Stage 1A / Stage 1B 两个子任务。
- 2026-04-06 00:04:02 +08:00: Stage 2 `HelloWorldObject` proof spec 已完成归档，canonical source / artifact / native landing 对齐。
- 2026-04-06 01:21:23 +08:00: Stage 3 `Managed Minimal Closure` 已完成归档，真实 `HelloWorldObject.dll` 到 managed closure bundle 的最小闭环成立。
- 2026-04-06 12:29:22 +08:00: Stage 4 `Native Bootstrap And Reference Proof` 已完成归档，真实 `Windows x64` native proof 输出 `Hello, World!`，`verify-roadmap-0` Windows gate 闭环。
- 2026-04-06 14:26:02 +08:00: Stage 5A `GenericEcho Windows Reference` 已完成归档，拿到真实 `generic-ok` 证据。
- 2026-04-06 15:35:12 +08:00: Stage 5B `ReflectionLite Windows Reference` 已完成归档，拿到真实 `reflection-ok|...` 证据。
- 2026-04-06 16:19:51 +08:00: Stage 5C `PInvokeLite Windows Reference` 已完成归档，拿到真实 `pinvoke-ok|14` 证据，并确认该 proof 是 Windows interop foundation，而不是跨宿主 parity 输入。
- 2026-04-06 16:19:51 +08:00: Stage 5 `Post-Proof Split` 与 Stage 5D `macOS Reference Parity` handoff plan 已同时完成归档；本轮无额外 wiki 更新需求，长期有效边界已沉淀到 roadmap、plan 与回归 gate 中。

## 下一步
- next_action: 无。若未来切到 `macOS` 宿主继续 parity，实现入口以 `20260406-07-stage-5d-macos-reference-parity` 的 `plan-v1-01.md` 为准。
- owner: codex
- trigger: Stage 1A-Stage 5 的所有 child 都已拿到归档证据，reboot 主线不再保留活跃执行职责。

## 风险/阻塞
### risks
- 如果后续重新把“样例扩张”和“平台扩张”混进同一轮，会重复走回旧路线的体量膨胀问题。
- 如果未来 `macOS` parity 忽略 Stage 5D handoff 中对 Stage 5C 的显式排除说明，会把 Windows 专属 interop 假设误写成跨宿主 contract。

### blockers
- 无。

## 关联任务
### parent
- 无。

### source
- `20260329-01-il2cpp-like-cs-to-cpp`：reboot-after-abandonment

### children
- `20260405-02-stage-1a-contract-surface-reset`
- `20260405-03-stage-1b-structure-and-test-migration`
- `20260405-04-stage-2-helloworld-proof-spec`
- `20260406-01-stage-3-managed-minimal-closure`
- `20260406-02-stage-4-native-bootstrap-and-reference-proof`
- `20260406-03-stage-5-post-proof-split`
