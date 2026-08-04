---
task_id: 20260422-01-test-and-benchmark-inventory-redesign
title: Test And Benchmark Inventory Redesign
task_type: plan
lifecycle_status: completed
phase: completed
created_at: 2026-04-22 14:54:00 +08:00
updated_at: 2026-04-22 20:09:47 +08:00
latest_stop_point: inventory source/generator/CLI/manifest closed the loop; completed docs were canonicalized under docs/dev/completed and the old in-progress directory was removed
current_dir: docs/dev/completed/20260422-01-test-and-benchmark-inventory-redesign
parent_task_id:
source_task_id:
source_relation:
design_doc: docs/dev/completed/20260422-01-test-and-benchmark-inventory-redesign/design-v1-01.md
plan_doc: docs/dev/completed/20260422-01-test-and-benchmark-inventory-redesign/plan-v1-01.md
active: false
preflight_review: pass
preflight_summary: approved scope remains within test inventory authority, export chain, and CLI entry; no new boundary or authority drift found
auto_execution_decision: continue
---

## 关键文档

- `docs/dev/completed/20260422-01-test-and-benchmark-inventory-redesign/design-v1-01.md`
- `docs/dev/completed/20260422-01-test-and-benchmark-inventory-redesign/plan-v1-01.md`
- `build/toolchains/run/testing/declarations.py`
- `build/toolchains/run/testing/compiled_catalog.py`
- `build/toolchains/run/testing/capability_coverage.py`
- `build/toolchains/run/testing/registry.py`
- `build/toolchains/run/testing/benchmark_dashboard_generator.py`
- `build/toolchains/run/testing/perf_dashboard.py`
- `docs/archive/architecture/managed-native-hotupdate-test-pipeline.md`
- `wiki/06-测试验证/AOT新Feature接入自测规范.md`

## 当前结论

- 这次清单重构拆成三张主表：
  - `Capability Inventory`
  - `UnitTest Inventory`
  - `Benchmark Inventory`
- `Capability Inventory` 行粒度固定为 `capability item`
- `UnitTest Inventory` 行粒度固定为 `declared-unit-test x stage`
- `Benchmark Inventory` 行粒度固定为 `declared-benchmark x device`
- 所有主表以宽表为主，不使用 card、chart、tab 作为主视图
- 所有表头必须提供中文悬浮 `hint`，且 `hint` 使用详细解释卡，而不是一句话短提示
- `plan-v1-01.md` 已冻结实现批次：
  - 先抽 shared inventory source
  - 再落三张 authority JSON
  - 再派生 CSV / HTML
  - 最后接统一命令入口与验证
- `UnitTest Inventory` 的内部 authority 模型已在计划中冻结为：
  - `StageRequirement`
  - `StageCoverage`
  - 派生 `StageStatus`
  - 派生 `StatusReason`
- benchmark dashboard 保持 auxiliary report 定位，不再承担完整 inventory authority

## 执行记录

- 2026-04-22 15:36:15 +08:00
  - preflight self-review: `pass`
  - 结论：当前任务仍在已批准边界内；authority 仍然以 declared catalog / capability contract / registry projection / workspace collection / benchmark docs 为准，不需要回到 brainstorming 或重写 plan
  - 下一步：先补 `inventory_source`、`inventory_generator`、`run test inventory` 的 RED tests，再进入实现

## 边界拍板

- 这次任务只设计 inventory authority、字段结构、状态口径、中文表头 hint 规范和最小生成链路
- 这次任务不修改现有 benchmark dashboard 的 UI
- 现有 benchmark dashboard 降级为辅助报告，不再作为完整清单 authority

## Authority 决策

- declared entry authority:
  - `build/toolchains/run/testing/compiled_catalog.py`
  - `build/toolchains/run/testing/declarations.py`
- capability / owner / support authority:
  - `build/toolchains/run/testing/capability_coverage.py`
  - `subjects/*/subject.features.json`
- registry projection authority:
  - `build/toolchains/run/testing/registry.py`
- benchmark latest evidence authority:
  - `docs/benchmark/overview.json`
  - subject benchmark docs payload
- stage/gate authority:
  - `docs/archive/architecture/managed-native-hotupdate-test-pipeline.md`
  - `wiki/06-测试验证/AOT新Feature接入自测规范.md`

## AOT/IL2CPP/Test Governance Intake

- capabilityFamily: `test-inventory-and-benchmark-inventory`
- capabilityItem: `inventory-authority-redesign`
- ownerSubjectId: `SolutionCorePack`
- proofRequired: `false`
- benchmarkRequired: `false`
- hotupdateImpact: `None`
- formalVerificationObjects: []
- requiredGates:
  - `collector`
  - `registry`
  - `workspace`

## 问题清零

- blocking_questions: []
- question_clearance: cleared
- clearance_confirmed_by_user: true

## 架构审核

- arch_review_mode: `critical`
- 结论: `ok`
- duplicate_code:
  - `inventory_source.py` 负责上游 authority 聚合
  - `inventory_generator.py` 负责 authority table 派生、CSV flatten 和 HTML payload/render
  - `test.py` 只做 dispatch，模板只做展示，没有把状态判定塞进 HTML
- structure:
  - `inventory_generator.py` 目前同时持有列 hint、表格派生和导出逻辑，但仍属于同一 generator 层职责，没有越界到 CLI 或上游 authority
- warn:
  - `inventory.html` 为了支持本地直接打开而内嵌完整 payload，生成文件偏大；这是已接受的 `warn`，不是 blocker

## 权责图审核

- authority:
  - `inventory_source.py` 只聚合 `compiled catalog / capability contract / registry projection / workspace collection / benchmark docs`
  - `inventory_generator.py` 只消费 source 并派生 authority JSON、CSV 和 HTML
  - `run test inventory` 只负责 dispatch 和输出路径解析
- upstream:
  - `build/toolchains/run/testing/declarations.py`
  - `build/toolchains/run/testing/compiled_catalog.py`
  - `build/toolchains/run/testing/capability_coverage.py`
  - `build/toolchains/run/testing/registry.py`
  - `build/toolchains/run/testing/workspace_manifests.py`
  - `build/toolchains/run/testing/workspace_declared_collection.py`
  - `docs/benchmark/overview.json`
  - `docs/benchmark/subjects/*.json`
- downstream:
  - `docs/testing-inventory/*.json`
  - `docs/testing-inventory/*.csv`
  - `docs/testing-inventory/inventory.html`
  - `run test inventory`
- allowed_deps:
  - `inventory_generator.py -> inventory_source.py -> upstream authorities`
  - `test.py -> inventory_generator.py`
  - `manifest.py / manifest json` 只做命令解析和注册
- shim_debt:
  - HTML 为了本地直接打开而内嵌 payload，和 JSON 存在字节级重复；如果后续要进一步缩小生成物体积，可以再评估外置数据加载方案
- proof:
  - RED/GREEN 单测通过
  - 真实 `run test inventory` 命令通过

## 完成证据

- arch_review: ok
- authority_review: ok
- verification:
  - canonicalCommand: `python build/toolchains/run/run.py test inventory --json`
  - formalObject: `[collector, registry, workspace]`
  - summaryPath: `n/a`
  - eventsPath: `n/a`
  - consolePath: `n/a`
- test_result: passed
- wiki: n/a
- next: merge

## 验证命令

- `python -m pytest tests/unit/performance/test_testing_inventory_source.py tests/unit/performance/test_testing_inventory_generator.py -q`
- `python -m pytest tests/unit/performance/test_benchmark_dashboard_generator_contracts.py tests/unit/performance/test_perf_dashboard.py tests/unit/registry/test_declared_registry_matrix_selection.py -q`
- `python -m pytest tests/tooling/run/test_subject_command_inventory.py tests/tooling/run/test_command_manifest_structure.py tests/tooling/run/test_command_manifest_cli.py -q`
- `python build/toolchains/run/run.py test inventory --json`

## 下一步入口

- 使用 `dev:executing-plans` 按 `plan-v1-01.md` 进入实现
- 优先批次：
  - Task 1 shared inventory authority / 权责图审核
  - Task 2 authority JSON
  - Task 3 CSV / HTML
  - Task 4 CLI 接入
  - Task 5 测试与收口
