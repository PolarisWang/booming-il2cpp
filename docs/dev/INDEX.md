# 开发任务总览

本目录汇总当前采用 `docs/dev` 流程管理的任务。

| 任务 | lifecycle_status | phase | 最近更新 | 下一步 | active |
| --- | --- | --- | --- | --- | --- |
| `20260406-03-subject-test-framework-rollout-roadmap` | `in_progress` | `roadmap` | `2026-04-06 18:50:00 +08:00` | 等待用户要求继续 `subject` 主线；届时按 roadmap 派生 `Stage D + Stage E` 合并计划与 `Stage F` 后续计划。 | `no` |
| `20260406-04-subject-stage-a-b-c-foundation-and-windows-cutover` | `completed` | `archived` | `2026-04-06 18:50:00 +08:00` | 无。若继续 `subject` 主线，以父 roadmap 派生 `Stage D + Stage E` follow-up plan 为下一入口。 | `no` |
| `20260406-07-stage-5d-macos-reference-parity` | `completed` | `archived` | `2026-04-06 16:19:51 +08:00` | 无。若未来切到 `macOS` 宿主执行 parity，实现入口以本任务冻结的 `plan-v1-01.md` 为准。 | `no` |
| `20260406-06-stage-5c-pinvoke-lite-windows-reference` | `completed` | `archived` | `2026-04-06 16:19:51 +08:00` | 无。Stage 5 的 Windows interop foundation 已闭环，`macOS` parity 交接已移入 `20260406-07-stage-5d-macos-reference-parity`。 | `no` |
| `20260406-03-stage-5-post-proof-split` | `completed` | `archived` | `2026-04-06 16:19:51 +08:00` | 无。Stage 5 的 follow-on 路线与 parity handoff 边界已冻结。 | `no` |
| `20260405-01-il2cpp-reboot-after-abandoned-roadmap` | `completed` | `archived` | `2026-04-06 16:19:51 +08:00` | 无。reboot 主线 Stage 1A-Stage 5 已全部完成归档。 | `no` |
| `20260406-05-stage-5b-reflection-lite-windows-reference` | `completed` | `archived` | `2026-04-06 15:35:12 +08:00` | 无。Stage 5 follow-on 已继续推进到 `20260406-06-stage-5c-pinvoke-lite-windows-reference`。 | `no` |
| `20260406-04-stage-5a-generic-echo-windows-reference` | `completed` | `archived` | `2026-04-06 14:26:02 +08:00` | 无。若继续 reboot 主线，下一步由 Stage 5 子 roadmap 决定是否派生 Stage 5B `ReflectionLite Windows Reference`。 | `no` |
| `20260406-02-stage-4-native-bootstrap-and-reference-proof` | `completed` | `archived` | `2026-04-06 12:29:22 +08:00` | 无。若继续 reboot 主线，下一步由父 roadmap 决定是否派生 `Stage 5: Post-Proof Split`。 | `no` |
| `20260406-01-stage-3-managed-minimal-closure` | `completed` | `archived` | `2026-04-06 01:21:23 +08:00` | 无。若继续 reboot 主线，下一步由父 roadmap 决定是否派生 Stage 4 `Native Bootstrap And Reference Proof`。 | `no` |
| `20260405-04-stage-2-helloworld-proof-spec` | `completed` | `archived` | `2026-04-06 00:04:02 +08:00` | 无。若继续 reboot 主线，下一步由父 roadmap 决定 Stage 3 / Stage 4 的派生节奏。 | `no` |
| `20260405-03-stage-1b-structure-and-test-migration` | `completed` | `archived` | `2026-04-05 23:32:04 +08:00` | 无。若继续 reboot 主线，下一步从父 roadmap 派生 Stage 2 `HelloWorld` proof spec。 | `no` |
| `20260405-02-stage-1a-contract-surface-reset` | `completed` | `archived` | `2026-04-05 23:07:20 +08:00` | 无。若继续 reboot 主线，下一步转到 `20260405-03-stage-1b-structure-and-test-migration`。 | `no` |
| `20260329-01-il2cpp-like-cs-to-cpp` | `abandoned` | `archived` | `2026-04-05 20:51:20 +08:00` | 无。历史内容已转为参考资产；后续请转到 `20260405-01-il2cpp-reboot-after-abandoned-roadmap`。 | `no` |
| `20260404-03-roadmap-1-assembly-and-typed-il-ir` | `abandoned` | `archived` | `2026-04-05 20:51:20 +08:00` | 无。若未来仍需该主题，请从 reboot 主线重新派生。 | `no` |
| `20260404-04-roadmap-3-runtime-core-v1` | `abandoned` | `archived` | `2026-04-05 20:51:20 +08:00` | 无。若未来仍需该主题，请从 reboot 主线重新派生。 | `no` |
| `20260404-05-test-verification-architecture` | `completed` | `archived` | `2026-04-05 01:40:59 +08:00` | 无。如需继续扩展测试对象或补充其他宿主验证，请新建 follow-up 任务。 | `no` |
| `20260331-01-roadmap-0-contract-freeze-and-skeleton` | `completed` | `archived` | `2026-04-04 14:09:00 +0800` | 无。 | `no` |
| `20260402-01-harness-engineer-automation` | `completed` | `archived` | `2026-04-04 14:09:00 +0800` | 无。 | `no` |
| `20260403-01-unified-run-entrypoint` | `completed` | `archived` | `2026-04-04 14:09:00 +0800` | 无。 | `no` |
| `20260403-02-unified-test-framework` | `completed` | `archived` | `2026-04-04 14:09:00 +0800` | 无。 | `no` |
| `20260404-01-skill-flow-refactor` | `completed` | `reviewing` | `2026-04-04 13:45:41 +0800` | 无。如需继续提升运行时验证稳定性，请新建 follow-up 任务。 | `no` |
| `20260404-02-booming-docs-migration` | `completed` | `reviewing` | `2026-04-04 13:57:28 +0800` | 无。如需彻底移除 `docs/booming`，请另开 follow-up 处理历史路径清理。 | `no` |
