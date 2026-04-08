# Phase A Linker Output Contract v1.01

Date: 2026-04-08
Status: design-discuss

## 1. 问题点

如果 `Phase A` 采用推荐的 B 方案：

- `SemanticWorld` 负责 canonical semantic layer
- `Linker` 负责 retained world / dependency closure
- `CodeGen` 负责 lowering family 选择
- `Emitter` 只负责消费 plan 并渲染

那么接下来必须补上的核心问题就是：

- **`Linker` 的输出 contract 最少该长什么样，才能让 `CodeGen` 真正接手 lowering，而不是继续靠下游猜语义。**

如果这一步不先冻结，后面很容易继续出现三类退化：

1. `CodeGen` 需要重新从 retained members 里再猜一次 semantic slice
2. `Linker` 为了“帮下游省事”继续把 lowering decision 偷渡进去
3. `Emitter` 最终还是不得不回头看 `typed-il` 或 proof-shape

所以这一轮真正要回答的不是“Linker 现在做了什么”，而是：

- **在主线推荐边界下，`Linker` 应该输出什么，明确不输出什么。**

## 2. 问题的具体描述

当前最容易混淆的是三类对象：

- retained members / retained types
- external managed dependencies
- lowering / runtime / host 侧的执行需求

这三类对象如果不分开，`LinkedWorld` 很快就会重新变成一个大杂烩：

- 既有 reachability 结果
- 又有 runtime helper
- 又有 host binding
- 甚至还有 template-specific lowering 决策

这样做短期可能省事，但长期会直接把 `Linker` 拉回“大泥球”。

因此，`Linker` output contract 的核心任务只有两个：

1. 让 `CodeGen` 看懂“哪些 managed 语义对象和 external managed dependencies 已经冻结”
2. 明确告诉后续层：
   - 哪些是 linker 的结论
   - 哪些必须留给 lowering plan

## 3. 有哪些可解决的路径

### 路径 A：最薄输出，只给 retained closure 和 dependency 列表

做法：

- `Linker` 只输出：
  - retained types
  - retained members
  - external managed dependencies
- 尽量少带 reason、ordering、diagnostics

优点：

- 最简单
- 最像传统“reachability/link set”结果

缺点：

- `CodeGen` 会被迫重新推导很多语义上下文
- dependency reason 失去稳定入口
- 很难支撑后续 deterministic emission 和可解释排障

判断：

- 不推荐

### 路径 B：平衡型输出，冻结 retained closure + declarative reasons + deterministic order + diagnostics

做法：

- `Linker` 输出：
  - entry set
  - retained types
  - retained members
  - external managed dependencies
  - 每个 retained/dependency 的 declarative reasons
  - 稳定的 emission/order view
  - linker 级 diagnostics
- 但明确不输出：
  - lowering family
  - runtime roles
  - host bindings
  - template kind
  - literal bindings

优点：

- `CodeGen` 拿到的是一个可解释、可稳定消费的 linked semantic closure
- 还能保持和 lowering 层的清晰分界
- 最符合当前 `Phase A` 的收口目标

缺点：

- 需要 upfront 冻结 reason taxonomy 和最小 ordering contract
- 比最薄输出多一层结构设计

判断：

- 推荐

### 路径 C：厚输出，把 quasi-lowering 信息提前塞进 `LinkedWorld`

做法：

- `Linker` 除 retained/dependency 外，再额外输出：
  - lowering-ready roles
  - runtime/helper 需求
  - 甚至 template 预选择信息

优点：

- `CodeGen` 看起来更轻
- 下游实现短期更方便

缺点：

- `Linker` 会重新侵入 lowering 层
- retained policy 和 lowering policy 被重新绑死
- `CodeGen` 很快退化成 artifact serializer

判断：

- 不推荐

## 4. 我的方案和理由

我推荐：

- **路径 B：平衡型输出**

一句话定义：

- `LinkedWorld` 应该是“已冻结的 managed semantic closure”，而不是“半成品 lowering plan”。

我更推荐它的原因有四个：

1. 它最符合已经收好的 B 方案分层
2. 它能让 `CodeGen` 站在稳定输入之上做 lowering 选择
3. 它不会把 runtime/helper/host 需求重新混回 `Linker`
4. 它为 deterministic output 和可解释 diagnostics 留下了足够空间

## 5. 推荐的输出 contract

我建议把 `Linker` 的输出先收成下面几类对象。

### A. `Entries`

回答：

- 当前 linked world 是从哪些 entry methods 出发冻结出来的

这层的意义是：

- 让 `CodeGen` 和后续 diagnostics 知道 retained closure 的根在哪里

### B. `RetainedTypes`

回答：

- 哪些 type 在当前 world 中必须存在
- 每个 type 为什么被保留

建议最少包含：

- `TypeId`
- `TypeKind`
- `RetainedReasons[]`

### C. `RetainedMembers`

回答：

- 哪些 methods / fields / properties 被保留
- 每个 member 为什么被保留

建议最少包含：

- `MemberId`
- `MemberKind`
- `OwnerTypeId`
- `RetainedReasons[]`
- 可选 `Token`

### D. `Dependencies`

回答：

- 当前 world 需要哪些 external managed dependencies
- 每个 dependency 为什么被引入

这里要明确：

- 只放 external managed dependency
- 不放 runtime roles
- 不放 host bindings

建议最少包含：

- `DependencyId`
- `DependencyKind`
  - 例如：
  - `managed-method`
  - `managed-type`
- `Reasons[]`

### E. `EmissionOrder`

回答：

- `CodeGen` 如果想 deterministic 地枚举 types / members，应该按什么稳定顺序消费

这层的价值是：

- 防止 `CodeGen` 再自己发明一套排序规则
- 让 linker 真正承担“冻结 linked closure”的职责

### F. `Diagnostics`

回答：

- linker 阶段发现了哪些 closure / dependency 级问题

例如：

- world 中存在 requirement，但找不到对应 managed dependency
- 某个 retained member 的 prerequisite 未满足

## 6. 明确不应出现在 `LinkedWorld` 里的东西

如果按推荐边界推进，下面这些不该出现在 linker output 里：

- `LoweringFamily`
- `TemplateKind`
- `RuntimeRoles`
- `HostBindings`
- `LiteralBindings`
- `ExpectedOutput`
- template-specific role names

这些都应该继续留给 `CodeGen / LoweringPlan`。

一句话说就是：

- `LinkedWorld` 负责“冻结 managed semantic closure”
- 不负责“决定如何把它降成 C++”

## 7. 用具体例子看这个 contract

继续用上一轮的 `Greeter` 例子：

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

如果按推荐 contract，`LinkedWorld` 的核心结果大致应像这样：

```json
{
  "Entries": [
    "Program::Main()"
  ],
  "RetainedTypes": [
    {
      "TypeId": "Greeter",
      "TypeKind": "reference-type",
      "RetainedReasons": ["entry-reachable", "semantic.object-construction"]
    }
  ],
  "RetainedMembers": [
    {
      "MemberId": "Program::Main()",
      "MemberKind": "method",
      "OwnerTypeId": "Program",
      "RetainedReasons": ["entry-root"]
    },
    {
      "MemberId": "Greeter::.ctor(string)",
      "MemberKind": "method",
      "OwnerTypeId": "Greeter",
      "RetainedReasons": ["semantic.object-construction"]
    },
    {
      "MemberId": "Greeter::Build()",
      "MemberKind": "method",
      "OwnerTypeId": "Greeter",
      "RetainedReasons": ["semantic.instance-message"]
    },
    {
      "MemberId": "Greeter::_name",
      "MemberKind": "field",
      "OwnerTypeId": "Greeter",
      "RetainedReasons": ["semantic.instance-field-state"]
    }
  ],
  "Dependencies": [
    {
      "DependencyId": "System.Object::.ctor()",
      "DependencyKind": "managed-method",
      "Reasons": ["semantic.object-base-ctor"]
    },
    {
      "DependencyId": "System.String::Concat(System.String,System.String)",
      "DependencyKind": "managed-method",
      "Reasons": ["semantic.string-concat"]
    },
    {
      "DependencyId": "System.Console::WriteLine(System.String)",
      "DependencyKind": "managed-method",
      "Reasons": ["semantic.console-string-output"]
    }
  ],
  "Diagnostics": []
}
```

这里最关键的是：

- 有 closure
- 有 dependency
- 有 reason
- 但没有 lowering family
- 也没有 runtime role `console-write-line-string`

因为后者应该等到 `CodeGen` 再决定。

## 8. 对 `CodeGen` 的直接意义

如果 `Linker` 输出 contract 按推荐方式冻结，`CodeGen` 就可以明确站在这层输入之上做自己的工作：

- 读取 retained closure
- 读取 external managed dependencies
- 读取 reasons
- 读取 stable order
- 再基于这些结果选择 lowering family

而不需要再去做这些事：

- 再从 `typed-il` 反推 retained intent
- 再自己造 dependency reasons
- 再自己补一套 member ordering

也就是说：

- `Linker` 把 managed 世界“收住”
- `CodeGen` 再把它“降下去”

## 9. 这一步之后最值得继续讨论的点

如果这一层大方向认可，下一步最值得继续细化的是两件事里的一个：

1. `RetainedReasons / DependencyReasons` 的 taxonomy 粒度
2. `EmissionOrder` 到底只需要 deterministic list，还是还需要 stronger grouping contract

如果只能先选一个，我更建议先聊：

- **reason taxonomy 的粒度**

因为只要 reason 还不稳定，`LinkedWorld` 看起来就仍然像一份“结构更好的中间结果”，还不像真正冻结的 contract。
