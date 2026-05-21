# Phase A SemanticWorld Diagnostics Contract v1.01

Date: 2026-04-08
Status: design-discuss

## 1. 问题点

到目前为止，我们已经把 `SemanticWorld` 里最核心的一批结构基本收住了：

- canonical semantic operation vocabulary
- typed payload / `ValueRef` / `LiteralRef` / `SpanId`
- `MethodSemanticBody`
- body well-formedness invariants

但如果 `Diagnostics` 这一块还不冻结，`SemanticWorld` 的输出面仍然缺一块正式 contract：

1. body validation 失败只能靠异常、日志或调试输出四处飘
2. `Linker` 会被迫重新猜“这个 semantic world 到底能不能信”
3. semantic-layer 问题容易被误报成 linker/codegen 问题
4. downstream 无法稳定断言“这是 canonical artifact 还是半成品”

所以这一轮真正要回答的不是：

- “系统要不要报错”

而是：

- **`SemanticWorld` 这一层到底要对哪些问题负责 structured diagnostics，以及这些 diagnostics 如何成为正式 contract**

## 2. 这里讨论的不是哪件事

这里讨论的不是：

- PE / metadata / IL 原始解码失败如何报
- retained closure / dependency / emission order 如何报
- `LoweringFamily` 选不出来如何报
- runtime / host / backend / template readiness 如何报

这里讨论的是：

- **semantic normalization 和 method body validation 失败，应该如何作为 `SemanticWorld` 正式输出的一部分表达**

也就是：

- 哪些问题属于 `SemanticWorld.Diagnostics`
- 它们和 `Linker.Diagnostics` / `CodeGen` diagnostics 怎么分层
- downstream 面对这些诊断时是否允许继续消费

## 3. 三种可选方案

## 3.1 方案 A：不定义正式 diagnostics contract，继续靠异常 / 日志 / 调试输出

### 方案描述

做法是：

- `SemanticWorld` 发现问题就抛异常、写日志、或仅在调试视图里可见
- downstream 自己决定要不要继续

### 优点

- 实现成本最低
- 早期最容易先跑起来

### 缺点

- 没有稳定消费面
- `Linker` 很难明确判断输入是否可信
- 同类 semantic 问题会在不同阶段重复被不同术语重报
- 无法形成稳定测试断言和可解释 artifact

### 我的判断

- **不推荐**

## 3.2 方案 B：冻结 `semantic-scope structured diagnostics`，并把 error 作为下游阻断条件

### 方案描述

做法是：

- `SemanticWorld` 只对自己边界内的问题产出结构化 diagnostics
- diagnostics 成为 `SemanticWorld` 正式输出的一部分
- 只要存在 `Severity = error`，`Linker` 就不应继续消费该 `SemanticWorld`
- `warning` 可以保留并继续进入 report/assertion surface

### 优点

- 边界最清楚
- semantic 层问题停留在 semantic 层
- `Linker` 不必再重做 body validation
- 整个主线 pipeline 的 artifact 可信度更强

### 缺点

- 需要 upfront 收一个最小 diagnostics shape 和分类面

### 我的判断

- **推荐**

## 3.3 方案 C：定义 structured diagnostics，但允许 `Linker` 跳过坏 body 继续部分消费

### 方案描述

做法是：

- `SemanticWorld` 仍产出 structured diagnostics
- 但 downstream 可以选择跳过 invalid methods / subjects，只消费剩余“看起来可用”的部分

### 优点

- 对交互式分析或 IDE 式场景看起来更“宽容”
- 某些只关心局部方法的工具可能更省事

### 缺点

- canonical pipeline 会引入“部分可信 artifact”
- closure/entry 语义会变得模糊
- 很容易出现“不是没问题，只是被悄悄跳过了”
- `Linker` 输入边界再次变软

### 我的判断

- **不推荐作为主线**

如果以后需要交互式或 IDE 视图，可以另做辅助 surface，
不应该污染 canonical pipeline contract。

## 4. 我的建议

如果这一轮要拍板，我建议采用：

- **方案 B：冻结 `semantic-scope structured diagnostics`，并把 `error` 作为 `Linker` 的阻断条件**

一句话定义：

- `SemanticWorld.Diagnostics` 只回答：
  - 当前 semantic normalization / method body validation 是否成功产出了一个自洽、可消费的 semantic world

它不回答：

- retained closure 是否完整
- dependency 是否冻结成功
- 最终该如何 lowering / render / bind runtime

## 5. `SemanticWorld` 应该负责哪些 diagnostics

我建议当前先把 `SemanticWorld` diagnostics 责任面冻结为下面七类：

### A. `identity`

回答：

- method-local / semantic-local identity 是否冲突或失效

典型例子：

- `OperationId` 冲突
- `ValueRef` 重复定义
- `SpanId` 冲突

### B. `input`

回答：

- method input shape 是否和 method subject / signature 一致

典型例子：

- instance method 缺少 `ThisValueRef`
- static method 非法携带 `ThisValueRef`
- `ParameterValueRefs[]` 个数或顺序不匹配

### C. `operation`

回答：

- operation 本体是否满足最小结构约束

典型例子：

- `Kind` 与 payload 类型不匹配
- value-producing operation 缺少 `ResultRef`
- non-value-producing operation 非法携带 `ResultRef`

### D. `reference`

回答：

- body 内外引用是否都能解析

典型例子：

- `SpanId` 无法解析
- `LiteralRef` 无法解析
- payload 中的 `TypeId / MethodId / FieldId / MemberId` 无法解析

### E. `def-use`

回答：

- 值使用时序是否满足前序可用性

典型例子：

- 前向 `ValueRef` 引用
- `return(value=...)` 返回未来才定义的值
- 某个 `SpanId` 在 use-site 展开后包含未定义值

### F. `support`

回答：

- 当前 semantic slice 是否能把某段 loader facts 正常归一化成 canonical semantic body

典型例子：

- 当前切片尚未支持某种 method body 语义形状
- 某个 operation 在当前 semantic vocabulary 中没有合法归一化路径

### G. `invariant`

回答：

- `SemanticWorld` 级整体最小不变量是否被破坏

典型例子：

- 同一个 method subject 出现冲突 body
- world-level canonical identity 映射自相矛盾

## 6. 明确不属于 `SemanticWorld` diagnostics 的东西

我建议把下面这些内容明确排除在当前 `SemanticWorld.Diagnostics` 之外：

### A. Loader / decode 级问题

例如：

- PE 读取失败
- metadata 表损坏
- IL 解码失败

这些属于 `Loader` 边界，不应伪装成 semantic diagnostics。

### B. Linker 级问题

例如：

- entry root 无法纳入 closure
- semantic obligation 无法映射到 external managed dependency
- `EmissionOrder` 无法稳定导出

这些已经属于 `Linker.Diagnostics` 的责任面。

### C. CodeGen / Emitter 级问题

例如：

- `LoweringFamily` 无法选出
- runtime/helper 缺失
- host binding 缺失
- backend/template 不支持

这些不应倒灌进 semantic diagnostics。

一句话标准就是：

- **只要问题开始回答“怎么链接、怎么降、怎么生成”，它就已经不属于 `SemanticWorld.Diagnostics`**

## 7. 推荐的最小 diagnostics shape

这一轮我建议把 `SemanticWorld.Diagnostics` 的最小 shape 冻得比 linker 略细一点，
因为这里需要指向 method-local anchor：

- `Code`
- `Severity`
- `Category`
- 可选 `MethodId`
- `AnchorKind`
- `AnchorId`
- 可选 `RelatedAnchors[]`
- `Message`

## 7.1 `Code`

回答：

- 这是一类什么样的稳定 semantic diagnostic

## 7.2 `Severity`

当前基线我建议先只冻结两级：

- `error`
- `warning`

## 7.3 `Category`

当前基线我建议先只冻结为：

- `identity`
- `input`
- `operation`
- `reference`
- `def-use`
- `support`
- `invariant`

## 7.4 `MethodId`

用于表达：

- 该 diagnostic 是否属于某个具体 method body 上下文

当 anchor 是 method-local 对象时：

- `MethodId` 必须存在

## 7.5 `AnchorKind / AnchorId`

回答：

- 这条 diagnostic 主要钉在谁身上

我建议当前先按下面几类理解：

- `world`
- `method`
- `operation`
- `value`
- `span`
- `literal`
- `subject`

说明：

- 如果 `AnchorKind` 是 `operation / value / span`，则它是 method-local anchor，需要配合 `MethodId`
- 如果 `AnchorKind` 是 `literal / subject / world`，则它本身可以直接指向 semantic-world 级对象

## 7.6 `RelatedAnchors[]`

用于表达：

- 这条 diagnostic 还和哪些对象发生了冲突或关联

例如：

- 重复 `ValueRef`
- 冲突 `OperationId`
- 某个 operation 使用了未定义的某个 `ValueRef`

我建议允许保留，但不要求所有 diagnostics 都使用。

## 7.7 `Message`

保留给人看的解释文本。

但稳定消费应优先依赖：

- `Code`
- `Severity`
- `Category`
- `MethodId`
- `AnchorKind`
- `AnchorId`

而不是依赖 message 文本。

## 8. 推荐冻结的最小 code family

我建议当前 `Phase A` 基线先冻结下面这批最小 code family：

- `semantic.identity.duplicate-operation-id`
- `semantic.identity.duplicate-value-ref`
- `semantic.identity.duplicate-span-id`
- `semantic.input.invalid-this-shape`
- `semantic.input.parameter-shape-mismatch`
- `semantic.operation.kind-payload-mismatch`
- `semantic.operation.invalid-result-shape`
- `semantic.reference.unresolved-span`
- `semantic.reference.unresolved-literal`
- `semantic.reference.unresolved-subject`
- `semantic.defuse.forward-value-use`
- `semantic.support.unsupported-semantic-shape`
- `semantic.invariant.invalid-semantic-world`

这些 code 的定位分别是：

- `semantic.identity.duplicate-operation-id`
  - 单个 method body 内 `OperationId` 冲突
- `semantic.identity.duplicate-value-ref`
  - 输入值与结果值之间或结果值彼此之间发生冲突
- `semantic.identity.duplicate-span-id`
  - method-local `SpanId` 冲突
- `semantic.input.invalid-this-shape`
  - `this` 形状与 method kind 不一致
- `semantic.input.parameter-shape-mismatch`
  - 参数值入口与签名不匹配
- `semantic.operation.kind-payload-mismatch`
  - operation `Kind` 与 payload 类型不匹配
- `semantic.operation.invalid-result-shape`
  - `ResultRef` 的有无与当前 operation 语义不一致
- `semantic.reference.unresolved-span`
  - `SpanId` 在当前 body 无法解析
- `semantic.reference.unresolved-literal`
  - `LiteralRef` 在当前 `SemanticWorld` 无法解析
- `semantic.reference.unresolved-subject`
  - payload 中的 canonical subject id 无法解析
- `semantic.defuse.forward-value-use`
  - 某个 use-site 使用了未来才定义的值
- `semantic.support.unsupported-semantic-shape`
  - 当前 semantic slice 尚不能把该输入归一化进 canonical vocabulary
- `semantic.invariant.invalid-semantic-world`
  - `SemanticWorld` 级最小不变量被破坏

这里我刻意没有一下子扩成很大的 code 表，
因为当前最重要的是先把 diagnostics contract 稳住，
不是先造一张庞大的错误码字典。

## 9. 下游可消费性规则

我建议当前明确冻结一条非常重要的规则：

- **只要 `SemanticWorld.Diagnostics` 中存在 `Severity = error`，`Linker` 就不应继续消费该 `SemanticWorld`**

相对地：

- `warning`
  - 可以继续消费
  - 但必须进入 report/assertion surface

这条规则的价值很大，
因为它把下面这种模糊状态彻底切断了：

- “semantic body 虽然有问题，但也许 linker 还能猜着往下跑”

也就是说：

- `error` = `SemanticWorld` contract 未成立
- `warning` = `SemanticWorld` contract 成立，但存在需要暴露的 caveat

## 10. 为什么我不建议 canonical pipeline 做 partial consume

这点我想单独强调。

表面上看：

- 某个 method body invalid
- 那能不能先跳过它，继续分析其他 methods

这个想法在交互式工具里不是没价值，
但我不建议把它作为 canonical pipeline 合同的一部分。

原因有三点：

### 1. closure/entry 语义会变模糊

你很难再说清楚：

- downstream 消费到的是完整 semantic world
- 还是“删掉坏 body 之后的残缺 world”

### 2. 容易掩盖系统性问题

很多时候 invalid body 不是局部噪音，
而是 semantic normalization 或 identity 体系出了根本问题。

### 3. 会把 `Linker` 输入边界再次做软

我们前面一路都在收紧 artifact 边界，
这里不应该再退回“尽量继续”模式。

所以我的建议是：

- **canonical pipeline 不做 partial consume**
- 如果以后需要面向 IDE / 交互分析的宽松模式，再另开辅助 surface

## 11. 用例子看这层边界

### 例子 A：前向值引用

如果某个 method body 出现：

```text
Spans:
  s0 = [v2]

Operations:
  op1: string-concat(inputSpan=s0) -> v1
  op2: load-const-string(literal=lit0) -> v2
```

这里应报：

- `semantic.defuse.forward-value-use`

而不是：

- `linker.*`
- `codegen.*`

因为问题还停留在 semantic body 自洽性层面。

### 例子 B：缺少 external managed dependency

如果 `SemanticWorld` 本身正常，
但 `Linker` 在冻结 dependency 时发现某个 semantic obligation 无法满足，
这时应报：

- `linker.dependency.*`

而不是：

- `semantic.reference.*`

### 例子 C：`LoweringFamily` 选不出来

如果 semantic world 和 linked world 都合法，
但 `CodeGen` 仍无法选择 lowering family，
那应属于：

- `CodeGen` diagnostics

而不应回灌成：

- `semantic.support.*`

因为这时 semantic normalization 已经成功完成了。

## 12. 我的最终建议

这一轮我建议正式拍板下面几件事：

1. **`SemanticWorld.Diagnostics` 是正式 contract 的一部分，而不是实现期日志**
2. **当前 `SemanticWorld` 只对 semantic-scope 问题负责，覆盖 `identity / input / operation / reference / def-use / support / invariant` 七类**
3. **Loader decode、Linker closure/dependency/order、CodeGen/Emitter lowering/runtime/backend/template 问题，明确不进入 `SemanticWorld.Diagnostics`**
4. **diagnostics 最小 shape 冻结为：`Code / Severity / Category / MethodId? / AnchorKind / AnchorId / RelatedAnchors[]? / Message`**
5. **只要存在 `Severity = error` 的 semantic diagnostic，`Linker` 就不应继续消费该 `SemanticWorld`**
6. **canonical pipeline 不做 partial consume；如果以后需要宽松分析模式，应单独设计辅助 surface**

## 13. 这一步之后最值得继续讨论的点

如果你认可这个方向，下一步最自然的问题就是：

- **`SemanticWorld` 的正式输出 contract 到底怎么整体收口**

也就是继续往下走到：

- `SemanticWorld` 输出面最小由哪些块组成
- 哪些是 canonical output
- 哪些只是派生视图 / 调试视图
- `Linker` 允许消费其中哪些部分

