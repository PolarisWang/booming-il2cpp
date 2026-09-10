---
task_id: 20260411-06-hot-update-skeleton
title: Hot Update Skeleton
task_type: plan
lifecycle_status: completed
phase: archived
created_at: 2026-04-11 15:00:53 +08:00
updated_at: 2026-04-11 15:21:32 +08:00
current_dir: docs/dev/completed/20260411-06-hot-update-skeleton
parent_task_id: 20260409-10-total-solution-and-ios-hot-update-analysis
source_task_id: 20260409-10-total-solution-and-ios-hot-update-analysis
source_relation: child_of_roadmap_phase_5
active: false
---

## 关键文档

- parent_roadmap: `docs/dev/in-progress/20260409-10-total-solution-and-ios-hot-update-analysis/roadmap-v1-01.md`
- phase_architecture: `docs/dev/in-progress/20260409-10-total-solution-and-ios-hot-update-analysis/phase-architecture-v1-01.md`
- plan: `docs/dev/completed/20260411-06-hot-update-skeleton/plan-v1-01.md`
- latest_progress: `docs/dev/completed/20260411-06-hot-update-skeleton/notes/progress-v1-01.md`

## 当前判断

- current_focus: Batch 1-4.4 已全部闭环；`Chaos.IL2CPP.HotUpdate` managed 项目、`src/native/hot-update/` native 模块、`runtime-core` mode query、subject-id 级 method registration/dispatch hook 以及 `subjects/HotUpdateSkeletonProof` 已形成 Phase 5 最小热更新骨架。
- why_now: Phase 5 已拿到 desktop 可验证的 runtime skeleton 证据，父 roadmap 可以推进到 Phase 6 `20260411-07-metadata-supplement-bridge`，继续补齐 supplemental metadata 与 bridge generation。
- done_definition: `plan-v1-01.md` 的 Batch 1-4.4 已完成；专项 pytest、repo layout 回归、managed build、solution build、native compile 以及 `HotUpdateSkeletonProof` 真实 subject 证据均已通过。

## 最近摘要

- 2026-04-11 15:00:53 +08:00: `20260411-05-mobile-runtime-host` 因缺少真实 Android/iOS 宿主环境挂起；父 roadmap 主线切换到 `20260411-06-hot-update-skeleton`，开始执行 desktop 可验证的 hot update skeleton。
- 2026-04-11 15:21:32 +08:00: 完成 package reader/validator、runtime mode switch、constant-return interpreter stub、method registration/dispatch hook 与 `HotUpdateSkeletonProof`；任务归档并切换到 `20260411-07-metadata-supplement-bridge`。

## 下一步

- next_action: 激活 `20260411-07-metadata-supplement-bridge`，先为 Batch 1 supplemental metadata template 写 RED，并从 `MetadataWriter` 产物接线开始闭环。
- owner: codex
- trigger: Phase 5 skeleton 已闭环，Phase 6 成为当前主线依赖入口。

## 验证

- passed: `python -m pytest tests/unit/run/test_phase5_hot_update_skeleton.py -v`
- passed: `python -m pytest tests/unit/run/test_repo_layout.py tests/unit/run/test_subject_manifest_schema.py -v`
- passed: `python -m pytest tests/unit/run/test_subject_planner.py -v`
- passed: `dotnet run --project subjects/HotUpdateSkeletonProof/source/HotUpdateSkeletonProof.csproj --`
- passed: `HotUpdateSkeletonProof` 输出 `before-load=1`, `after-load=42`, `after-unload=1`, `corruption=rejected`
- passed: `dotnet build src/managed/Chaos.IL2CPP.HotUpdate/Chaos.IL2CPP.HotUpdate.csproj -c Release`
- passed: `dotnet build solutions/core/windows/chaos-il2cpp-core.sln -c Release`
- passed: `cmake -S . -B artifacts/.tmp-hot-update-reference -G "Visual Studio 17 2022" -DROADMAP0_PRESET_TARGET=windows-x64-reference`
- passed: `cmake --build artifacts/.tmp-hot-update-reference --config Release --target chaos_hot_update`

## 风险 / 阻塞

### risks

- Phase 5 交付的是最小 hot update skeleton，不等于 supplemental metadata、bridge generation 或完整 interpreter。
- 当前 method dispatch proof 仍是 subject-id 定向注册的最小闭环，尚未覆盖跨模式 virtual/interface/delegate route。
- `HotUpdateSkeletonProof` 是 desktop managed-output proof，不代表 mobile runtime host 已完成。

### blockers

- 无。
