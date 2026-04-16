# Phase 4 Native AOT Planner Entry ABI And Scriban Widening Design v1.01

Date: 2026-04-15 22:37:41 +08:00
Status: active

## 1. 设计目标

扩大 native AOT planner / emitter 对 widened method shape 的真实消费能力，同时保持：

- `AotCoreIr -> NativeAotLoweringPlanner -> NativeAotEmitter + Scriban`
- 不引入新的字符串协议面
- 不把测试框架逻辑混进 AOT core / planner / emitter

## 2. 当前批次边界

本批次优先处理：

- generic virtual dispatch
- generic interface dispatch
- 与 dispatch widening 直接耦合的 planner / emitter 模型补齐
- Scriban 生成产物级验证

本批次暂不处理：

- public workload ABI 扩张
- benchmark/dashboard/release gate 收口
- 与 dispatch 无关的更大范围 byref / entry ABI 族群

## 3. Owner Driver

### 3.1 真实 driver

- `CoreRuntimeFeatures/DispatchProofEntry::Run()`
- `CoreRuntimeFeatures/InterfaceDispatchProofEntry::Run()`

### 3.2 当前不足

- 现有 retained proof 仍然是非泛型 dispatch 语义壳，无法代表 Phase 0 ledger 中的 generic dispatch gap。
- Phase 4 需要把 owner proof 提升到 generic virtual/interface 形状，再用 native AOT 真实闭环。

## 4. 最小 contract

本批次至少要满足：

- `AotCoreIr` 中 generic dispatch slot 的 subject / target shape 保持稳定
- planner 能为 generic virtual/interface slot 收集真实 lowerable target
- emitter 模型仍然通过 Scriban 模板落地
- artifact-level 测试能证明 generated C++ 已包含 widened generic dispatch 路径

## 5. 验证策略

- 先 RED：新增 generic dispatch compatibility driver
- 再 GREEN：补齐 planner / emitter widening
- 再跑 targeted suites
- 阶段结束前至少跑一轮完整 `tests/unit/compatibility -q`
