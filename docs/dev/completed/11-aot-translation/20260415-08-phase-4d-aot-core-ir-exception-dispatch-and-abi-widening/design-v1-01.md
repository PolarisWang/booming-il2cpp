# Phase 4D 设计

## 目标

把 `AotCoreIr` 从当前“static direct-call + limited runtime service”能力面，扩展到可表达并可 lowering 的：

- instance / virtual / interface dispatch
- external runtime call / bridge call
- exception region lowering contract
- 更宽的参数与返回值 ABI

## 设计边界

- contract 先行：先扩展 `Chaos.IL2CPP.Contracts` 与 RED 测试，再动 lowering。
- planner 继续消费 `AotCoreIr`，不回绑 loader/world 细节。
- emitter 继续只负责 Scriban 渲染，不接受新的字符串拼接协议。
- `Chaos.TestFramework` 不进入 `AotCoreIr` / planner / emitter；proof 只通过 subject 与 additional assembly 引入。

## 切片顺序

1. dispatch widening
2. exception lowering
3. ABI widening

## retained proof 策略

- dispatch widening 优先以 `DispatchProofEntry` 为 owner proof。
- 如果现有 proof 形状包含 Phase 4D 无关噪音，例如字符串或多余对象协议，允许重塑为更窄的 retained shape，但必须保持 capability 目标清晰。
- exception / ABI widening 如无合适 retained proof，可在 `SolutionCorePack` 现有 feature slice 下补最小 proof，不新增临时 subject。
