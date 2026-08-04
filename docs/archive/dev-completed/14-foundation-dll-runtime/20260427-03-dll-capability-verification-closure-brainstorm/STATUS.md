---
task_id: 20260427-03-dll-capability-verification-closure-brainstorm
title: DLL Capability Verification Closure Brainstorm
task_type: brainstorm
lifecycle_status: completed
phase: completed
created_at: 2026-04-27 02:35:00 +08:00
updated_at: 2026-04-27 04:25:00 +08:00
current_dir: docs/dev/completed/20260427-03-dll-capability-verification-closure-brainstorm
parent_task_id:
source_task_id: 20260419-01-foundation-dll-translation-audit-roadmap
source_relation: verification-closure-redesign-before-plan
brainstorm_doc: docs/dev/completed/20260427-03-dll-capability-verification-closure-brainstorm/brainstorm-v1-01.md
design_doc:
roadmap_or_plan: docs/dev/in-progress/20260427-04-dll-capability-verification-closure-roadmap/roadmap-v1-01.md
active: false
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
preflight_review: pass
preflight_summary: 用户已拍板 Hybrid Layer、A2 Source-Annotation-First、method 级全映射、G2 reviewable-bundle，以及正式主 `.sln` + family 局部加速入口的方案，brainstorm 输入已清零，可进入正式 roadmap。
auto_execution_decision: continue
---

## 关键文档

- brainstorm: `docs/dev/completed/20260427-03-dll-capability-verification-closure-brainstorm/brainstorm-v1-01.md`
- next_roadmap: `docs/dev/in-progress/20260427-04-dll-capability-verification-closure-roadmap/roadmap-v1-01.md`

## 当前结论

- DLL capability verification closure 采用 `Hybrid Layer`
- 导航结构固定为 `DLL -> family -> method -> method test detail tooltip`
- `Subject Validation` 采用 `methodSubjectId -> test case / benchmark case / hotupdate case` 全映射
- case 实现采用 `A2 Source-Annotation-First`
- `Review Bundle` 成为 completed-before gate，强度采用 `G2 reviewable-bundle`
- annotation scanner 只允许从 generated source / metadata 产出 formal index，不允许从运行结果反推 truth
- 正式主入口保留单一 `.sln`，family 局部入口仅作开发加速入口

## 下一步

- 进入 `20260427-04-dll-capability-verification-closure-roadmap`
