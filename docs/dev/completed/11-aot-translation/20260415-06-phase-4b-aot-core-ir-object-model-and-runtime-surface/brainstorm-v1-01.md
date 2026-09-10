# Phase 4B NativeAotEmitter Scriban 迁移补充讨论

## 背景

- 原始 `Phase 4B` 计划默认先稳定 `AotCoreIr` contract 与 `NativeAotEmitter` 语义面，再把模板化迁移留到后续阶段。
- 当前要求已提升为：`NativeAotEmitter` 必须在本轮内一起迁到 Scriban。
- 用户最终选择了更激进的方案 3：不是简单给 `NativeAotEmitter` 套模板，而是直接把 Native AOT 生成链重构为 `NativeAotLoweringPlanner + NativeAotEmitter` 双阶段结构。

## 约束

- 不把 `Chaos.TestFramework` 逻辑混入 `AotCoreIr`、`NativeAotLoweringPlanner` 或 `NativeAotEmitter`。
- 不扩大 string protocol；新增 carrier 优先使用枚举和紧凑字段。
- 不新建临时 subject；proof 继续回收到 `subjects/SolutionCorePack`。
- 本轮仍聚焦 `generic-managed-entry` 这一条 Native AOT plan kind，不顺带扩 plan-kind 家族。
- 迁移后的管线必须继续消费 `AotCoreIr`，不能重新耦回 loader / linked world。

## 方案比较

### 方案 1：单一大 Scriban 模板

- 做法：保留现有 `NativeAotEmitter` 架构，仅把整份 translation unit 改成单模板输出。
- 优点：改动路径最短。
- 缺点：模板巨大，method body、runtime helper、type declaration 全耦合；后续扩 opcode 维护成本高。

### 方案 2：分层模板化，但仍只有 Emitter

- 做法：`NativeAotEmitter` 同时承担 template model 构建与 Scriban 渲染，结构上比当前更好，但不新增 planner。
- 优点：比方案 1 干净，改动可控。
- 缺点：语义规划和模板渲染仍混在一个类里，长期仍偏重。

### 方案 3：新增 Planner，Emitter 收窄为纯渲染器

- 做法：
  - 新增 `NativeAotLoweringPlanner`
  - 负责 `AotCoreIr -> NativeAotTemplateModel` 的语义归一、依赖收集、布局整理
  - `NativeAotEmitter` 只负责加载 `.scriban` 模板并渲染结果
- 优点：长期结构最整洁；和现有 `NativeReferenceLoweringPlanner + NativeReferenceProofEmitter` 对齐；后续扩 opcode、helper、layout 更容易。
- 缺点：本轮改动面明显扩大。

## 结论

- 按用户选择，采用方案 3。
- 本轮交付目标升级为：
  - 完成 `NativeAotLoweringPlanner`
  - 完成 `Templates/NativeAot*.scriban`
  - 把 `NativeAotEmitter` 收窄为模板加载与渲染器
  - 在新结构上继续收口 Phase 4B 剩余语义，当前优先 `array covariance`

## 本轮新增验收点

- `NativeAotEmitter` 不再通过大面积 `StringBuilder.AppendLine(...)` 直接生成整份 C++。
- Native AOT 输出显式经过 planner 产生的 template model 与 Scriban 模板。
- 模板缺失、模板解析失败、planner model 缺字段都要显式失败。
- 当前已转绿的 Phase 4B 切片不得因架构迁移回退。
