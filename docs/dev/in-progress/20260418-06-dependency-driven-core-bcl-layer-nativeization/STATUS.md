---
task_id: 20260418-06-dependency-driven-core-bcl-layer-nativeization
title: Dependency-Driven Core BCL Layer Nativeization
task_type: plan
lifecycle_status: in_progress
phase: planning
created_at: 2026-04-18 11:33:19 +08:00
updated_at: 2026-04-18 11:33:19 +08:00
latest_stop_point: child task created from roadmap after completing 20260418-05; design and execution plan are now ready and dependency-layer planning can start
current_dir: docs/dev/in-progress/20260418-06-dependency-driven-core-bcl-layer-nativeization
parent_task_id: 20260418-01-dotnet-foundation-pack-nativeization-roadmap
source_task_id: 20260418-01-dotnet-foundation-pack-nativeization-roadmap
source_relation: roadmap-child-dependency-layer-nativeization
design_doc: docs/dev/in-progress/20260418-06-dependency-driven-core-bcl-layer-nativeization/design-v1-01.md
plan_doc: docs/dev/in-progress/20260418-06-dependency-driven-core-bcl-layer-nativeization/plan-v1-01.md
total_tasks: 6
current_task: step-1-freeze-dependency-layer-scope
active: true
---

## 关键文档

- parent_status: `docs/dev/in-progress/20260418-01-dotnet-foundation-pack-nativeization-roadmap/STATUS.md`
- parent_roadmap: `docs/dev/in-progress/20260418-01-dotnet-foundation-pack-nativeization-roadmap/roadmap-v1-01.md`
- completed_semantic_family_status: `docs/dev/completed/20260418-05-phase-3-system-private-corelib-semantic-family-nativeization/STATUS.md`
- design: `docs/dev/in-progress/20260418-06-dependency-driven-core-bcl-layer-nativeization/design-v1-01.md`
- plan: `docs/dev/in-progress/20260418-06-dependency-driven-core-bcl-layer-nativeization/plan-v1-01.md`

## 当前判断

- `20260418-05` 已经证明 corelib 首批 semantic-family execution entry 与 real-command 闭环可用，当前可以把主线推进到 dependency-driven 的核心 `System.*` DLL 分层认证。
- 当前 child task 还处于 planning 起点；首要工作是冻结 layer plan 输入 authority 和首层 ready / blocked 分类。

## 最近摘要

- 2026-04-18 11:33:19 +08:00: 新建 dependency-layer child task，改用语义化 task_id，避免继续新增 `Phase{x}` 风格命名。
- 2026-04-18 11:21:56 +08:00: `20260418-05` 已完成归档，首批 semantic-family 27 条 canonical commands 全部通过。

## 下一步

- step-1: 冻结 dependency-layer 范围、命名与输入 authority，确定本任务要消费的 foundation pack / contract / substrate / semantic-family 产物。

## 风险 / 阻塞

### risks

- 如果 layer plan 只做“名称分组”而不做真实依赖裁剪，后续 DLL 认证仍会回到人工猜测。
- 如果首层 DLL 没有与 proof / benchmark authority 一起设计，后续 execution entry 会再次出现“能规划但不能落地”的断层。

### blockers

- 当前 blocker: `none`
- 说明: 当前已具备继续规划 dependency-layer 的输入 authority。

## 验证

- `Get-Content docs/dev/in-progress/20260418-06-dependency-driven-core-bcl-layer-nativeization/design-v1-01.md -Raw | Out-Null`
  - 结果: 通过
- `Get-Content docs/dev/in-progress/20260418-06-dependency-driven-core-bcl-layer-nativeization/plan-v1-01.md -Raw | Out-Null`
  - 结果: 通过

## wiki

- 本轮暂不写 wiki
- 原因: 当前只完成 child task 建立与 planning intake，尚未形成长期 authority
