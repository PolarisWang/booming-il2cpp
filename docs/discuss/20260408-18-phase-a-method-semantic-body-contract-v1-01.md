# Phase A Method Semantic Body Contract v1.01

Date: 2026-04-08
Status: design-discuss

## 1. 问题点

前面几步我们已经把 method 内几个核心引用基本收住了：

- `ValueRef` 是 method-local、stable 的值身份
- `LiteralRef` 是 `SemanticWorld` 级、按 `kind + value` 去重的常量条目身份
- `SpanId` 是 method-local、stable 的有序 `ValueRef[]` 序列句柄

那么下一步最自然的问题就是：

- **一个 method 的 canonical semantic body 本身，到底应该怎么组织**

也就是：

- 输入值放哪里
- operation stream 放哪里
- span table 放哪里
- literal 和其他 world-level 资源如何引用

如果这一步不收住，后面很容易出现几种漂移：

1. 有的 method body 只是 operation list，输入和值空间全靠隐含约定
2. 有的 method body 又开始加本地 literal 表、subject summary、diagnostic summary，越长越重
3. `CodeGen` 消费时不得不自己猜“哪些部分是 canonical，哪些部分只是派生视图”
4. 同样叫 method semantic body，实际结构在不同切片里不一致

所以这一轮真正要回答的是：

- **`MethodSemanticBody` 的最小 canonical contract 到底包含哪些块**

## 2. 这里讨论的不是哪件事

这里讨论的不是：

- 最终 C# 类型定义代码怎么写
- JSON 序列化外观长什么样
- emitter 最终如何遍历这些结构
- 后续是否要为调试额外生成可视化索引

这里讨论的是：

- **`SemanticWorld -> CodeGen` 之间一个 method 的最小 canonical body surface**

## 3. 三种可选方案

## 3.1 方案 A：平铺 operation list，其他都靠隐含约定

### 方案描述

做法是：

- method body 基本只放 `Operations[]`
- `this` / parameters 通过外部上下文隐含给出
- span table、literal usage、值来源等都尽量不显式建块

### 优点

- 结构看起来最轻
- 早期最容易先跑起来

### 缺点

- 输入值空间不显式，消费方容易各自补脑
- `SpanId`、`ValueRef` 的边界会再次模糊
- method body 自描述性太弱
- 不利于稳定诊断和验证

### 我的判断

- **不推荐**

它太像“有一点结构的 operation dump”，不够像稳定 contract。

## 3.2 方案 B：显式 `MethodSemanticBody`，只包含最小必要块

### 方案描述

做法是给每个 method 一层明确的 canonical body 容器，内部只放最小必要块：

- `Inputs`
- `Spans`
- `Operations`

而 world-level 资源继续在 body 外部引用：

- `LiteralRef`
- `TypeId / MethodId / FieldId / MemberId`

### 优点

- body 结构清楚
- 输入值空间显式化
- `SpanId` 有明确宿主
- 不重复 world-level pool
- `CodeGen` 消费边界最稳定

### 缺点

- 比“纯 operation list”多一层容器
- 需要明确哪些派生视图不进入 canonical body

### 我的判断

- **推荐**

这是当前 `Phase A` 最平衡的方案。

## 3.3 方案 C：method body 内再塞一组 summary/index/bundle

### 方案描述

除了 `Inputs / Spans / Operations`，再加入很多局部块：

- `ReferencedLiteralRefs[]`
- `ReferencedSubjects[]`
- `ProducedValues[]`
- `Diagnostics[]`
- `DerivedCapabilities[]`

### 优点

- 看起来对消费方很友好
- 某些视图可以直接取用

### 缺点

- 大量信息其实都可由 operation body 派生
- 很容易造成重复和漂移
- canonical surface 会越来越重

### 我的判断

- **不建议作为主线**

这些更适合做派生视图或调试索引，而不是 method canonical body 本体。

## 4. 我的建议

如果这一轮要拍板，我建议采用：

- **方案 B：显式 `MethodSemanticBody`，只包含最小必要块**

一句话定义：

- method body 只负责承载 method-local semantic execution body
- 不重复 world-level pool
- 不混入派生 summary

## 5. 我建议冻结的最小 body 结构

当前 `Phase A` 我建议先把 method body 收成下面这个形状：

```text
MethodSemanticBody {
  MethodId
  Inputs
  Spans
  Operations
}
```

其中：

### `MethodId`

- 表示当前 body 属于哪个 canonical method subject

### `Inputs`

- 显式声明 method-local 输入值空间
- 至少包含：
  - `ThisValueRef?`
  - `ParameterValueRefs[]`

### `Spans`

- method-local span table
- 每个 span entry 是稳定的 `SpanId -> ordered ValueRef[]`

### `Operations`

- 按 canonical semantic order 排列的 operation stream
- operation 自己继续携带：
  - `OperationId`
  - `Kind`
  - `ResultRef?`
  - `PayloadRef`

## 6. 我为什么不建议现在就加本地 `Values` 表

一个常见想法是：

- 既然有 `ValueRef`，那是不是 method body 里还应该有一张显式 `Values[]` 表

我当前不建议把它放进首轮最小 contract。

原因是：

- method input 值已经由 `Inputs` 声明
- operation result 值已经由 `Operations.ResultRef` 声明
- 对当前 `Phase A` 来说，这两类来源已经足够恢复 value origin

也就是说，目前最小 body 已经能回答：

- 这个 `ValueRef` 是输入值，还是某个 operation 结果值

如果以后真的需要更强的值索引或 provenance 表，
可以作为增强层引入，而不是现在就提前做重。

## 7. 我为什么不建议 method body 内再建本地 literal 表

这一点我也建议收得很硬：

- method body 不重复持有 local literal pool

原因是前一轮已经拍板：

- `LiteralRef` 是 `SemanticWorld` 级 pool

所以 method body 中的 `load-const-*` payload 直接引用 world-level `LiteralRef` 即可。

如果 method body 再引一层本地 literal usage 表，会出现：

- world pool 一份
- body 内 usage summary 再一份

这类重复更适合作为调试或分析派生视图，不适合作为 canonical body 组成部分。

## 8. 我建议的组织关系和边界

## 原则 1：operation order 是 method body 内唯一的语义顺序主轴

我建议明确收成：

- `Operations[]` 的顺序承载 canonical semantic sequence
- `Inputs` 没有执行顺序
- `Spans` 也没有执行顺序

这很重要，因为：

- method body 的“发生顺序”只应由 operation stream 负责
- span table 只是 side table，不应被误读成执行轨迹

## 原则 2：world-level 资源只引用，不复制

例如：

- literal 走 `LiteralRef`
- 成员身份走 `MethodId / FieldId / TypeId`

body 本身不再复制：

- literal entries
- member descriptors
- type summaries

## 原则 3：method-local 资源显式归属到 body

我建议明确：

- `ValueRef`
- `SpanId`
- `OperationId`

这些 method-local identity 都归属到当前 `MethodSemanticBody`

这样边界会很清楚：

- world-level identity 在外
- method-level identity 在内

## 原则 4：派生 summary 不进入首轮 canonical body

例如当前不建议把下面这些直接塞进 body：

- `ReferencedLiteralRefs[]`
- `ReferencedSubjects[]`
- `DerivedCapabilities[]`
- `Diagnostics[]`

因为它们都可以从 operation body 派生或由其他层负责。

## 9. 用一个具体例子看推荐方案

以 `Greeter.Build()` 为例：

```csharp
public string Build()
{
    return "Hello, " + _name;
}
```

推荐近似理解成：

```text
MethodSemanticBody {
  MethodId = Greeter::Build

  Inputs {
    ThisValueRef = v0
    ParameterValueRefs = []
  }

  Spans {
    s0 = [v1, v2]
  }

  Operations {
    op1: load-const-string(literal=lit0) -> v1
    op2: read-instance-field(receiver=v0, field=Greeter::_name) -> v2
    op3: string-concat(inputSpan=s0) -> v3
    op4: return(value=v3)
  }
}
```

这里可以看到：

- `lit0` 仍然来自 world-level literal pool
- `v0/v1/v2/v3` 都属于当前 method 的值空间
- `s0` 也只属于当前 method
- 真正的语义顺序只在 `Operations` 中

## 10. 我建议的最小验证视角

虽然这一轮不是专门谈验证规则，
但 method body 结构一旦定下，至少已经天然带出几条最小检查：

- `Inputs` 中的 `ValueRef` 不能重复
- `Operations.ResultRef` 不能和输入值冲突
- `Spans` 中的所有 `ValueRef` 必须能在当前 method 解析
- `Operations` 中引用的 `SpanId` 必须能在当前 body 解析
- `LiteralRef` 必须能在 `SemanticWorld` literal pool 中解析

这些规则现在先不展开成单独验证文档，
但我建议把它们视为下一步很自然的继续点。

## 11. 我给你的三个判断建议

## 11.1 如果你更在意“先极简表示 operation stream”

你可能会喜欢方案 A。

但我的建议仍然是：

- **别选**

因为它会把前面已经拍板的 `ValueRef / SpanId` 边界重新打回隐含约定。

## 11.2 如果你更在意“method 级 contract 现在就稳定”

那我明确建议选方案 B：

- 显式 body 容器
- `Inputs`
- `Spans`
- `Operations`

这是我当前最推荐的主线。

## 11.3 如果你更在意“消费方拿到更多现成视图”

你会更容易被方案 C 吸引。

但我不建议把这些 summary/index 先写进 canonical body，
否则后面很容易被重复数据拖重。

## 12. 我的最终建议

这一轮我建议正式收成下面几点：

1. **`MethodSemanticBody` 采用显式容器，而不是仅靠平铺 `Operations[]` + 隐含约定**
2. **首轮最小 body 结构冻结为：`MethodId + Inputs + Spans + Operations`**
3. **`ValueRef / SpanId / OperationId` 都明确归属当前 method body**
4. **`LiteralRef` 和 subject ids 继续走 world-level pool / identity，不在 body 内重复建本地池**
5. **`ReferencedLiteralRefs / ReferencedSubjects / DerivedCapabilities / Diagnostics` 等派生 summary 不进入首轮 canonical body**
6. **method body 内唯一的语义顺序主轴是 `Operations[]`，`Spans` 只是 side table**

## 13. 这一步之后最值得继续讨论的点

如果你认可这个方向，下一步最自然的问题就是：

- **`MethodSemanticBody` 的 well-formedness / validation invariants 到底怎么冻结**

也就是继续往下收：

- `ValueRef` 唯一性
- `ResultRef` / input ref 不冲突
- span resolve 规则
- operation 引用前序值还是任意值
- 哪些错误归 `SemanticWorld`，哪些错误要拖到 `CodeGen`

