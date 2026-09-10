---
task_id: 20260417-03-phase-1-framework-contract-and-generator-rebuild
title: Phase 1 Framework Contract And Generator Rebuild
task_type: plan
lifecycle_status: completed
phase: completed
created_at: 2026-04-17 13:30:09 +08:00
updated_at: 2026-04-17 14:20:00 +08:00
latest_stop_point: Phase 1 completed; collection contract, file-backed capability authority, generated host template assets, native workspace template assets, and collector tool boundary have all been rebuilt
current_dir: docs/dev/completed/20260417-03-phase-1-framework-contract-and-generator-rebuild
parent_task_id: 20260417-03-subject-feature-matrix-rebaseline-plan
source_task_id: 20260417-03-subject-feature-matrix-rebaseline-plan
source_relation: roadmap-child-phase-1
design_doc: docs/dev/completed/20260417-03-phase-1-framework-contract-and-generator-rebuild/design-v1-01.md
plan_doc: docs/dev/completed/20260417-03-phase-1-framework-contract-and-generator-rebuild/plan-v1-01.md
total_tasks: 6
current_task: completed
active: false
---

## 完成结论

- collection 已成为 Python / Runtime 共用的 versioned contract。
- capability coverage 已从硬编码 owner/support matrix 切到 `subject.features.json`。
- generated managed/hotupdate host 的整文件输出已切到模板资产。
- `project_workspace.py` 中 native reference/native aot 的 CMake 整文件输出已切到模板资产。
- declaration collector 工具已迁出 `src/managed`，落到 `src/tools/Chaos.TestFramework.CollectionGen`。

## 本轮收口

- 新增 `build/toolchains/run/testing/template_assets.py`
- 新增 managed/hotupdate host 模板资产
- 新增 native workspace / generated / proof / benchmark 模板资产
- `compiled_catalog.py` 已消费新的 collector 工具路径

## 已验证

- `python -m pytest tests/unit/planning/test_generated_managed_hosts.py tests/unit/planning/test_generated_hotupdate_hosts.py -q`
  - `10 passed`
- `python -m pytest tests/unit/compatibility/test_declared_metadata_discovery_contract.py tests/integration/registry/test_declared_metadata_discovery.py -q`
  - `10 passed`
- `python -m pytest tests/contracts/shared/test_subject_authority_freeze.py tests/contracts/shared/test_testframework_collection_contract.py tests/unit/compatibility/test_phase5_owner_subject_coverage.py tests/unit/compatibility/test_declared_metadata_discovery_contract.py tests/unit/planning/test_generated_managed_hosts.py tests/unit/planning/test_generated_hotupdate_hosts.py tests/unit/planning/test_project_workspace.py tests/unit/performance/test_workspace_declared_collection.py tests/integration/registry/test_declared_metadata_discovery.py tests/unit/performance/test_benchmark_dashboard_generator.py tests/unit/performance/test_benchmark_command.py tests/tooling/run/test_subject_command.py -q`
  - `104 passed`

## 风险 / 余留

- planner / registry / workspace manifest 仍有历史字段负担；这部分进入 Phase 2 处理。
- completed 历史文档与 wiki 尚未统一更新新的 collector 命名与路径。

## 下一步

- 切入 `20260417-03-phase-2-business-pipeline-rebuild`
- 聚焦：
  - planner/registry/workspace 对旧 manifest 字段的依赖收缩
  - dashboard/report 的 capability/scenario 单轨消费

## wiki

- 本阶段暂不直接写入 wiki。
- 原因：Phase 2 还要完成业务消费管线切换，待主链稳定后统一写入架构与治理文档。
