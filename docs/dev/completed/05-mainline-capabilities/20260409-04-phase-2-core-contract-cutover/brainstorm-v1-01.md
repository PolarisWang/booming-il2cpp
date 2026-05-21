# Phase 2 Core Contract Cutover Brainstorm v1.01

Date: 2026-04-09
Status: design-approved-enter-plan

## 1. 目标

本轮 `Phase 2` 不扩 capability 范围，也不提前做完整 lowering arbitration，而是先把已经冻结的 performance-first contract 真正切进主干代码：

- `SemanticWorld` 不能再只是 `LoadedAssemblyModel` 的透传壳
- `Linker` 不能继续承担 concat 归一化这类上游 canonicalization
- `CodeGen` 不能继续只把 `ManagedMethodModel.Body` 搬运成 `typed-il-ir`
- downstream 至少要开始消费显式的 shape/capability contract，而不是只靠 sample-shaped instruction guess

## 2. 约束

- 当前主线仍然不是完整 `C# -> C++ total solution`
- 当前仍是 `Phase A performance-first core mainline`
- `exceptions` 继续保留在 roadmap 强制范围内，但不在本阶段扩实现
- `engine binding` 仍然放在 roadmap 最后
- 本阶段不把 `Phase 3` 的 lowering family / arbitration rewrite 提前混进来

## 3. 方案比较

### 方案 A：只补文档，不改代码 contract

优点：

- 风险最小

缺点：

- `SemanticWorld`/`CodeGen`/`Emitter` 的边界问题完全没有被结构化解决
- 父 roadmap 里的 `Phase 2` 将变成空转

结论：

- 不采用

### 方案 B：最小 contract cutover

做法：

- 在 `SemanticWorldModel` 里正式承接 `CanonicalSubjects / SemanticShapes / CapabilityBundles`
- 让 `SemanticWorld` 生成当前 frozen slice 的 canonical body
- 让 `LinkedWorld` 把这些 contract 正式 threaded 到下游
- 让 `CodeGen` 把 `methodRole / bodyAvailability / capabilities` 落到 `typed-il-ir`
- `Emitter` 先消费这些显式字段做基础校验，但不重写 family 选择主逻辑

优点：

- 改动面可控
- 能把 `Phase 0/1` 的 contract 变成结构性代码边界
- 不会把 `Phase 3` 提前做胖

缺点：

- `Emitter` 仍保留部分 sample-shape selection
- `String.Concat(3)` 的 pairwise canonicalization 暂时仍以当前 proof contract 为中心，不是最终 lowering 设计

结论：

- 采用

### 方案 C：直接连带重写 lowering family / emitter

优点：

- 最终边界更干净

缺点：

- 直接跨进 `Phase 3`
- 回归面和验证成本都明显扩大

结论：

- 当前不采用

## 4. 特别拍板

### 4.1 `String.Concat(string,string,string)` 放哪一层

本轮结论：

- 先从 `CodeGen` 移出
- 在 `Phase 2` 里前移到 `SemanticWorld` 的 canonical body 构建
- 保持现有 `typed-il-ir` sample 对 pairwise concat 的冻结结果不变

说明：

- 这不是对长期 lowering family 设计的最终拍板
- 只是为了让当前 frozen slice 的 canonicalization 先回到上游 contract 层
- 更完整的 legality filter + performance arbitration 仍在 `Phase 3`

### 4.2 `Emitter` 在本阶段的边界

本轮结论：

- 不强行重写 `PlanKind` 选择主逻辑
- 但要开始消费显式 `methodRole / bodyAvailability / capabilities`
- 让 downstream 至少不再完全只靠隐式 method shape 猜测

## 5. 输出

`Phase 2` 进入 plan 的正式结论是：

1. 采用最小 contract cutover 路线
2. `SemanticWorld` 正式承接 canonical subjects / shapes / capabilities，并前移当前 frozen slice 的 body canonicalization
3. `LinkedWorld` 继续把这些 contract 明确传给 `CodeGen`
4. `CodeGen` 产出的 `typed-il-ir` 必须带出显式 `methodRole / bodyAvailability / capabilities`
5. `Emitter` 只做有限 contract consumption，不提前展开完整 lowering arbitration 重写
