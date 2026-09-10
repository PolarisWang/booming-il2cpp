---
task_id: 20260413-24-solution-core-pack-subject-consolidation
title: Subject Solution Core-Pack Consolidation Roadmap
task_type: roadmap
lifecycle_status: completed
phase: archived
created_at: 2026-04-13 08:43:27 +08:00
updated_at: 2026-04-13 13:39:09 +08:00
current_dir: docs/dev/completed/20260413-24-solution-core-pack-subject-consolidation
parent_task_id:
source_task_id: 20260412-06-test-system-suite-redesign
source_relation: follow-up-roadmap-after-v1.03
active: false
---

## 关键文档

- reference_previous_roadmap: `docs/dev/completed/20260412-06-test-system-suite-redesign/roadmap-v1-03.md`
- brainstorm: `docs/dev/completed/20260413-24-solution-core-pack-subject-consolidation/brainstorm-v1-01.md`
- design: `docs/dev/completed/20260413-24-solution-core-pack-subject-consolidation/design-v1-01.md`
- roadmap_or_plan: `docs/dev/completed/20260413-24-solution-core-pack-subject-consolidation/roadmap-v1-01.md`
- latest_progress: `docs/dev/completed/20260413-24-solution-core-pack-subject-consolidation/notes/progress-v1-04.md`
- related_hanging_benchmark: `docs/dev/hanging/20260412-05-phase-b-generic-native-aot-benchmark-backend/STATUS.md`
- related_hanging_post_phase7: `docs/dev/hanging/20260411-10-post-phase7-development-roadmap/STATUS.md`

## 当前判断

- current_focus: 父 roadmap 已完成；仓库默认 retained solution-mode subject 已收敛为 `SolutionCorePack`、`HotUpdateHostPack`、`MixedExecutionFeaturePack`，并完成了真实 proof / benchmark 验证与 alias / legacy cleanup 收口。
- why_done: `SolutionCorePack` 已吸收 3 个 archetype solution subject 并删除旧根；`SolutionCorePack`、`HotUpdateHostPack`、`MixedExecutionFeaturePack` 的 proof / benchmark 真实运行全部通过；`SolutionCorePack` 的 3 个 archetype managed-output matrix 也已真实跑通；compatibility alias 已一次性删除。
- done_definition: 完成整个父 roadmap，把仓库收敛到极少数 retained solution-mode subject，跑通 proof / benchmark 验证闭环，并在 Phase 6 一次性删除全部 compatibility alias。

## 最近摘要

- 2026-04-13 08:43:27 +08:00: 基于仓库现状完成新一轮 brainstorm，确认“验证所有 solution 模式 subject”，并得到用户对方案 A 的批准。
- 2026-04-13 09:08:24 +08:00: Phase 0 完成并归档；激活 Phase 1 子任务 `20260413-26-phase-1-explicit-solution-upgrade`。
- 2026-04-13 09:33:43 +08:00: Phase 1 完成并收口；`.sln + primaryProjectPath` 在 Python/C# 双侧打通，主线转入 Phase 2。
- 2026-04-13 11:07:49 +08:00: Phase 2 子任务 `20260413-27-phase-2-solution-core-pack-consolidation` 完成，`SolutionCorePack` 真实跑通 `windows-native-check` proof 与 `windows-native-perf` benchmark。
- 2026-04-13 11:18:24 +08:00: 激活 Phase 3 子任务 `20260413-28-phase-3-hotupdate-and-mixed-consolidation`，开始收口 `HotUpdateHostPack` / `MixedExecutionFeaturePack` retained-subject 能力闭环。
- 2026-04-13 12:14:06 +08:00: Phase 3 完成并准备归档；两个 retained subject 都已拿到真实 proof / benchmark 证据，主线切换到 Phase 4 `20260413-29-phase-4-unified-entry-and-registry-cutover`。
- 2026-04-13 13:26:39 +08:00: Phase 4 child 收口后直接完成 verification + cleanup closure；3 个 archetype solution subject 已并入 `SolutionCorePack` 并删除旧根，compatibility alias 已清零，所有 retained solution-mode subject 的 proof / benchmark 与 archetype managed-output matrix 均已真实跑通。
- 2026-04-13 13:39:09 +08:00: 归档后补跑最终验证：`142 passed`，`registry refresh` 成功，`SolutionCorePack`、`HotUpdateHostPack`、`MixedExecutionFeaturePack` 的 proof / benchmark 与 3 个 archetype managed-output matrix 均再次真实跑通。

## 下一步

- next_action: 无。本 roadmap 已完成并归档；如需继续推进更大范围的 `tests/` taxonomy 重构或新的 solution subject 规划，需要另起 follow-up。
- owner: codex
- trigger: 用户确认需要新的 follow-up 主题。

## 风险 / 阻塞

### risks

- 本 roadmap 已完成。若后续要处理 `tests/unit/planning/test_subject_planner.py` 一类更大范围的 subject-heavy 机制清理，建议另立 follow-up，避免与已完成的 retained-subject consolidation 收口混在一起。

### blockers

- 当前无外部 blocker。
