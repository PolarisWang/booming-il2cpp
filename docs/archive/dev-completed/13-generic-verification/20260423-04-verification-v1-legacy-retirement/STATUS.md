---
task_id: 20260423-04-verification-v1-legacy-retirement
title: Verification V1 Legacy Retirement
task_type: task
lifecycle_status: completed
phase: completed
created_at: 2026-04-23 14:12:40 +08:00
updated_at: 2026-04-23 14:12:40 +08:00
latest_stop_point: Phase 6 已完成；旧 benchmark docs authority consumer 已清零，旧测试流程入口已退役为 legacy，run-id 只保留在非 formal 的临时 artifacts 链路
current_dir: docs/dev/completed/20260423-04-verification-v1-legacy-retirement
parent_task_id: 20260423-01-verification-v1-roadmap
source_task_id: verification-v1-legacy-retirement
source_relation: roadmap-child
roadmap_or_plan: wiki/03-功能模块/06-il2cpp核心架构/05-架构参考/verification-v1/roadmap-v1-01.md
clearance_source: parent-roadmap
active: false
preflight_review: warn
preflight_summary: 旧 run-id 运行态报告机制仍存在于 `artifacts/` 临时链路，但它不再属于 verification-v1 的 formal source；本 child 的 approved 边界是 retire old formal authority 和旧主入口，因此继续完成而非回退 design
auto_execution_decision: continue
---

# 20260423-04 Verification V1 Legacy Retirement

## 关键文档

- `wiki/03-功能模块/06-il2cpp核心架构/05-架构参考/verification-v1/cutover-and-rollout.md`
- `wiki/03-功能模块/06-il2cpp核心架构/05-架构参考/verification-v1/spec.md`
- `build/toolchains/run/testing/inventory_source.py`
- `build/toolchains/run/testing/inventory_generator.py`
- `build/toolchains/run/testing/benchmark_dashboard_generator.py`
- `tests/unit/performance/test_benchmark_dashboard_generator_legacy.py`
- `tests/tooling/run/test_subject_command_inventory.py`
- `wiki/06-测试验证/INDEX.md`
- `wiki/06-测试验证/测试管线/INDEX.md`

## 当前结论

- active code 已不再把 `docs/benchmark/overview.json` 或 `docs/benchmark/subjects/*` 作为 authority 输入；这些文件只保留 projection/output 角色。
- `run test inventory` 已固定 materialize `docs/testing-inventory/verification/latest|master|reports`，并从 formal source 投影回 `docs/testing-inventory/*.json|*.csv|inventory.html`。
- `benchmark --dashboard` 在 formal source 存在时直接消费 `docs/testing-inventory/verification/master/*`；旧 benchmark docs 不再回流为 authority。
- `wiki/06-测试验证/测试管线/INDEX.md` 已退役为 legacy 入口，仅保留历史 pipeline 对象导航。
- `runId` 仍存在于 `artifacts/subjects/*/runs/<run-id>/...` 与运行态日志链路，但这部分只是临时过程产物，不属于 verification-v1 formal source，也不再作为 formal 归档路径。

## 问题清零

- blocking_questions: []
- question_clearance: cleared
- clearance_confirmed_by_user: true

## AOT/IL2CPP/Test Governance Intake

- capabilityFamily: `verification-v1`
- capabilityItem: `legacy-retirement`
- ownerSubjectId: `SolutionCorePack`
- proofRequired: `false`
- benchmarkRequired: `false`
- hotupdateImpact: `None`
- formalVerificationObjects:
  - `docs/testing-inventory/verification/latest`
  - `docs/testing-inventory/verification/master`
  - `docs/testing-inventory/verification/reports`
  - `docs/benchmark/*`
- requiredGates:
  - `collector`
  - `registry`
  - `workspace`

## 架构审视

- arch_review_mode: `critical`
- 结论: `ok`
- duplicate_code:
  - inventory/benchmark 不再同时维护“formal source 口径”和“旧 docs authority 口径”两套链路
- structure:
  - 旧 run-id 机制留在 `artifacts/` 临时证据层
  - 新 formal source 固定在 `docs/testing-inventory/verification/*` 与 `subjects/*/verification/codegen-stubs/*`
- warn:
  - legacy pipeline 对象页仍保留在 wiki 中作为历史对象说明；但其主入口已退役，不再构成 authority 分流

## 权责图审核

- authority:
  - `docs/testing-inventory/verification/*` 是 formal source
  - `docs/benchmark/*` 与 legacy pipeline wiki 只保留输出/历史说明角色
- upstream:
  - `inventory_source.py`
  - `verification_bundle.py`
  - `verification_projection.py`
  - `benchmark_dashboard_generator.py`
- downstream:
  - `docs/testing-inventory/*`
  - `docs/benchmark/*`
  - `wiki/06-测试验证/测试管线/INDEX.md`
- shim_debt:
  - `artifacts/` 的 run-id 临时报告链路继续服务运行调试，但不再是 verification-v1 formal archive；已降为非 blocker
- proof:
  - legacy authority consumer 清零扫描通过
  - benchmark/inventory legacy-regression tests 通过

## 完成证据

- arch_review: ok
- authority_review: ok
- verification:
  - canonicalCommand: `python -m pytest tests/unit/verification/test_verification_inventory_cutover.py tests/unit/performance/test_testing_inventory_source.py tests/unit/performance/test_testing_inventory_generator.py tests/unit/performance/test_benchmark_dashboard_generator_legacy.py tests/tooling/run/test_subject_command_inventory.py tests/tooling/run/test_command_manifest_cli.py -q`
  - formalObject: [`docs/testing-inventory/verification/latest`, `docs/testing-inventory/verification/master`, `docs/testing-inventory/verification/reports`, `docs/benchmark/dashboard.html`]
  - summaryPath: `docs/testing-inventory/verification/reports/completed/testing-inventory/summary.md`
  - eventsPath: `n/a`
  - consolePath: `n/a`
- test_result: passed
- wiki: updated:`wiki/06-测试验证/INDEX.md`
- next: follow-up

## 验证命令

- `python -m pytest tests/unit/verification/test_verification_inventory_cutover.py tests/unit/performance/test_testing_inventory_source.py tests/unit/performance/test_testing_inventory_generator.py tests/unit/performance/test_benchmark_dashboard_generator_legacy.py tests/tooling/run/test_subject_command_inventory.py tests/tooling/run/test_command_manifest_cli.py -q`
  - 结果: 已包含在本轮 `43 passed` 总回归内
- `git grep -n --full-name -- "docs/benchmark/overview.json" -- build tests wiki wiki/03-功能模块/06-il2cpp核心架构/05-架构参考 docs/dev`
  - 结果: active code 无 consumer；匹配仅剩历史 `docs/dev/completed|hanging` 文档
- `git grep -n --full-name -- "docs/benchmark/subjects" -- build tests wiki wiki/03-功能模块/06-il2cpp核心架构/05-架构参考 docs/dev`
  - 结果: active code 无 consumer；匹配仅剩历史 `docs/dev/completed|hanging` 文档

