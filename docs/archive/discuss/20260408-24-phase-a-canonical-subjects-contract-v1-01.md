# Phase A CanonicalSubjects Contract v1.01

Date: 2026-04-08
Status: design-discuss

## 1. 问题点
前一轮我们已经把 `SemanticWorld` 的正式输出面收成了六块：

- `CanonicalSubjects`
- `SemanticShapes`
- `MethodBodies`
- `CapabilityBundles`
- `LiteralPool`
- `Diagnostics`

其中 `SemanticShapes / MethodBodies / CapabilityBundles / LiteralPool / Diagnostics` 的边界已经基本站稳，
但 `CanonicalSubjects` 这块还只是被点了名，还没有真正冻结它自己的 contract。

如果这一步不继续收口，后面会很容易继续漂：

1. `Linker` 不知道自己该从哪里拿 authoritative subject identity
2. `SemanticShapes` 很容易偷偷开始承接 owner/declaring 关系
3. `MethodBodies` 里的 `MethodId / FieldId` 会失去一个统一、显式的 subject resolution 面
4. generic definition 和 closed instantiation 的边界会重新混掉
5. `MemberId` 会变成一个“大家都提到，但没人定义它和 `MethodId / FieldId` 到底是什么关系”的悬空概念

所以这一轮真正要回答的是：

- **`CanonicalSubjects` 到底回答什么问题**
- **`TypeId / MethodId / FieldId / MemberId` 的最小关系面应该怎么冻结**
- **owner / declaring relationship 放在哪一层**
- **definition identity 和 instantiated identity 的边界怎么切**

## 2. 这里讨论的不是什么

这里讨论的不是：

- id 的二进制编码或 JSON 落盘格式
- 人类可读名称如何展示
- metadata token 是否直接暴露给下游
- `Linker` 的 retained closure / entry 决策
- `CodeGen` 的 lowering family 选择
- debug/provenance 视图怎么做

这里讨论的是：

- **在 `SemanticWorld` 的 canonical output 里，谁是 canonical subject，谁拥有谁，谁声明谁**

一句话说：

- `CanonicalSubjects` 回答“**谁是这张 semantic world 里的 canonical subject，以及它们之间最小的 declaring graph 是什么**”

## 3. 三种可选方案

## 3.1 方案 A：只有 typed id 集合，不显式冻结 member/owner 关系

### 方案描述

做法是：

- `CanonicalSubjects` 只暴露：
  - `TypeId[]`
  - `MethodId[]`
  - `FieldId[]`
- owner / declaring 关系让下游自己回 metadata lookup 或别的索引里推
- `MemberId` 也不在这里正式落位

### 优点

- 表面上最薄
- 早期看起来实现最省事

### 缺点

- `Linker` 会被迫自己补“这个 method 属于哪个 type”
- `MethodId / FieldId` 之外的泛化 member 概念没有正式锚点
- downstream 很容易重新把 raw metadata lookup 升级成事实来源
- 看似轻，实际上把 contract 压成了隐式约定

### 我的判断

- **不推荐**

## 3.2 方案 B：typed subject families + 显式 declaring graph + 轻量 member 投影

### 方案描述

做法是：

- `CanonicalSubjects` 显式冻结三类主 subject family：
  - `Types`
  - `Methods`
  - `Fields`
- 同时给出一个轻量的 `MemberId` 投影面，用来统一表达“某个 member”，但不取代 typed id
- owner / declaring 关系直接放在 `CanonicalSubjects` 中，而不是让下游再去猜

可以理解成：

```text
CanonicalSubjects {
  Types: TypeSubject[]
  Methods: MethodSubject[]
  Fields: FieldSubject[]
  Members: MemberSubject[]
}
```

其中推荐的最小关系大致是：

```text
TypeSubject {
  TypeId
  DeclaringTypeId?   // 仅当它是 nested type 时存在
}

MethodSubject {
  MethodId
  DeclaringTypeId
  MemberId
}

FieldSubject {
  FieldId
  DeclaringTypeId
  MemberId
}

MemberSubject {
  MemberId
  Kind = method | field
  ConcreteId = MethodId | FieldId
}
```

### 优点

- authoritative subject identity 面最清楚
- owner / declaring 关系不再靠隐式 metadata 推理
- `MemberId` 有正式落点，但不会吞掉 `MethodId / FieldId` 的 typed contract
- 很符合我们前面一路都在收紧的 “typed canonical artifacts” 方向
- 对未来扩展 property/event 等 member kind 也有演进空间

### 缺点

- 比极薄方案多一层结构
- 需要 upfront 冻结“什么是 primary identity，什么是 umbrella projection”

### 我的判断

- **推荐**

## 3.3 方案 C：统一 generalized subject graph

### 方案描述

做法是：

- 不再强调 `TypeId / MethodId / FieldId` 三族
- 直接上一个更抽象的统一图：
  - `SubjectId`
  - `SubjectKind`
  - `Edges(owner/declares/contains/instantiates/...)`

### 优点

- 表面上最通用
- 理论上后续扩展空间最大

### 缺点

- 对当前 `Phase A` 来说过重
- typed contract 会被抽象图稀释
- 下游会重新承担 kind discrimination 和边界判断成本
- 很容易把目前根本还没决定要不要进入主线 contract 的关系也一起带进来

### 我的判断

- **不推荐**

## 4. 我的建议

如果这一轮要拍板，我建议采用：

- **方案 B：typed subject families + 显式 declaring graph + 轻量 member 投影**

一句话定义：

- `CanonicalSubjects` 是 `SemanticWorld` 的 **authoritative identity and declaring graph layer**

它负责：

- subject identity
- subject kind family
- declaring / owning relationships
- member 泛化投影

它不负责：

- semantic shape
- method body
- capability summary
- retained / entry / dependency 决策
- generic instantiation 运行时展开

## 5. 我建议冻结的最小 contract

## 5.1 主体 family

当前 `Phase A` 我建议先明确三类 primary canonical subject：

- `TypeId`
- `MethodId`
- `FieldId`

这三类是 typed、不可互换的主 identity。

`MemberId` 的位置我建议定义成：

- **对 `MethodId / FieldId` 的 canonical umbrella projection**

也就是说：

- `MemberId` 是“某个 member”的统一 handle
- 但它不是一个要吞掉 typed world 的第四套主语义层
- method-specific / field-specific contract 仍然以 `MethodId / FieldId` 为准

## 5.2 推荐的最小 subject 关系

我建议把 `CanonicalSubjects` 最少收成下面几种关系：

### `TypeSubject`

回答：

- 这个 `TypeId` 是一个 canonical type subject
- 它是否有 declaring type

推荐最小形状：

```text
TypeSubject {
  TypeId
  DeclaringTypeId?
}
```

这里的 `DeclaringTypeId?` 只用于 nested type 关系。

我当前 **不建议** 在 `CanonicalSubjects` 首轮直接承接：

- assembly
- namespace
- token
- display name
- generic arity 文本化信息

这些更适合留在稳定 metadata lookup 边界，而不是先塞进 canonical subject graph。

### `MethodSubject`

回答：

- 这个 `MethodId` 是一个 canonical method subject
- 它声明在谁下面
- 它对应哪个 `MemberId`

推荐最小形状：

```text
MethodSubject {
  MethodId
  DeclaringTypeId
  MemberId
}
```

### `FieldSubject`

回答：

- 这个 `FieldId` 是一个 canonical field subject
- 它声明在谁下面
- 它对应哪个 `MemberId`

推荐最小形状：

```text
FieldSubject {
  FieldId
  DeclaringTypeId
  MemberId
}
```

### `MemberSubject`

回答：

- 这个 `MemberId` 到底落到哪个 concrete member kind

推荐最小形状：

```text
MemberSubject {
  MemberId
  Kind = method | field
  ConcreteId = MethodId | FieldId
}
```

这里的关键边界是：

- `MemberId` 可以统一表达 “declaring type 下面的一个 member”
- 但它不承载 body、shape、capability
- 它也不成为 method/field 特化 contract 的替代品

## 6. owner / declaring relationship 放哪一层

这一点我建议明确钉死：

- **owner / declaring relationship 属于 `CanonicalSubjects`**

不要放在：

- `SemanticShapes`
- `MethodBodies`
- metadata debug view

原因很简单：

1. owner/declaring 关系是 identity graph 的一部分，不是 shape
2. 它也不是 method body 内部发生了什么
3. 如果不在 `CanonicalSubjects` 冻住，下游就会重新回头依赖 raw metadata lookup

当前 `Phase A` 我建议这样切：

- `MethodSubject.DeclaringTypeId`：必填
- `FieldSubject.DeclaringTypeId`：必填
- `TypeSubject.DeclaringTypeId?`：只在 nested type 时存在

而下面这些先不进入 `CanonicalSubjects` 主 contract：

- assembly owner
- namespace owner
- module owner

这些信息如果下游确实需要，可以通过稳定 metadata lookup 边界去拿，而不是先把它们抬进 `SemanticWorld` canonical graph。

## 7. `TypeId / MethodId / FieldId / MemberId` 的关系怎么理解

我建议把这四者的关系压成下面一句：

- **`TypeId / MethodId / FieldId` 是 primary typed identity；`MemberId` 是 field/method 的 canonical umbrella identity**

更展开一点说：

1. `TypeId` 不等于 `MemberId`
2. `MethodId` 和 `FieldId` 不互相混用
3. 每个 `MethodId` / `FieldId` 都可以映射到恰好一个 `MemberId`
4. 每个 `MemberId` 必须反解到恰好一个 concrete `MethodId` 或 `FieldId`
5. `MemberId` 的存在是为了让某些 contract 可以说“某个 member”，而不必在所有地方都拆成 method/field 双分支

一个很具体的例子：

```csharp
class Greeter
{
    private readonly string _name;

    public Greeter(string name) { _name = name; }
    public string Build() { return "Hello, " + _name; }
}
```

推荐理解成：

```text
TypeId(T_Greeter)

FieldId(F_Greeter_name)
MethodId(M_Greeter_ctor)
MethodId(M_Greeter_Build)

MemberId(MB_Greeter_name)  -> FieldId(F_Greeter_name)
MemberId(MB_Greeter_ctor)  -> MethodId(M_Greeter_ctor)
MemberId(MB_Greeter_Build) -> MethodId(M_Greeter_Build)
```

这里：

- `Build` 的 body 仍然归 `MethodBodies`
- `Build` 是 instance-method 归 `SemanticShapes`
- `Build` 需要 string concat 归 `CapabilityBundles`
- `Build` 声明在 `Greeter` 下面，归 `CanonicalSubjects`

## 8. definition identity 和 instantiation identity 的边界

这一点我建议这一轮明确拍死：

- **`CanonicalSubjects` 只承接 definition identity，不承接 closed instantiation identity**

也就是说，当前 `Phase A` 推荐理解成：

- `TypeId` 指向 type definition subject
- `MethodId` 指向 method definition subject
- `FieldId` 指向 field definition subject

而不是：

- 每出现一个 `List<int>` 就生成一个新的 canonical `TypeId`
- 每出现一个 `Foo<string>.Bar<int>` 就生成一个新的 canonical `MethodId`

## 8.1 为什么我建议这样切

原因主要有三点：

### 1. canonical subject 图需要稳定、克制

如果把 closed instantiation 也抬成 canonical subjects，
`CanonicalSubjects` 会很快从“定义层 identity 图”膨胀成“按使用点展开的实例图”。

### 2. 我们前面已经有更适合承接 closed-world 语义的地方

例如前面已经讨论过：

- `QueryKeyId`
- `requires-closed-type-member-query`
- `requires-generic-type-definition-query`

这说明“某个定义在某个 closed context 下被查询/消费”的语义，
更适合放在 query/payload/capability 这一层，而不是抬升为新的 canonical definition subject。

### 3. 这更符合当前 `SemanticWorld -> Linker -> CodeGen` 的分层

`CanonicalSubjects` 先把“定义层谁是谁”钉死，
后面的闭包、查询、lowering 再去处理“这个定义在具体 closed context 下怎么被用到”。

## 8.2 一个泛型例子

```csharp
class Box<T>
{
    public T Value;
    public T Get() { return Value; }
}

static void Main()
{
    var box = new Box<int>();
    _ = box.Get();
}
```

我建议理解成：

```text
CanonicalSubjects {
  TypeId(Box<T>)
  FieldId(Box<T>::Value)
  MethodId(Box<T>::Get)
  MethodId(Program::Main)
}
```

而不是：

```text
TypeId(Box<int>)
MethodId(Box<int>::Get)
```

换句话说：

- `Box<T>` 是 canonical subject
- `Box<int>` 是某个 closed usage context

前者属于 `CanonicalSubjects`，
后者更适合通过 query / payload / later lowering context 表达。

## 9. 我建议明确禁止的几类混入

为了防止 `CanonicalSubjects` 再次漂移，我建议把下面几类东西明确列成禁区：

### A. shape 信息混入 `CanonicalSubjects`

例如：

- `instance-method`
- `imported-method`
- `has-canonical-body`

这些属于 `SemanticShapes`，不属于 subject identity graph。

### B. body 证据混入 `CanonicalSubjects`

例如：

- `reads-field`
- `returns-literal`
- `calls-imported-method`

这些属于 `MethodBodies` 的 operation 证据，而不是 subject identity。

### C. capability 信息混入 `CanonicalSubjects`

例如：

- `requires-string-concat`
- `uses-instance-field-state`

这些属于 `CapabilityBundles`。

### D. linker/codegen 决策混入 `CanonicalSubjects`

例如：

- `entry`
- `retained`
- `pair-concat-lowering`
- `imported-call-lowering-ready`

这些已经越过 semantic identity layer 了。

### E. closed instantiation 被升级为 canonical subject

例如：

- `List<int>` 直接变成新的 canonical `TypeId`
- `Box<int>::Get` 直接变成新的 canonical `MethodId`

当前 `Phase A` 我不建议这么做。

## 10. 我建议冻结的最小 well-formedness 关系

虽然这一轮不是专门谈 validation，但 `CanonicalSubjects` 一旦冻结，至少自然带出下面几条 invariant：

1. 每个 `MethodSubject.DeclaringTypeId` 必须能解析到现有 `TypeId`
2. 每个 `FieldSubject.DeclaringTypeId` 必须能解析到现有 `TypeId`
3. 若 `TypeSubject.DeclaringTypeId` 存在，则它也必须解析到现有 `TypeId`
4. 每个 `MethodId` 必须映射到恰好一个 `MemberId`
5. 每个 `FieldId` 必须映射到恰好一个 `MemberId`
6. 每个 `MemberId` 必须反解到恰好一个 concrete `MethodId` 或 `FieldId`
7. `CanonicalSubjects` 中不得出现“只因 closed instantiation 才存在”的新 subject

这些 invariant 不是为了让 `CanonicalSubjects` 变重，
而是为了让下游可以把它当作 authoritative subject graph，而不是半成品索引。

## 11. 用一个例子看推荐方案

继续用前面的 `Greeter` 例子，推荐理解成：

```text
CanonicalSubjects {
  Types {
    T0 = Program
    T1 = Greeter
  }

  Fields {
    F0 = Greeter::_name {
      DeclaringTypeId = T1
      MemberId = MB0
    }
  }

  Methods {
    M0 = Greeter::.ctor {
      DeclaringTypeId = T1
      MemberId = MB1
    }

    M1 = Greeter::Build {
      DeclaringTypeId = T1
      MemberId = MB2
    }

    M2 = Program::Main {
      DeclaringTypeId = T0
      MemberId = MB3
    }
  }

  Members {
    MB0 => field(F0)
    MB1 => method(M0)
    MB2 => method(M1)
    MB3 => method(M2)
  }
}
```

这里非常清楚地分层成：

- `CanonicalSubjects`：谁存在、谁声明在谁下面
- `SemanticShapes`：这些 subject 在当前 slice 里是什么语义角色
- `MethodBodies`：method 内部发生了什么
- `CapabilityBundles`：对下游提出了什么能力要求

## 12. 我的最终建议

这一轮我建议正式拍板下面几点：

1. **`CanonicalSubjects` 的角色冻结为 `SemanticWorld` 的 authoritative identity and declaring graph layer**
2. **当前 `Phase A` 的 primary canonical subject families 冻结为：`TypeId / MethodId / FieldId`**
3. **`MemberId` 明确定位为 `FieldId / MethodId` 的 canonical umbrella projection，而不是取代 typed ids 的第四主语义层**
4. **owner / declaring relationship 明确属于 `CanonicalSubjects`，不应漂到 `SemanticShapes`、`MethodBodies` 或 raw metadata fallback**
5. **`MethodSubject / FieldSubject` 必须显式给出 `DeclaringTypeId`**
6. **`TypeSubject` 只在需要表达 nested type 时才给出 `DeclaringTypeId?`**
7. **`CanonicalSubjects` 当前只承接 definition identity，不承接 closed instantiation identity**
8. **assembly / namespace / token / display-name 等信息先不抬进 `CanonicalSubjects` 主 contract，而留给稳定 metadata lookup 边界**

## 13. 这一步之后最值得继续讨论的点

如果你认可这个方向，下一步最自然的问题就是：

- **稳定 metadata lookup 边界到底怎么和 `CanonicalSubjects` 对接**

也就是继续往下收：

- 哪些 definition-level 元信息必须可稳定反查
- token / signature / arity / assembly-qualified identity 放在哪一层
- 下游什么时候只看 `CanonicalSubjects` 就够了，什么时候允许进入 metadata lookup
