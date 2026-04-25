---
task_id: 20260419-01-foundation-dll-translation-audit-roadmap
title: Foundation DLL Translation Audit Roadmap
task_type: roadmap
lifecycle_status: in-progress
phase: roadmap
created_at: 2026-04-19 12:00:00 +08:00
updated_at: 2026-04-25 22:30:00 +08:00
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
- parallel_reporting_roadmap: `docs/dev/in-progress/20260425-03-foundation-dll-capability-closure-reporting-roadmap/roadmap-v1-01.md`
- resumed_after: `docs/dev/completed/20260425-01-runtime-skeleton-4c-handler-framework-roadmap/STATUS.md`

## 当前判断

- 独立的 `runtime-skeleton 4C` roadmap 已完成，foundation audit 主线已恢复为 `in-progress` 并继续自动执行。
- DLL-first reporting contract 已冻结；后续 reporting / projection contract 改动必须同时刷新 `verification-v1` 正式数据，不能只改 schema 或页面读取逻辑。
- 当前唯一推荐 child 仍是 `20260419-37-corelib-supplemental-runtime-skeleton-coverage-widening`。
- 当前 widening 的结构前提已经变化：
  - legacy dispatcher / legacy family 已被清理
  - 后续 `System.Convert` / CoreLib widening 必须建立在新的 4C family framework 上

## 最新摘要

- 2026-04-25 22:30:00 +08:00：已把用户批准的 `Capability Closure + Workflow Progress` 报告方案写入新的并行 roadmap `20260425-03-foundation-dll-capability-closure-reporting-roadmap`。该 child 不抢占当前 canonical widening 主线，先以 `manual` 模式冻结 denominator / workflow / completion authority，后续再推进 projection 和 HTML cutover。
- 2026-04-25 18:31:00 +08:00：已把 DLL-first 最新报告正式发布到 `docs/verification/foundation-dll-audit/`。`verify verification-v1` 现在会同时刷新 `verification/projections/foundation-dll-audit/*` 与 docs 镜像；最新 formal refresh 为 `20260425-183023-windows-e58f`。
- 2026-04-25 17:55:00 +08:00：继续收口 DLL-first reporting projection。`foundation_dll_audit_generator` 现已把 primary evidence 与 support refs 分层；`artifact-index` 只保留 `artifacts/**` 真实产物，`docs/**` 等支撑引用改为 DLL detail 中的 `supportRefs`。formal refresh 已完成于 `20260425-175326-windows-4cb7`。
- 2026-04-25 16:20:56 +08:00：同步 child `20260419-37` 最新 stop point。`System.Convert::ToChar(...)` 已在 formal `20260425-153952-windows-ff29` 中清零；`System.Convert` 当前剩余 supplemental uncovered 为 `251`，child 已自动切到 `System.Convert::ToString` widening。
- 2026-04-25 20:50:00 +08:00：已把 DLL-first reporting 的 “formal data refresh” 规则同步到 `dev-project-test-governance` skill、`wiki/06-测试验证/*` 与 `docs/architecture/verification-v1/spec.md`；命中 report / projection contract 变更时，必须刷新 `verification-v1` 正式数据与对应 projection。
- 2026-04-25 20:35:00 +08:00：已冻结 DLL-first reporting contract；program scope 为 `System.Private.CoreLib + Complex BCL 13 DLL`，verification project 模板固定为 `audit-input-and-ledger / managed-proof / native-proof / hotupdate-proof / benchmark / codegen-review / completion-certification`，且所有状态必须绑定可点击证据。
- 2026-04-25 11:10:52 +08:00：`20260425-01-runtime-skeleton-4c-handler-framework-roadmap` completed，foundation audit 主线恢复为 `in-progress`，并重新指定 `20260419-37` 为推荐 child。

## 下一步

- next_action: 继续自动执行 `20260419-37-corelib-supplemental-runtime-skeleton-coverage-widening`，基于新的 4C framework 扩大 `System.Private.CoreLib` supplemental runtime-skeleton translated coverage；当前优先簇为 `System.Convert::ToString`。
- owner: codex
- trigger: immediate
- parallel_followup:
  - 将 DLL-first reporting contract 映射成正式 `ProgramRecord / DllAuditRecord / VerificationProjectRecord / ArtifactRecord`
  - 定义 `Program Overview / DLL Matrix / DLL Detail / Artifact Index` 页面字段契约
  - 推进 `20260425-03-foundation-dll-capability-closure-reporting-roadmap`，将“真实能力进度条”收口到 `capability-family ledger + required workflow gates`

## 风险 / 阻塞

### risks

- `UnsupportedReason` diagnostics 还没有完全接入顶层 dispatcher；如果后续 widening 需要更强的 uncovered 聚类，可在 child 内并行评估。
- 恢复 widening 后，必须持续避免把新的 `System.Convert` / CoreLib lane 回流到 legacy 风格入口。

### blockers

- none

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
