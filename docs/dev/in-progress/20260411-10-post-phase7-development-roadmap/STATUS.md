---
task_id: 20260411-10-post-phase7-development-roadmap
title: Post Phase-7 Development Roadmap (Phase A-E)
task_type: roadmap
lifecycle_status: in_progress
phase: roadmap
created_at: 2026-04-11 21:30:00 +08:00
updated_at: 2026-04-12 14:31:35 +08:00
current_dir: docs/dev/in-progress/20260411-10-post-phase7-development-roadmap
parent_task_id: "20260409-10-total-solution-and-ios-hot-update-analysis"
source_task_id: "20260409-10-total-solution-and-ios-hot-update-analysis"
source_relation: successor_roadmap
active: true
---

## 关键文档

- parent_roadmap: `docs/dev/completed/20260409-10-total-solution-and-ios-hot-update-analysis/roadmap-v1-01.md`
- phase_architecture: `docs/dev/completed/20260409-10-total-solution-and-ios-hot-update-analysis/phase-architecture-v1-01.md`
- roadmap: `docs/dev/in-progress/20260411-10-post-phase7-development-roadmap/roadmap-v1-01.md`
- phase_b_followup: `docs/dev/in-progress/20260412-05-phase-b-generic-native-aot-benchmark-backend/roadmap-v1-01.md`

## 当前判断

- current_focus: 虽然 Phase D 的仓库内基础设施子任务已经完成，但 Phase B 暴露出新的 follow-up 缺口：benchmark native 仍错误地绑定在 proof backend 上，需要单独的 roadmap 子任务修正。
- why_now: 这个缺口的真实 blocker 是仓库内的 backend 结构，而不是外部设备资源；与等待 macOS/Xcode 或 Android/iOS 真机相比，它是当前可立即继续推进的主线。
- done_definition: 父 roadmap 只有在 Phase B follow-up、Phase D 真机验证、以及后续 Phase E 收尾都完成后，才能整体关闭。

## 最近摘要

- 2026-04-12 11:42:02 +08:00: 已派生子任务 `20260412-05-phase-b-generic-native-aot-benchmark-backend`，专门承载“proof backend 与 generic native AOT backend 拆线”的 follow-up roadmap。
- 2026-04-12 11:02:14 +08:00: 子任务 `20260412-04-phase-d-mobile-foundation` 完成并归档；iOS 合规校验、移动 perf worker/collector 骨架、移动热更新桥接和 benchmark manifest 扩展已通过 `tests/unit/run -v` 全量 267 项验证。
- 2026-04-11 21:30:00 +08:00: 完成 Phase A-E 全量实施方案设计，形成 5 Phase / 35 sub-task 路线图。

## 下一步

- next_action: 先推进子任务 `20260412-05-phase-b-generic-native-aot-benchmark-backend`，并从其中派生第一执行子任务，冻结 `generated-native-aot` contract 与 benchmark `workloadEntry` 边界。
- trigger: 立即。

## 风险

### risks

- 如果继续用 proof family 特判去“修复” benchmark native，Phase B 会形成错误的长期结构，后续 `Bench*` 扩展会重复返工。
- Phase D 真机验证仍受外部环境约束，不能替代当前这条仓库内主线。
- Phase E 的 benchmark 回归与持续监控依赖真实 native benchmark backend，不能建立在 proof backend 假通路上。

### blockers

- Phase D 的外部环境 blocker 仍然存在，但不阻塞当前这个 Phase B follow-up 子任务。

## 2026-04-12 14:31:35 执行更新

- Phase B follow-up 子任务 `20260412-05-phase-b-generic-native-aot-benchmark-backend` 已完成 `BenchArithmetic` generic native AOT MVP：native benchmark 不再复用 proof host，也不再停在 placeholder `return 0`。
- 最新证据：`python build/toolchains/run/run.py benchmark --subject BenchArithmetic --mode native --record` 已通过；最新 native record 的 `meanDurationMs = 5.883 ms`，`meanChecksum = -182045701`，dashboard 已恢复真实 AOT(native) 数据。
- 父 roadmap 的 Phase B 剩余工作缩窄为 Stage 4 rollout：如需继续，把同一路径扩到其他 `Bench*` subject；当前主 blocker 已从“native AOT 无数据”收敛为“多 subject 迁移排期”。
