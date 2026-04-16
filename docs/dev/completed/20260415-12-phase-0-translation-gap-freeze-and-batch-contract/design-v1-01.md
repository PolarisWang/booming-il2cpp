# Phase 0 Translation Gap Freeze And Batch Contract Design v1.01

Date: 2026-04-15 20:32:24 +08:00

## 1. 设计目标

- 冻结当前 `typed-il -> AotCoreIr -> planner -> emitter` 的真实 gap。
- 用 capability-first 视角建立 batch，而不是只按源码文件切分。
- 为 Phase 1-4 提供稳定的 child-task 边界。

## 2. 输出结构

本任务输出一份 `gap-freeze-v1-01.md`，至少包含：

- gap group
- stage tags
- 代码锚点
- 受影响 capability / owner subject
- 对应 child task
- 当前优先级
- deferred 理由
- 测试锚点

## 3. 分组规则

### Group A: Loader Decode And Metadata Shape

- 覆盖 `LoaderStage`、`MetadataTypeResolver`、`TypeProviders`
- 重点是 opcode / handle / signature shape 能否进入 typed-il

### Group B: Core IR Carrier And Dispatch Contract

- 覆盖 `AotCoreIrLowering`
- 重点是 ABI carrier、target reference、dispatch metadata、exception contract 是否完整

### Group C: NativeAOT Planner Widening

- 覆盖 `NativeAotLoweringPlanner`
- 重点是 opcode legalization、dispatch、ABI、structured control flow

### Group D: NativeAOT Entry ABI And Template Boundary

- 覆盖 `NativeAotEmitter` 与 `Templates/NativeAot*.scriban`
- 重点是 widened planner model 能否被 emitter/template 消费

## 4. 批次归属策略

- 只要缺口首先阻塞 typed-il 输入面，就归 Phase 1。
- 只要缺口首先阻塞 widened scalar / ABI carrier，就归 Phase 2。
- 只要缺口首先阻塞 structured control flow / EH，就归 Phase 3。
- 只要缺口首先阻塞 planner model / emitter template / entry ABI，就归 Phase 4。
- 只要缺口只是“底层已支持但没有 owner evidence”，就归 Phase 5。

## 5. 验收标准

- 每个主要未支持点都能归到唯一 child task。
- 不再出现 Phase 1-4 的边界重叠或遗漏。
- 父 roadmap 的 child mapping 与 active 状态同步更新。
