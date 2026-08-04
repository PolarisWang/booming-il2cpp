# Phase A Method Semantic Body Well-Formedness v1.01

Date: 2026-04-08
Status: design-discuss

## 1. 问题点

上一轮我们已经把 `MethodSemanticBody` 的最小形状基本收住了：

- `MethodId`
- `Inputs`
- `Spans`
- `Operations`

那么下一步最自然的问题就是：

- **什么样的 `MethodSemanticBody` 才算 well-formed**

因为一旦 body contract 形状定了，
下一步就必须回答：

- 哪些约束由 `SemanticWorld` 保证
- 哪些错误不能留给 `CodeGen` 再猜
- 哪些检查虽然“看起来有用”，但其实已经越界到了 lowering/runtime 侧

如果这一步不收住，后面很容易出现几种漂移：

1. `SemanticWorld` 只输出“长得像 body 的东西”，真正是否合法交给 `CodeGen`
2. `CodeGen` 被迫重新做一遍解析、引用解析、顺序检查
3. 一部分检查做成结构约束，一部分检查做成实现约束，边界越来越乱
4. downstream 组件无法判断“这个 body 是 canonical truth，还是半成品”

所以这一轮真正要回答的是：

- **`MethodSemanticBody` 的最小 well-formedness / validation invariants 到底有哪些**

## 2. 这里讨论的不是哪件事

这里讨论的不是：

- lowering family 是否已可选出
- 当前 runtime 是否已经支持该 operation
- emitter 模板是否已准备好
- 最终生成的 C++ 是否一定能编译通过

这里讨论的是：

- **在纯 semantic layer 内，一个 method body 要满足哪些基础结构与引用约束，才有资格作为 canonical body 发布**

## 3. 三种可选方案

## 3.1 方案 A：极宽松，只做 id 可解析性检查

### 方案描述

做法是：

- 只检查大部分 id 是否存在
- 很多时序和引用约束留给 `CodeGen`

例如：

- `SpanId` 能解析就算通过
- `ValueRef` 是否前序可用，后面再说
- `ReceiverRef` 是否合理，后面再说

### 优点

- `SemanticWorld` 实现最省事
- 早期容易先跑起来

### 缺点

- `CodeGen` 必然要重做语义体检
- canonical body 变成“半可信”
- 违背我们前面已经拍板的强边界思路

### 我的判断

- **不推荐**

这条路会让 `SemanticWorld` 和 `CodeGen` 的职责再次缠住。

## 3.2 方案 B：`SemanticWorld` 负责结构 + 本地数据流 well-formedness

### 方案描述

做法是：

- `SemanticWorld` 保证 body 的结构完整性、引用可解析性、method-local def-use 合法性
- `CodeGen` 可以把 body 当成已通过基础合法性验证的 canonical 输入

但这里不强求：

- lowering readiness
- runtime support readiness
- backend-specific implementability

### 优点

- 边界最清楚
- `CodeGen` 不再需要做“重新理解 body 是否合法”
- 非 lowering 语义错误在上游收敛

### 缺点

- `SemanticWorld` 需要承担更明确的验证责任

### 我的判断

- **推荐**

这是当前 `Phase A` 最稳的主线。

## 3.3 方案 C：把 lowering/readiness 检查也打进 body well-formedness

### 方案描述

做法是进一步要求：

- 不仅结构正确
- 还必须当前 `CodeGen` 已支持
- 当前 runtime/helper 也必须可落地

例如：

- `string-concat` 的具体 arity 是否已有 lowering
- imported call 的某些签名是否已有 runtime path

都提前作为 body well-formedness 一部分。

### 优点

- 下游失败更少

### 缺点

- semantic validation 和 lowering readiness 混在一起
- 一旦实现能力变化，well-formedness 定义也跟着变
- canonical body 不再是稳定语义产物，而变成“当前实现版本下的可落地产物”

### 我的判断

- **不推荐**

这会把 semantic layer 做脏。

## 4. 我的建议

如果这一轮要拍板，我建议采用：

- **方案 B：`SemanticWorld` 负责结构 + 本地数据流 well-formedness**

一句话定义：

- body 的合法性，必须在 semantic layer 内自洽
- 但它不需要提前回答 lowering/runtime/backend 是否已完全支持

## 5. 我建议冻结的 invariant 分层

当前 `Phase A` 我建议把 invariant 收成下面五类：

1. body ownership / identity invariants
2. input invariants
3. operation / result invariants
4. span / reference resolution invariants
5. temporal def-use invariants

## 6. Body Ownership / Identity Invariants

## 6.1 `MethodId` 必须可解析且唯一归属当前 body

我建议明确：

- 每个 `MethodSemanticBody` 必须绑定一个可解析的 canonical `MethodId`
- 一个 body 不能同时归属多个 method subject

## 6.2 method-local identity 不能冲突

在单个 body 内，下面几类 id 各自必须唯一：

- `OperationId`
- `ValueRef`
- `SpanId`

这里的“`ValueRef` 唯一”指的是：

- 输入值引用彼此不重复
- 结果值引用彼此不重复
- 输入值引用与结果值引用之间也不冲突

## 7. Input Invariants

## 7.1 `ThisValueRef` 与 method kind 一致

我建议明确：

- instance method 必须有 `ThisValueRef`
- static method 不得有 `ThisValueRef`

## 7.2 参数值引用数量与顺序必须和 method signature 对齐

也就是：

- `ParameterValueRefs[]` 的个数与当前 method 参数个数一致
- 参数顺序必须保持 canonical signature order

这里不要求 payload 每次都重新携带参数身份，
因为 method input 区域已经给出稳定入口。

## 7.3 输入值引用必须彼此唯一

例如不能出现：

- `ThisValueRef = v0`
- `ParameterValueRefs = [v0]`

这类冲突。

## 8. Operation / Result Invariants

## 8.1 每个 operation 的 `Kind` 与 payload 类型必须匹配

例如：

- `Kind = string-concat`
- 却挂了 `InvokeInstanceMethodPayload`

这是直接的结构错误。

## 8.2 `ResultRef` 只能出现在 value-producing operation 上

例如：

- `return`
- `emit-console-string`

这类如果当前语义定义为不产出值，
就不应携带 `ResultRef`。

反过来，像：

- `load-const-string`
- `construct-object`
- `read-instance-field`

这类产出值的 operation，
则必须提供 `ResultRef`。

## 8.3 所有 `ResultRef` 在 body 内必须唯一

这点和前面的 value uniqueness 一起成立，
但这里建议单独强调，
因为它是 method-local SSA-like 值模型的核心。

## 9. Span / Reference Resolution Invariants

## 9.1 `SpanId` 引用必须能在当前 body 的 span table 中解析

例如：

- `ArgumentSpanId = s4`

则 `s4` 必须存在于当前 `Spans` 中。

## 9.2 `SpanEntry.Values[]` 中的每个 `ValueRef` 必须属于当前 body

也就是：

- 必须是 input value
- 或某个 operation 的 `ResultRef`

不能引用：

- 其他 method 的值
- 不存在的值

## 9.3 `LiteralRef` 必须能在 `SemanticWorld` literal pool 中解析

## 9.4 subject ids 必须能在 `SemanticWorld` canonical identity 中解析

例如：

- `TypeId`
- `MethodId`
- `FieldId`
- `MemberId`

这些如果出现在 payload 中，都必须可解析。

## 10. Temporal Def-Use Invariants

这是我认为最关键的一组约束。

## 10.1 operation 只能消费 method inputs 或前序已定义值

我建议当前明确收成：

- 某个 operation payload 中出现的 `ReceiverRef`
- 某个 `SpanId` 在该 operation 的 use-site 展开后的所有 `ValueRef`

都必须在当前 operation 之前已经可用。

也就是来源只能是：

- `Inputs`
- 更早的 `Operations.ResultRef`

我当前**不建议**允许前向引用。

## 10.2 `SpanId` 的可用性按 use-site 检查，不按 table 位置检查

这是一个容易混淆的点。

因为：

- `Spans` 是 side table
- span table 本身没有执行顺序

所以我建议明确：

- span 中的值是否“已定义”，是根据某个 operation 使用该 `SpanId` 的时点来判断
- 不是看 `SpanEntry` 自己在文档里出现在哪个位置

## 10.3 `return(value=...)` 同样只能返回已可用值

这其实是 10.1 的特例，
但我建议单独强调，
因为它是 method body 结束点最直观的一类 use-site。

## 11. 我不建议纳入 body well-formedness 的检查

为了防止边界再次膨胀，
我建议明确下面这些当前**不属于** `MethodSemanticBody` well-formedness：

- lowering family 能否选出
- runtime helper 是否已存在
- backend/emitter 是否已支持该 operation 组合
- imported call 最终怎么绑定 host/runtime
- template/render 级约束

这些问题虽然重要，
但它们已经属于更下游的实现 readiness，
不应倒灌成 semantic body 的合法性定义。

## 12. 用几个反例看边界

## 12.1 重复 `ResultRef`

```text
op1: load-const-string(literal=lit0) -> v1
op2: read-instance-field(receiver=v0, field=F) -> v1
```

这里 `v1` 重复定义，body 不 well-formed。

## 12.2 前向值引用

```text
Spans:
  s0 = [v2]

Operations:
  op1: string-concat(inputSpan=s0) -> v1
  op2: load-const-string(literal=lit0) -> v2
```

这里 `op1` 在 use-site 使用了未来才定义的 `v2`，
body 不 well-formed。

## 12.3 instance/static 输入形状不匹配

例如：

- static method 却声明了 `ThisValueRef`
- instance method 却没有 `ThisValueRef`

都应视为 body 结构错误。

## 12.4 payload kind 不匹配

```text
Kind = invoke-instance-method
Payload = StringConcatPayload
```

这是直接结构错误，
不应留给 `CodeGen` 再发现。

## 13. 我给你的三个判断建议

## 13.1 如果你更在意“尽快先把 body 跑通”

你会更容易被方案 A 吸引。

但我的建议仍然是：

- **别选**

因为它会让 downstream 重新承担 semantic validation。

## 13.2 如果你更在意“canonical body 一旦产出就可信”

那我明确建议选方案 B。

这也是我当前最推荐的主线。

## 13.3 如果你更在意“减少 downstream 失败”

你可能会被方案 C 吸引。

但我不建议把 readiness 检查混进 well-formedness，
否则 semantic layer 会开始受实现版本波动牵制。

## 14. 我的最终建议

这一轮我建议正式收成下面几点：

1. **`MethodSemanticBody` 的 well-formedness 由 `SemanticWorld` 负责，不下放给 `CodeGen` 重新判断**
2. **well-formedness 至少覆盖：body ownership、input shape、operation/result shape、reference resolution、temporal def-use**
3. **所有 `ValueRef` 消费都必须来自 method inputs 或前序 operation results，不允许前向引用**
4. **`SpanId` 的合法性按 use-site 判断，不按 span table 位置判断**
5. **lowering/runtime/backend/template readiness 不进入 body well-formedness 定义**

## 15. 这一步之后最值得继续讨论的点

如果你认可这个方向，下一步最自然的问题就是：

- **这些 invariant 的失败应该如何表达：需要不需要一套 `SemanticWorld` 级 diagnostics contract**

也就是继续往下收：

- body validation 错误有哪些分类
- 是不是和 linker diagnostics 分开
- downstream 遇到 invalid body 时是否允许继续部分消费

