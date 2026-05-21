# Phase A SpanId Contract v1.01

Date: 2026-04-08
Status: design-discuss

## 1. 问题点

前面两步我们已经把两件事基本收住了：

- `ValueRef` 是 method-local、stable、canonical 的值身份
- `LiteralRef` 是 `SemanticWorld` 级、按 `kind + value` 去重的常量条目身份

那么下一步最自然的问题就是：

- **`SpanId` 到底怎么定义**

因为现在已经有一类信息不适合继续内联在 payload 里：

- 调用参数列表
- `string-concat` 输入列表
- 其他可变长、同角色的输入序列

如果这一步不收住，后面很容易出现几种漂移：

1. 有的 payload 直接内联 `ValueRef[]`
2. 有的 payload 用 `SpanId`
3. 有的地方空参数列表写成 `null`
4. 有的地方把 `receiver` 也塞进 span，变成“无角色输入列表”

所以这一轮真正要回答的是：

- **`SpanId` 的作用域、承载内容、顺序语义、空序列表示，以及是否需要 canonical 去重**

## 2. 这里讨论的不是哪件事

这里讨论的不是：

- 最终内存布局如何压缩
- 序列化时 JSON 是否展开 span
- 后续优化器是否会重建 operand list
- emitter 最终是否直接忽略 span table

这里讨论的是：

- **`SemanticWorld -> CodeGen` 之间的 canonical 可变长输入引用 contract**

也就是：

- operation payload 里的 `ArgumentSpanId` / `InputSpanId` 到底引用什么。

## 3. 三种可选方案

## 3.1 方案 A：直接在 payload 中内联 `ValueRef[]`

### 方案描述

例如：

```text
InvokeInstanceMethodPayload {
  ReceiverRef
  MethodId
  Arguments = [v2, v3]
}
```

```text
StringConcatPayload {
  Inputs = [v1, v4, v7]
}
```

### 优点

- 最直观
- 不需要单独 span table
- 早期实现最省事

### 缺点

- payload 大小不稳定
- 不利于统一 contract
- 重复序列无法共享
- 诊断、遍历、序列化会反复遇到“每个 payload 都带自己的数组”

### 我的判断

- **不推荐**

这和我们前面已经定下来的“小 header + typed payload + shared spans”方向冲突。

## 3.2 方案 B：method-local span table，`SpanId` 只是稳定序列句柄

### 方案描述

做法是：

- 每个 method 拥有自己的 span table
- `SpanId` 只在该 method 内有效
- span entry 表示一个 **有序的 `ValueRef` 序列**
- payload 通过 `ArgumentSpanId` / `InputSpanId` 引用该序列

例如：

```text
SpanTable:
  s0 = []
  s1 = [v2, v3]
  s2 = [v1, v4, v7]
```

operation 中：

```text
InvokeInstanceMethodPayload {
  ReceiverRef = v1
  MethodId = M
  ArgumentSpanId = s1
}
```

```text
StringConcatPayload {
  InputSpanId = s2
}
```

### 优点

- contract 简单稳定
- 可变长数据统一外置
- span 本身语义很清楚，就是“method-local ordered `ValueRef` list”
- 后续即使内部想做 interning，也不必先把它写死进 contract

### 缺点

- 如果不做去重，某些重复序列可能重复存储
- `SpanId` 本身不表达“序列 canonical identity”，只表达“表项句柄”

### 我的判断

- **推荐**

这是当前 `Phase A` 最平衡的方案。

## 3.3 方案 C：method-local canonical span interning

### 方案描述

做法是再进一步：

- 同一个 method 中，完全相同、顺序也相同的 `ValueRef[]` 序列共享同一个 `SpanId`
- `SpanId` 不再只是表项句柄，而更接近“序列 identity”

例如：

- 两次调用都用 `[v2, v3]`
- 那它们必须共享同一个 `SpanId`

### 优点

- 去重更彻底
- 某些比较和缓存逻辑可能更方便

### 缺点

- 把本来属于实现优化的事情提前抬成 contract
- 构造期需要做 sequence interning
- 后面如果想放宽策略，反而会改 contract

### 我的判断

- **不建议把它写死成 `Phase A` contract**

它可以作为实现优化存在，但不必先上升为 canonical 语义要求。

## 4. 我的建议

如果这一轮要拍板，我建议采用：

- **方案 B：method-local span table，`SpanId` 只是稳定序列句柄**

一句话定义：

- `SpanId` 是 **method-local、stable、ordered `ValueRef` sequence handle**
- 它不跨 method
- 它不承载 receiver / subject 这类特殊语义角色
- 它也不要求“相同序列必须共享 id”

## 5. 我建议冻结的 `SpanId` 基本原则

## 原则 1：`SpanId` 作用域限定在单个 method 内

这点几乎由 `ValueRef` 决定了：

- `SpanId` 里承载的是 `ValueRef[]`
- `ValueRef` 本身是 method-local

所以我建议明确收成：

- `SpanId` 只在单个 method 内有效
- 不跨 method
- 不跨 `SemanticWorld`

## 原则 2：`SpanId` 只承载有序的 `ValueRef` 列表

我建议当前收得很硬：

- span entry 的元素类型统一是 `ValueRef`
- 不混入 `LiteralRef`
- 不混入 `MethodId`
- 不混入富对象

这点很重要，因为：

- literal 已经有 `LiteralRef`
- 成员身份已有 `MethodId / FieldId / TypeId`
- span 的职责只是承载“可变长值输入序列”

## 原则 3：顺序有语义，不允许把 span 当集合

例如：

- 参数列表 `[v1, v2]`
- 和 `[v2, v1]`

必须视为两个不同序列。

这不仅是调用参数的自然要求，
对 `string-concat` 这种操作也同样成立。

## 原则 4：span 只承载“同角色输入”

我建议明确禁止下面这种做法：

- 把 `receiver` 和普通参数一起塞进一个 `AllOperandsSpanId`

例如对于：

- `invoke-instance-method`

推荐仍然保持：

- `ReceiverRef`
- `ArgumentSpanId`

而不是：

- `OperandSpanId = [receiver, arg0, arg1]`

因为：

- `receiver` 在语义上不是普通参数
- 把它压平会削弱 payload 的可读性和语义边界

## 原则 5：空序列显式化，不用 `null`

这一条我建议收得非常明确：

- 空参数列表、空输入列表，不用 `null`
- 使用显式的空 span

例如：

```text
SpanTable:
  s0 = []
```

那么：

- 无参静态调用
- 无参实例调用

都可以显式写成：

- `ArgumentSpanId = s0`

这比：

- 缺字段
- `null`
- 特殊布尔标志

都更稳定。

## 6. 我建议的 span role 边界

我建议把 span 的 role 理解成：

- role 来自 payload 字段名
- 不来自 span entry 本身

也就是：

- `ArgumentSpanId`
  - 表示“调用参数序列”
- `InputSpanId`
  - 表示“helper 输入序列”

而 span table 自己只知道：

- 这是一个 `ValueRef[]`

我当前**不建议**一开始给 span entry 自己再加：

- `SpanKind`
- `SpanRole`

因为这会把 role 逻辑重复存两次：

- payload 已经有 role
- span 自己再存一遍，很容易漂

## 7. 用几个 operation 看推荐方案

## 7.1 `construct-object`

推荐长成：

```text
SpanTable:
  s0 = [v1]

ConstructObjectPayload {
  TargetTypeId
  CtorMethodId
  ArgumentSpanId = s0
}
```

如果构造函数无参，则：

```text
SpanTable:
  sEmpty = []

ConstructObjectPayload {
  TargetTypeId
  CtorMethodId
  ArgumentSpanId = sEmpty
}
```

## 7.2 `invoke-instance-method`

推荐长成：

```text
SpanTable:
  s1 = [v2, v3]

InvokeInstanceMethodPayload {
  ReceiverRef = v1
  MethodId = M
  ArgumentSpanId = s1
}
```

这里 `receiver` 不进 span。

## 7.3 `string-concat`

推荐长成：

```text
SpanTable:
  s2 = [v4, v5, v6]

StringConcatPayload {
  InputSpanId = s2
}
```

这里顺序必须保留。

## 8. 为什么我不建议现在就要求 span 去重

表面上看，“完全相同序列必须共享 `SpanId`” 很有吸引力，
但我当前不建议把它写进 contract。

原因有三点：

### 1. `SpanId` 的核心职责是引用，不是表达更高阶 identity

当前最重要的是把可变长输入从 payload 中稳定抽离出来。

### 2. 去重更像实现优化，不是语义承诺

后面可以做：

- exact-sequence interning
- small-span caching

但这些都可以留给实现层。

### 3. 不把 interning 写死，主线 contract 更稳

这样以后即使内部实现变了，
`CodeGen` 消费契约仍然不需要改。

## 9. 我建议的最小 span table contract

当前 `Phase A` 我建议先把 span table 收成最小必需面：

```text
SpanEntry {
  SpanId
  Values[]
}
```

其中：

- `SpanId` 是 method-local stable handle
- `Values[]` 是有序 `ValueRef` 列表

我当前**不建议**一开始把下面这些东西混进 `SpanEntry`：

- `SpanRole`
- source provenance
- declaring operation id
- cached hash as contract field

这些都不属于 canonical semantic surface 本身。

## 10. 我给你的三个判断建议

## 10.1 如果你更在意“最少抽象，先快速表达输入列表”

你会更容易被方案 A 吸引。

但我的建议仍然是：

- **别选**

因为它会把 variable-length 结构重新打回 payload 本体。

## 10.2 如果你更在意“先把主线 contract 稳住”

那我明确建议选方案 B：

- method-local span table
- `SpanId` 作为稳定句柄
- 空 span 显式化
- 不把 receiver 压进 span

这是我当前最推荐的主线。

## 10.3 如果你更在意“后面进一步做共享和缓存”

那方案 C 可以作为实现优化方向存在。

但我不建议它现在就上升成 contract 要求。

## 11. 我的最终建议

这一轮我建议正式收成下面几点：

1. **`SpanId` 定义为 method-local、stable 的有序 `ValueRef` 序列句柄**
2. **`SpanId` 只承载 `ValueRef[]`，不混入 `LiteralRef`、subject id 或富对象**
3. **顺序有语义，`[v1, v2]` 与 `[v2, v1]` 必须区分**
4. **span 只承载同角色输入，`receiver / subject` 等语义特殊输入保持显式字段**
5. **空序列使用显式 empty span，而不是 `null` 或缺字段**
6. **不把 exact-sequence interning 写死成 `Phase A` contract，可作为后续实现优化**

## 12. 这一步之后最值得继续讨论的点

如果你认可这个方向，下一步最自然的问题就是：

- **method-level canonical operation container 本身应该怎么长：输入表、operation 列表、literal usage、span table 的组织关系怎么冻结**

也就是继续往下收：

- 一个 method 的 semantic body 最小 contract
- 各子表之间的引用顺序
- 诊断和可视化时如何稳定展开

