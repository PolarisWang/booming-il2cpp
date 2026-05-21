# Phase 4D 头脑风暴

- Phase 4C 已把 generic contract 接进 `AotCoreIr`，但真实 native translation surface 仍缺三块主能力：
  - exception lowering
  - dispatch widening
  - ABI widening
- 这三块必须继续沿用用户确认过的结构：
  - `NativeAotLoweringPlanner`
  - `NativeAotEmitter`
  - `Templates/NativeAot*.scriban`
- `Chaos.TestFramework` 继续只作为 `src/reference/Chaos.TestFramework` 中的真实 managed dependency，不得进入 `AotCoreIr` / planner / emitter。
- 第一刀优先做 dispatch widening，原因：
  - 现有 retained `DispatchProof` 已经提供了最直接的 owner-subject 验证入口
  - dispatch widening 也是 generic / interface / external runtime call 的基础前置
- exception lowering 与 ABI widening 不应被静默并入 dispatch patch；需要单独有 typed contract、RED 测试与 proof 闭环。
