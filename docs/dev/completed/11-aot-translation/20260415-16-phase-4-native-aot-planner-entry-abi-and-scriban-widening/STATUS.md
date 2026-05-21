---
task_id: 20260415-16-phase-4-native-aot-planner-entry-abi-and-scriban-widening
title: Phase 4 Native AOT Planner Entry ABI And Scriban Widening
task_type: plan
lifecycle_status: completed
phase: completed
created_at: 2026-04-15 22:37:41 +08:00
updated_at: 2026-04-15 23:14:13 +08:00
current_dir: docs/dev/completed/20260415-16-phase-4-native-aot-planner-entry-abi-and-scriban-widening
parent_task_id: 20260415-11-phase-7-aot-core-ir-typed-il-translation-expansion-roadmap
source_task_id: 20260415-11-phase-7-aot-core-ir-typed-il-translation-expansion-roadmap
source_relation: roadmap-child
design_doc: docs/dev/completed/20260415-16-phase-4-native-aot-planner-entry-abi-and-scriban-widening/design-v1-01.md
plan_doc: docs/dev/completed/20260415-16-phase-4-native-aot-planner-entry-abi-and-scriban-widening/plan-v1-01.md
total_tasks: 4
current_task: completed
active: false
---

## 关键文档

- parent_status: `docs/dev/completed/20260415-11-phase-7-aot-core-ir-typed-il-translation-expansion-roadmap/STATUS.md`
- parent_roadmap: `docs/dev/completed/20260415-11-phase-7-aot-core-ir-typed-il-translation-expansion-roadmap/roadmap-v1-01.md`
- phase_3_status: `docs/dev/completed/20260415-15-phase-3-structured-control-flow-and-exception-expansion/STATUS.md`
- brainstorm: `docs/dev/completed/20260415-16-phase-4-native-aot-planner-entry-abi-and-scriban-widening/brainstorm-v1-01.md`
- design: `docs/dev/completed/20260415-16-phase-4-native-aot-planner-entry-abi-and-scriban-widening/design-v1-01.md`
- plan: `docs/dev/completed/20260415-16-phase-4-native-aot-planner-entry-abi-and-scriban-widening/plan-v1-01.md`

## 当前结论

- Task 1 完成：`DispatchProof` / `InterfaceDispatchProof` 的 retained owner driver 已切到真实 generic virtual/interface dispatch，并通过新增 compatibility 测试锁定了 native AOT 真实 RED。
- Task 2 完成：`NativeAotLoweringPlanner` 新增 reference-type 接口实现采集与 instantiated generic base/interface slot 匹配，generic virtual/interface dispatch 不再只依赖非泛型定义命中。
- Task 3 完成：`LinkerStage` 补齐 `newobj -> stloc -> ldloc -> callvirt` receiver 追踪，`NativeAotEmitter + Scriban` 生成出口保持不变，artifact-level 断言已覆盖 widened generic dispatch 产物。
- Task 4 完成：目标回归与完整 `tests/unit/compatibility -q` 均已通过，Phase 4 可以退出并切入 `20260415-17-phase-5-owner-subject-benchmark-and-gate-closure`。

## 最近摘要

- 2026-04-15 22:37:41 +08:00: `20260415-15` 完成并归档；父 roadmap 切入 `20260415-16`。
- 2026-04-15 22:52:00 +08:00: `test_phase4_generic_dispatch_native_aot.py` 锁定 generic virtual/interface dispatch RED；失败点收敛到 planner 只比较非泛型基类定义、未比较接口实现，以及 linker receiver 追踪过窄。
- 2026-04-15 23:04:00 +08:00: `NativeAotLoweringPlanner` 完成 generic base/interface slot widening，`LinkerStage` 完成局部变量接收者追踪，目标 compatibility suites 全部转绿。
- 2026-04-15 23:14:13 +08:00: 运行完整 `tests/unit/compatibility -q`，结果为 `242 passed`；Phase 4 标记完成，转入 Phase 5 收口 owner subject / benchmark / gate。

## 下一步

- next_action: 无。本 child task 已完成，父 roadmap 切入 `20260415-17-phase-5-owner-subject-benchmark-and-gate-closure`。
- owner: codex
- trigger: 无。

## 风险 / 阻塞

### risks

- Phase 4 本轮闭环的是 generic virtual/interface dispatch；更大范围的 byref / entry ABI widening 仍需按 capability batch 继续拆分，不能误判为整个 planner/emitter contract 已收口。
- 虽然 generic dispatch 已打通，但 owner subject / benchmark / unsupported/gate 层还没有同步收口；如果不进入 Phase 5，外层证据仍会继续失真。

### blockers

- 当前无外部 blocker。

## 验证

- `python -m pytest tests/unit/compatibility/test_phase4_generic_dispatch_native_aot.py -q` -> `3 passed`
- `python -m pytest tests/unit/compatibility/test_phase4_generic_dispatch_native_aot.py tests/unit/compatibility/test_phase4d_dispatch_native_aot.py tests/unit/compatibility/test_phase4d_dispatch_aot_core_ir.py tests/unit/compatibility/test_interface_dispatch_aot_runtime_completeness.py -q` -> `10 passed`
- `python -m pytest tests/unit/compatibility -q` -> `242 passed`

## wiki

- 本阶段先不单独沉淀长期 wiki；Phase 4 的有效结论已经收口到 task 文档与父 roadmap。若后续 generic dispatch lowering contract 在更多 capability batch 中保持稳定，再抽取到 architecture/wiki。
