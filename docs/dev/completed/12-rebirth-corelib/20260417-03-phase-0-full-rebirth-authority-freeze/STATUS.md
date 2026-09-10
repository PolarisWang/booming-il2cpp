---
task_id: 20260417-03-phase-0-full-rebirth-authority-freeze
title: Phase 0 Full Rebirth Authority Freeze
task_type: plan
lifecycle_status: completed
phase: completed
created_at: 2026-04-17 13:03:34 +08:00
updated_at: 2026-04-17 13:30:09 +08:00
latest_stop_point: Phase 0 completed; canonical subject authority files now carry frozen scenario seeds and owner capability entries, and capability coverage has been cut over to file-backed authority
current_dir: docs/dev/completed/20260417-03-phase-0-full-rebirth-authority-freeze
parent_task_id: 20260417-03-subject-feature-matrix-rebaseline-plan
source_task_id: 20260417-03-subject-feature-matrix-rebaseline-plan
source_relation: roadmap-child-phase-0
design_doc: docs/dev/completed/20260417-03-phase-0-full-rebirth-authority-freeze/design-v1-01.md
plan_doc: docs/dev/completed/20260417-03-phase-0-full-rebirth-authority-freeze/plan-v1-01.md
total_tasks: 5
current_task: completed
active: false
---

## 结论

- `subject.features.json` 已正式成为 canonical subject 的 capability authority seed。
- 3 个 canonical subject 的最小 engineering scenario 集合已经冻结，并由 contract test 锁定。
- `build/toolchains/run/testing/capability_coverage.py` 已从硬编码 owner/support matrix 切换为消费 `subjects/*/subject.features.json`。
- `Phase 1` 的起始输入已经具备：
  - collection schema versioning
  - file-backed feature authority
  - framework/runtime collection contract 对齐

## 本轮收口

- 为 `SolutionCorePack`、`MixedExecutionFeaturePack`、`HotUpdateHostPack` 补齐 owner capability entries。
- 新增并跑通 authority contract test，覆盖：
  - canonical subject features file 存在性
  - 最小 scenario 形状
  - frozen scenario set
  - owner capability seed presence
- 新增 capability coverage 文件 authority 红测，并完成实现切换。

## 已验证

- `python -m pytest tests/contracts/shared/test_subject_authority_freeze.py tests/unit/compatibility/test_phase5_owner_subject_coverage.py -q`
  - `9 passed`
- `python -m pytest tests/contracts/shared/test_subject_authority_freeze.py tests/contracts/shared/test_testframework_collection_contract.py tests/unit/compatibility/test_phase5_owner_subject_coverage.py tests/unit/performance/test_workspace_declared_collection.py tests/integration/registry/test_declared_metadata_discovery.py tests/unit/performance/test_benchmark_dashboard_generator.py tests/unit/performance/test_benchmark_command.py tests/tooling/run/test_subject_command.py -q`
  - `60 passed`

## 风险 / 余留

- `subject.features.json` 当前只完成了 Phase 0/1 所需的最小 capability authority 形状，后续仍需在 Phase 4 扩展到完整 obligation ledger。
- generated managed/native/hotupdate host 仍未完成 template contract 重建；这部分进入 Phase 1 后续批次处理。
- collector 责任边界仍暂时借用 `Chaos.IL2CPP.DeclarationDiscovery`，尚未彻底收敛到测试框架域。

## 下一步

- 切换到 `20260417-03-phase-1-framework-contract-and-generator-rebuild`
- 继续完成：
  - generated host contract rebuild
  - file-level template cutover
  - collector boundary cleanup

## wiki

- 本阶段暂不直接写入 wiki。
- 原因：authority freeze 已完成，但 Phase 1 的 generator/collector 主链还未收口，待新主链落地后统一写入架构与治理文档。
