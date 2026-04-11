---
task_id: 20260409-10-total-solution-and-ios-hot-update-analysis
title: 完整 IL2CPP 总方案与 iOS 热更新分析
task_type: roadmap
lifecycle_status: in_progress
phase: roadmap
created_at: 2026-04-09 23:20:00 +08:00
updated_at: 2026-04-11 22:45:00 +08:00
current_dir: docs/dev/in-progress/20260409-10-total-solution-and-ios-hot-update-analysis
parent_task_id:
source_task_id:
source_relation:
active: false
---

## 关键文档

- brainstorm: `docs/dev/in-progress/20260409-10-total-solution-and-ios-hot-update-analysis/brainstorm-v1-01.md`
- design: `docs/dev/in-progress/20260409-10-total-solution-and-ios-hot-update-analysis/design-v1-01.md`
- roadmap_or_plan: `docs/dev/in-progress/20260409-10-total-solution-and-ios-hot-update-analysis/roadmap-v1-01.md`
- phase_architecture: `docs/dev/in-progress/20260409-10-total-solution-and-ios-hot-update-analysis/phase-architecture-v1-01.md`
- native_output: `docs/dev/in-progress/20260409-10-total-solution-and-ios-hot-update-analysis/native-output-structure-v1-01.md`

## 当前判断

- current_focus: 主任务已完成 Phase 0/1/2/3/5/6/7 的本地落地与证据化验证；当前仅剩 Phase 4 `20260411-05-mobile-runtime-host` 仍因缺少真实 Android NDK / emulator / device 与 macOS/Xcode 环境而挂起。
- why_now: 本轮已把 Phase 7 的剩余解释器主线缺口收口为 same-assembly `CallVirt` 真实执行，以及真实 `ManagedExceptionRegionModel -> IRExceptionRegion` EH lowering / dispatcher minimal proof；因此父 roadmap 的主要阻塞重新回到 mobile runtime host 环境证据，而不是 desktop 主线实现缺口。
- done_definition: roadmap 需要按顺序落地全部阶段，并补齐 mobile runtime host 的真实 Android/iOS 运行证据后，才能宣称 `AOT 主线 + Interpreter 热更新 + Metadata Supplement + Mobile Host` 总方案闭环。

## 最近摘要

- 2026-04-11 22:45:00 +08:00: 为 Phase 4 外部阻塞新增 `run doctor` mobile runtime preflight；当前 Windows 宿主已可直接枚举 Android SDK/NDK/adb/emulator 缺口，并明确 iOS runtime 仍需要 macOS/Xcode 宿主。
- 2026-04-11 12:51:22 +08:00: `20260411-03-aot-runtime-completeness` 归档，`InterfaceDispatchProof` native perf baseline 与 `GoldenMultiProject` convert perf baseline 闭环。
- 2026-04-11 13:59:04 +08:00: `20260411-04-engine-binding-contract` 归档，`EngineHostProof` 与 `HostEmbeddingLite` 形成 windows host proof + ownership baseline。
- 2026-04-11 15:00:53 +08:00: `20260411-05-mobile-runtime-host` 因缺少真实 Android/macOS/Xcode 环境挂起，主线切换到 `20260411-06-hot-update-skeleton`。
- 2026-04-11 15:21:32 +08:00: `20260411-06-hot-update-skeleton` 归档，热更新骨架最小 proof 闭环。
- 2026-04-11 16:00:23 +08:00: `20260411-07-metadata-supplement-bridge` 归档，metadata supplement 与 bridge baseline 落地。
- 2026-04-11 19:10:21 +08:00: `20260411-08-interpreter-mixed-execution` 补齐实例 receiver / same-assembly `CallVirt` 真实执行，以及真实 `ManagedExceptionRegionModel -> IRExceptionRegion` EH lowering / dispatcher minimal proof 后归档；父 roadmap 在当前环境内已无新的 desktop 主线缺口。

## 下一步

- next_action: 先在目标宿主运行 `run doctor` 补齐 Android SDK/NDK/adb/emulator，并准备 macOS/Xcode 宿主；环境到位后恢复 Phase 4 mobile host proof，并据此决定父 roadmap 的最终归档方式。
- owner: codex
- trigger: 当前仓库内已没有新的解释器/热更新本地落地缺口，剩余阻塞为外部运行环境证据。

## 风险 / 阻塞

### risks

- 若在 Phase 4 真实 mobile runtime 证据缺失时提前宣称主任务完成，会把 desktop proof 错误外推成 mobile runtime 已闭环。
- 当前 Phase 7 交付的是 proof-first baseline，而不是完整 production interpreter；但它已足以支撑父 roadmap 在 Windows 环境内的解释器主线收口。

### blockers

- Android 真实运行证据仍需要 NDK / emulator / device。
- iOS 真实运行证据仍需要 macOS/Xcode。
