---
task_id: 20260413-23-phase-8-compatibility-closure-and-legacy-cleanup-batch-1
title: Phase 8 Compatibility Closure And Legacy Cleanup Batch 1
task_type: plan
lifecycle_status: completed
phase: archived
created_at: 2026-04-13 05:58:37 +08:00
updated_at: 2026-04-13 07:24:06 +08:00
current_dir: docs/dev/completed/20260413-23-phase-8-compatibility-closure-and-legacy-cleanup-batch-1
parent_task_id: 20260412-06-test-system-suite-redesign
source_task_id: 20260412-06-test-system-suite-redesign
source_relation: roadmap-child-batch
active: false
---

## 关键文档
- parent_design: `docs/dev/completed/20260412-06-test-system-suite-redesign/design-v1-03.md`
- parent_roadmap: `docs/dev/completed/20260412-06-test-system-suite-redesign/roadmap-v1-03.md`
- completed_previous_batch: `docs/dev/completed/20260413-22-phase-7-registry-selector-artifact-pipeline-cutover-batch-1/plan-v1-01.md`
- latest_progress: `docs/dev/completed/20260413-23-phase-8-compatibility-closure-and-legacy-cleanup-batch-1/notes/progress-v1-02.md`
- plan: `docs/dev/completed/20260413-23-phase-8-compatibility-closure-and-legacy-cleanup-batch-1/plan-v1-01.md`

## 当前判断
- current_focus: 本批已完成归档，Phase 8 的 tests taxonomy 收口、legacy wrapper root 退役、compatibility/planning 回归修正均已落地。
- why_done: `tests/proof`、`tests/registry`、`tests/platform` 已退役，`tests/gate` 仅保留 wrapper；compatibility、performance、planning 相关测试均已切到 canonical packs / solution subjects，不再依赖已删除的 legacy shell manifest。
- done_definition: 已满足。Phase 8 的 safe cleanup batch 已执行完成，且 `python -m pytest -q` 全量结果为 `781 passed, 40 skipped`，父 roadmap 可以同步收口。

## 最近摘要
- 2026-04-13 05:58:37 +08:00: 激活 `20260413-23-phase-8-compatibility-closure-and-legacy-cleanup-batch-1`，承接 Phase 7 完成后的 legacy cleanup 收口工作。
- 2026-04-13 07:24:06 +08:00: 完成 planning 层 legacy shell 用例向 canonical packs 的切换；全量验证结果为 `781 passed, 40 skipped`，Phase 8 正式归档。

## 下一步
- next_action: 无。父 roadmap `20260412-06-test-system-suite-redesign` 已同时归档完成。
- owner: codex
- trigger: 若未来要继续处理兼容 alias 的版本化退役周期，应以新任务承接，不再复用本批执行上下文。

## 风险 / 阻塞

### risks

- `tests/gate` 仍保留 wrapper 是有意保留的兼容入口，不再被视为 Phase 8 blocker。
- `tests/perf/convert-golden-multi-project` 仍由 `build/toolchains/run/testing/perf.py` 直接读取 baseline；这部分已被归类为当前 canonical perf fixture 入口，而非 legacy shell 债务。

### blockers

- 无。
