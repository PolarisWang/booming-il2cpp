# Phase A ValueRef Model v1.01

Date: 2026-04-08
Status: design-discuss

## 1. 问题点

前面几步我们已经基本收住了：

- operation vocabulary 停在 semantic-action 级
- payload 采用 typed payload，而不是 bag-shaped payload
- operation contract 倾向于“小 header + kind-specific payload + shared spans”

那么下一步最关键的问题就是：

- **operation 之间到底如何传递“值”**

也就是：

- `ReceiverRef` 指向的到底是什么
- `ArgumentSpanId` 里的元素到底是什么
- `return(value=...)` 里的 `value` 到底是什么

如果这里不先收住，后面 `CodeGen` 消费时会很容易出现三种漂移：

1. 有的地方直接拿 `OperationId` 当值引用
2. 有的地方参数单独用 `ParameterRef`，结果又用 `ResultRef`
3. 有的地方仍然保留接近 IL stack 的“第几个操作数槽位”

所以这一轮真正要回答的是：

- **`ValueRef` 的 canonical 作用域、身份和来源模型到底是什么**

## 2. 这里讨论的不是哪件事

这里讨论的不是：

- 最终 C++ 层怎么表达临时变量
- 运行时对象身份怎么追踪
- 完整 SSA IR 要不要做控制流 merge / phi 节点
- 优化器怎么做值编号

这里讨论的是：

- **在 `SemanticWorld` 的 method-level canonical operation stream 中，值引用应该怎么定义**

也就是一个更基础的问题：

- operation payload 里的“输入值”究竟引用什么抽象对象

## 3. 三种可选方案

## 3.1 方案 A：接近 IL/evaluation-stack 的位置型引用

### 方案描述

做法是让消费者按“位置”理解值来源，例如：

- 第 N 个 operation 的结果
- 当前 stack slot 0
- 当前 stack slot 1
- 某个临时槽位

也可能表现成：

- `OperandRef(opIndex=7, slot=0)`

### 优点

- 从 IL 过渡过来最直观
- 早期 Loader 到 SemanticWorld 的实现似乎更省事

### 缺点

- evaluation stack 的味道太重
- 一旦做 semantic normalization，位置含义就不再稳定
- `CodeGen` 仍然要理解“这个值到底是谁”
- 很容易把 semantic layer 拉回“漂亮一点的 IL dump”

### 我的判断

- **不推荐**

这条路和我们前面已经拍板的 semantic-action 方向冲突。

## 3.2 方案 B：直接用 `OperationId` 充当值引用

### 方案描述

做法是：

- 每个产出值的 operation，都默认“它自己的 `OperationId` 就是值 id”
- payload 中凡是需要输入值的地方，都直接引用前序 `OperationId`
- 方法参数和 `this` 另走独立通道，例如：
  - `ThisRef`
  - `ParameterRef`

### 优点

- 表面上最简洁
- 少一层 id 映射
- 对“一个 operation 最多产出一个值”的切片比较自然

### 缺点

- operation 身份和 value 身份被绑死
- 输入参数 / `this` / operation 结果变成不同 ref 家族
- payload 消费方要处理更多分支
- 后续若出现“需要保留 operation identity，但替换 value identity”的场景，扩展会比较别扭

### 我的判断

- **可以工作，但不是最稳**

它适合非常轻量的内部 IR，但作为 canonical semantic contract，边界不够干净。

## 3.3 方案 C：方法内统一 `ValueRef` 命名空间

### 方案描述

做法是：

- 每个 method 拥有自己的 canonical `ValueRef` 命名空间
- 所有“可被后续 operation 消费的值”都统一表示为 `ValueRef`
- `ValueRef` 的来源可以是：
  - method input
  - operation result

例如：

- `this` 是一个 `ValueRef`
- 参数 `name` 是一个 `ValueRef`
- `construct-object` 结果是一个 `ValueRef`
- `read-instance-field` 结果也是一个 `ValueRef`

operation payload 只关心“我消费哪个 `ValueRef`”，
而不必关心这个值最初来自参数、`this` 还是前一个 operation。

### 优点

- 值的消费模型最统一
- payload contract 最干净
- `SpanId` 可以天然承载 `ValueRef` 列表
- `CodeGen` 看到的是统一 def-use 关系，而不是混杂的 ref 家族
- 很符合 method-level SSA-like 思维，但又不等于现在就引入完整 SSA IR

### 缺点

- 需要单独定义输入值表或 value origin 表
- 比“直接拿 OperationId 当值”多一层抽象

### 我的判断

- **推荐**

这条路最符合我们前面已经定下来的 canonical semantic layer 目标。

## 4. 我的建议

如果这一轮要拍板，我建议采用：

- **方案 C：方法内统一 `ValueRef` 命名空间**

一句话定义：

- `ValueRef` 是 **method-local、stable、canonical 的值身份**
- 它不等于 IL stack slot
- 它不等于 `OperationId`
- 它也不拆成参数值 / `this` / 结果值三套不同 ref 语言

## 5. 我建议冻结的 `ValueRef` 基本原则

## 原则 1：`ValueRef` 作用域限定在单个 method 内

我建议当前明确收成：

- `ValueRef` 只在单个 method 的 canonical operation stream 内有效
- 不跨 method
- 不跨 type
- 不跨 `SemanticWorld` 全局命名空间

原因是：

- 值流天然是 method-local 语义
- 跨 method 的 identity 应该由 `MethodId / TypeId / MemberId` 这类 subject id 负责

## 原则 2：`ValueRef` 是“值身份”，不是“操作身份”

也就是：

- `OperationId` 用于标识某个语义动作
- `ValueRef` 用于标识某个语义动作产出的值，或 method 输入值

这两个身份虽然常常一一对应，
但我建议在 contract 上明确分离。

## 原则 3：`ValueRef` 是 SSA-like 的单一定义值

当前 `Phase A` 我建议把 `ValueRef` 理解成：

- 每个 `ValueRef` 都有唯一来源
- 来源要么是 method input
- 要么是某个 value-producing operation 的 result

这是一种 **SSA-like** 的值模型，
但它不等于我们现在就引入完整 SSA block/phi 体系。

## 原则 4：method input 也进入同一套 `ValueRef` 空间

例如：

- `this` 可以是一个 `ValueRef`
- 参数 `arg0`、`arg1` 也都是 `ValueRef`

这样 payload 中：

- `ReceiverRef`
- `ArgumentSpan`
- `ReturnPayload.ValueRef`

都只需要说同一种语言：`ValueRef`

## 6. 我建议的 method-level value shape

我更推荐方法级别显式拥有一份输入值定义，例如：

```text
MethodValueSpace {
  ThisValueRef?
  ParameterValueRefs[]
}
```

再加上一组 operation result：

```text
Operation {
  OperationId
  Kind
  ResultRef?
  PayloadRef
}
```

这样整个 method 内的值来源就只有两类：

1. method inputs
2. operation results

这已经足够覆盖当前 `Phase A` 讨论范围。

## 7. 用例子看为什么统一 `ValueRef` 更顺

## 7.1 `Greeter.Build()`

```csharp
public string Build()
{
    return "Hello, " + _name;
}
```

如果采用推荐方案，可以近似理解成：

```text
Inputs:
  v0 = this

Operations:
  op1: load-const-string("Hello, ") -> v1
  op2: read-instance-field(receiver=v0, field=Greeter::_name) -> v2
  op3: string-concat(inputs=[v1, v2]) -> v3
  op4: return(value=v3)
```

这里最关键的是：

- `this` 和 `op2` 的结果都只是 `ValueRef`
- `string-concat` 不需要知道输入分别来自参数、字段读取还是常量加载

## 7.2 `Main()`

```csharp
static void Main()
{
    var g = new Greeter("Alice");
    Console.WriteLine(g.Build());
}
```

可以近似理解成：

```text
Operations:
  op1: load-const-string("Alice") -> v0
  op2: construct-object(type=Greeter, args=[v0]) -> v1
  op3: invoke-instance-method(receiver=v1, method=Greeter::Build, args=[]) -> v2
  op4: emit-console-string(value=v2)
  op5: return()
```

这里：

- `v1` 是对象值
- `v2` 是调用结果值
- 对下游来说，这就是统一的值引用语言

## 8. 为什么我不建议把参数 / `this` / 结果拆成不同 ref 类型

一种常见诱惑是这样分：

- `ThisRef`
- `ParameterRef`
- `ResultRef`

看起来很精确，但我不建议作为 payload 消费面的主语言。

原因是：

- 消费方需要写更多分支
- `SpanId` 中的元素类型会变复杂
- 统一 def-use 关系会被拆碎
- 很多 operation 其实不在乎“这个值起源于哪里”，只在乎“这是我要消费的输入值”

所以我更推荐：

- **origin 可以区分**
- **payload 消费面仍然统一只看 `ValueRef`**

也就是：

- 来源信息存在 value table / origin table
- 但 operation payload 不直接暴露多套 ref 家族

## 9. `ValueRef` 和 `LiteralRef` 的关系

我建议先明确一条边界：

- `LiteralRef` 不是 `ValueRef`
- `LiteralRef` 指向 literal pool 中的稳定常量条目
- `load-const-string` / `load-const-int32` 这样的 operation 才把 literal materialize 成 method 内的 `ValueRef`

也就是：

- literal 是“静态常量条目”
- value 是“当前 method 值流中的可消费值”

这条边界很重要，
否则后面很容易把“常量条目引用”和“值流引用”混成一件事。

## 10. `ValueRef` 和 `SpanId` 的关系

如果采用推荐方案，那么 `SpanId` 最自然的定义就是：

- **有序的 `ValueRef` 列表引用**

例如：

- 调用实参列表
- `string-concat` 输入列表

但我仍然建议保留前一轮已经提出的原则：

- `receiver` 这类语义特殊输入，优先显式字段化
- `SpanId` 只承载角色同质的输入序列

例如：

- `InvokeInstanceMethodPayload`
  - `ReceiverRef`
  - `ArgumentSpanId`

而不是：

- 一个没有角色区分的 `AllOperandSpanId`

## 11. 我给你的三个判断建议

## 11.1 如果你更在意“尽快从 IL 过渡到第一版语义流”

你会更容易被方案 A 吸引。

但我的建议仍然是：

- **别选**

因为它会让 semantic layer 一直带着 stack 影子。

## 11.2 如果你更在意“contract 简洁、实现最省一层 id”

你可能会倾向方案 B。

它不是不能做，但我认为它把 operation identity 和 value identity 绑得太紧。

## 11.3 如果你更在意“主线 contract 长期稳定”

那我明确建议选方案 C：

- method-local unified `ValueRef`

这是当前最稳的主线。

## 12. 我的最终建议

这一轮我建议正式收成下面几点：

1. **`ValueRef` 定义为 method-local、stable、canonical 的值身份**
2. **`ValueRef` 不等于 IL stack slot，不等于 `OperationId`**
3. **当前 `Phase A` 采用 SSA-like 的单一定义值模型，但暂不引入完整 phi/merge 体系**
4. **method inputs（`this`、parameters）也进入同一套 `ValueRef` 命名空间**
5. **operation payload 统一消费 `ValueRef`，而不是拆成 `ThisRef / ParameterRef / ResultRef` 多套 ref 语言**

## 13. 这一步之后最值得继续讨论的点

如果你认可这个方向，下一步最自然的问题就是：

- **`LiteralRef` 应该采用什么粒度和池化策略**

也就是继续往下收：

- literal 是 method-local pool、module-local pool，还是 `SemanticWorld` 级 pool
- 哪些常量必须进 literal pool
- 哪些 query key 应不应该和 literal 共池

