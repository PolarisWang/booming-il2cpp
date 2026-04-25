---
task_id: 20260419-37-corelib-supplemental-runtime-skeleton-coverage-widening
title: CoreLib Supplemental Runtime Skeleton Coverage Widening
task_type: plan
lifecycle_status: in-progress
phase: plan
created_at: 2026-04-24 21:20:00 +08:00
updated_at: 2026-04-25 11:10:52 +08:00
current_dir: docs/dev/in-progress/20260419-37-corelib-supplemental-runtime-skeleton-coverage-widening
parent_task_id: 20260419-01-foundation-dll-translation-audit-roadmap
source_task_id: 20260419-03-system-private-corelib-full-verification
source_relation: unblocker-for-corelib-full-verification
clearance_source: parent-roadmap
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
preflight_review: warn
preflight_summary: 旧 widening child 已获得新的 4C framework 前置条件；当前可继续沿 `System.Convert` 与 supplemental CoreLib lane 扩面，但需要保持新 family/router 边界，不回退到 legacy 入口。
auto_execution_decision: continue
design_doc: docs/dev/in-progress/20260419-01-foundation-dll-translation-audit-roadmap/design-v1-01.md
plan_doc: docs/dev/in-progress/20260419-37-corelib-supplemental-runtime-skeleton-coverage-widening/plan-v1-01.md
active: true
---

# 20260419-37 CoreLib Supplemental Runtime Skeleton Coverage Widening

## Current State

- 最新 supplemental runtime-skeleton coverage 基线：
  - `requestedMethodCount = 59860`
  - `emittedMethodCount = 1826`
  - `uncoveredMethodCount = 52983`
- 最新 `System.Convert` 未覆盖数：
  - `303 -> 267`
- 新的结构前提已具备：
  - `20260425-01-runtime-skeleton-4c-handler-framework-roadmap` 已 completed
  - `20260425-02-runtime-skeleton-4c-framework-foundation-cutover` 已 completed
  - 顶层 runtime-skeleton 已切到 4C family router，legacy dispatcher 不再是主路径

## Current Judgment

- 本 child 结束 hanging，恢复为 active in-progress。
- 继续 widening 时应直接建立在新的 `Convert family` / `ConvertLike family` / shared handler loop 基础上推进。
- 本轮恢复后不再需要等待独立 blocker roadmap。

## Latest Summary

- 2026-04-25 11:10:52 +08:00: 独立 4C roadmap 已完成并归档，本 child 恢复为 in-progress，继续作为 foundation audit 主线的当前 active child。
- 2026-04-25 02:35:00 +08:00: 因用户要求先完成独立 4C roadmap，本 child 主动挂起。
- 2026-04-25 01:28:00 +08:00: widening 已覆盖 `Convert` primitive lane 的一批真实方法，未覆盖数降至 `267`。

## Next

- next_action: 先基于新 4C framework 重新锁定下一批 `System.Convert` / supplemental CoreLib runtime-skeleton widening 目标，再补 RED 测试并继续扩面。
- owner: codex
- trigger: immediate

## Verification

- `python -m pytest tests/unit/compatibility/test_full_assembly_closure_codegen_audit_plan.py -k "primitive_convert or overflow_throw" -q`
- `python -m pytest tests/unit/compatibility/test_full_assembly_closure_codegen_contracts_runtime_templates.py -k "byte_forwarder or primitive_convert" -q`
- `python -m pytest tests/unit/compatibility/test_full_assembly_closure_codegen_contracts_string_templates_basic.py -k "primitive_convert or checked_byte" -q`
- `python build/toolchains/run/run.py test subject --id subject/SolutionCorePack --matrix windows-corelib-reference-native-hotupdate-proof --json`
- `.\run.ps1 test subject --id subject/SolutionCorePack --json`
  - `runId = 20260425-110513-windows-ab4b`
  - `finalStatus = ok`

## Risks / Blockers

### risks

- 如果 widening 继续把 Convert/object/string/provider/decimal/base64/hex lane 直接堆进 emitter 单点，仍会放大结构债务；必须维持 4C family 边界。

### blockers

- none
