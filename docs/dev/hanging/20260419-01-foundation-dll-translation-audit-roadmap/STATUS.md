---
task_id: 20260419-01-foundation-dll-translation-audit-roadmap
title: Foundation DLL Translation Audit Roadmap
task_type: roadmap
lifecycle_status: hanging
phase: hanging
created_at: 2026-04-19 12:00:00 +08:00
updated_at: 2026-04-21 23:45:00 +08:00
latest_stop_point: 用户要求暂停 20260419 主线；保留当前 audit roadmap、CoreLib blocker 结论与子任务分解，待恢复后再继续推进
current_dir: docs/dev/hanging/20260419-01-foundation-dll-translation-audit-roadmap
parent_task_id:
source_task_id: 20260418-01-dotnet-foundation-pack-nativeization-roadmap
source_relation: followup-audit-roadmap-after-foundation-pack-nativeization
brainstorm_doc: docs/dev/in-progress/20260419-01-foundation-dll-translation-audit-roadmap/brainstorm-v1-01.md
design_doc: docs/dev/in-progress/20260419-01-foundation-dll-translation-audit-roadmap/design-v1-01.md
roadmap_or_plan: docs/dev/in-progress/20260419-01-foundation-dll-translation-audit-roadmap/roadmap-v1-01.md
active: false
---

## 2026-04-21 挂起

- 用户明确要求先暂停 `20260419-*` 相关任务。
- 当前 roadmap 结论保持不变：
  - `System.Private.CoreLib` full verification 仍是 Complex BCL lane 之前的硬前置。
  - 已完成的 blocker normalization 与子任务拆分继续有效。
- 本次挂起不改变既有 completed 子任务结论，也不新增验证。

## 下一步

- 等待用户恢复本 roadmap。
- 恢复后先重新核对：
  - `20260419-07` 的最新 CoreLib packet 边界
  - `20260419-03` 的 blocked gate 结论
  - `FoundationDllTranslationSolution` 与 codegen review authority 是否仍与当前主线一致

## 当前结论

- 上一条 `20260418-01-dotnet-foundation-pack-nativeization-roadmap` 已完成并提交，但它不能直接等价为“所有目标 DLL 已完成强语义完整翻译验证”。
- 本任务承接新的要求：建立从实现到 DLL 完全翻译、再到 codegen 结果审核的明确 audit 路径。
- `System.Private.CoreLib` 必须作为第一阶段完整验证对象，按方案 C 执行，不再接受“首批 semantic-family 已通过”作为完整结论。
- Complex BCL lane 的 13 个 DLL 必须按约定顺序逐个验证，并在可实现时同时保留 hotupdate 路径。
- subject 验证项目必须能生成 `.sln`，并能让用户看到 `managed -> native -> hotupdate` 的产物链。
- codegen 审核产物必须同时输出 JSON 和 Markdown。

## 本轮产物

- `brainstorm-v1-01.md`
- `design-v1-01.md`
- `roadmap-v1-01.md`

## AOT/IL2CPP/Test Governance Intake

- capabilityFamily: `FrameworkPackNativeization`
- capabilityItem: `FoundationDllTranslationAudit`
- ownerSubjectId: `SolutionCorePack`
- proofRequired: `true`
- benchmarkRequired: `true`
- hotupdateImpact: `Proof`
- formalVerificationObjects:
  - `System.Private.CoreLib` full verification gate
  - `FoundationDllTranslationSolution` subject/workspace gate
  - ordered Complex BCL DLL verification gates
  - generated native code review reports
  - generated hotupdate artifact review reports
- requiredGates:
  - `collector`
  - `registry`
  - `workspace`
  - `managed-proof`
  - `native-proof`
  - `hotupdate-proof`
  - `benchmark`
  - `codegen-json-review`
  - `codegen-markdown-review`

## 待确认事项

- 是否新增独立 `FoundationDllTranslationSolution`，还是先扩展 `SolutionCorePack` 的工程集合。当前 roadmap 推荐新增独立 solution 并挂在 `SolutionCorePack` subject 下。
- Phase 2/3/4 的完整 DLL 清单是否作为长期 scope 冻结。当前 roadmap 已给出推荐范围，但先只把 `System.Private.CoreLib` 与 Complex BCL 13 DLL 设为硬顺序执行对象。
