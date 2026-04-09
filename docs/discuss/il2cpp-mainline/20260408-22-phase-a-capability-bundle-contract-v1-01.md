# Phase A Capability Bundle Contract v1.01

Date: 2026-04-08
Status: design-discuss

## 1. 问题点

上一轮我们已经正式把 `CapabilityBundles` 提升成了 `SemanticWorld` 的 canonical output 之一。

但如果这一步不继续收口，后面很容易重新漂掉：

1. `SemanticWorld` 虽然输出了 capability，但没人知道它到底挂在哪一层
2. `Linker` 和 `CodeGen` 很可能各自再发明一套 capability 语言
3. capability bundle 容易退化成“换个名字的 operation list”
4. 也容易反过来长成“半个 lowering family 名字表”

所以这一轮真正要回答的是：

- **capability bundle 的附着粒度到底是什么**
- **bundle 本身应该是什么形状**
- **capability vocabulary 应该收在哪个层次**
- **它和 operation stream 的职责边界到底怎么切**

## 2. 这里讨论的不是哪件事

这里讨论的不是：

- `CodeGen` 最终怎么选 `LoweringFamily`
- dependency reasons 的 taxonomy 怎么继续细化
- runtime role / host binding vocabulary 怎么定义

这里讨论的是：

- **semantic layer 已经完成归一化之后，应该向下游额外承诺什么样的 capability summary**

也就是：

- `MethodBodies` 之外，`SemanticWorld` 还应该如何用稳定的摘要语言告诉下游：
  - 这组 semantic slice 需要什么能力
  - 下游应该把注意力放在哪类 requirement 上

## 3. 三种可选方案

## 3.1 方案 A：只做 method-level capability bundles

### 方案描述

做法是：

- capability 只挂在 method 上
- `SemanticWorld` 只输出：
  - `MethodId -> CapabilitySet`

不提供 world-slice 或更高层 summary。

### 优点

- 结构最简单
- capability 来源最清晰
- 最不容易过度设计

### 缺点

- `Linker` / `CodeGen` 仍然需要各自再做一次聚合
- entry-centered slice 或 retained slice 级别的 capability 语言不够直接
- 很多“世界级 requirement”只能靠下游再拼出来

### 我的判断

- **可以工作，但不够平衡**

## 3.2 方案 B：method-level primary bundles + world-slice aggregate bundle

### 方案描述

做法是：

- method 是 capability 的 primary attachment point
- 同时允许 `SemanticWorld` 再给出一个 world-slice aggregate bundle

也就是：

- `MethodId -> CapabilitySet`
- `WorldCapabilityBundle`

这里 world-slice aggregate 不是替代 method bundle，
而是对整个当前 semantic slice 的稳定汇总。

### 优点

- method-level 来源仍然清楚
- 下游可以直接获得整体 capability 视图
- 和前面已经拍板的 `CodeGen` family selection surface 更一致
- `Linker` 和 `CodeGen` 不必各自重复做第一轮聚合

### 缺点

- 需要明确“aggregate 是 canonical summary，不是随便做的报表”
- 要克制，不让 aggregate 长成带计数和解释的大杂烩

### 我的判断

- **推荐**

这是当前 `Phase A` 最平衡的方案。

## 3.3 方案 C：任意 subject 粒度都允许挂 capability

### 方案描述

做法是：

- method、type、field、member、world 都允许各自带 capability bundle

例如：

- `TypeId -> CapabilitySet`
- `FieldId -> CapabilitySet`
- `MethodId -> CapabilitySet`
- `WorldSlice -> CapabilitySet`

### 优点

- 表面上最灵活
- 某些查询场景看起来更方便

### 缺点

- capability 附着语义会迅速变乱
- 很多本应由 `SemanticShapes` 表达的结构信息，会被错误挪到 capability
- 下游会越来越难分辨“shape”和“capability”各自回答什么

### 我的判断

- **不推荐**

## 4. 我的建议

如果这一轮要拍板，我建议采用：

- **方案 B：method-level primary bundles + world-slice aggregate bundle**

一句话定义：

- capability 的 primary 归属是 method
- world-level bundle 只是对当前 semantic slice 的 canonical aggregate
- 不在 type/field/member 这些任意 subject 上泛滥挂 capability

## 5. 我建议冻结的 capability bundle 形状

当前 `Phase A` 我建议把 capability bundle 收成非常克制的形状：

```text
CapabilityBundle {
  Capabilities[]
}
```

其中：

- `Capabilities[]` 是 **无序的 canonical capability name set**

也就是说我当前明确**不建议**把下面这些混进 bundle 本体：

- occurrence count
- confidence score
- source explanation
- dependency mapping
- lowering hints
- runtime/backend preference

原因很简单：

- bundle 的职责是 summary
- 不是理由系统
- 不是 lowering classifier 细则
- 也不是 debug report

## 6. 我建议的附着粒度分工

## 6.1 MethodCapabilityBundle 是 primary fact

我建议明确：

- 每个拥有 canonical body 的 method，都可以拥有 method-level capability bundle

例如：

- `Greeter::Build -> { uses-instance-field-state, requires-string-concat }`
- `Program::Main -> { requires-console-string-output }`

这里 method bundle 是最直接的 semantic summary，
因为 capability 大多来自 method body 内的 semantic actions。

## 6.2 WorldCapabilityBundle 是 aggregate fact

我建议同时允许：

- 整个当前 semantic slice 再拥有一个 aggregate bundle

例如：

- `WorldCapabilityBundle -> { uses-instance-field-state, requires-string-concat, requires-console-string-output }`

它回答的是：

- 当前 semantic slice 整体对下游提出了哪些 requirement

但它不替代 method bundle，
也不应成为“唯一 capability source”。

## 6.3 我不建议先引入 type/field/member capability bundles

原因是：

- type/field/member 的结构语义更适合放在 `SemanticShapes`
- capability 更像“method body 驱动出来的下游 requirement summary”

例如：

- “这是 instance field”
  - 属于 shape
- “这段 slice uses-instance-field-state”
  - 属于 capability

如果一开始就允许 `FieldId -> uses-instance-field-state`，
边界会迅速糊掉。

## 7. 我建议的 capability vocabulary 原则

## 原则 1：capability 讲“下游需要支持什么”，不讲“这里发生了哪个具体 operation”

例如：

- `requires-string-concat`
- `requires-console-string-output`
- `requires-imported-call`

比下面这些更合适：

- `has-string-concat-operation`
- `has-emit-console-string-operation`

前者更像 requirement，
后者更像 operation dump。

## 原则 2：capability 要比 operation 更粗，但不能粗到失去作用

例如：

- `read-instance-field`
- `write-instance-field`

可以被归纳为：

- `uses-instance-field-state`

但如果把：

- `query-closed-type-member`
- `query-generic-type-definition`

都粗暴压成：

- `requires-some-reflection`

那对下游可能就太粗了。

## 原则 3：capability 不得泄漏 lowering/runtime/backend 决策

当前明确不应出现：

- `requires-pair-concat-lowering`
- `requires-console-icall`
- `requires-host-bridge-import`

这些已经越过 semantic layer 了。

## 原则 4：capability vocabulary 必须共享，不按 family 私有发明

也就是：

- 先有一套 shared capability language
- 不允许每个 `LoweringFamily` 自己发明一套只给自己看的 capability 词表

否则 capability 很快会退化成：

- “换皮的 family classifier 输入”

## 8. 我建议冻结的首轮最小 capability vocabulary

当前 `Phase A` 我建议先冻结下面这批最小 capability names：

- `uses-instance-field-state`
- `requires-string-concat`
- `requires-console-string-output`
- `requires-closed-type-member-query`
- `requires-generic-type-definition-query`
- `requires-imported-call`

## 8.1 为什么是这几个

它们刚好覆盖当前已经冻结进 operation vocabulary 和 proof spine 的几条主线：

- instance state
- string concat
- console string output
- reflection closed-type query
- generic type definition query
- imported direct call

## 8.2 为什么不是给每个 operation 一对一造 capability

因为那样会迅速变成：

- `requires-load-const-string`
- `requires-read-instance-field`
- `requires-return`

这类名字几乎不提供新的下游信息，
只是在重复 operation stream。

## 8.3 为什么不先压得特别粗

例如只保留：

- `requires-reflection-query`

也不是当前最佳选择。

因为对当前切片来说：

- `query-closed-type-member`
- `query-generic-type-definition`

已经是不同的稳定 semantic requirement，
粗暴合并会降低 bundle 的判别力。

## 9. capability bundle 和 operation stream 的职责边界

这是这一轮最重要的一条边界。

我建议把二者关系明确理解成：

- `MethodBody.OperationStream`
  - 是 method-level semantic evidence
- `CapabilityBundle`
  - 是对这些 evidence 的稳定 requirement summary

也就是：

- operation stream 讲“method 内发生了什么语义动作”
- capability bundle 讲“这组语义动作让下游必须具备哪些能力”

## 9.1 operation stream 仍然是更细的事实来源

例如：

- `read-instance-field`
- `write-instance-field`

都可能落到：

- `uses-instance-field-state`

所以 capability 不能替代 operation stream，
也不应让下游完全失去细节。

## 9.2 capability bundle 不应退化成“下游自己推一遍就行的可选缓存”

这点也很关键。

因为前面已经拍板：

- `CodeGen` family selection 的 primary signals 之一就是 capability bundle
- `Linker` 也可用它做 requirement / obligation 归纳

所以 capability 不是“有也行，没有也行”的派生缓存，
而是 semantic layer 正式承诺给下游的 summary surface。

## 9.3 capability bundle 不负责单独唯一决定 family

即使 capability bundle 是正式 output，
我仍然不建议把它抬成唯一 classifier。

前面已经拍板：

- `LoweringFamily` 的主决策面 = `semantic subject structure + capability bundle`

也就是说：

- capability 重要
- 但它始终是和 semantic structure 一起工作
- 它不应自己退化成“family 名字的另一种写法”

## 10. 用例子看推荐方案

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
MethodCapabilityBundles {
  Greeter::Build = {
    uses-instance-field-state,
    requires-string-concat
  }

  Program::Main = {
    requires-console-string-output
  }
}

WorldCapabilityBundle = {
  uses-instance-field-state,
  requires-string-concat,
  requires-console-string-output
}
```

这里：

- method bundle 给出局部 semantic requirement
- world bundle 给出当前 semantic slice 的整体 requirement 视图
- `Linker` 和 `CodeGen` 都不需要再自己重做第一轮 capability 聚合

## 11. 我建议明确禁止的几类 capability

为了防止 capability 这条线长歪，
我建议把下面几类名字明确列入禁区：

### A. operation 镜像名

例如：

- `has-read-instance-field`
- `has-load-const-string`

这类名字太贴近 operation stream，没有必要。

### B. family/proof 名

例如：

- `captured-state-instance-message`
- `generic-forwarder-getter`

这类名字会把 semantic summary 直接拉成 family classifier。

### C. lowering/runtime/backend 名

例如：

- `requires-pair-concat-lowering`
- `requires-console-write-line-icall`
- `requires-native-import-resolution`

这类名字明显越界。

## 12. 我的最终建议

这一轮我建议正式拍板下面几点：

1. **`CapabilityBundles` 采用“method-level primary bundles + world-slice aggregate bundle”路线**
2. **capability 的 primary attachment point 是 method，不在 type/field/member 上泛滥挂 capability**
3. **bundle 形状先冻结成无序 capability name set，不引入 count/confidence/reason/lowering hint**
4. **首轮最小 shared capability vocabulary 建议冻结为：**
   - `uses-instance-field-state`
   - `requires-string-concat`
   - `requires-console-string-output`
   - `requires-closed-type-member-query`
   - `requires-generic-type-definition-query`
   - `requires-imported-call`
5. **capability bundle 是 operation stream 的 requirement summary，不是 operation dump，也不是 lowering family 名字表**
6. **`Linker` 和 `CodeGen` 可以正式消费 capability bundles，但 capability bundle 不能单独替代 semantic subject structure**

## 13. 这一步之后最值得继续讨论的点

如果你认可这个方向，下一步最自然的问题就是：

- **`SemanticShapes` 这块自身的 contract 到底怎么冻结**

也就是继续往下收：

- shape vocabulary 的最小集合
- method role / body availability / imported-ness 如何表达
- shape 和 capability / body / canonical subject identity 的边界怎么进一步钉死

