# Phase A LiteralRef Pooling Model v1.01

Date: 2026-04-08
Status: design-discuss

## 1. 问题点

上一轮我们已经把 `ValueRef` 基本收住了：

- `ValueRef` 是 method-local、stable、canonical 的值身份
- method inputs 和 operation results 都统一进入同一套 `ValueRef` 空间
- operation payload 消费时只看 `ValueRef`

那么下一步最自然的问题就是：

- **`LiteralRef` 应该怎么定义，池化范围应该放在哪里**

因为现在已经出现了两个不同层次的概念：

1. **literal 本身**
   - 例如 `"Hello"`、`42`
2. **method 内值流中的 materialized value**
   - 例如 `load-const-string("Hello") -> v1`

如果这一步不收住，后面很容易出现几种漂移：

1. 有的 operation 直接内联 literal 值，有的 operation 又改成 `LiteralRef`
2. `LiteralRef` 有时是 method-local，有时又变成全局 id
3. `query key`、字符串常量、数字常量混进同一个“通用池”
4. provenance 和 value identity 被混成一件事

所以这一轮真正要回答的是：

- **`LiteralRef` 的作用域、池化粒度和边界到底是什么**

## 2. 这里讨论的不是哪件事

这里讨论的不是：

- .NET metadata heap 的原始布局
- C++ 最终如何存储常量
- 运行时 intern string 策略
- 常量折叠优化怎么做

这里讨论的是：

- **`SemanticWorld` canonical semantic model 中，literal 的稳定身份如何表达**

也就是：

- `load-const-string`
- `load-const-int32`

这类 operation 的 payload 到底引用什么。

## 3. 三种可选方案

## 3.1 方案 A：literal 直接内联在 operation payload 中

### 方案描述

例如：

```text
LoadConstStringPayload {
  Value = "Hello"
}
```

```text
LoadConstInt32Payload {
  Value = 42
}
```

### 优点

- 看起来最直观
- 不需要额外 literal pool
- 小规模时实现最省事

### 缺点

- 重复 literal 无法共享 identity
- payload 体积容易变大，尤其是字符串
- 诊断 / 序列化 / 统计时更难统一处理
- 很容易又回到“每个 operation 各带一份值”

### 我的判断

- **不推荐作为 canonical 主线**

它适合临时原型，不适合长期 contract。

## 3.2 方案 B：method-local 或 module-local literal pool

### 方案描述

做法是：

- 每个 method 或每个 module/assembly 维护自己的 literal pool
- `LiteralRef` 只在该局部作用域内有效

例如：

- method A 里的 `"Hello"` 是 `lit-0`
- method B 里的 `"Hello"` 也是另一个 `lit-0`

### 优点

- 局部性较强
- 实现难度适中
- 不必维护全局 interning

### 缺点

- 相同 literal 在不同 method / module 中会重复持有 identity
- `LinkedWorld / CodeGen` 看到的是很多局部池，不是一个统一 literal 语言
- 诊断和统计口径会被池边界打散
- “literal identity 表示值本身”这件事会变得不彻底

### 我的判断

- **可以工作，但不够理想**

这条路的主要问题不是不能做，而是 canonical 性不够强。

## 3.3 方案 C：`SemanticWorld` 级 typed literal pool

### 方案描述

做法是：

- 整个 `SemanticWorld` 维护一套 canonical literal pool
- `LiteralRef` 在该 `SemanticWorld` 范围内稳定有效
- pool 里的 key 由 `LiteralKind + LiteralValue` 共同决定

例如：

- `"Hello"` 无论在哪个 method 出现，都共享同一个 `LiteralRef`
- `42` 无论在哪个 method 出现，也共享同一个 `LiteralRef`

### 优点

- literal identity 真正表达“值本身”
- 去重最彻底
- `CodeGen` 和诊断侧看到的是统一 literal 语言
- 和 method-local `ValueRef` 的边界清晰

### 缺点

- 需要一层全局 interning
- 要明确“provenance 另存，不混进 literal identity”

### 我的判断

- **推荐**

这条路最符合 canonical semantic model 的目标。

## 4. 我的建议

如果这一轮要拍板，我建议采用：

- **方案 C：`SemanticWorld` 级 typed literal pool**

一句话定义：

- `LiteralRef` 是 **`SemanticWorld` 级、按 kind+value 去重的稳定常量条目引用**
- 它不等于 `ValueRef`
- 它不等于源代码位置
- 它也不等于 metadata heap 原始偏移

## 5. 我建议冻结的 `LiteralRef` 基本原则

## 原则 1：`LiteralRef` 是静态常量条目身份，不是 method 值身份

也就是：

- `LiteralRef` 描述的是 `"Hello"` 这项常量
- `ValueRef` 描述的是某个 method 里“被 materialize 出来的值”

例如：

```text
lit7 = "Hello"
op3: load-const-string(literal=lit7) -> v2
```

这里：

- `lit7` 是常量条目
- `v2` 是 method 内可消费的值

## 原则 2：`LiteralRef` 作用域放在 `SemanticWorld`

我建议明确收成：

- `LiteralRef` 在单个 `SemanticWorld` 内稳定有效
- 不做 method-local
- 不做 assembly-local
- 也不要求跨进程、跨运行全局稳定

这和前一轮正好形成清晰分工：

- `ValueRef` = method-local
- `LiteralRef` = semantic-world-local

## 原则 3：literal canonical identity 按 `kind + value` 去重

例如：

- `string("Hello")`
- `int32(42)`

是两种不同 kind 的条目。

即使将来出现：

- `int32(1)`
- `bool(true)`

它们值看起来接近，也不能混成同一个 literal identity。

## 原则 4：provenance 不进入 `LiteralRef` 身份

也就是：

- 同样的 `"Hello"` 在 A.cs 和 B.cs 同时出现
- 如果 literal value 相同，它们可以共享一个 `LiteralRef`

如果后续需要知道“这个 literal 从哪些 method/site 出现过”，
应由独立的 usage/provenance 表表达，
而不是把 `LiteralRef` 身份做成 source-site-sensitive。

## 6. 我建议的首轮 literal kind

当前 `Phase A` 我建议 literal pool 先只冻结最小集合：

- `string`
- `int32`

原因很简单：

- 这正好对应当前已经冻结进 vocabulary 的
  - `load-const-string`
  - `load-const-int32`

现在先把边界做稳，比一开始铺一张“大而全 literal kind 表”更重要。

## 7. 为什么我仍然推荐 `int32` 也进 literal pool

有人会直觉觉得：

- string 放 pool 很合理
- `int32` 这种小值是不是直接内联更省事

这个想法不奇怪，但我当前仍然更倾向统一：

- `load-const-int32` 也走 `LiteralRef`

原因是：

### 1. contract 更统一

`load-const-*` 这类 materialization 操作都遵守同一种模式：

- 先引用 literal 条目
- 再 materialize 成 `ValueRef`

### 2. 诊断和统计更一致

如果后面要看：

- 当前切片用了哪些 literal
- 哪些常量重复最多

统一 literal pool 会更顺。

### 3. 将来扩展更自然

以后如果加：

- `int64`
- `bool`
- `float`

不会又回到“有的 const 走 pool，有的直接内联”的混合 contract。

当然，未来如果实现层想做性能优化，
可以在内部把极小标量做特殊 fast path，
但我不建议把这种优化泄漏到 canonical contract。

## 8. 我不建议把 query key 和 literal 共池

这是这一轮我想特别强调的一条边界。

例如：

- `query-closed-type-member`
  - 可能有 `QueryKeyId`

很多人会直觉觉得：

- query key 里面也可能有字符串名
- 那是不是可以和 literal pool 合并

我的建议是：

- **不要合并**

原因是：

### 1. semantic role 不同

- literal 表示“可 materialize 的静态常量值”
- query key 表示“查找描述符 / 查询键”

### 2. 消费路径不同

- `load-const-string` 会把 `LiteralRef` 变成 `ValueRef`
- `query key` 不会被当成 method value 来 materialize

### 3. 后续扩展方向不同

- literal pool 未来扩展的是常量种类
- query key 未来扩展的是查询描述结构

所以我建议：

- `LiteralRef` 和 `QueryKeyId` 明确分池

## 9. 用例子看推荐方案

## 9.1 `load-const-string`

推荐长成：

```text
LiteralPool:
  lit0 = string("Hello, ")

Operation:
  op1: load-const-string(literal=lit0) -> v1
```

## 9.2 `load-const-int32`

推荐长成：

```text
LiteralPool:
  lit4 = int32(42)

Operation:
  op7: load-const-int32(literal=lit4) -> v9
```

## 9.3 重复字符串跨 method 共享

如果两个 method 都写了 `"Hello"`，
推荐理解成：

```text
LiteralPool:
  lit0 = string("Hello")

Method A:
  op1: load-const-string(literal=lit0) -> v1

Method B:
  op3: load-const-string(literal=lit0) -> v5
```

这里共享的是 constant identity，
不是 method value identity。

## 10. 我建议的最小 literal pool contract

当前 `Phase A` 我建议先把 literal pool 收成最小必需面：

```text
LiteralEntry {
  LiteralRef
  Kind
  Value
}
```

这里的 `Value` 按 `Kind` 解释。

例如：

- `Kind = string` 时，`Value` 是字符串值
- `Kind = int32` 时，`Value` 是 32 位整数值

我当前**不建议**一开始就把下面这些信息混进 `LiteralEntry`：

- source file / line
- declaring method
- metadata token provenance
- runtime-specific storage hint

这些都不属于 literal identity 本身。

## 11. 我给你的三个判断建议

## 11.1 如果你更在意“最少抽象、先跑起来”

你会更容易接受方案 A。

但我的建议仍然是：

- **别选**

因为它会把 literal identity 完全打散在 operation payload 里。

## 11.2 如果你更在意“局部实现简单”

你可能会倾向方案 B。

这条路不是不行，
但它会让 canonical contract 里出现很多局部池边界。

## 11.3 如果你更在意“主线语义模型干净稳定”

那我明确建议选方案 C：

- `SemanticWorld` 级 typed literal pool

这是我当前最推荐的主线。

## 12. 我的最终建议

这一轮我建议正式收成下面几点：

1. **`LiteralRef` 定义为 `SemanticWorld` 级、按 `kind + value` 去重的稳定常量条目引用**
2. **`LiteralRef` 不等于 `ValueRef`，`load-const-*` 负责把 literal materialize 成 method-local `ValueRef`**
3. **首轮 literal kind 先冻结为 `string / int32`**
4. **provenance 不进入 literal identity，若需要则单独建 usage/provenance 表**
5. **`QueryKeyId` 与 `LiteralRef` 明确分池，不共用统一“通用常量池”**

## 13. 这一步之后最值得继续讨论的点

如果你认可这个方向，下一步最自然的问题就是：

- **`SpanId` 的 contract 到底怎么定义，尤其是顺序、同质性、稳定性和诊断可读性**

也就是继续往下收：

- `SpanId` 是否只承载 `ValueRef[]`
- 是否允许空 span 作为 canonical 条目
- span 是否按值序列去重
- 诊断时如何把 span 展开成人类可读结构

