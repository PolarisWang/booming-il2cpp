---
task_id: 20260427-01-family-verification-kernel-hard-cut-roadmap
title: Family Verification Kernel Hard Cut Roadmap
task_type: roadmap
lifecycle_status: completed
phase: completed
created_at: 2026-04-27 00:35:00 +08:00
updated_at: 2026-04-27 01:18:00 +08:00
latest_stop_point: verification kernel hard cut completed; generator claim routing was aligned to claimId, targeted regressions passed again, and formal verification-v1 refresh 20260427-011722-windows-d71e regenerated family-verification and family-verification-claims outputs.
current_dir: docs/dev/completed/20260427-01-family-verification-kernel-hard-cut-roadmap
parent_task_id:
source_task_id: 20260426-01-capability-family-test-code-first-pipeline
source_relation: hard-cut-followup-after-test-code-first-roadmap
design_doc: docs/dev/completed/20260427-01-family-verification-kernel-hard-cut-roadmap/design-v1-01.md
roadmap_or_plan: docs/dev/completed/20260427-01-family-verification-kernel-hard-cut-roadmap/roadmap-v1-01.md
child_execution_mode: manual
auto_continue: false
auto_stop_policy: manual-review
recommended_next_child:
active: false
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
preflight_review: pass
preflight_summary: 用户要求的 hard cut 边界、全面验证、旧逻辑清理、多轮复查和 dashboard 数据要求均已落实，roadmap 已收口。
auto_execution_decision: continue
---

# 20260427-01 Family Verification Kernel Hard Cut Roadmap

## 当前判断

- 用户已明确选择 `方案 1`，要求执行 family verification kernel hard cut。
- 这次任务不是局部修补，而是正式重建 verification kernel，并在完成时删除旧逻辑。
- 当前没有执行前未决问题；hard cut 需求、范围和完成标准均已明确。

## 关键文档

- design: `docs/dev/completed/20260427-01-family-verification-kernel-hard-cut-roadmap/design-v1-01.md`
- roadmap: `docs/dev/completed/20260427-01-family-verification-kernel-hard-cut-roadmap/roadmap-v1-01.md`
- source_task_status: `docs/dev/completed/20260426-01-capability-family-test-code-first-pipeline/STATUS.md`

## 当前结论

- 必须引入正式的 `VerificationFact / VerificationClaim / VerificationEvaluator / VerificationSnapshot`
- 必须把 `testCode + nativeProof` 先切到新 kernel
- 后续 `managedProof / hotupdateProof / benchmark` 也必须进入同一套链路
- completed 前必须完成：
  - 全面验证
  - dashboard 有真实数据
  - 旧逻辑多轮清理确认
  - checklist 全勾选
  - 设计与实现一致性 review

## 完成证据

- arch_review: ok
- authority_review: ok
- verification:
  - canonicalCommand: `python build/toolchains/run/run.py verify verification-v1 --json`
  - formalObject: `verification-v1`
  - summaryPath: `artifacts/logs/run/20260427-011722-windows-d71e/summary.json`
  - eventsPath: `artifacts/logs/run/20260427-011722-windows-d71e/events.jsonl`
  - consolePath: `artifacts/logs/run/20260427-011722-windows-d71e/console.log`
- verification_data:
  - refreshCommand: `python build/toolchains/run/run.py verify verification-v1 --json`
  - verificationOutputRoot: `verification/`
  - latestResultPath: `verification/archive/latest/result-snapshot.json`
  - masterResultPath: `verification/archive/master/result-master.json`
  - reportSummaryPath: `verification/archive/reports/completed/foundation-dll-audit/summary.md`
  - codegenStubPath: `verification/evidence/owners/SolutionCorePack/codegen-stubs/`
- test_result: passed
- tests:
  - `python -m pytest tests/verification/unit/projections/foundation_dll/test_family_verification_claims.py tests/verification/unit/projections/foundation_dll/test_family_verification_kernel.py tests/verification/unit/projections/foundation_dll/test_foundation_dll_audit_generator.py tests/integration/registry/test_declared_metadata_discovery_family_projects.py tests/unit/compatibility/test_declared_metadata_discovery_contract.py tests/unit/compatibility/test_chaos_test_framework_contracts.py tests/tooling/run/test_foundation_dll_command.py tests/tooling/run/test_foundation_dll_manifest.py tests/integration/run/test_command_manifest.py -q`
  - `dotnet test tests/families/System.Private.CoreLib/convert-char/ConvertCharTests.csproj -c Release -v minimal`
  - `dotnet test tests/families/System.Private.CoreLib/buffer-memory/BufferMemoryTests.csproj -c Release -v minimal`
  - `dotnet test tests/families/System.Private.CoreLib/enum-parsing/EnumParsingTests.csproj -c Release -v minimal`
  - `python build/toolchains/run/run.py foundation-dll analyze-gaps --scope System.Private.CoreLib --update-ledger true --json`
- dashboard:
  - `docs/verification/foundation-dll-audit/dlls/System.Private.CoreLib.html`
  - `verification/projections/foundation-dll-audit/dlls/System.Private.CoreLib.json`
  - exact native proof values confirmed:
    - `Convert.Char Conversions = 18/18`
    - `Buffer.Memory Operations = 0/10`
    - `Enum Parsing And Formatting = 0/12`
- cleanup:
  - generator-specific native proof family mapping removed
  - native proof claim authority moved to `family_verification_claims.py`
  - snapshot rendering only consumes kernel outputs
- wiki: n/a
- next: n/a

## 风险 / 阻塞

### risks

- 这是高风险 hard cut，不能在没有阶段性 review 的情况下自动推进
- 旧 generator 已承载多层逻辑，删除顺序必须受 checklist 约束

### blockers

- none
