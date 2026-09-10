# Phase 5 Brainstorm v1.01

## 问题定义

- Phase 4B-4D 已经把 `AotCoreIr`、`NativeAotLoweringPlanner`、`NativeAotEmitter + Scriban` 的核心 contract 打通，但 capability ledger 还没有重新映射回 owner subjects。
- 当前 declared metadata / registry 仍然缺少显式的 capability family / item 暴露，以及 owner subject / support-state 聚合结果。
- declaration discovery 依赖已有 build 输出时，可能读到过期 assembly，导致 capability coverage 审计出现假绿。

## 本轮判断

- Phase 5 不应该再新开 ownerless subject，也不应该把 coverage 继续留在纯文档层。
- 最小闭环应包含三层：
  - frozen capability ledger 到代码内的 owner/support-state 映射
  - declared catalog / registry 显式带出 capability 元数据与 owner/support-state
  - 覆盖审计使用新鲜 subject assemblies，而不是静默复用过期 build 输出

## 本轮不做

- 不把 `Chaos.TestFramework` 逻辑混入 `AotCoreIr` / planner / emitter。
- 不在本轮重做 dashboard 布局或 release evidence，那是 Phase 6。
- 不重新发明 string 协议；coverage identity 继续以 `family + item` 数值为中心。
