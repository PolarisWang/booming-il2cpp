---
task_id: 20260415-13-phase-1-loader-and-metadata-decode-expansion
title: Phase 1 Loader And Metadata Decode Expansion
task_type: plan
lifecycle_status: completed
phase: completed
created_at: 2026-04-15 20:43:14 +08:00
updated_at: 2026-04-15 21:23:30 +08:00
current_dir: docs/dev/completed/20260415-13-phase-1-loader-and-metadata-decode-expansion
parent_task_id: 20260415-11-phase-7-aot-core-ir-typed-il-translation-expansion-roadmap
source_task_id: 20260415-11-phase-7-aot-core-ir-typed-il-translation-expansion-roadmap
source_relation: roadmap-child
design_doc: docs/dev/completed/20260415-13-phase-1-loader-and-metadata-decode-expansion/design-v1-01.md
plan_doc: docs/dev/completed/20260415-13-phase-1-loader-and-metadata-decode-expansion/plan-v1-01.md
total_tasks: 4
current_task: completed
active: false
---

## 关键文档

- parent_status: `docs/dev/in-progress/20260415-11-phase-7-aot-core-ir-typed-il-translation-expansion-roadmap/STATUS.md`
- parent_roadmap: `docs/dev/in-progress/20260415-11-phase-7-aot-core-ir-typed-il-translation-expansion-roadmap/roadmap-v1-01.md`
- phase_0_status: `docs/dev/completed/20260415-12-phase-0-translation-gap-freeze-and-batch-contract/STATUS.md`
- brainstorm: `docs/dev/completed/20260415-13-phase-1-loader-and-metadata-decode-expansion/brainstorm-v1-01.md`
- design: `docs/dev/completed/20260415-13-phase-1-loader-and-metadata-decode-expansion/design-v1-01.md`
- plan: `docs/dev/completed/20260415-13-phase-1-loader-and-metadata-decode-expansion/plan-v1-01.md`

## 当前结论

- Task 1-2 已完成：真实 `FunctionPointerProofEntry::Run()` / `FunctionPointerBenchmarkEntry::RunWorkload()` 的 `calli/function pointer` 路径已通过 RED/GREEN 闭环，`calli` 现在会携带稳定的 `callSiteSignature`，并贯穿到 `typed-il` 与 `aot-core-ir`。
- Task 3 已完成：真实 `GenericInstantiationProofEntry::Run()` 的 closed generic method call 在 `typed-il` 中已带上 `reference`；`aot-core-ir` 继续保留 `genericContext`，不再只剩 `callee` 字符串。
- Task 4 已完成：`tests/unit/compatibility` 全量回归通过，说明当前 retained C# subject 集在 loader / metadata decode 阶段已无直接硬失败。
- `Loader.EntryAndHandleKinds` 中剩余的 method/field `ldtoken` handle kind 分支未在本阶段继续强推，因为当前 C# 前端无法直接产出 `__methodref` / `__fieldref` 这类语法；本轮将其明确标记为 deferred，等待后续 IL-authored subject 或新增前端再重开。

## 最近摘要

- 2026-04-15 21:11:59 +08:00: 新增并跑通 `tests/unit/compatibility/test_phase7_loader_function_pointer_decode.py`，确认 `calli` 的 `resultType` 与 `callSiteSignature` 已稳定出现在 `typed-il` / `aot-core-ir`。
- 2026-04-15 21:16:07 +08:00: 新增 `tests/unit/compatibility/test_phase7_loader_generic_method_reference_decode.py`，先 RED 后 GREEN，补齐真实 closed generic call 的 `typed-il reference` contract。
- 2026-04-15 21:23:30 +08:00: 跑通 `python -m pytest tests/unit/compatibility -q`，结果 `232 passed`，Phase 1 进入 completed。

## 下一步

- next_action: 无。本 child task 已完成，父 roadmap 切入 `20260415-14-phase-2-scalar-conversion-and-abi-carrier-widening`。
- owner: codex
- trigger: 无。

## 风险 / 阻塞

### risks

- method/field `ldtoken` handle kind 仍在 `LoaderStage.cs` 保留 defensive `NotSupportedException` 分支；当前将其视为“前端不可达”而非“功能完成”，后续如果引入 IL-authored subject，必须重新打开。
- 后续 Phase 2-4 会继续同时触碰 `AotCoreIr`、planner 与 emitter；若 ABI widening 不保持枚举化与紧凑 contract，容易重新滑回字符串推断。

### blockers

- 当前无外部 blocker。

## 验证

- `python -m pytest tests/unit/compatibility/test_phase7_loader_function_pointer_decode.py -q`
- `python -m pytest tests/unit/compatibility/test_phase7_loader_generic_method_reference_decode.py -q`
- `python -m pytest tests/unit/compatibility/test_phase4_aot_core_ir_contract.py -q`
- `python -m pytest tests/unit/compatibility/test_mainline_capability_batch_b.py -q`
- `python -m pytest tests/unit/compatibility/test_managed_closure_contract_bundle.py -q`
- `python -m pytest tests/unit/compatibility/test_phase4c_metadata_closure_contract.py -q`
- `python -m pytest tests/unit/compatibility -q` -> `232 passed`

## wiki

- 本阶段未单独沉淀长期 wiki；关键结论已留在 Phase 1 文档中，后续如果 method/field `ldtoken` 可达性边界反复被复用，再考虑抽取到 architecture/wiki。
