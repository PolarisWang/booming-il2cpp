# Phase A Layered Core Pipeline Concrete Responsibilities v1.01

Date: 2026-04-08
Status: design-discuss

## 1. 问题点

前几轮我们已经把 `Phase A` 的推荐方向收敛到：

- `SemanticWorld` 负责当前支持切片的 canonical semantic layer
- `Linker` 负责 retained world / dependency closure
- `CodeGen` 负责 lowering family 选择与 lowering plan 组装
- `Emitter` 只负责消费 plan 并渲染最终 native proof

但这些描述仍然比较抽象。  
如果没有一个“拿最小 C# 例子逐层走一遍”的具体说明，后面很容易继续出现三类错位：

1. 把 `SemanticWorld` 当成“更漂亮的 Loader 输出”
2. 让 `Linker` 继续顺手做 lowering 级 decision
3. 让 `Emitter` 继续从 `typed-il` 或 proof-shape 反推语义

所以这一轮真正要补的是：

- **在推荐的 B 方案下，这几层分别做什么，分别不做什么。**

## 2. 问题的具体描述

当前最容易混淆的是下面这几个边界：

- `Loader` 读出的是 raw metadata / raw IL / raw member records
- `SemanticWorld` 应该讲清“这些东西在当前支持子集里是什么意思”
- `Linker` 应该回答“为了让这些语义真正可运行，需要留下什么”
- `CodeGen` 应该回答“这些已保留语义要走哪种 lowering family”
- `Emitter` 应该回答“按既定 family 和 role bindings，最终 C++ 怎么写”

如果这五层不被具体例子钉住，后续讨论就会一直停在抽象句子：

- “这里是不是语义归一化？”
- “这里是不是 dependency policy？”
- “这里是不是 lowering?”

而不是能明确说出：

- “这一步该在 world 做”
- “这一步该在 linker 做”
- “这一步该在 codegen 做”

所以这一轮的关键不是再发明新术语，而是给已有边界一个可反复复用的 concrete walkthrough。

## 3. 有哪些可解决的路径

### 路径 A：只保留抽象边界定义，不补具体例子

做法：

- 继续只用：
  - “world 讲语义”
  - “linker 做 retained”
  - “codegen 选 lowering”
  - “emitter 做渲染”
- 不再补具体 C# 例子

优点：

- 文档最短
- 不容易被样例细节带偏

缺点：

- 仍然很抽象
- 后续不同人会对同一句边界有不同理解
- 不能有效回答“`String.Concat(3)` 拆 pair concat 到底算哪层”这类关键问题

判断：

- 不推荐

### 路径 B：用最小 C# 例子把推荐分层逐层走一遍，并明确每层的非职责

做法：

- 选 1-2 个最小 C# 例子
- 对每一层都写：
  - 输入是什么
  - 输出是什么
  - 它具体做什么
  - 它明确不做什么
- 特别补一类容易混淆的 lowering 例子

优点：

- 最容易把抽象边界变成稳定共识
- 最适合后续继续讨论 `Linker` output contract、`LoweringPlan` role contract
- 可以直接当作未来 review 或设计讨论的参照物

缺点：

- 需要克制样例数量，避免又退回 sample-driven 思维
- 需要明确“例子只是解释，不是 contract 真源”

判断：

- 推荐

### 路径 C：直接用实现层 pseudo-code 来解释职责

做法：

- 不先写层次 walkthrough
- 直接用 pseudo-code 或未来的 data model 草图解释

优点：

- 对实现导向的人更直观

缺点：

- 容易把“推荐边界”直接绑死到当前实现形态
- 当前 `Phase A` 目标是收 contract，不是先冻具体类和函数签名

判断：

- 暂不推荐作为主路径

## 4. 我的方案和理由

我推荐：

- **路径 B：用最小 C# 例子把推荐分层逐层走一遍，并明确每层的非职责**

原因很简单：

1. 当前真正缺的不是新概念，而是 concrete intuition
2. 只要 concrete walkthrough 稳了，后续 `Linker / CodeGen / Emitter` 的争议会明显变少
3. 这一步不会把 `Phase A` 拉成实现计划，仍然停留在 contract 层

如果按这条路推进，我建议先固定两个最小例子。

### 例子 1：对象创建 + 实例状态 + 字符串拼接 + 控制台输出

```csharp
class Greeter
{
    private readonly string _name;

    public Greeter(string name)
    {
        _name = name;
    }

    public string Build()
    {
        return "Hello, " + _name;
    }
}

static void Main()
{
    var g = new Greeter("Alice");
    Console.WriteLine(g.Build());
}
```

这个例子适合解释当前推荐分层，因为它同时覆盖：

- reference type
- instance field state
- constructor
- instance method
- string concat
- console output

#### `Loader` 在这个例子里做什么

- 读取出：
  - `Greeter` type
  - `_name` field
  - `.ctor(string)` method
  - `Build()` method
  - `Main()` method
- 读取 raw IL 指令流
- 读取 `Console.WriteLine(string)`、`String.Concat(...)` 这类 raw member reference

它不应做：

- 判断这是哪种 lowering family
- 判断最终需要哪些 retained dependencies
- 判断 `_name` 在当前支持切片里是不是“captured state field”

#### `SemanticWorld` 在这个例子里做什么

- 把 raw records 归一化成当前支持切片里的 canonical semantic objects：
  - `Greeter` 是 reference type
  - `_name` 是 instance field
  - `.ctor(string)` 是 constructor role
  - `Build()` 是 instance method role
  - `Main()` 是 entry method role
- 输出更高一层的 semantic operation stream，例如：
  - `new-object`
  - `call-instance`
  - `read-instance-field`
  - `string-concat`
  - `call-static(console-write-line)`
- 输出 capability / requirement hints，例如：
  - `uses-instance-field-state`
  - `requires-string-concat`
  - `requires-console-string-output`

它不应做：

- 决定最终保留哪些 methods/types
- 决定 `String.Concat` 最终在 C++ 中怎么 lowering
- 决定生成哪套模板

#### `Linker` 在这个例子里做什么

- 从 `Main()` 出发形成 retained world：
  - `Main`
  - `Greeter::.ctor`
  - `Greeter::Build`
  - `Greeter::_name`
  - `Greeter`
- 根据 semantic requirements 计算 external managed dependencies
- 给 dependencies 附 reason，例如：
  - 因 `requires-console-string-output` 引入 `Console.WriteLine(string)`
  - 因 `requires-string-concat` 引入 `String.Concat(...)`

它不应做：

- 选择 lowering family
- 决定 `String.Concat(string,string,string)` 要不要拆成 pair concat
- 继续从 raw instruction shape 推断 business semantics

#### `CodeGen` 在这个例子里做什么

- 看 linked semantic world，判断这属于哪一类 lowering family
- 组装显式 `LoweringPlan`，例如把这些 role 绑定进去：
  - `entry-method = Main`
  - `constructor-method = Greeter::.ctor`
  - `instance-message-method = Greeter::Build`
  - `reference-type = Greeter`
  - `captured-state-field = _name`
  - `runtime-role = console-write-line-string`
  - `runtime-role = string-concat-pair`
  - `literal = "Hello, "`

它不应做：

- 把 plan 留成一堆未命名的 nullable 字段袋
- 要求 emitter 再次回去理解 typed-il 才能知道这些 role 是谁

#### `Emitter` 在这个例子里做什么

- 只消费 `LoweringPlan`
- 选择对应模板
- 把 translation unit 渲染出来

它不应做：

- 再去扫描 typed-il 猜“这是不是 HelloWorldObject 那类 shape”
- 再去推断 `_name` 到底是不是 captured state field
- 再去决定 `string concat` 走哪种 lowering

### 例子 2：`String.Concat` 的 lowering 决策应该落在哪层

```csharp
Console.WriteLine(string.Concat(a, b, c));
```

这个例子专门用来钉住一件当前最容易摇摆的事：

- **3 参数 concat 拆不拆成 pair concat，不是 `Linker` 的事，而是 `CodeGen` 的 lowering 决策。**

按推荐边界：

- `Loader`
  - 只读出 raw call 和 raw member reference
- `SemanticWorld`
  - 只讲清“这里存在 string concat 语义”
- `Linker`
  - 只保留需要的 methods/dependencies，并说明这是因为 `requires-string-concat`
- `CodeGen`
  - 才决定：
  - 是直接 lowering 成某个 family
  - 还是拆成两次 pair concat
- `Emitter`
  - 按既定 lowering plan 渲染

这一点非常关键，因为它直接说明：

- lowering 级 canonicalization 不是 linker policy
- 而是 codegen policy

### 建议固定的记忆方式

为了让这套边界更容易在后续讨论里复用，我建议把五层先记成下面五句话：

- `Loader`：把程序集和 IL 读出来
- `SemanticWorld`：讲清这些 IL 在当前支持切片里是什么意思
- `Linker`：决定为了运行这些语义，要留下什么
- `CodeGen`：决定这些已保留语义如何降成 C++
- `Emitter`：把既定 plan 真正写成 C++

## 5. 审查下这个问题本身，是否合理，以及是否有其他方式让这个方案在整体框架中更合理地解决

我认为这个问题本身非常合理，而且它正好是当前主线讨论最该补的一环。

因为前几轮文档已经把边界收得越来越清楚：

- `SemanticWorld` 负责 canonical semantic layer
- `Linker` 负责 retained/dependency
- `CodeGen` 负责 lowering
- `Emitter` 负责渲染

但如果没有 concrete walkthrough，这些句子仍然容易在实现和设计讨论里被重新解释。

这个问题更合理的处理方式，不是现在就进入实现层 pseudo-code，而是先补两条约束：

### A. 例子只用于解释边界，不作为 sample-driven contract 真源

也就是说：

- `Greeter` 例子只是帮助理解职责
- 不意味着主线 contract 以后要围着这个样例命名

### B. 例子要优先解释“哪层不该做什么”

当前最重要的不是证明每层都能做事，而是防止职责回流：

- 防止 `Linker` 回流成 lowering selector
- 防止 `Emitter` 回流成 proof-shape detector
- 防止 `SemanticWorld` 退化回 passthrough

### C. 这份 concrete walkthrough 应成为后续讨论的共同前提

下一轮如果继续细化，我建议直接基于这份 concrete walkthrough 往下走：

1. `Linker` 的输出 contract 最少该长什么样
2. dependency reason taxonomy 还需要冻结到什么程度
3. `CodeGen` 选择 lowering family 时，哪些输入必须显式来自 `LinkedWorld`

所以，这一轮的最终结论是：

- **推荐继续采用 B 方案的推荐分层**
- **推荐用最小 C# 例子把五层职责具体化**
- **推荐把 “3 参数 concat 拆 pair concat” 明确归到 `CodeGen` lowering policy，而不是 `Linker` policy**
- **推荐把这份 concrete walkthrough 作为后续 `Linker / CodeGen / Emitter` 继续讨论的共同上下文**
