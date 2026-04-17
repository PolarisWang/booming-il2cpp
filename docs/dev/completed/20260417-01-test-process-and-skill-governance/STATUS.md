---
task_id: 20260417-01-test-process-and-skill-governance
title: Test Process And Skill Governance
task_type: roadmap
lifecycle_status: completed
phase: completed
created_at: 2026-04-17 00:00:00 +08:00
updated_at: 2026-04-17 06:47:45 +08:00
latest_stop_point: roadmap completed; unified test pipeline now closes on Sdk + Runtime + collection + manifest, and Phase 7 terminology cleanup is finished
current_dir: docs/dev/completed/20260417-01-test-process-and-skill-governance
parent_task_id:
source_task_id:
source_relation:
roadmap_or_plan: docs/dev/completed/20260417-01-test-process-and-skill-governance/roadmap-v1-01.md
active: false
---

## 关键文档

- `docs/dev/completed/20260417-01-test-process-and-skill-governance/brainstorm-v1-01.md`
- `docs/dev/completed/20260417-01-test-process-and-skill-governance/design-v1-01.md`
- `docs/dev/completed/20260417-01-test-process-and-skill-governance/roadmap-v1-01.md`
- `docs/architecture/managed-native-hotupdate-test-pipeline.md`

## 完成结论

- 已冻结统一测试主线：`managed solution -> dotnet 8 collection analysis -> collection files -> managed test project -> native project -> native test project -> hotupdate patch project + hotupdate test host project`
- 已确认采用方案 A，并将 `Chaos.TestFramework.Annotation` 改名为 `Chaos.TestFramework.Sdk`
- 已确认 `Assert` 属于 `Sdk`，`Runtime` 属于执行宿主面
- 已确认 native / hotupdate 专有绑定继续通过 manifest 分层，不进入 collection file

## 本轮收口

- 已完成 `workspace_declared_catalog.py -> workspace_declared_collection.py` 重命名，并同步 `benchmark.py`、`benchmark_dashboard_generator.py`、`subject_workers.py`、`test_subject_workers.py` 与对应单测文件命名。
- 已清掉当前代码与测试里残留的 `workspace catalog` / `declared catalog` 用户可见文案，统一到 `collection` 口径。
- HotUpdate proof / benchmark host 已稳定走 `collection + binding manifest` 真链路，`PerfHarness` 不再回退到旧反射扫描路径。
- 现行 architecture / wiki / skill 均已对齐 `Sdk + Runtime + collection + manifest` 主线，本任务无额外 wiki 待补。

## 已验证

- `python -m pytest tests/unit/planning/test_project_workspace.py tests/unit/planning/test_generated_managed_hosts.py tests/unit/planning/test_generated_hotupdate_hosts.py tests/unit/execution/test_subject_workers.py tests/unit/performance/test_workspace_declared_collection.py tests/unit/performance/test_benchmark_command.py tests/unit/performance/test_benchmark_dashboard_generator.py tests/unit/performance/test_subject_workers_perf.py tests/tooling/run/test_subject_command.py tests/integration/registry/test_declared_metadata_discovery.py tests/integration/run/test_perf_harness_hotupdate_benchmark_host.py tests/unit/compatibility/test_phase5_hotupdate_host_pack_declared_proof.py -q`
  - `134 passed`
- `python -u build/toolchains/run/run.py test declared-benchmark --id "declared-benchmark/HotUpdateHostPack::HotUpdateHostPack::HotUpdateHostPack.HotUpdateLoadBenchmarkEntry::RunWorkload()" --json`
  - `final-summary.status = ok`
- `python -u build/toolchains/run/run.py test declared-unit-test --id 'declared-unit-test/HotUpdateHostPack::HotUpdateHostPack::HotUpdateHostPack.HotUpdateSkeletonProofEntry::Run()' --json`
  - `final-summary.status = ok`

## 剩余风险 / 任务外阻塞

- `SolutionCorePack` 仍无法完成全量 workspace regenerate；阻塞点不在测试框架，而在 native proof emission：
  - `unsupported managed lowering plan kind 'generic-analysis-only'`
  - 调用栈落在 `Chaos.IL2CPP.CodeGen.NativeReferenceProofEmitter.ValidateManagedLoweringPlan(...)`
- 当前测试治理 roadmap 已无剩余阻塞；上面的 `SolutionCorePack` 问题继续留在独立 AOT roadmap 中处理。

## 下一步

- 本任务已归档完成。
- 若后续要继续推进 `SolutionCorePack` native proof emission，请转回 `20260416-01-aot-core-ir-gap-audit-and-next-expansion-roadmap` 主线处理。
