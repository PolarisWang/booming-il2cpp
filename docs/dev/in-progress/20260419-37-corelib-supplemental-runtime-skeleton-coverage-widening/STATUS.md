---
task_id: 20260419-37-corelib-supplemental-runtime-skeleton-coverage-widening
title: CoreLib Supplemental Runtime Skeleton Coverage Widening
task_type: plan
lifecycle_status: in-progress
phase: implementation
created_at: 2026-04-24 21:20:00 +08:00
updated_at: 2026-04-25 11:57:41 +08:00
current_dir: docs/dev/in-progress/20260419-37-corelib-supplemental-runtime-skeleton-coverage-widening
parent_task_id: 20260419-01-foundation-dll-translation-audit-roadmap
source_task_id: 20260419-03-system-private-corelib-full-verification
source_relation: unblocker-for-corelib-full-verification
clearance_source: parent-roadmap
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
preflight_review: warn
preflight_summary: widening child 已获得新的 4C framework 前置条件；当前可继续沿 `System.Convert` 的 supplemental CoreLib lane 扩面，但必须保持新 family/router 边界，不回退到 legacy 入口。
auto_execution_decision: continue
design_doc: docs/dev/in-progress/20260419-01-foundation-dll-translation-audit-roadmap/design-v1-01.md
plan_doc: docs/dev/in-progress/20260419-37-corelib-supplemental-runtime-skeleton-coverage-widening/plan-v1-01.md
active: true
---

# 20260419-37 CoreLib Supplemental Runtime Skeleton Coverage Widening

## Current State

- 最新 supplemental runtime-skeleton coverage 基线：
  - `requestedMethodCount = 59829`
  - `emittedMethodCount = 1814`
  - `uncoveredMethodCount = 52975`
- 最新 `System.Convert` 未覆盖数：
  - `268 -> 262`
- 最新 `System.Convert::ToChar(...)` 未覆盖数：
  - `16 -> 11`
- 本轮已真实消化的 `ToChar` lane：
  - `System.SByte`
  - `System.Int16`
  - `System.UInt16`
  - `System.UInt32`
  - `System.UInt64`
- 新的结构前提已具备：
  - `20260425-01-runtime-skeleton-4c-handler-framework-roadmap` 已 completed
  - `20260425-02-runtime-skeleton-4c-framework-foundation-cutover` 已 completed
  - 顶层 runtime-skeleton 已切到 4C family router，legacy dispatcher 不再是主路径

## Current Judgment

- 当前 widening 应继续建立在新的 `Convert family` / `ConvertLike family` / shared handler loop 基础上推进。
- 本轮通过共享 checked primitive convert builder 消化 `checked char`，没有回退到 legacy dispatcher，也没有引入新的 subject / proof authority。
- 下一批最自然的续点仍然是 `ToChar(Int32/Int64)` forwarder 形态，其调用目标 `UInt32/UInt64 -> Char` 已经在本轮打通。

## Latest Summary

- 2026-04-25 11:57:41 +08:00: 已在 `Convert family` 内把 `checked byte` builder 收敛为共享 checked primitive convert 核心，并新增 `checked char` lane；新的 RED fixture、contracts 与 formal `SolutionCorePack::windows-corelib-reference-native-hotupdate-proof` 全部通过，真实消化了 `ToChar(SByte/Int16/UInt16/UInt32/UInt64)` 五个 CoreLib supplemental method。
- 2026-04-25 11:10:52 +08:00: 独立 4C roadmap 已完成并归档，本 child 恢复为 in-progress，继续作为 foundation audit 主线的当前 active child。
- 2026-04-25 02:35:00 +08:00: 因用户要求先完成独立 4C roadmap，本 child 主动挂起。
- 2026-04-25 01:28:00 +08:00: widening 已覆盖 `Convert` primitive lane 的一批真实方法，未覆盖数降至 `267`。

## Next

- next_action: 继续沿 `System.Convert::ToChar(...)` 收口剩余 primitive lane，优先处理 `Int32/Int64 -> UInt32/UInt64 -> Char` forwarder 形态，再评估是否顺手吸收 `Single/Double -> Char`，或切到下一批 `Convert` primitive family。
- owner: codex
- trigger: immediate

## Verification

- `python -m pytest tests/unit/compatibility/test_full_assembly_closure_codegen_contracts_string_templates_basic.py -q`
  - `9 passed in 0.12s`
- `python -m pytest tests/unit/compatibility/test_full_assembly_closure_codegen_contracts_runtime_templates.py -q`
  - `17 passed in 0.15s`
- `python -m pytest tests/unit/compatibility/test_full_assembly_closure_codegen_audit_plan.py -q`
  - `13 passed in 43.41s`
- `python build/toolchains/run/run.py test subject --id subject/SolutionCorePack --matrix windows-corelib-reference-native-hotupdate-proof --json`
  - `runId = 20260425-115500-windows-2b2f`
  - `finalStatus = ok`
  - `summaryPath = artifacts/subjects/SolutionCorePack/runs/20260425-115500-windows-2b2f/run-report/summary.json`
  - `eventsPath = artifacts/subjects/SolutionCorePack/runs/20260425-115500-windows-2b2f/run-report/events.jsonl`
  - `consolePath = artifacts/subjects/SolutionCorePack/runs/20260425-115500-windows-2b2f/run-report/console.log`

## Risks / Blockers

### risks

- 如果 widening 继续把 `Convert` object/string/provider/decimal/base64/hex lane 直接堆进 emitter 单点，仍会放大结构债务；必须维持 4C family 边界。
- 当前共享 checked convert 模板文件名仍沿用 `StaticCheckedByteConvertStub`，但内容已被 `byte/char` 共用；这在当前批次可接受，但若继续扩到更多输出类型，应考虑统一命名或再抽一层 catalog/template authority。

### blockers

- none
