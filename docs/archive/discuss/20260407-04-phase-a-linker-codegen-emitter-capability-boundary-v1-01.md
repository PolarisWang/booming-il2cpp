# Phase A Linker / CodeGen / Emitter Capability Boundary v1.01

Date: 2026-04-07
Status: design-discuss

## 1. 问题

在前一轮讨论里，`SemanticWorld` 的职责已经开始收口为“当前已支持切片的 canonical semantic layer”。  
接下来的关键问题是：

- `SemanticWorld` 把能力与需求讲清楚之后，谁来决定 retained/dependency
- 谁来决定 lowering family
- 谁来把 lowering family 变成 native proof 的具体模板与生成产物

从当前代码看，职责仍然明显混杂：

- `LinkerStage`
  - 还在做 `NormalizeDependencySubjectId(...)`
  - 还在用 `ResolveDependencyReason(...)` 把具体 subject id 直接映射成 reason
- `CodeGenStage`
  - 还在把 `String.Concat(string,string,string)` 现场改写成两次 pair concat
- `NativeReferenceProofEmitter`
  - 还在通过 `Is...EntryPointShape(...)`
  - `Validate...Shape(...)`
  - 指令下标探测
  - 反推“这到底是哪一种 proof”

这意味着当前 pipeline 还不是：

- `SemanticWorld` 负责语义归一化
- `Linker` 负责 retained world
- `CodeGen` 负责 lowering
- `Emitter` 负责渲染

而是下游多个阶段共同分摊“语义解释”。

## 2. 这一轮要回答什么

这一轮不是讨论新增异常、数组、虚调用、boxing 等新语义。  
这一轮只讨论 `Phase A` 范围内，如何把当前已支持切片的能力边界收清楚。

目标只有三个：

1. 让下游阶段不再靠 proof-shape 猜语义
2. 让新增一个 lowering family 时，改动点是可预测的
3. 让 `Phase B` 扩语义时，不会再次把职责打散到 `Linker / CodeGen / Emitter`

## 3. 三种方案

## 3.1 方案 A：把能力判定和 lowering 选择继续收进 Linker

### 方案描述

- `SemanticWorld` 负责 canonical identity / member shape / operation stream
- `Linker` 除 retained world 之外，再额外负责：
  - capability 分类
  - dependency reason 归因
  - lowering family 选择
  - 生成供 `CodeGen / Emitter` 直接消费的 lowered-ready world
- `CodeGen` 只负责把 linker 的决定转成 artifact
- `Emitter` 只负责模板渲染

### 优点

- 与当前现状最接近
- `LinkerStage` 本来就已经在做 dependency normalization / reason mapping
- 可以比较快地把 `Emitter` 从 shape detection 中拉出来

### 缺点

- `Linker` 会膨胀成“下游唯一大脑”
- retained policy 和 lowering policy 被绑死在同一层
- 后续 `Phase B` 一旦新增语义家族，`Linker` 会成为最大变更热点
- `MetadataWriter / CodeGen / runtime helper` 的扩展点会继续依赖 linker 内部判断

### 判断

- 不推荐

这条路短期省事，但会把 `Linker` 做成新的“大泥球”，只是把现在散落的复杂度集中转移，并没有把边界真正做干净。

## 3.2 方案 B：`SemanticWorld` 产出 capability/requirement，`Linker` 做 retained world，`CodeGen` 选择 lowering family，`Emitter` 只渲染

### 方案描述

这是更严格的分层方案：

- `SemanticWorld`
  - 输出 canonical semantic objects
  - 输出 method-level capability / requirement hints
  - 输出当前切片可解释的 semantic diagnostics
- `Linker`
  - 基于 semantic world 计算 retained world
  - 基于 capability / requirement 计算 external dependency closure
  - 给 retained/dependency 附上显式 reason
- `CodeGen`
  - 基于 linked world 选择 lowering family
  - 把 lowering family 所需的 method roles / tokens / symbols / literals 组装成 plan
  - 输出 typed-il / lowering-plan / code-registration 等 artifact
- `NativeReferenceProofEmitter`
  - 只消费显式 plan
  - 只负责模板选择与渲染
  - 最多做 plan 完整性校验，不再反向猜语义

### 优点

- retained policy 与 lowering policy 清晰拆开
- `CodeGen` 真正成为 lowering 层，而不是 artifact 序列化层
- `Emitter` 可以从“proof-shape 识别器”退回成“后端渲染器”
- 更符合 `Phase A` 目标：不扩新语义，只把当前已支持切片收成可扩展的主线

### 缺点

- 相比方案 A，需要更明确地定义 capability / requirement vocabulary
- `CodeGen` 的职责会比现在更重，不再只是被动导出 typed-il
- 需要接受一个现实：某些 dependency 不是“从原始 IL 直接看到”，而是“由 lowering policy 导出的运行时需求”

### 判断

- 推荐

这是当前最稳的一条路。  
它既没有把 `SemanticWorld` 过早做成 full semantic engine，也没有继续让 `Emitter` 保持 proof-first 的硬编码推断模式。

## 3.3 方案 C：保留当前 proof-first 结构，只把 shape detection 文档化

### 方案描述

- `SemanticWorld` 继续只做轻度归一化
- `Linker` 继续做 dependency normalization / reason mapping
- `CodeGen` 继续做局部 canonicalization
- `Emitter` 保留 `TryCreate...LoweringPlan(...)` + `Validate...Shape(...)` 主路径
- 只是把现有约定写成正式文档

### 优点

- 改动最小
- 最不容易影响当前 `HelloWorldObject / GenericEcho / ReflectionLite / PInvokeLite`

### 缺点

- 本质上没有解决问题
- 仍然是“样例通过”而不是“核心层收口”
- `Phase B` 一旦扩语义，很快又会退化成新的 case-by-case 硬编码
- roadmap 看起来前进，实际上核心边界没有建立

### 判断

- 明确不建议

这条路适合临时救火，不适合作为 IL2CPP mainline 的 Phase A 收口路径。

## 4. 推荐结论

推荐采用：

- **方案 B：`SemanticWorld` 产出 capability/requirement，`Linker` 做 retained world，`CodeGen` 选择 lowering family，`Emitter` 只渲染**

一句话边界定义：

- `SemanticWorld` 负责“当前支持切片在语义上是什么”
- `Linker` 负责“为了保留与运行这些语义，需要留下什么”
- `CodeGen` 负责“这些已保留语义在当前 backend 上走哪一种 lowering family”
- `Emitter` 负责“按已选 lowering family 生成最终 native proof 产物”

## 5. 推荐边界的具体拆法

## 5.1 `SemanticWorld` 应该产出什么

建议最少产出五类对象：

- canonical identity
  - type / field / property / method / parameter 的稳定身份
- canonical member shape
  - constructor / static / instance / imported member / getter / field-backed member 等当前切片需要的角色信息
- canonical semantic operation stream
  - 不是 raw opcode 序列
  - 而是当前切片下游真正消费的 semantic operations
- method-level capability / requirement
  - 例如：
  - `requires-console-string-output`
  - `requires-string-concat`
  - `requires-reflection-query`
  - `requires-imported-call`
  - `uses-instance-field-state`
- semantic diagnostics
  - 哪些语义无法在当前支持切片内归一化

它不应该负责：

- retained world 决策
- final dependency closure
- lowering family 选择
- 模板/后端渲染

## 5.2 `Linker` 应该负责什么

`Linker` 应该收窄成 retained/dependency 层：

- 按 entry point 和 semantic requirements 形成 retained methods/types
- 基于 capability / requirement 决定 external dependencies
- 给每个 dependency 附加 reason
- 给 retained members 保留稳定顺序和 retained reason 语义

它应该停止承担：

- 从 raw instruction 反推语义角色
- 用硬编码 subject id 做 canonicalization
- 用 `String.Concat(3) -> pair concat` 这种 lowering 级改写来替代语义建模

更准确地说：

- `Linker` 仍然可以给 dependency 起 reason
- 但 reason 的来源应是 semantic requirement 或 declarative policy
- 而不是直接把某个具体 subject id 当成规则入口

## 5.3 `CodeGen` 应该负责什么

`CodeGen` 在推荐方案里不再只是“导出 typed-il 文件”，而应该成为 lowering 选择层：

- 消费 linked semantic world
- 按 capability / requirement / member roles 选择 lowering family
- 组装 lowering plan 所需的显式输入：
  - plan kind
  - entry / ctor / getter / instance / import method roles
  - metadata tokens
  - method symbols
  - required icalls / helpers
  - literals / expected output
- 输出下游稳定消费的 artifact

它应该停止承担：

- 在晚阶段临时改写 instruction 序列来“补 canonicalization”
- 把 typed-il 当作原始 IL 搬运结果

对当前代码最直接的含义是：

- `CodeGenStage.NormalizeInstructions(...)` 这类逻辑不应长期停留在这里
- 如果某个行为属于“语义归一化”，应上移到 `SemanticWorld`
- 如果某个行为属于“lowering 选择”，应显式变成 lowering policy，而不是 instruction list 的现场修补

## 5.4 `NativeReferenceProofEmitter` 应该负责什么

`Emitter` 应退回成一个纯后端：

- 根据显式 `PlanKind` 或未来的 `LoweringFamily`
- 选择模板
- 渲染 translation unit
- 校验 plan 是否完整

它不应再负责：

- `Is...EntryPointShape(...)`
- `Validate...Shape(...)` 作为语义发现主路径
- 按指令下标去猜 method role / field role / literal role

允许保留的 only-if：

- 在过渡阶段，`Emitter` 可以保留少量 defensive validation
- 但这些 validation 的前提是“校验 plan 和输入一致”
- 而不是“重新从 typed-il 里识别这到底是哪一种 proof”

## 6. 推荐的 Phase A 迁移解释

如果采用推荐方案，当前几个混杂点可以这样理解：

- `LinkerStage.NormalizeDependencySubjectId(...)`
  - 不应继续作为 linker 内的字符串修补逻辑长期存在
  - 应被替换成 semantic requirement 或 declarative dependency policy
- `LinkerStage.ResolveDependencyReason(...)`
  - 可以保留“reason 归因”这件事
  - 但应该从 capability/requirement 出发，而不是直接从具体 subject id 出发
- `CodeGenStage.NormalizeInstructions(...)`
  - 不应继续承担语义归一化主职责
  - 应拆成：
  - world-level canonical semantic operations
  - 或 codegen-level lowering policy
- `NativeReferenceProofEmitter.TryCreate...LoweringPlan(...)`
  - 长期应从“识别 lowering”改成“消费 lowering”
  - 也就是由上游把 plan 讲清楚，emitter 只负责落地

## 7. 对 roadmap 的直接影响

这份边界结论会直接影响 `Phase A` 的执行顺序：

1. `SemanticWorld` 先把 canonical semantic layer 收清楚
2. `Linker` 退回 retained/dependency 角色
3. `CodeGen` 接手 lowering family 选择与 plan 组装
4. `Emitter` 从 proof-shape detector 退回模板后端

这样做的价值是：

- `Phase A` 真正完成“核心层收口”
- `Phase B` 新增语义时，可以明确知道改动入口是：
  - world vocabulary
  - dependency policy
  - lowering family
  - runtime/helper

而不是再次在 `Linker / CodeGen / Emitter` 三处同时打补丁。

## 8. 下一步最值得细化的话题

基于这份讨论，下一份最自然的文档应是二选一：

1. `Phase A lowering family vocabulary`
   - 当前四类 proof 切片应该如何正式命名为 capability/lowering families
2. `Phase A dependency policy contract`
   - semantic requirement 如何映射成 retained/dependency reason 与 external dependency

如果只能先选一个，我更建议先写：

- **`Phase A lowering family vocabulary`**

因为只要 lowering family 词汇还没冻结，`CodeGen` 和 `Emitter` 的分界就仍然会摇摆。
