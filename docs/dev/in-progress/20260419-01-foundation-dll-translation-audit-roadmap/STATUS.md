---
task_id: 20260419-01-foundation-dll-translation-audit-roadmap
title: Foundation DLL Translation Audit Roadmap
task_type: roadmap
lifecycle_status: in-progress
phase: roadmap
created_at: 2026-04-19 12:00:00 +08:00
updated_at: 2026-04-25 11:10:52 +08:00
current_dir: docs/dev/in-progress/20260419-01-foundation-dll-translation-audit-roadmap
parent_task_id:
source_task_id: 20260418-01-dotnet-foundation-pack-nativeization-roadmap
source_relation: followup-audit-roadmap-after-foundation-pack-nativeization
brainstorm_doc: docs/dev/in-progress/20260419-01-foundation-dll-translation-audit-roadmap/brainstorm-v1-01.md
design_doc: docs/dev/in-progress/20260419-01-foundation-dll-translation-audit-roadmap/design-v1-01.md
roadmap_or_plan: docs/dev/in-progress/20260419-01-foundation-dll-translation-audit-roadmap/roadmap-v1-01.md
child_execution_mode: auto
auto_continue: true
auto_stop_policy: blocking-only
recommended_next_child: 20260419-37-corelib-supplemental-runtime-skeleton-coverage-widening
active: false
---

## 关键文档

- brainstorm: `docs/dev/in-progress/20260419-01-foundation-dll-translation-audit-roadmap/brainstorm-v1-01.md`
- design: `docs/dev/in-progress/20260419-01-foundation-dll-translation-audit-roadmap/design-v1-01.md`
- roadmap: `docs/dev/in-progress/20260419-01-foundation-dll-translation-audit-roadmap/roadmap-v1-01.md`
- resumed_after: `docs/dev/completed/20260425-01-runtime-skeleton-4c-handler-framework-roadmap/STATUS.md`

## 当前判断

- 独立 `runtime-skeleton 4C` roadmap 已于 2026-04-25 完成，不再阻塞 foundation audit 主线。
- 当前主线重新恢复为 in-progress，并切回自动执行模式。
- 恢复后的唯一推荐 child：
  - `20260419-37-corelib-supplemental-runtime-skeleton-coverage-widening`
- 继续推进的结构前提已变化：
  - 旧 legacy dispatcher / legacy family 已被清理
  - 后续 `System.Convert` / CoreLib widening 应建立在新的 4C framework 上

## 最新摘要

- 2026-04-25 11:10:52 +08:00: `20260425-01-runtime-skeleton-4c-handler-framework-roadmap` completed，foundation audit 主线恢复为 in-progress，并重新指定 `20260419-37` 为推荐 child。
- 2026-04-25 02:35:00 +08:00: 因用户要求先完成独立 4C roadmap，本任务从 `in-progress` 暂时转为 `hanging`。
- 2026-04-25 01:30:00 +08:00: 最新 canonical `20260425-012548-windows-b1f7` 证明 `System.Convert` primitive lane 存在真实回收，但“把 Convert 一次性做完”超出旧 widening child 的 authority。

## 下一步

- next_action: 自动恢复 `20260419-37-corelib-supplemental-runtime-skeleton-coverage-widening`，基于新的 4C framework 继续扩大 `System.Private.CoreLib` supplemental runtime-skeleton translated coverage。
- owner: codex
- trigger: immediate

## 风险 / 阻塞

### risks

- `UnsupportedReason` diagnostics 尚未接入顶层 dispatcher；如果后续 widening 需要更强的 uncovered 分类，可在恢复 child 中并行评估。
- 恢复 widening 后，需要持续避免把新的 Convert/CoreLib lane 再塞回 legacy 风格入口。

### blockers

- none
