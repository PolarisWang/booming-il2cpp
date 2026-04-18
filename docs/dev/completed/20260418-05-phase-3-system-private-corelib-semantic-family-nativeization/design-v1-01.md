# System.Private.CoreLib Semantic Family Nativeization Design v1.01

Date: 2026-04-18 03:14:08 +08:00
Status: design-frozen-for-execution

## 1. 目标

把当前工作收敛成“基于 corelib substrate 的语义族 nativeization 主线”，不再按零散 proof 补洞，而是按语义族 batch 推进 `System.Private.CoreLib` 的整 DLL 收口：

- family partition 明确
- 每个 family 都有 native proof / conformance / benchmark
- runtime-backed/helper contracts 被逐族消费
- 最终把 corelib 推到单 DLL completed 标准

## 2. 边界

### 2.1 本任务负责

- 基于 `System.Private.CoreLib Certification Substrate` 切出 semantic families
- 建立 family-level native proof / conformance / benchmark contract
- 把首批语义族绑定到现有 registry / subject / engineering workload authority
- 为后续 family batch child task 提供稳定执行入口

### 2.2 本任务暂不负责

- core BCL `System.*` 分层推进
- 常用外部 DLL 认证
- hotupdate 消费者验证
- 把首批 execution entry 直接等同于 family 完成判定

## 3. 核心决策

### 3.1 语义族是主执行单元

- 不再按单点 bug 或单条 proof 推进
- 每个 family 必须覆盖一组可以共同收口的语义 obligations

### 3.2 先冻结 family map，再生成 execution entry，再进入实跑

- 第一动作不是直接改 emitter，而是先冻结 family partition 与优先级
- 没有 stable family map，就不能可靠地下发 child tasks
- 没有 execution entry，就不能把 planning substrate 变成可执行 contract

### 3.3 每个语义族都必须具备三类对象

- native proof
- conformance gate
- semantic-family benchmark

### 3.4 首批 execution authority 绑定到 SolutionCorePack

- 首批 batch 固定为 `managed-cil`、`internal-call`、`field-metadata`
- execution entry 直接消费 registry 中的 `declared-unit-test`、`engineering-validation`、`declared-benchmark` 与 `engineering-workload` canonical command
- 当前落地的不是“这些 family 已经完成 native 化”，而是“这些 family 已经拥有正式实跑入口”

## 4. 风险

- family 切分过粗会导致单批次失控
- family 切分过碎会导致调度开销与反复返工
- representative proof 与 full-family coverage 之间仍有缺口，后续必须补 coverage map
- 如果 helper obligations 不能被首批 batch 正确消费，仍可能回补到 corelib substrate
