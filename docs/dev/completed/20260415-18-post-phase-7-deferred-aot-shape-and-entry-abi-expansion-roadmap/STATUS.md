---
task_id: 20260415-18-post-phase-7-deferred-aot-shape-and-entry-abi-expansion-roadmap
title: Post Phase 7 Deferred AOT Shape And Entry ABI Expansion Roadmap
task_type: roadmap
lifecycle_status: completed
phase: completed
created_at: 2026-04-15 23:52:48 +08:00
updated_at: 2026-04-16 00:48:20 +08:00
current_dir: docs/dev/completed/20260415-18-post-phase-7-deferred-aot-shape-and-entry-abi-expansion-roadmap
parent_task_id:
source_task_id: 20260415-11-phase-7-aot-core-ir-typed-il-translation-expansion-roadmap
source_relation: follow-up-on-deferred-aot-shape-and-entry-abi-surface
active: false
---

## 关键文档

- source_roadmap: `docs/dev/completed/20260415-11-phase-7-aot-core-ir-typed-il-translation-expansion-roadmap/roadmap-v1-01.md`
- source_status: `docs/dev/completed/20260415-11-phase-7-aot-core-ir-typed-il-translation-expansion-roadmap/STATUS.md`
- phase_0_translation_surface: `docs/dev/completed/20260415-01-phase-0-contract-freeze-and-aot-translation-ledgers/aot-translation-surface-ledger-v1-01.md`
- phase_0_unsupported_surface: `docs/dev/completed/20260415-01-phase-0-contract-freeze-and-aot-translation-ledgers/aot-unsupported-surface-ledger-v1-01.md`
- phase_0_identity_freeze: `docs/dev/completed/20260415-01-phase-0-contract-freeze-and-aot-translation-ledgers/stable-identity-and-body-availability-v1-01.md`
- brainstorm: `docs/dev/completed/20260415-18-post-phase-7-deferred-aot-shape-and-entry-abi-expansion-roadmap/brainstorm-v1-01.md`
- design: `docs/dev/completed/20260415-18-post-phase-7-deferred-aot-shape-and-entry-abi-expansion-roadmap/design-v1-01.md`
- roadmap_or_plan: `docs/dev/completed/20260415-18-post-phase-7-deferred-aot-shape-and-entry-abi-expansion-roadmap/roadmap-v1-01.md`

## 当前结论

- `20260415-19` 到 `20260415-24` 六个 child task 已全部完成，并已分别归档到 `docs/dev/completed/`。
- deferred AOT follow-up 的五类核心收口面已经闭环：
  - `byref` 参数 / 返回 / carrier contract
  - widened method shape 与 planner-visible wrapper / thunk shape
  - entry ABI normalization
  - `generic + byref + value-type` mixed dispatch / invocation
  - `NativeAotEmitter + Scriban` widened output 与 owner evidence / benchmark / gate
- 本轮收口保持了既定边界：
  - `Chaos.TestFramework` 没有混入 `AotCoreIr` / planner / emitter
  - widened codegen 继续走 `AotCoreIr -> NativeAotLoweringPlanner -> NativeAotEmitter + Scriban`
  - 没有继续扩大 subject 内部调度的 string 协议面

## 子任务完成映射

| task_id | phase | lifecycle_status | summary |
| --- | --- | --- | --- |
| `20260415-19-phase-0-deferred-surface-freeze-and-contract-ledger` | `Phase 0` | `completed` | deferred surface ledger、capability family、owner/evidence mapping 已冻结。 |
| `20260415-20-phase-1-byref-and-method-shape-contract-expansion` | `Phase 1` | `completed` | loader / metadata / `AotCoreIr` 的 `byref` 与 widened method shape contract 已扩展。 |
| `20260415-21-phase-2-entry-abi-normalization-and-planner-widening` | `Phase 2` | `completed` | planner entry ABI、wrapper / thunk / hidden receiver / return carrier plan model 已统一。 |
| `20260415-22-phase-3-generic-byref-mixed-dispatch-closure` | `Phase 3` | `completed` | `generic + byref + value-type` mixed dispatch / invocation lowering 已闭环。 |
| `20260415-23-phase-4-native-aot-emitter-and-scriban-template-widening` | `Phase 4` | `completed` | `NativeAotEmitter` 与 Scriban 模板已完成 widened entry / byref / mixed shape 输出。 |
| `20260415-24-phase-5-owner-evidence-benchmark-and-gate-closure` | `Phase 5` | `completed` | owner proof / benchmark / reporting / gate evidence 已与 widened surface 对齐。 |

## 最近摘要

- 2026-04-16 00:31:55 +08:00: 已为 `20260415-19` 到 `20260415-24` 创建并归档完整 child task 文档，承接既有实现结果。
- 2026-04-16 00:31:55 +08:00: 已运行 roadmap 相关回归 `python -m pytest tests/unit/compatibility tests/integration/registry/test_declared_metadata_discovery.py tests/unit/performance/test_native_aot_workload_entry_bundle.py tests/unit/reporting tests/unit/registry`，结果为 `274 passed`。
- 2026-04-16 00:48:20 +08:00: 已运行项目级 `python -m pytest -q`，结果为 `713 passed, 40 skipped`，满足父 roadmap 归档条件。
- 2026-04-16 00:48:20 +08:00: 已将父任务更新为 completed，清理 `docs/dev/ACTIVE.md`，并补齐 `docs/dev` 索引。

## 下一步

- next_action: 无。本 roadmap 已完成并归档；如需继续扩大 deferred AOT surface，请另起 follow-up task。
- owner: codex
- trigger: 新的 deferred typed-IL / ABI / hotupdate 目标被正式立项。

## 风险 / 阻塞

### risks

- 无新增执行风险；本 roadmap 的 deferred 收口项已全部进入 completed 归档链路。

### blockers

- 无。

## 验证

- `python -m pytest tests/unit/compatibility tests/integration/registry/test_declared_metadata_discovery.py tests/unit/performance/test_native_aot_workload_entry_bundle.py tests/unit/reporting tests/unit/registry`
  - 结果：`274 passed`
- `python -m pytest -q`
  - 结果：`713 passed, 40 skipped`

## wiki

- 本轮不新增 wiki。该任务的长期知识已体现在各 child task 的 design / plan / status 归档中。
