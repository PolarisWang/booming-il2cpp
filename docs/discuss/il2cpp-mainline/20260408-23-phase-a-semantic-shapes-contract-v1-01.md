# Phase A SemanticShapes Contract v1.01

Date: 2026-04-08
Status: design-discuss

## 1. 问题点

上一轮我们已经把 `SemanticWorld` 的正式输出面收成了六块：

- `CanonicalSubjects`
- `SemanticShapes`
- `MethodBodies`
- `CapabilityBundles`
- `LiteralPool`
- `Diagnostics`

但其中 `SemanticShapes` 这块还只是被“点了名”，还没有真正冻结它自己的 contract。

如果这一步不继续收口，后面很容易重新漂掉：

1. 下游会分不清哪些信息属于 `CanonicalSubjects`，哪些属于 `SemanticShapes`
2. method role、body availability、imported-ness 很容易又靠隐式推理去猜
3. `CapabilityBundles` 会被迫承接本该由 shape 表达的结构信息
4. `entry`、retained、dependency 这类 link-time 概念也容易错误混进 shape

所以这一轮真正要回答的是：

- **`SemanticShapes` 到底回答什么问题**
- **它的最小 vocabulary 应该怎么冻结**
- **它和 `CanonicalSubjects / MethodBodies / CapabilityBundles` 的边界怎么切**

## 2. 这里讨论的不是哪件事

这里讨论的不是：

- subject 的 canonical id 长什么样
- method body 内部 operation stream 长什么样
- capability bundle 怎么归纳
- `Linker` 怎么选 entry / retained closure
- `CodeGen` 怎么选 lowering family

这里讨论的是：

- **某个 canonical subject 在当前 semantic slice 里，到底是什么语义形状**

一句话说：

- `CanonicalSubjects` 回答“谁是谁”
- `SemanticShapes` 回答“它在当前切片里是什么”

## 3. 三种可选方案

## 3.1 方案 A：平面 shape 名字表，直接把所有差异编码进名字

### 方案描述

做法是：

- 每个 subject 只挂一个平面 shape 名字

例如：

- `reference-type`
- `instance-field`
- `constructor-with-body`
- `instance-method-with-body`
- `static-entry-method-with-body`
- `imported-method-no-body`

### 优点

- 看起来最直接
- 早期容易写例子

### 缺点

- 很容易把多个维度揉成一个名字
- role、body availability、entry-ness、imported-ness 全挤在一起
- vocabulary 会很快膨胀
- 下游很难判断某两个 shape 是否只是“同角色不同附加状态”

### 我的判断

- **不推荐**

这条路短期直观，长期很容易名字爆炸。

## 3.2 方案 B：按 subject kind 分族的 typed shapes，正交表达 role / body availability

### 方案描述

做法是：

- `SemanticShapes` 按 subject kind 给出不同 shape family
- 每个 family 只表达该类 subject 真正需要的 shape 维度

例如：

- `TypeShape`
- `FieldShape`
- `MethodShape`

其中 method 再把：

- `Role`
- `BodyAvailability`

拆成正交字段，而不是揉进一个大名字。

### 优点

- 边界最清楚
- vocabulary 更稳定
- 便于明确哪些组合合法，哪些组合非法
- 最符合我们前面一路都在做的“typed contract”方向

### 缺点

- 比平面名字表多一层结构
- 需要 upfront 决定各类 subject 的最小 shape family

### 我的判断

- **推荐**

这是当前 `Phase A` 最平衡的方案。

## 3.3 方案 C：不显式输出 shapes，下游从 body/capability/metadata 自己推

### 方案描述

做法是：

- `SemanticShapes` 尽量做薄，甚至不单独成块
- 下游需要时自己从：
  - `CanonicalSubjects`
  - `MethodBodies`
  - `CapabilityBundles`
  - metadata lookup

再推“这应该是什么 shape”。

### 优点

- 表面上输出面更轻

### 缺点

- 又回到隐式约定
- `Linker`/`CodeGen` 很容易各自补一套 shape 推理
- 和前面已经拍板的强边界路线直接冲突

### 我的判断

- **不推荐**

## 4. 我的建议

如果这一轮要拍板，我建议采用：

- **方案 B：按 subject kind 分族的 typed shapes，正交表达 role / body availability**

一句话定义：

- `SemanticShapes` 是 canonical subject 的 typed semantic-role layer
- 它不重复 identity
- 不重复 method body
- 不重复 capability summary
- 也不承接 linker/codegen 的决策语义

## 5. 我建议冻结的最小 shape family

当前 `Phase A` 我建议先把 `SemanticShapes` 收成下面三族：

- `TypeShape`
- `FieldShape`
- `MethodShape`

我当前**不建议**一开始就冻结：

- `PropertyShape`
- `ParameterShape`
- 更复杂的 type/member 专用 family

原因很简单：

- 先覆盖当前已经真正进入主线 contract 的 semantic slice
- 不为了“以后可能会用到”提前把 shape 体系做大

## 6. 我建议的 `TypeShape`

当前 `Phase A` 我建议 `TypeShape` 先收得很克制：

```text
TypeShape {
  Kind = reference-type
}
```

也就是说，当前 shape 层先只回答：

- 这是当前 slice 中的 `reference type`

而不急着把更多东西混进去，例如：

- token
- owner
- generic arguments
- runtime layout

这些都不属于 type shape 本身。

如果后面 `Phase B` 进入更多 type 语义，再扩 type family。

## 7. 我建议的 `FieldShape`

当前 `Phase A` 我建议 `FieldShape` 先冻结为：

```text
FieldShape {
  Kind = instance-field
}
```

这回答的是：

- 这个 field 在当前 slice 里是实例状态字段

它不回答：

- 有没有被读取
- 有没有被写入
- 下游需要什么能力

那些分别属于：

- method body evidence
- capability summary

## 8. 我建议的 `MethodShape`

这是这一轮最关键的一族。

我建议把 `MethodShape` 明确拆成两个正交维度：

```text
MethodShape {
  Role
  BodyAvailability
}
```

## 8.1 `Role`

当前 `Phase A` 我建议首轮最小 `Role` 集合冻结为：

- `constructor`
- `instance-method`
- `static-method`
- `imported-method`

这里的 `imported-method` 指的是：

- semantic 上属于 imported direct-call subject 的 method

它不是：

- “外部 managed dependency method”
- 也不是“没有 body 的普通 static method”

## 8.2 `BodyAvailability`

我建议显式冻结为：

- `has-canonical-body`
- `no-canonical-body`

注意这里我刻意不用“有无原始 method body”这种说法，
而是强调：

- **是否拥有 canonical semantic body**

因为我们现在讨论的是 semantic layer contract，
不是 loader 原始输入状态。

## 8.3 为什么我建议把 `Role` 和 `BodyAvailability` 分开

因为如果揉成一个 shape 名字，很快就会长成：

- `constructor-with-body`
- `static-method-no-body`
- `imported-method-no-body`

这样会有两个问题：

### 1. vocabulary 会迅速膨胀

### 2. 下游无法清楚看出“这是 role 变了，还是 body availability 变了”

拆开之后就清楚很多：

- `Role = static-method`
- `BodyAvailability = has-canonical-body`

和：

- `Role = imported-method`
- `BodyAvailability = no-canonical-body`

是两个明确维度。

## 9. 我建议明确冻结的 shape 边界

## 9.1 shape 不回答 identity

例如下面这些仍然属于 `CanonicalSubjects` 或 stable metadata：

- `TypeId`
- `MethodId`
- `FieldId`
- owner
- signature
- token
- arity

shape 不应重复这些。

## 9.2 shape 不回答 body 内发生了什么

例如：

- `read-instance-field`
- `string-concat`
- `emit-console-string`

这些属于 `MethodBodies.OperationStream`，
不是 shape。

## 9.3 shape 不回答“下游需要什么能力”

例如：

- `uses-instance-field-state`
- `requires-string-concat`
- `requires-console-string-output`

这些属于 `CapabilityBundles`。

## 9.4 shape 不回答 linker/codegen 决策

例如：

- `entry`
- `retained`
- `dependency-root`
- `captured-state-instance-message`
- `pair-concat-lowering`

这些都不应进入 `SemanticShapes`。

## 10. 一个很关键的收口：`entry` 不是 shape

这一点我建议明确钉死。

前面举例时，为了快速说明，有过类似：

- `static-entry-method-with-body`

这种近似表达。

但如果这一轮要正式收 contract，
我建议把它收正为：

- `Role = static-method`
- `BodyAvailability = has-canonical-body`

而：

- `entry`

不属于 method shape，
它属于：

- `Linker` 的 `Entries`
- 或更上游的 root selection 语义

原因很简单：

- 同一个 method 可以在一个运行配置里是 entry
- 在另一个配置里不是 entry

但它的 semantic shape 并没有变。

所以：

- **entry 是 link/root 语义，不是 semantic shape**

## 11. 用例子看推荐方案

继续用 `Greeter` 例子：

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

推荐理解成：

```text
SemanticShapes {
  Greeter => TypeShape {
    Kind = reference-type
  }

  Greeter::_name => FieldShape {
    Kind = instance-field
  }

  Greeter::.ctor => MethodShape {
    Role = constructor
    BodyAvailability = has-canonical-body
  }

  Greeter::Build => MethodShape {
    Role = instance-method
    BodyAvailability = has-canonical-body
  }

  Program::Main => MethodShape {
    Role = static-method
    BodyAvailability = has-canonical-body
  }
}
```

注意这里：

- `Program::Main` 没有 shape `entry`
- 是否作为 entry root，交给 `Linker.Entries`

## 12. 我建议明确禁止的几类 shape

为了防止 shape 这条线长歪，
我建议把下面几类名字或字段明确列入禁区：

### A. capability 名

例如：

- `requires-string-concat`
- `uses-instance-field-state`

这些是 capability，不是 shape。

### B. body 内容摘要

例如：

- `reads-instance-field`
- `emits-console-string`

这些是 body evidence 或 capability summary，不是 shape。

### C. linker/root 名

例如：

- `entry-method`
- `retained-method`

这些是 link/root 语义，不是 shape。

### D. lowering/family 名

例如：

- `captured-state-instance-message`
- `generic-forwarder-getter`
- `imported-call-lowering-ready`

这些都已经越过 semantic shape 层了。

## 13. 我建议的最小 well-formedness 关系

虽然这一轮不是专门谈 validation，
但 `SemanticShapes` 一旦冻结，至少自然带出几条关系：

- `MethodShape.BodyAvailability = has-canonical-body`
  - 则应存在对应 `MethodBody`
- `MethodShape.Role = imported-method`
  - 当前 `Phase A` 下应与 `BodyAvailability = no-canonical-body` 一致
- static method
  - 不应要求 `ThisValueRef`
- instance method / constructor
  - 若存在 canonical body，则 input shape 应与 instance receiver 一致

这些规则后面可以继续细化，
但现在先把 shape contract 钉住更重要。

## 14. 我的最终建议

这一轮我建议正式拍板下面几点：

1. **`SemanticShapes` 采用按 subject kind 分族的 typed shape contract，而不是平面 shape 名字表**
2. **首轮最小 shape family 冻结为：`TypeShape / FieldShape / MethodShape`**
3. **`MethodShape` 明确拆成 `Role + BodyAvailability` 两个正交维度**
4. **首轮最小 method role vocabulary 建议冻结为：**
   - `constructor`
   - `instance-method`
   - `static-method`
   - `imported-method`
5. **`BodyAvailability` 明确冻结为：`has-canonical-body / no-canonical-body`**
6. **`entry` 明确不属于 `SemanticShape`，而属于 `Linker` 的 root/entry 语义**
7. **shape 只回答“subject 在当前 semantic slice 里是什么”，不回答 identity、body evidence、capability summary、linker/root 语义或 lowering 决策**

## 15. 这一步之后最值得继续讨论的点

如果你认可这个方向，下一步最自然的问题就是：

- **`CanonicalSubjects` 自身的 contract 到底怎么冻结**

也就是继续往下收：

- `TypeId / MethodId / FieldId / MemberId` 的最小关系面
- owner / declaring relationship 放到哪一层
- definition identity 和 instantiated identity 的边界怎么切

