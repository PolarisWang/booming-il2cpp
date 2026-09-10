---
task_id: 20260415-02-phase-1-subject-source-reshape-and-test-metadata-cutover
title: Phase 1 Subject Source Reshape And Test Metadata Cutover
task_type: plan
lifecycle_status: completed
phase: completed
created_at: 2026-04-15 00:32:10 +08:00
updated_at: 2026-04-15 01:52:44 +08:00
current_dir: docs/dev/completed/20260415-02-phase-1-subject-source-reshape-and-test-metadata-cutover
parent_task_id: 20260414-37-hybridclr-aligned-aot-hotupdate-development-plan
source_task_id: 20260414-37-hybridclr-aligned-aot-hotupdate-development-plan
source_relation: roadmap-child
active: false
---

## 关键文档

- parent_status: `docs/dev/in-progress/20260414-37-hybridclr-aligned-aot-hotupdate-development-plan/STATUS.md`
- parent_roadmap: `docs/dev/in-progress/20260414-37-hybridclr-aligned-aot-hotupdate-development-plan/roadmap-v1-01.md`
- parent_review: `docs/dev/in-progress/20260414-37-hybridclr-aligned-aot-hotupdate-development-plan/review-v1-01.md`
- phase_0_status: `docs/dev/completed/20260415-01-phase-0-contract-freeze-and-aot-translation-ledgers/STATUS.md`
- phase_0_taxonomy: `docs/dev/completed/20260415-01-phase-0-contract-freeze-and-aot-translation-ledgers/capability-taxonomy-freeze-v1-01.md`
- phase_0_identity: `docs/dev/completed/20260415-01-phase-0-contract-freeze-and-aot-translation-ledgers/stable-identity-and-body-availability-v1-01.md`
- plan: `docs/dev/completed/20260415-02-phase-1-subject-source-reshape-and-test-metadata-cutover/plan-v1-01.md`

## 当前结论

- `subjects/` 的 proof / benchmark metadata 路由已经从历史 `source_entry` 字符串面继续收口到 `entrySelection` / `entryIndex` 与声明式 metadata。
- launcher / host / declaration discovery / command / planner / registry / dashboard / perf harness 已完成一轮贯通 cutover，不再继续扩大 `ChaosSourceEntry` 兼容协议面。
- registry 的 subject 级摘要字段已经明确退回 display-only 语义，避免展示字段再次被误用为真实路由主键。
- benchmark runtime 执行链路已经补上 `entryIndex -> stableId -> declared benchmark metadata` 解析，perf harness 优先走结构化参数，不再依赖 `workloadEntry` 做真实定位。

## 最近摘要

- 2026-04-15 00:32:10 +08:00: 创建 Phase 1 child task，开始收口 subject source reshape 与 metadata cutover。
- 2026-04-15 00:53:52 +08:00: 完成第一批 cutover，移除 launcher / worker 的 `--chaos-source-entry` 兼容路径，并把 declaration discovery 切到 schema 表驱动。
- 2026-04-15 01:17:10 +08:00: 完成第二批 cutover，收口 `test.py`、`benchmark.py`、`subject_planner.py`、`registry.py` 与 `benchmark_dashboard_generator.py` 中的 declared object string routing。
- 2026-04-15 01:29:51 +08:00: 完成 registry display-only contract rename，把 subject 级默认摘要字段统一改成 `display*` 命名。
- 2026-04-15 01:52:44 +08:00: 完成 Task 8，runtime benchmark harness 与 declared matrix 选择逻辑改为优先消费 compact identity 与结构化 benchmark metadata；Phase 1 达到退出标准。

## 下一步

- next_action: 返回父 roadmap，创建并执行 `Phase 2: Typed-IL Shared Contract And Hybrid Dispatch Foundation` child task。
- owner: codex
- trigger: Phase 1 已完成，无额外阻塞。

## 风险 / 阻塞

### risks

- Phase 1 已把 subject/test metadata 的字符串路由面压到最小，但 Phase 2 之前系统内部仍缺统一的 typed-il / body availability / hybrid dispatch 真正共享契约。
- benchmark / dashboard 仍保留少量 display / evidence string 字段用于展示与追踪，这些字段后续不得重新承担执行决策职责。

### blockers

- 当前无外部 blocker。

## 验证

1. `python -m pytest tests/unit/registry/test_declared_registry_matrix_selection.py tests/unit/performance/test_subject_workers_perf.py -k "declared_registry_matrix_selection or declared_benchmark_metadata or preserves_entry_index"`
   - 结果：`4 passed`
2. `dotnet build src/validation/perf/Benchmark.WorkloadEntry.PerfHarness/Benchmark.WorkloadEntry.PerfHarness.csproj -c Release -m:1`
   - 结果：`build succeeded`
3. `python -m pytest tests/unit/execution/test_subject_workers.py tests/unit/performance/test_subject_workers_perf.py tests/unit/registry/test_declared_registry_matrix_selection.py tests/integration/registry/test_registry_scan.py -k "declared_benchmark_by_entry_index_for_structured_harness_execution or runtime_perf or interpreter_runtime_perf or declared_registry_matrix_selection or projects_engineering_and_declared_catalog_object_families"`
   - 结果：`13 passed`
4. `python -m pytest tests/tooling/run/test_subject_command.py tests/unit/performance/test_benchmark_command.py tests/unit/planning/test_subject_planner.py tests/unit/performance/test_benchmark_dashboard_generator.py tests/integration/registry/test_declared_metadata_discovery.py tests/integration/registry/test_registry_scan.py tests/unit/execution/test_subject_workers.py tests/unit/performance/test_subject_workers_perf.py tests/unit/registry/test_declared_registry_matrix_selection.py`
   - 结果：`110 passed`

## wiki

- 本子任务当前只在 `docs/dev` 记录执行细节；等 Phase 2 的 shared contract 稳定后，再评估是否同步到长期 architecture 文档。
