---
task_id: 20260419-37-corelib-supplemental-runtime-skeleton-coverage-widening
title: CoreLib Supplemental Runtime Skeleton Coverage Widening
task_type: plan
lifecycle_status: in-progress
phase: implementation
created_at: 2026-04-24 21:20:00 +08:00
updated_at: 2026-04-25 20:03:42 +08:00
current_dir: docs/dev/in-progress/20260419-37-corelib-supplemental-runtime-skeleton-coverage-widening
parent_task_id: 20260419-01-foundation-dll-translation-audit-roadmap
source_task_id: 20260419-03-system-private-corelib-full-verification
source_relation: unblocker-for-corelib-full-verification
clearance_source: parent-roadmap
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
preflight_review: warn
preflight_summary: 当前 widening child 仍在父 roadmap 已批准边界内，可继续沿新的 4C `Convert family` 扩大 `System.Private.CoreLib` supplemental runtime-skeleton translated coverage；风险是 `ToString/object/provider/value-type` 族继续堆进单一 emitter 会放大结构债务，因此本轮继续坚持 family router 边界，不回流 legacy 入口。
auto_execution_decision: continue
design_doc: docs/dev/in-progress/20260419-01-foundation-dll-translation-audit-roadmap/design-v1-01.md
plan_doc: docs/dev/in-progress/20260419-37-corelib-supplemental-runtime-skeleton-coverage-widening/plan-v1-01.md
active: true
---

# 20260419-37 CoreLib Supplemental Runtime Skeleton Coverage Widening

## 当前状态

- 最新 supplemental runtime-skeleton coverage 基线：
  - `requestedMethodCount = 59819`
  - `emittedMethodCount = 1832`
  - `uncoveredMethodCount = 52936`
- 最新 `System.Convert` 未覆盖数：
  - `251`
- 最新 `System.Convert::ToChar(...)` 未覆盖数：
  - `0`
- 当前 `System.Convert` 最大剩余簇：
  - `ToString = 35`
  - `ToDateTime = 18`
  - `ToDecimal = 18`
  - `ToSByte = 18`
  - `ToInt16 = 17`
  - `ToInt32 = 17`
  - `ToUInt32 = 17`
- 已完成并真实消化的 `ToChar` lane：
  - checked primitive convert：`System.SByte`、`System.Int16`、`System.UInt16`、`System.UInt32`、`System.UInt64`
  - primitive return forwarder：`System.Int32`、`System.Int64`
  - boxed `IConvertible` invalid-cast：`System.Boolean`、`System.Single`、`System.Double`
  - string char provider：`System.String`、`System.String,System.IFormatProvider`
  - object char provider：`System.Object`、`System.Object,System.IFormatProvider`
  - boxed value-type char invalid-cast：`System.Decimal`、`System.DateTime`
- 结构前提：
  - `20260425-01-runtime-skeleton-4c-handler-framework-roadmap` 已完成
  - `20260425-02-runtime-skeleton-4c-framework-foundation-cutover` 已完成
  - 顶层 runtime-skeleton 已切到 4C family router，当前 widening 继续建立在新 family handler 之上

## 当前判断

- `System.Convert::ToChar` 已清零，本轮 child 可以从 `ToChar` 正式切到下一簇。
- 下一刀优先做 `System.Convert::ToString`，因为它当前在 `Convert` 家族里剩余最多，而且 string passthrough 与 value-type instance `ToString` 形态都具备较强复用价值。
- `System.Convert::ToString(System.Object,System.IFormatProvider)` 仍是复杂分支，优先级低于 string passthrough 和 value-type instance call；本轮先吃最小可复用子集。

## 最新摘要

- 2026-04-25 20:03:42 +08:00：已完成 `System.Convert::ToString` 纯 A 路径的 manifest authority 收口。`closure.manifest.json` 新增 `resolvedAssemblies`，full-closure direct DLL 现在会把解析后的 closure 程序集路径（含 `System.Private.CoreLib.dll`）落盘；`NativeReferenceProofEmitter` / `NativeAotLoweringPlanner` 已改为优先消费该 authority。期间修复 external metadata resolver 在并行 emit 下持有已释放 `MetadataReader` 导致的 `AccessViolationException`。定向 `convert_to_string_runtime_skeleton_methods`、相关 contract pytest、全量 audit pytest 与 formal `20260425-200102-windows-9f4c` 均已通过；最新 supplemental coverage 为 `requestedMethodCount = 59837`、`emittedMethodCount = 1858`、`uncoveredMethodCount = 52928`，`System.Convert::ToString` canonical residual 已收敛到 9 条，当前剩余集中在 `object(+provider)`、`radix` 系列（`Byte/Int16/Int32/Int64`）与少量 `bool/char` wrapper。
- 2026-04-25 16:20:56 +08:00：已补回本轮 `System.Convert::ToChar` 收口状态。最新 formal `20260425-153952-windows-ff29` 证明 `System.Convert::ToChar(...)` supplemental uncovered 已归零；`System.Convert` 总剩余降至 `251`，下一步切到 `System.Convert::ToString` widening。
- 2026-04-25 15:39:52 +08:00：补齐 `System.Convert::ToChar(string/object)` 与 boxed value-type / boxed `IConvertible` char invalid-cast 的最后几条 lane，并在 `TryResolveConcreteTypeCallTarget(...)` 中支持显式接口实现名后缀匹配，避免 `System.IConvertible.ToChar` 这类 canonical target 在 full-closure 下误判为缺失。
- 2026-04-25 13:13:19 +08:00：把 `System.Convert::ToChar(Boolean/Single/Double)` 的 boxed `IConvertible` invalid-cast lane 接进新的 `Convert family` 4C framework，并保持“有 canonical body 时严格做 target shape 校验；fixture/full-closure 缺 body 时仅对白名单 source type 走最小 fallback”的边界。
- 2026-04-25 12:24:10 +08:00：把 `System.Convert::ToChar(Int32/Int64)` 吃进 primitive return forwarder，并把原本偏 byte 命名的 forwarder 提升成 byte/char 共用 lane。
- 2026-04-25 11:57:41 +08:00：把 `checked byte` builder 收敛成共享 checked primitive convert 核心，并新增 `checked char` lane，真实消化 `ToChar(SByte/Int16/UInt16/UInt32/UInt64)`。

## 下一步

- next_action: 继续在 `System.Convert` 内推进 `ToString` residual 收口；优先补 `bool/char` wrapper 与 `object(+provider)` family，再处理 `radix` 系列 `Byte/Int16/Int32/Int64 -> ToString(value, base)`。
- owner: codex
- trigger: immediate

## 验证

- `python -m pytest tests/unit/compatibility/test_full_assembly_closure_codegen_audit_plan.py -k object_to_char_runtime_skeleton_methods -q`
  - `1 passed`
- `python -m pytest tests/unit/compatibility/test_full_assembly_closure_codegen_audit_plan.py -k decimal_and_datetime_to_char_runtime_skeleton_methods -q`
  - `1 passed`
- `python -m pytest tests/unit/compatibility/test_full_assembly_closure_codegen_contracts_runtime_templates.py -k "boxed_value_type_char_invalid_cast or object_char_provider" -q`
  - `2 passed`
- `python -m pytest tests/unit/compatibility/test_full_assembly_closure_codegen_contracts_string_templates_basic.py -q`
  - `9 passed in 0.10s`
- `python -m pytest tests/unit/compatibility/test_full_assembly_closure_codegen_contracts_runtime_templates.py -q`
  - `21 passed in 0.13s`
- `python -m pytest tests/unit/compatibility/test_full_assembly_closure_codegen_audit_plan.py -q`
  - `17 passed in 53.16s`
- `python build/toolchains/run/run.py test subject --id subject/SolutionCorePack --matrix windows-corelib-reference-native-hotupdate-proof --json`
  - `runId = 20260425-153952-windows-ff29`
  - `finalStatus = ok`
  - `summaryPath = artifacts/subjects/SolutionCorePack/runs/20260425-153952-windows-ff29/run-report/summary.json`
  - `eventsPath = artifacts/subjects/SolutionCorePack/runs/20260425-153952-windows-ff29/run-report/events.jsonl`
  - `consolePath = artifacts/subjects/SolutionCorePack/runs/20260425-153952-windows-ff29/run-report/console.log`
- `python -m pytest tests/unit/compatibility/test_full_assembly_closure_codegen_audit_plan.py -k convert_to_string_runtime_skeleton_methods -q`
  - `1 passed in 9.32s`
- `python -m pytest tests/unit/compatibility/test_full_assembly_closure_codegen_contracts_runtime_templates.py -k convert_to_string_helpers_have_templates -q`
  - `1 passed in 0.13s`
- `python -m pytest tests/unit/compatibility/test_full_assembly_closure_codegen_contracts_runtime_templates.py -q`
  - `22 passed in 0.17s`
- `python -m pytest tests/unit/compatibility/test_full_assembly_closure_codegen_contracts_string_templates_basic.py -q`
  - `9 passed in 0.15s`
- `python -m pytest tests/unit/compatibility/test_managed_closure_contract_bundle.py -q`
  - `6 passed in 10.17s`
- `python -m pytest tests/unit/compatibility/test_full_assembly_closure_codegen_audit_plan.py -q`
  - `18 passed in 83.77s`
- `python build/toolchains/run/run.py test subject --id subject/SolutionCorePack --matrix windows-corelib-reference-native-hotupdate-proof --json`
  - `runId = 20260425-200102-windows-9f4c`
  - `finalStatus = ok`
  - `summaryPath = artifacts/subjects/SolutionCorePack/runs/20260425-200102-windows-9f4c/run-report/summary.json`
  - `eventsPath = artifacts/subjects/SolutionCorePack/runs/20260425-200102-windows-9f4c/run-report/events.jsonl`
  - `consolePath = artifacts/subjects/SolutionCorePack/runs/20260425-200102-windows-9f4c/run-report/console.log`

## 风险 / 阻塞

### risks

- `System.Convert::ToString` 如果直接把 string/object/provider/value-type lane 混堆进 emitter 单点，会继续放大结构债务；本轮应优先抽出最小共享 helper，而不是为每个 primitive 单独堆 handler。
- 当前存在 `StaticByteForwarderStub`、`StaticCheckedByteConvertStub` 这类历史命名已经被 byte/char 共用的模板；继续向 string 家族扩面时，要避免再复制一套语义等价但命名分裂的模板层。
- `System.Convert::ToString(System.Object,System.IFormatProvider)` 仍涉及 `IConvertible`、`IFormattable`、`Object::ToString()` 与 `String::Empty` 分支；本轮 pure A blocker 已清掉，但 residual 收口仍应保持 family router 边界，避免把 object/radix 逻辑重新堆回单点 emitter。

### blockers

- none

## DLL-First Audit Authority Supplement

- recorded_at: `2026-04-25 20:03:42 +08:00`
- current_dll: `System.Private.CoreLib`
- canonical_run:
  - `artifacts/subjects/SolutionCorePack/runs/20260425-200102-windows-9f4c/run-report/summary.json`
  - `artifacts/subjects/SolutionCorePack/runs/20260425-200102-windows-9f4c/run-report/events.jsonl`
  - `artifacts/subjects/SolutionCorePack/runs/20260425-200102-windows-9f4c/run-report/console.log`
  - `artifacts/subjects/SolutionCorePack/runs/20260425-153952-windows-ff29/run-report/summary.json`
  - `artifacts/subjects/SolutionCorePack/runs/20260425-153952-windows-ff29/run-report/events.jsonl`
  - `artifacts/subjects/SolutionCorePack/runs/20260425-153952-windows-ff29/run-report/console.log`
- proof_artifacts:
  - `artifacts/subjects/SolutionCorePack/runs/20260425-200102-windows-9f4c/matrices/windows-corelib-reference-native-hotupdate-proof/pipeline-report/report/native-hotupdate-audit.json`
  - `artifacts/subjects/SolutionCorePack/runs/20260425-200102-windows-9f4c/matrices/windows-corelib-reference-native-hotupdate-proof/pipeline-report/report/codegen-summary.json`
  - `artifacts/subjects/SolutionCorePack/runs/20260425-200102-windows-9f4c/declared/unit/corelib-reference-hotupdate-proof/summary.json`
  - `artifacts/subjects/SolutionCorePack/runs/20260425-200102-windows-9f4c/declared/unit/corelib-reference-hotupdate-proof/review-bundle.json`
  - `artifacts/subjects/SolutionCorePack/runs/20260425-153952-windows-ff29/matrices/windows-corelib-reference-native-hotupdate-proof/pipeline-report/report/native-hotupdate-audit.json`
  - `artifacts/subjects/SolutionCorePack/runs/20260425-153952-windows-ff29/matrices/windows-corelib-reference-native-hotupdate-proof/pipeline-report/report/codegen-summary.json`
  - `artifacts/subjects/SolutionCorePack/runs/20260425-153952-windows-ff29/declared/unit/corelib-reference-hotupdate-proof/summary.json`
  - `artifacts/subjects/SolutionCorePack/runs/20260425-153952-windows-ff29/declared/unit/corelib-reference-hotupdate-proof/review-bundle.json`
- supplemental_coverage_artifacts:
  - `artifacts/subjects/SolutionCorePack/runs/20260425-200102-windows-9f4c/analysis/generated/supplemental-full-closures/system-private-corelib/native-reference/generated/runtime/native-reference.runtime-skeleton.coverage.json`
  - `artifacts/subjects/SolutionCorePack/runs/20260425-200102-windows-9f4c/analysis/generated/supplemental-full-closures/system-private-corelib/native-aot/native-aot.plan.json`
  - `artifacts/subjects/SolutionCorePack/runs/20260425-200102-windows-9f4c/analysis/generated/supplemental-full-closures/system-private-corelib/native-reference/generated/runtime/`
  - `artifacts/subjects/SolutionCorePack/runs/20260425-153952-windows-ff29/analysis/generated/supplemental-full-closures/system-private-corelib/native-reference/generated/runtime/native-reference.runtime-skeleton.coverage.json`
  - `artifacts/subjects/SolutionCorePack/runs/20260425-153952-windows-ff29/analysis/generated/supplemental-full-closures/system-private-corelib/native-aot/native-aot.plan.json`
  - `artifacts/subjects/SolutionCorePack/runs/20260425-153952-windows-ff29/analysis/generated/supplemental-full-closures/system-private-corelib/native-reference/generated/runtime/`
- audit_conclusion:
  - `native-proof`: passed-with-current-canonical-evidence
  - `hotupdate-proof`: passed-with-current-canonical-evidence
  - `completion-certification`: blocked-by-coverage-widening
