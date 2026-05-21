# Phase 4 Native AOT Planner Entry ABI And Scriban Widening Brainstorm v1.01

Date: 2026-04-15 22:37:41 +08:00
Status: active

## 1. 目标

在 Phase 3 完成 `structured EH` 闭环之后，继续扩大 native planner / emitter 真正可消费的 widened shape，并保持生成出口仍然是 `NativeAotEmitter + Scriban`。

## 2. 当前批次为什么先做 generic dispatch

- Phase 0 ledger 已经把 `GenericVirtualDispatch` / `GenericInterfaceDispatch` 标成 native toolchain gap。
- 当前 retained dispatch proof 还是非泛型语义壳，不能代表真正的 generic dispatch 闭环。
- 这条链路天然覆盖 `AotCoreIr -> NativeAotLoweringPlanner -> NativeAotEmitter + Scriban`，适合作为 Phase 4 的第一批真实 owner driver。

## 3. 本阶段先不做什么

- 不扩 public workload ABI。
- 不把 `Chaos.TestFramework` 逻辑混进 planner / emitter。
- 不在这一步顺带做 benchmark/dashboard/gate 收口，那是 Phase 5。

## 4. 推荐推进顺序

1. 先用 generic virtual/interface dispatch owner proof 锁 RED。
2. 再扩大 planner 的 dispatch slot / generic target 解析与 emitter 模型。
3. 用 artifact-level 测试确认生成代码仍然经 Scriban 落地。
4. 目标回归通过后，再切入 Phase 5。
