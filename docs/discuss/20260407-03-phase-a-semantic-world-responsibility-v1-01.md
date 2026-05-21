# Phase A SemanticWorld 职责设计 v1.01

Date: 2026-04-07
Status: design-discuss

## 1. 问题

在当前 pipeline 里，`SemanticWorld` 这一层几乎还是一个 passthrough：

- 输入：`LoadedAssemblyModel`
- 输出：`SemanticWorldModel`
- 当前行为：基本原样透传 `types / fields / properties / methods`

这会带来三个直接问题：

- `Loader` 和 `SemanticWorld` 的边界不清，导致“输入解析”和“语义归一化”混在一起
- `Linker` 被迫继续承担本应由 world 提供的语义判断
- `CodeGen / NativeReferenceProofEmitter` 只能继续靠 proof-shape 识别路径，而不是消费稳定的语义对象

所以这一轮的核心问题不是“SemanticWorld 要不要存在”，而是：

- `SemanticWorld` 到底应该承担到什么程度
- 才能既帮助 `Phase A` 收口，又不把 `Phase B` 的语义扩张提前做爆

## 2. 设计目标

本轮对 `SemanticWorld` 的设计目标只有三个：

1. 让它真正成为 `Loader` 和 `Linker` 之间的独立层
2. 让它承接当前 proof spine 已经涉及的 canonical semantic objects
3. 不在这一轮提前扩张到异常、接口分发、虚调用、数组、boxing 等新语义家族

## 3. 三种方案

## 3.1 方案 A：保持薄层，只做名称与结构透传

### 方案描述

- `Loader` 继续负责读 metadata、方法体、泛型实例化
- `SemanticWorld` 只做轻量包装
- 主要语义判断仍留在 `Linker`、`CodeGen`、`Emitter`

### 优点

- 改动最小
- `Phase A` 风险最低

### 缺点

- 只是把当前问题继续往后拖
- `Linker` 与 `CodeGen` 仍会继续沾染输入侧语义
- 后续讨论 `retained world`、`lowering family`、`.NET 10` 输入兼容时会反复卡在“这到底该在 world 还是下游做”

### 判断

- 不推荐

这条路短期省事，但会让 `Phase A` 看似结束、实则把核心边界问题留到后面反复返工。

## 3.2 方案 B：把 SemanticWorld 提升为“当前支持切片的 canonical semantic layer”

### 方案描述

`SemanticWorld` 不追求在这一轮承接所有未来语义，而是只承接：

- 当前 proof spine 已经真实覆盖到的语义切片
- 且这些切片要以样例无关的 canonical object 形式表达

它需要输出的不是“更漂亮的 LoadedAssembly 副本”，而是：

- 规范化的 type/member identity
- 规范化的 method/import/member shape
- 规范化的 semantic operation stream
- 规范化的 current-slice capability / requirement hints

### 优点

- 正好匹配 `Phase A` 的目标
- 能把 `Loader`、`Linker`、`CodeGen` 的职责重新拉开
- 能为后续 `Phase B` 新语义扩张提供稳定插槽

### 缺点

- 比纯 passthrough 明显更复杂
- 需要先定义一套“当前切片”的 semantic vocabulary

### 判断

- 推荐

这条路最符合当前主线：不虚张声势做 full semantic engine，但也不继续让 world 空转。

## 3.3 方案 C：直接把 SemanticWorld 做成长期 full semantic engine

### 方案描述

在这一轮就让 `SemanticWorld` 一次性承接：

- CFG / 多基本块
- 异常模型
- 接口 / 虚调用
- 数组 / boxing
- 泛型扩张
- 更强的 reflection / interop 语义

### 优点

- 长期看最完整

### 缺点

- 这已经不是 `Phase A`
- 会直接把 `Phase B` 的工作提前并打散
- 当前 proof spine 的稳定化反而会被拖慢

### 判断

- 明确不建议

这条路会让“补内核”和“扩语义”重新混在一起。

## 4. 推荐结论

推荐采用：

- **方案 B：SemanticWorld 作为“当前支持切片的 canonical semantic layer”**

一句话定义：

- `Loader` 负责“读出来”
- `SemanticWorld` 负责“讲清楚它在当前支持子集里到底意味着什么”
- `Linker` 负责“决定保留什么、依赖什么”
- `MetadataWriter` 负责“把 linked world 写成 registration / manifest”
- `CodeGen` 负责“基于 linked world 选择 lowering family 并生成结果”

## 5. 推荐职责边界

## 5.1 Loader 负责什么

`Loader` 只负责输入解析，不负责 policy。

建议它长期只承担：

- 读取 PE / metadata / method body
- 组装 raw type / field / property / method records
- 读取 raw IL instruction stream
- 读取 import metadata
- 读取当前支持子集内必要的 generic instantiation 信息

它不应承担：

- dependency reason
- retained world 策略
- lowering family 判断
- capability 分类
- “这个 shape 在当前主线里是否可支持”的业务级结论

## 5.2 SemanticWorld 负责什么

`SemanticWorld` 应该承接“当前支持切片的语义归一化”。

建议它至少负责五类东西。

### A. canonical identity

- 类型、字段、属性、方法、参数的 canonical subject identity
- definition identity 与 instantiated identity 的统一表示
- declaring owner / member binding 的稳定表达

### B. canonical member shape

- type shape：reference type / generic instantiation / imported type 引用
- method shape：static / instance / constructor / imported method
- property/field shape：instance field、property query target 等当前切片已用能力

### C. canonical operation stream

当前不必上 CFG，但不应再只保留 raw IL op。

建议 `SemanticWorld` 为当前已支持切片输出一层 canonical semantic operations，例如：

- `load-string`
- `load-arg`
- `new-object`
- `call-static`
- `call-instance`
- `read-field`
- `write-field`
- `load-token`
- `return`
- `return-const-i4`

核心要求不是命名本身，而是：

- 这些 operation 应体现“语义角色”
- 而不是下游还要靠 raw opcode 序列和位置猜

### D. current-slice capability hints

`SemanticWorld` 不应直接做 retained world 决策，但可以输出当前切片级 capability hints，例如：

- `uses-imported-method`
- `uses-reflection-query`
- `uses-field-backed-instance-state`
- `uses-string-concat`
- `requires-console-string-output`

这些 hint 的作用是：

- 帮 `Linker`、`CodeGen` 看懂当前 world 里有哪些语义族
- 但不直接替它们做后续选择

### E. semantic diagnostics

`SemanticWorld` 应承担“在语义层就能看出来的问题”的诊断：

- 当前支持子集外的 opcode
- 当前支持子集外的 member/reference shape
- 当前 world 无法规范化的 generic/materialization 情况

这样后续失败就不会都被推迟到 `Linker` 或 `CodeGen`

## 5.3 Linker 不再负责什么

如果采用推荐方案，`Linker` 就不应再继续承担这些责任：

- 从 raw instruction 猜 semantic meaning
- 用 proof-shape 假设去推方法角色
- 用字符串表驱动的方式隐式补齐本该属于 world 的 canonicalization

`Linker` 应收窄为：

- 基于 semantic world 做 retained world
- 基于 semantic capability / reference graph 计算 external dependencies
- 给已决定保留的依赖附加 reason

## 5.4 CodeGen / NativeReferenceProofEmitter 不再负责什么

`CodeGen` 与 `Emitter` 也不应继续承担：

- 从 raw IL 序列反推“这是不是某个样例”
- 靠 instruction index 硬猜某个 method 的 semantic role

它们应逐步转向：

- 消费 `LinkedWorldModel` 中已经被 world 规范化过的 semantic method shape
- 基于 lowering family / capability slice 选择模板或生成路径

注意：

- 这一轮不要求把 `Emitter` 完全抽象成最终长期形态
- 但至少应该让它从“proof-shape detection”转向“capability/lowering family selection”

## 6. 推荐的数据模型方向

这一轮不必立刻定死最终 schema，但建议 `SemanticWorldModel` 未来至少往下面方向长：

- `Types`
  - canonical type identity
  - definition/instantiation relationship
  - type capabilities
- `Members`
  - field / property / method 的 canonical member shape
- `Methods`
  - canonical method kind
  - canonical semantic operation stream
  - import shape
  - semantic requirements
- `Diagnostics`
  - normalization failures / unsupported-slice reasons

也就是说，未来最好不再只是：

- `Types`
- `Fields`
- `Properties`
- `Methods`

这种“loader 输出原样重放”的模型。

## 7. 对当前 proof spine 的具体影响

如果按推荐方案推进，短期应该先服务当前四类切片：

1. `constructorThenInstanceCall`
   - `HelloWorldObject` 一类
2. `staticCallCtorGetter`
   - `GenericEcho` 一类
3. `reflectionQueryMinimal`
   - `ReflectionLite` 一类
4. `pinvokeDllImportMinimal`
   - `PInvokeLite` 一类

关键点是：

- 这些仍然可以继续存在
- 但它们不应再被识别为“样例名字”
- 而应被识别为“当前支持的 lowering family / semantic slice”

## 8. 不在这一轮进入的内容

为避免 `Phase A` 膨胀，以下明确不在本轮 `SemanticWorld` 设计里展开：

- 异常模型
- 多基本块 CFG
- interface dispatch
- virtual dispatch
- arrays
- boxing / unboxing
- 更宽的 generic sharing

这些都应该进入后续 `Phase B` 的语义扩张专题。

## 9. 当前建议

如果继续沿这条线细化，下一份最自然的讨论稿应该是：

- `Phase A linker/codegen/emitter capability boundary`

因为一旦 `SemanticWorld` 的职责讲清楚，下一步就该回答：

- 哪些 capability / requirement 由 world 输出
- 哪些 retained/dependency 决策属于 linker
- 哪些 lowering family 判断属于 codegen/emitter
