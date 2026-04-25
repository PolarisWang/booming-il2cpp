---
task_id: 20260419-37-corelib-supplemental-runtime-skeleton-coverage-widening
title: CoreLib Supplemental Runtime Skeleton Coverage Widening
task_type: plan
lifecycle_status: in-progress
phase: implementation
created_at: 2026-04-24 21:20:00 +08:00
updated_at: 2026-04-25 12:24:10 +08:00
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
  - `requestedMethodCount = 59836`
  - `emittedMethodCount = 1823`
  - `uncoveredMethodCount = 52962`
- 最新 `System.Convert` 未覆盖数：
  - `262 -> 260`
- 最新 `System.Convert::ToChar(...)` 未覆盖数：
  - `11 -> 9`
- 本轮累计已真实消化的 `ToChar` lane：
  - `System.SByte`
  - `System.Int16`
  - `System.UInt16`
  - `System.UInt32`
  - `System.UInt64`
  - `System.Int32`
  - `System.Int64`
- 当前 `ToChar(...)` 剩余未覆盖：
  - `System.Boolean`
  - `System.DateTime`
  - `System.Decimal`
  - `System.Double`
  - `System.Object`
  - `System.Object,System.IFormatProvider`
  - `System.Single`
  - `System.String`
  - `System.String,System.IFormatProvider`
- 新的结构前提已具备：
  - `20260425-01-runtime-skeleton-4c-handler-framework-roadmap` 已 completed
  - `20260425-02-runtime-skeleton-4c-framework-foundation-cutover` 已 completed
  - 顶层 runtime-skeleton 已切到 4C family router，legacy dispatcher 不再是主路径

## Current Judgment

- 当前 widening 应继续建立在新的 `Convert family` / `ConvertLike family` / shared handler loop 基础上推进。
- 本轮把 `byte`/`char` 两条线都收敛到了共享 primitive return forwarder / checked primitive convert 组合，`Convert family` 内重复逻辑进一步下降。
- `ToChar(Single/Double)` 已确认不是简单 checked primitive 形态，而是 `box + ldnull + callvirt System.IConvertible::ToChar(System.IFormatProvider)`；后续若继续推进，应单独作为 boxed-IConvertible lane 设计，不应误塞回当前 primitive helper。

## Latest Summary

- 2026-04-25 12:24:10 +08:00: 已把 `System.Convert::ToChar(Int32/Int64)` forwarder 吃掉，并把原本只支持 `byte` 的 return forwarder 提升为共享 primitive return forwarder；新的 checked-char/forwarder fixture、contracts 与 formal `SolutionCorePack::windows-corelib-reference-native-hotupdate-proof` 全部通过，真实额外消化了 `ToChar(Int32/Int64)` 两个 CoreLib supplemental method。
- 2026-04-25 11:57:41 +08:00: 已在 `Convert family` 内把 `checked byte` builder 收敛为共享 checked primitive convert 核心，并新增 `checked char` lane；新的 RED fixture、contracts 与 formal `SolutionCorePack::windows-corelib-reference-native-hotupdate-proof` 全部通过，真实消化了 `ToChar(SByte/Int16/UInt16/UInt32/UInt64)` 五个 CoreLib supplemental method。
- 2026-04-25 11:10:52 +08:00: 独立 4C roadmap 已完成并归档，本 child 恢复为 in-progress，继续作为 foundation audit 主线的当前 active child。
- 2026-04-25 02:35:00 +08:00: 因用户要求先完成独立 4C roadmap，本 child 主动挂起。
- 2026-04-25 01:28:00 +08:00: widening 已覆盖 `Convert` primitive lane 的一批真实方法，未覆盖数降至 `267`。

## Next

- next_action: 从 `System.Convert::ToChar` 切到下一批最小可复用 lane。优先候选是 `ToChar(Single/Double)` 的 boxed `IConvertible` 双方法一组；如其 helper 抽象成本偏高，则转去下一批纯 primitive `Convert` family widening，避免把 object/string/provider lane 直接堆进 emitter 单点。
- owner: codex
- trigger: immediate

## Verification

- `python -m pytest tests/unit/compatibility/test_full_assembly_closure_codegen_contracts_string_templates_basic.py -q`
  - `9 passed in 0.13s`
- `python -m pytest tests/unit/compatibility/test_full_assembly_closure_codegen_contracts_runtime_templates.py -q`
  - `17 passed in 0.15s`
- `python -m pytest tests/unit/compatibility/test_full_assembly_closure_codegen_audit_plan.py -q`
  - `13 passed in 44.87s`
- `python build/toolchains/run/run.py test subject --id subject/SolutionCorePack --matrix windows-corelib-reference-native-hotupdate-proof --json`
  - `runId = 20260425-122043-windows-0253`
  - `finalStatus = ok`
  - `summaryPath = artifacts/subjects/SolutionCorePack/runs/20260425-122043-windows-0253/run-report/summary.json`
  - `eventsPath = artifacts/subjects/SolutionCorePack/runs/20260425-122043-windows-0253/run-report/events.jsonl`
  - `consolePath = artifacts/subjects/SolutionCorePack/runs/20260425-122043-windows-0253/run-report/console.log`

## Risks / Blockers

### risks

- 如果 widening 继续把 `Convert` object/string/provider/decimal/base64/hex lane 直接堆进 emitter 单点，仍会放大结构债务；必须维持 4C family 边界。
- 当前共享 forwarder/template 文件名仍沿用 `StaticByteForwarderStub` / `StaticCheckedByteConvertStub`，但内容已被 `byte/char` 共用；这在当前批次可接受，但若继续扩到更多输出类型，应考虑统一命名或再抽一层 catalog/template authority。

### blockers

- none
