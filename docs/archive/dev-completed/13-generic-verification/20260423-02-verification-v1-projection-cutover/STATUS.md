---
task_id: 20260423-02-verification-v1-projection-cutover
title: Verification V1 Projection Cutover
task_type: task
lifecycle_status: completed
phase: completed
created_at: 2026-04-23 12:25:58 +08:00
updated_at: 2026-04-23 14:12:40 +08:00
latest_stop_point: Phase 4 已按 roadmap 口径完成 closeout；inventory、benchmark 和首页固定视图已全部切到 formal source / projection generator，真实命令链重验通过
current_dir: docs/dev/completed/20260423-02-verification-v1-projection-cutover
parent_task_id: 20260423-01-verification-v1-roadmap
source_task_id: verification-v1-projection-cutover
source_relation: roadmap-child
roadmap_or_plan: docs/archive/architecture/verification-v1/roadmap-v1-01.md
clearance_source: parent-roadmap
active: false
preflight_review: warn
preflight_summary: 当前 child 始终处于 projection cutover 已批准边界内；虽然 `inventory_generator.py` 仍聚合 header metadata、导出与 HTML render，但未出现新的 authority 漂移，也不需要回到 design 或改 roadmap
auto_execution_decision: continue
---

# 20260423-02 Verification V1 Projection Cutover

## 关键文档

- `docs/archive/architecture/verification-v1/roadmap-v1-01.md`
- `docs/archive/architecture/verification-v1/projection-and-reporting.md`
- `docs/archive/architecture/verification-v1/cutover-and-rollout.md`
- `build/toolchains/run/testing/inventory_source.py`
- `build/toolchains/run/testing/inventory_generator.py`
- `build/toolchains/run/testing/benchmark_dashboard_generator.py`
- `build/toolchains/run/testing/verification_bundle.py`
- `build/toolchains/run/testing/verification_projection.py`

## 当前结论

- `docs/testing-inventory/*` 现在只从 `verification/latest|master|reports` 与 projection generator 派生，不再把旧 `docs/benchmark/*` 当 authority 输入。
- `docs/benchmark/*` 现在优先从 formal source 生成；当 formal source 存在时，`benchmark_dashboard_generator.py` 直接消费 `docs/testing-inventory/verification/master/*`。
- 首页固定视图已经落在 projection 层，而不是 HTML 内硬编码；顺序固定为：
  - `Closure Strip`
  - `Responsibility View`
  - `Route Topology View`
  - `Platform-Device Matrix`
- Phase 4 closeout sweep 已确认：`docs/benchmark/overview.json` / `docs/benchmark/subjects/*` 的 active consumer 已清零，残余匹配仅存在于历史 `docs/dev` 文档。

## 问题清零

- blocking_questions: []
- question_clearance: cleared
- clearance_confirmed_by_user: true

## AOT/IL2CPP/Test Governance Intake

- capabilityFamily: `verification-v1`
- capabilityItem: `projection-cutover`
- ownerSubjectId: `SolutionCorePack`
- proofRequired: `false`
- benchmarkRequired: `false`
- hotupdateImpact: `None`
- formalVerificationObjects:
  - `docs/testing-inventory/verification/latest`
  - `docs/testing-inventory/verification/master`
  - `docs/testing-inventory/verification/reports`
  - `docs/testing-inventory/*.json|*.csv|inventory.html`
  - `docs/benchmark/*`
- requiredGates:
  - `collector`
  - `registry`
  - `workspace`

## 架构审视

- arch_review_mode: `critical`
- 结论: `ok`
- duplicate_code:
  - projection authority 已集中到 `verification_bundle.py`、`verification_projection.py`、`inventory_generator.py` 与 `benchmark_dashboard_generator.py`
  - inventory / benchmark / fixed views 没有再各自维护第二套状态口径
- structure:
  - `verification_projection.py` 负责 formal source 到 fixed views / detail rows 的投影
  - `inventory_generator.py` 负责列元数据、JSON/CSV/HTML 输出与本地静态页面渲染
  - `benchmark_dashboard_generator.py` 负责 benchmark projection 文档输出；formal source 存在时不再回读旧 docs authority
- warn:
  - `inventory_generator.py` 仍同时承载 hint 元数据、导出与 render 逻辑，但仍处于同一 generator 层职责内，不构成 blocker

## 权责图审核

- authority:
  - `docs/testing-inventory/verification/` 与 `subjects/*/verification/codegen-stubs/` 是 formal source
  - `docs/testing-inventory/*` 与 `docs/benchmark/*` 仅作为 projection/output
- upstream:
  - `verification_bundle.py`
  - `verification_projection.py`
  - `inventory_source.py`
  - `subjects/*/benchmark-records/records.jsonl`
- downstream:
  - `docs/testing-inventory/*.json`
  - `docs/testing-inventory/*.csv`
  - `docs/testing-inventory/inventory.html`
  - `docs/benchmark/dashboard.html`
  - `docs/benchmark/overview.json`
  - `docs/benchmark/subjects/*.json`
- allowed_deps:
  - `inventory_generator.py -> verification bundle/projection + inventory_source.py`
  - `benchmark_dashboard_generator.py -> formal source or raw benchmark records`
  - `run test inventory` / `run benchmark --dashboard` 仅作为命令入口
- shim_debt:
  - `benchmark_dashboard_generator.py` 仍保留“formal source 缺失时退回 raw benchmark records”的 fallback；这属于 cutover 后兼容读取，不影响 docs cutover 已完成的事实
- proof:
  - projection/unit/CLI/real command 验证全部通过

## 完成证据

- arch_review: ok
- authority_review: ok
- verification:
  - canonicalCommand: `python -m pytest tests/unit/verification/test_verification_formal_contracts.py tests/unit/verification/test_verification_bundle.py tests/unit/verification/test_verification_inventory_cutover.py tests/unit/performance/test_testing_inventory_source.py tests/unit/performance/test_testing_inventory_generator.py tests/unit/performance/test_benchmark_dashboard_generator_overview.py tests/unit/performance/test_benchmark_dashboard_generator_contracts.py tests/unit/performance/test_benchmark_dashboard_generator_modes.py tests/unit/performance/test_benchmark_dashboard_generator_legacy.py tests/tooling/run/test_subject_command_inventory.py tests/tooling/run/test_command_manifest_cli.py -q`
  - formalObject: [`docs/testing-inventory/verification/latest`, `docs/testing-inventory/verification/master`, `docs/testing-inventory/verification/reports`, `docs/testing-inventory/inventory.html`, `docs/benchmark/dashboard.html`]
  - summaryPath: `docs/testing-inventory/verification/reports/completed/testing-inventory/summary.md`
  - eventsPath: `n/a`
  - consolePath: `n/a`
- test_result: passed
- wiki: n/a
- next: follow-up

## 验证命令

- `python -m pytest tests/unit/verification/test_verification_formal_contracts.py tests/unit/verification/test_verification_bundle.py tests/unit/verification/test_verification_inventory_cutover.py tests/unit/performance/test_testing_inventory_source.py tests/unit/performance/test_testing_inventory_generator.py tests/unit/performance/test_benchmark_dashboard_generator_overview.py tests/unit/performance/test_benchmark_dashboard_generator_contracts.py tests/unit/performance/test_benchmark_dashboard_generator_modes.py tests/unit/performance/test_benchmark_dashboard_generator_legacy.py tests/tooling/run/test_subject_command_inventory.py tests/tooling/run/test_command_manifest_cli.py -q`
  - 结果: `43 passed`
- `python build/toolchains/run/run.py test inventory --json`
  - 结果: `ok`
  - outputRoot: `docs/testing-inventory`
  - formal bundle: `closure=1`, `capability=68`, `evidenceClaim=490`, `stage=7`, `result=68`
  - projected tables: `capability=68`, `unitTest=444`, `benchmark=28`
- `python build/toolchains/run/run.py benchmark --dashboard`
  - 结果: `ok`
- `git grep -n --full-name -- "docs/benchmark/overview.json" -- build tests wiki wiki/03-功能模块/06-il2cpp核心架构/05-架构参考 docs/dev`
  - 结果: active code 无 consumer；匹配仅剩历史 `docs/dev/completed|hanging` 文档
- `git grep -n --full-name -- "docs/benchmark/subjects" -- build tests wiki wiki/03-功能模块/06-il2cpp核心架构/05-架构参考 docs/dev`
  - 结果: active code 无 consumer；匹配仅剩历史 `docs/dev/completed|hanging` 文档

