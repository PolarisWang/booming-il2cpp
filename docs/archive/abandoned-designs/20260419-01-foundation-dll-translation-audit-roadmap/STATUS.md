---
task_id: 20260419-01-foundation-dll-translation-audit-roadmap
title: Foundation DLL Translation Audit Roadmap
task_type: roadmap
lifecycle_status: cancelled
phase: abandoned
created_at: 2026-04-19 12:00:00 +08:00
updated_at: 2026-05-01 15:30:00 +08:00
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
recommended_next_child: 20260419-21-complex-bcl-ordered-dll-01-system-collections-immutable
active: false
---

## 关键文档

- brainstorm: `docs/dev/in-progress/20260419-01-foundation-dll-translation-audit-roadmap/brainstorm-v1-01.md`
- design: `docs/dev/in-progress/20260419-01-foundation-dll-translation-audit-roadmap/design-v1-01.md`
- roadmap: `docs/dev/in-progress/20260419-01-foundation-dll-translation-audit-roadmap/roadmap-v1-01.md`
- parallel_reporting_roadmap: `docs/dev/completed/20260425-03-foundation-dll-capability-closure-reporting-roadmap/roadmap-v1-01.md`
- resumed_after: `docs/dev/completed/20260425-01-runtime-skeleton-4c-handler-framework-roadmap/STATUS.md`

## 当前判断

- 用户已决定取消当前 roadmap，准备重新规划。
- Phase 0-1 的完成成果（CoreLib 91.5%、Reference Assembly、4C/ D3-C、ldind/stind）仍然有效，只是 roadmap 结构废弃。
- Phase 2 的 `System.Collections.Immutable` 代码生成结果已落地，可从新规划中按需取用。

## 最新摘要

### Phase 0-1 完成状态

- Phase 0（audit schema + subject chain freeze）：1/1 任务完成
- Phase 1（CoreLib full verification）：7/7 任务完成
  - 最新 canonical formal `20260501-135403-windows-b6a6`：`finalStatus = ok`
  - supplemental coverage：54863/59942（91.5%），剩余 20682 均为 `unsupportedShapeOrCapability`
  - Reference Assembly 架构落地，4C family router + D3-C pluginization 落地
  - ldind/stind 全系列补齐（C++ 模板 + 参数化 C# 发射器）
- Cross-cutting（capability closure reporting）：1/1 任务完成
- 额外 orphan 任务：20260430-01-ldind-stind（completed）/ 20260430-02-closure-corelib-resolution（superseded）

### Phase 2-5 待推进

- Phase 2（Complex BCL 13 DLL）：13 个任务，全部 pending
- Phase 3（Core System DLL）：1 个任务，pending
- Phase 4（Network/IO/Crypto）：1 个任务，pending
- Phase 5（Extended Platform-heavy）：1 个任务，pending

## 下一步

- next_action: 启动 Phase 2，执行 `20260419-21-complex-bcl-ordered-dll-01-system-collections-immutable` — 验证 `System.Collections.Immutable` 的完整翻译审计
- 用户已确认 Phase 1 终态（CoreLib 91.5%，剩余均为 platform intrinsics/COM/Console）
- owner: codex
- trigger: immediate
- parallel_followup:
  - 推进 `20260425-03-foundation-dll-capability-closure-reporting-roadmap` 的 projection 和 HTML cutover
  - Phase 2 启动前确认第一个 DLL（System.Collections.Immutable）的 audit packet 模板

## 风险 / 阻塞

### risks

- 剩余 20682 个 uncovered 方法均为 `unsupportedShapeOrCapability`（platform intrinsics/COM/Console），在当前框架内不构成 Phase 1 blocker，但在 Phase 3-5 扩展到其他 DLL 时需确认同口径适用。
- Phase 2 每个 DLL 都需要完整的 audit packet + managed/native/hotupdate proof + codegen review，首次执行的模板成本较高。

### blockers

- none — Phase 1 已自然终态，Phase 2 等待用户决策后即可启动

## DLL-First Reporting Sync

- recorded_at: `2026-04-25 17:55:00 +08:00`
- new_status_authority:
  - `docs/dev/in-progress/20260419-21-complex-bcl-ordered-dll-01-system-collections-immutable/STATUS.md`
  - `docs/dev/in-progress/20260419-37-corelib-supplemental-runtime-skeleton-coverage-widening/STATUS.md`
- intent:
  - 把 `System.Collections.Immutable` 从“只有 roadmap 表项”补成正式 DLL-first 审计入口。
  - 把 `20260419-37` 明确为当前 `System.Private.CoreLib` canonical proof/native/hotupdate/supplemental coverage authority。
- projection_contract:
  - `artifacts/**` primary evidence 才允许进入 `artifact-index.json` 与项目 evidence 列表。
  - `docs/**`、`subjects/**`、`verification/**` 只作为 `supportRefs` 出现在 DLL detail，不能驱动项目状态置绿。
- docs_publish_root:
  - `docs/verification/foundation-dll-audit/dashboard.html`
  - `docs/verification/foundation-dll-audit/summary.md`
  - `docs/verification/foundation-dll-audit/dlls/`
- formal_refresh:
  - `python build/toolchains/run/run.py verify verification-v1 --json`
  - `runId = 20260425-183023-windows-e58f`
