---
task_id: 20260411-01-shared-contract-freeze
title: Shared Contract Freeze - 冻结 AOT 与热更共享 contract
task_type: plan
lifecycle_status: completed
phase: archived
created_at: 2026-04-11 14:00:00 +08:00
updated_at: 2026-04-11 14:50:00 +08:00
current_dir: docs/dev/completed/20260411-01-shared-contract-freeze
parent_task_id: 20260409-10-total-solution-and-ios-hot-update-analysis
source_task_id: 20260409-10-total-solution-and-ios-hot-update-analysis
source_relation: child_of_roadmap_phase_0
active: false
---

## 关键文档

- parent_roadmap: `docs/dev/in-progress/20260409-10-total-solution-and-ios-hot-update-analysis/roadmap-v1-01.md`
- phase_architecture: `docs/dev/in-progress/20260409-10-total-solution-and-ios-hot-update-analysis/phase-architecture-v1-01.md`
- plan: `docs/dev/completed/20260411-01-shared-contract-freeze/plan-v1-01.md`
- latest_progress: `docs/dev/completed/20260411-01-shared-contract-freeze/notes/progress-v1-02.md`

## 当前判断

- current_focus: `contracts/shared/v0`、schema、proof subject 与 repo enforcement 已全部落地，当前任务已归档为共享 contract v0 的冻结基线。
- why_now: 所有后续 Phase 都依赖统一的 identity/ABI/handle 模型。
- done_definition: 11 份 contract spec 文档 + schema + snapshot tests + 双边 mock 验证全部通过。

## 最近摘要

- 2026-04-11 14:10:00 +08:00: 复核父 roadmap、`phase-architecture-v1-01.md` 与 `plan-v1-01.md`，确认主任务已经拆成 9 个 child phase，当前必须从 `shared-contract-freeze` 开始。
- 2026-04-11 14:18:00 +08:00: 复核 `runtime_abi.h`、`runtime_core.cpp`、`bootstrap.cpp` 与 `ManagedClosureContracts.cs`，确认 Phase 0 的事实来源已经足够支撑 contract 文档冻结。
- 2026-04-11 14:30:00 +08:00: 恢复执行上下文，创建 `ACTIVE.md` 与本轮 progress 记录，准备先提交 `tests/contracts/shared/*` 的 RED 测试和 `test_repo_layout.py` 的 shared-contract enforcement。
- 2026-04-11 14:50:00 +08:00: 重跑 `tests/contracts/shared/`、`test_repo_layout.py` 与 `tests/unit/run/ tests/tooling/run/`，确认 shared contract v0 与相关回归保持全绿；随后将长期规则写入 wiki，并切换主线到 `20260411-02-full-project-ingestion`。

## 下一步

- next_action: 无。下一阶段由父 roadmap 继续推进 `20260411-02-full-project-ingestion`。
- owner: codex
- trigger: 无。

## 验证

- passed: `python -m pytest tests/contracts/shared/ -v` (`9 passed`)
- passed: `python -m pytest tests/unit/run/test_repo_layout.py -v` (`18 passed`)
- passed: `python -m pytest tests/unit/run/ tests/tooling/run/ -q` (`298 passed, 34 skipped`)

## 风险 / 阻塞

### risks

- identity 格式如果不够灵活，后续 generic instantiation 会遇到编码问题。
- `runtime_core.cpp` 当前字符串对象仍是 proof 级最小布局；Phase 0 文档必须以“当前可验证实现”为准，不能在没有代码支撑的情况下假冻结更强语义。

### blockers

- 无。
