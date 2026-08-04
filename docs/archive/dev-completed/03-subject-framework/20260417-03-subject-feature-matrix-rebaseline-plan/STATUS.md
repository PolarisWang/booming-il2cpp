---
task_id: 20260417-03-subject-feature-matrix-rebaseline-plan
title: Subject Feature Matrix Rebaseline Plan
task_type: roadmap
lifecycle_status: completed
phase: completed
created_at: 2026-04-17 12:20:00 +08:00
updated_at: 2026-04-17 17:12:13 +08:00
latest_stop_point: Phase 0~6 are complete; canonical subject authority, remigration, legacy purge contracts, regression, and docs closeout are finished
current_dir: docs/dev/completed/20260417-03-subject-feature-matrix-rebaseline-plan
parent_task_id:
source_task_id:
source_relation:
design_doc: docs/dev/completed/20260417-03-subject-feature-matrix-rebaseline-plan/design-v1-02.md
plan_doc:
roadmap_or_plan: docs/dev/completed/20260417-03-subject-feature-matrix-rebaseline-plan/roadmap-v1-02.md
active: false
---

## 关键文档

- `docs/dev/completed/20260417-03-subject-feature-matrix-rebaseline-plan/brainstorm-v1-01.md`
- `docs/dev/completed/20260417-03-subject-feature-matrix-rebaseline-plan/design-v1-02.md`
- `docs/dev/completed/20260417-03-subject-feature-matrix-rebaseline-plan/roadmap-v1-02.md`
- `docs/dev/completed/20260417-03-phase-0-full-rebirth-authority-freeze/STATUS.md`
- `docs/dev/completed/20260417-03-phase-1-framework-contract-and-generator-rebuild/STATUS.md`
- `docs/dev/completed/20260417-03-phase-2-business-pipeline-rebuild/STATUS.md`
- `docs/dev/completed/20260417-03-phase-3-subject-system-full-rebirth/STATUS.md`
- `docs/dev/completed/20260417-03-phase-4-completed-feature-remigration-and-verification-rebuild/STATUS.md`

## 当前结论

- 方案 C 已经完整落地：整个 `subject/test framework` 体系以单轨重生收口，不再保留长期双轨兼容。
- canonical owner subjects 固定为 `SolutionCorePack`、`MixedExecutionFeaturePack`、`HotUpdateHostPack`。
- `subject.features.json` 已成为 completed feature owner / obligation authority，formal proof / benchmark 资产与之对齐。
- 正式 correctness 主链稳定为：
  - `managed source with Sdk attributes -> collector -> collection -> generated managed/native/hotupdate hosts`
- legacy cutover contract 已补齐，禁止旧目录语义、旧入口协议和 stdout correctness 回流。
- 主干 regression 与文档/wiki 收口已完成。

## 阶段结果

- Phase 0: 完成 canonical `subject.features.json` authority freeze。
- Phase 1: 完成 `Sdk / Runtime / collector / template` contract rebuild。
- Phase 2: 完成 planner / registry / workspace / dashboard 的业务管线 rebuild。
- Phase 3: 完成三大 canonical subjects 的 full rebirth。
- Phase 4: 完成 completed feature remigration、obligation coverage、declared metadata 收口。
- Phase 5: 完成 one-shot legacy purge contract，锁住旧目录、旧协议、stdout correctness 禁止回流。
- Phase 6: 完成 coverage automation 主干、Phase 回归与 architecture/wiki 收口。

## 验证

- 继承子任务 `20260417-03-phase-4-completed-feature-remigration-and-verification-rebuild` 的 `156 passed` 主干回归集。
- 额外收口：
  - `tests/contracts/shared/test_phase5_legacy_cutover_contract.py`
  - architecture/wiki 已按规则更新。

## 风险

- 本 roadmap 完成判断基于 subject/test framework 主干与 canonical subjects 相关回归集，不包含全仓所有历史 AOT 子线。

## 下一步

- 本 roadmap 已完成，无后续执行步骤。
