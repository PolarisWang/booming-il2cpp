---
task_id: 20260415-15-phase-3-structured-control-flow-and-exception-expansion
title: Phase 3 Structured Control Flow And Exception Expansion
task_type: plan
lifecycle_status: completed
phase: completed
created_at: 2026-04-15 22:04:44 +08:00
updated_at: 2026-04-15 22:37:41 +08:00
current_dir: docs/dev/completed/20260415-15-phase-3-structured-control-flow-and-exception-expansion
parent_task_id: 20260415-11-phase-7-aot-core-ir-typed-il-translation-expansion-roadmap
source_task_id: 20260415-11-phase-7-aot-core-ir-typed-il-translation-expansion-roadmap
source_relation: roadmap-child
design_doc: docs/dev/completed/20260415-15-phase-3-structured-control-flow-and-exception-expansion/design-v1-01.md
plan_doc: docs/dev/completed/20260415-15-phase-3-structured-control-flow-and-exception-expansion/plan-v1-01.md
total_tasks: 4
current_task: completed
active: false
---

## 关键文档

- parent_status: `docs/dev/in-progress/20260415-11-phase-7-aot-core-ir-typed-il-translation-expansion-roadmap/STATUS.md`
- parent_roadmap: `docs/dev/in-progress/20260415-11-phase-7-aot-core-ir-typed-il-translation-expansion-roadmap/roadmap-v1-01.md`
- phase_2_status: `docs/dev/completed/20260415-14-phase-2-scalar-conversion-and-abi-carrier-widening/STATUS.md`
- brainstorm: `docs/dev/completed/20260415-15-phase-3-structured-control-flow-and-exception-expansion/brainstorm-v1-01.md`
- design: `docs/dev/completed/20260415-15-phase-3-structured-control-flow-and-exception-expansion/design-v1-01.md`
- plan: `docs/dev/completed/20260415-15-phase-3-structured-control-flow-and-exception-expansion/plan-v1-01.md`

## 当前结论

- Task 1 完成：新增 `tests/unit/compatibility/test_phase3_finally_filter_native_aot.py`，先用真实 `FinallyAndFilterProofEntry::Run()` 锁定了 owner proof 的 RED。
- Task 2 完成：owner proof 去除了与 Phase 3 无关的 `ArgumentException.get_ParamName()` dispatch 噪音，也进一步去掉了 `ldstr/String.Concat` 依赖，改为纯整数顺序编码来验证 `filter/finally` 语义。
- Task 3 完成：`NativeAotLoweringPlanner` 新增了 `filter + nested finally` 形状识别与 lowering 路径；`brtrue` 已补齐；`endfilter` 与 `endfinally` 在专用 structured-EH 路径中闭环。
- Task 4 完成：目标回归与完整 `tests/unit/compatibility -q` 均已通过，Phase 3 退出并切入 `20260415-16-phase-4-native-aot-planner-entry-abi-and-scriban-widening`。

## 最近摘要

- 2026-04-15 22:04:44 +08:00: 创建并激活 `20260415-15-phase-3-structured-control-flow-and-exception-expansion`。
- 2026-04-15 22:15:00 +08:00: 新增 `test_phase3_finally_filter_native_aot.py`，确认当前失败点先撞在 `System.Private.CoreLib/System.ArgumentException::get_ParamName()` unresolved dispatch slot。
- 2026-04-15 22:20:00 +08:00: owner proof 去噪完成，`FinallyAndFilterProofEntry` 改为纯整数顺序验证；RED 从 dispatch blocker 收敛到 “current exception region shape unsupported”。
- 2026-04-15 22:31:00 +08:00: `NativeAotLoweringPlanner` 完成 `filter + nested finally` widening，`test_phase3_finally_filter_native_aot.py` 转绿。
- 2026-04-15 22:37:41 +08:00: 运行 targeted compatibility 回归与完整 `tests/unit/compatibility -q`；结果分别为 `18 passed` 与 `239 passed`。

## 下一步

- next_action: 无。本 child task 已完成，父 roadmap 切入 `20260415-16-phase-4-native-aot-planner-entry-abi-and-scriban-widening`。
- owner: codex
- trigger: 无。

## 风险 / 阻塞

### risks

- Phase 3 当前闭环聚焦 `filter + nested finally`；`fault`、更广义 `switch` 和更复杂 EH shape 仍然留给后续批次，不应误判为“全部 structured EH 已完成”。
- 这轮 finally 采用 scope-guard 路径承接，后续若 finally body 引入可抛异常或更复杂控制流，需要在 Phase 4/后续批次继续审视 contract 边界。

### blockers

- 当前无外部 blocker。

## 验证

- `python -m pytest tests/unit/compatibility/test_phase3_finally_filter_native_aot.py -q` -> `3 passed`
- `python -m pytest tests/unit/compatibility/test_phase2_small_integer_carrier_native_aot.py tests/unit/compatibility/test_phase2_floating_point_carrier_native_aot.py tests/unit/compatibility/test_phase4d_exception_aot_core_ir.py tests/unit/compatibility/test_phase4d_exception_native_aot.py tests/unit/compatibility/test_phase3_finally_filter_native_aot.py tests/unit/compatibility/test_mainline_capability_batch_a.py tests/unit/compatibility/test_mainline_capability_batch_b.py -q` -> `18 passed`
- `python -m pytest tests/unit/compatibility -q` -> `239 passed`

## wiki

- 本阶段先不单独沉淀长期 wiki；Phase 3 的有效边界已经收口到 task 文档与父 roadmap。若后续 structured-EH lowering contract 稳定到可复用层级，再抽取到 architecture/wiki。
