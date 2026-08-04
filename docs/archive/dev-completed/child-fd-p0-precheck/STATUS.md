---
task_id: fd-p0-precheck
task_name: Pipeline 预检 — 修复已知阻塞问题
task_type: plan
phase: executing
lifecycle_status: completed
parent_task_id: foundation-dll-industrialization
source_task_id: foundation-dll-industrialization
source_relation: roadmap-child
clearance_source: parent-roadmap
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
entry_skill: dev-writing-plans
preflight_review: pass
preflight_summary: scope clear, exit criteria defined, no blocking questions, dependencies none
auto_execution_decision: continue
---

# Phase 0: Pipeline 预检

## 最近摘要

- ✅ 步骤 1 完成：排查并修复 microbench crash（BgcTryMark 添加 SEH 保护）
- ✅ 步骤 2 完成：修复 audit 计数口径不一致（mechanism_audit.py 改用 principle_overall != "VIOLATION"，orchestrator 新增显式日志）
- ✅ 步骤 3 完成：补齐 Asn1/encoding-rules subject IDs（5 个 AsnEncodingRules 方法）
- ✅ 步骤 4 完成：补齐 Text.Json/attributes subject IDs（23 个 JSON 属性方法）
- ✅ 步骤 5 完成：修复 pipeline-config.yaml 与实际运行脱节（更新为 13-stage 全量管线，标记 migrationSkips 为空）
- ✅ 步骤 6 完成：端到端验证（Brotli/decoder + Asn1/tag-model 全 13-stage 通过除 codegen 外的全部阶段）
- ✅ 步骤 7 完成：记录输出产物（pipeline-fix-log.md、KNOWN_ISSUES.md、verification-tracker.json）

## 当前步骤

步骤 8/8：子任务收尾

## 跟踪

- [x] 步骤 1：排查并修复 microbench crash
- [x] 步骤 2：修复 audit 计数口径不一致
- [x] 步骤 3：补齐 Asn1/encoding-rules subject IDs
- [x] 步骤 4：补齐 Text.Json/attributes subject IDs
- [x] 步骤 5：修复 pipeline-config.yaml 与实际运行脱节
- [x] 步骤 6：端到端验证
- [x] 步骤 7：记录输出产物
- [x] 步骤 8：子任务收尾

## 下一步

Phase 0 预检完成，可进入 Phase 1：基线扫描

## 完成证据

- arch_review: ok
- authority_review: n/a
- verification:
  - canonicalCommand: verify_one_family.py System.IO.Compression.Brotli decoder
  - formalObject: n/a (precheck — no formal object added)
  - summaryPath: unified-verification-report.json (Brotli/decoder)
  - eventsPath: n/a
  - consolePath: n/a
- verification_data:
  - refreshCommand: n/a
  - verificationOutputRoot: n/a
  - latestResultPath: n/a
  - masterResultPath: n/a
  - reportSummaryPath: n/a
  - codegenStubPath: n/a
- test_result: preflight+fact+audit+microbench+hotupdate passed; codegen build: known issue (KI-001)
- wiki: n/a
- next: sync parent roadmap, archive child
