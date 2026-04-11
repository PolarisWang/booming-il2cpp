---
task_id: 20260411-09-productization-gates
title: Productization Gates
task_type: plan
lifecycle_status: in_progress
phase: executing
created_at: 2026-04-11 21:10:00 +08:00
updated_at: 2026-04-11 19:59:47 +08:00
current_dir: docs/dev/in-progress/20260411-09-productization-gates
parent_task_id: 20260409-10-total-solution-and-ios-hot-update-analysis
source_task_id: 20260409-10-total-solution-and-ios-hot-update-analysis
source_relation: child_of_roadmap_phase_8
active: true
---

## 关键文档

- parent_roadmap: `docs/dev/in-progress/20260409-10-total-solution-and-ios-hot-update-analysis/roadmap-v1-01.md`
- plan: `docs/dev/in-progress/20260411-09-productization-gates/plan-v1-01.md`
- brainstorm: `docs/dev/in-progress/20260411-09-productization-gates/brainstorm-v1-01.md`
- design: `docs/dev/in-progress/20260411-09-productization-gates/design-v1-01.md`

## 当前判断

- current_focus: 在 Phase 4 外部阻塞仍存在的前提下，Phase 8 已完成 Batch 3、Batch 2、Batch 1 的本地最小闭环，下一步转向 Batch 4 的本地可交付骨架。
- why_now: compatibility matrix 已落地到 proof config、runner 与 JSON report；本地仍可继续沉淀 perf dashboard、unsupported feature report 与 soak harness skeleton，减少后续移动端环境恢复后的收尾压力。
- done_definition: 本轮已完成 `docs/architecture/ios-distribution-policy.md`、`docs/architecture/version-compatibility-matrix.md`、`docs/architecture/release-checklist.md`、`HotUpdateSemanticVersion` helper、`RuntimeManager` 的 rollback/active patches/integrity report，以及 `CompatibilityMatrixProof` 的 config/runner/report 骨架和对应定向验证。

## 最近摘要

- 2026-04-11 21:10:00 +08:00: 激活 `20260411-09-productization-gates`，决定在 Phase 4 外部阻塞期间先执行 Phase 8 的本地可验证批次。
- 2026-04-11 21:22:00 +08:00: 完成 Batch 3 最小切片：新增 iOS distribution policy 与 version compatibility matrix 文档，`PackageValidator` 升级为 `major.minor` compatibility band 校验。
- 2026-04-11 21:45:00 +08:00: 完成 Batch 2 最小闭环：`RuntimeManager` 新增 `Rollback()`、`GetActivePatches()`、`ValidateIntegrity()` / `HotUpdateIntegrityReport`，并补 `release-checklist.md`。
- 2026-04-11 19:59:47 +08:00: 完成 Batch 1 最小闭环：新增 `subjects/CompatibilityMatrixProof/`、compatibility matrix JSON config、`compatibility_matrix_runner.py` 与 JSON report skeleton，定向验证已覆盖 config schema、runner 执行、subject manifest 与 proof 输出。

## 下一步

- next_action: 转向 Batch 4，先盘点现有 perf/registry/reporting 资产，补 perf regression dashboard 配置与 unsupported feature report skeleton；是否继续切 soak harness 取决于现有 perf harness 的可复用程度。
- owner: codex
- trigger: Batch 1 已闭环，Batch 4 仍有本地可实现资产且不依赖真实 Android NDK / emulator / device 或 macOS/Xcode。

## 验证

- passed: `python -m pytest tests/unit/run/test_phase8_productization_gates.py -v`
- passed: `python -m pytest tests/unit/run/test_subject_manifest_schema.py -v`
- passed: `dotnet run --project subjects/CompatibilityMatrixProof/source/CompatibilityMatrixProof.csproj`
- passed: `python -m pytest tests/unit/run/test_phase5_hot_update_skeleton.py -v`
- passed: `dotnet build src/managed/Chaos.IL2CPP.HotUpdate/Chaos.IL2CPP.HotUpdate.csproj -c Release`

## 风险 / 阻塞

### risks

- compatibility matrix proof 当前采用 `1.0.0 / 1.0.1 / 1.0.2` patch-level band 来对齐已落地的 `major.minor` 兼容规则；若后续要恢复 roadmap 中跨 minor 的样例，需要先统一版本策略。
- 当前 compatibility matrix runner 仍是 proof-first skeleton：它通过 `CompatibilityMatrixProof` 与 JSON config/report 固化批量验证入口，但尚未接入统一 public command / registry。
- 若把 Phase 8 全量目标和 Phase 4 外部环境证据混在一起推进，仍会被 Android NDK / emulator / device 与 macOS/Xcode 阻塞拖住。

### blockers

- Phase 8 的 full completion 仍依赖 Phase 4 mobile runtime host 的真实运行证据。
- Batch 4 的 soak test / perf dashboard 最终验收仍需要更完整的平台与长期运行环境；当前只能先沉淀本地 skeleton 或配置。

## Wiki 决策

- 本轮长期知识继续沉淀在 `docs/architecture/*.md` 与 `subjects/CompatibilityMatrixProof/compatibility-matrix.json`，暂不额外写入 `wiki/`。
