# Phase A SemanticWorld Output Contract v1.01

Date: 2026-04-08
Status: design-discuss

## 1. 问题点

到目前为止，我们已经把 `SemanticWorld` 的几个关键内部块基本收住了：

- canonical semantic operation vocabulary
- typed payload / `ValueRef` / `LiteralRef` / `SpanId`
- `MethodSemanticBody`
- well-formedness invariants
- `SemanticWorld.Diagnostics`

但如果 `SemanticWorld` 最终“正式输出什么”还不冻结，
那么这条线仍然有一个很大的缺口：

1. `Linker` 不知道自己面对的是哪一组 authoritative semantic artifacts
2. `CodeGen` 也无法稳定区分“canonical output”与“调试/派生视图”
3. 很多本该是辅助索引或报表的信息，会悄悄升级成下游依赖面
4. `SemanticWorld` 很容易一边像规范产物，一边又像混杂了各种缓存和 debug dump 的工作区

所以这一轮真正要回答的不是：

- “`SemanticWorld` 里现在有哪些数据”

而是：

- **`SemanticWorld` 的正式输出 contract 到底由哪些块组成**
- **哪些块属于 canonical output**
- **哪些块只属于 derived/debug views**
- **`Linker` 被允许消费其中哪些部分**

## 2. 这里讨论的不是哪件事

这里讨论的不是：

- 具体 C# 类型定义怎么写
- JSON 落盘格式怎么排版
- 内部缓存如何组织
- 调试 UI 怎么展示 semantic world

这里讨论的是：

- **`Loader -> SemanticWorld -> Linker` 这条主线里，`SemanticWorld` 作为正式 artifact 到底发布什么**

## 3. 三种可选方案

## 3.1 方案 A：胖输出面，canonical 与派生视图混在一起

### 方案描述

做法是：

- 只要 `SemanticWorld` 内部已经算出来的东西，都一并作为公开输出面

例如同时暴露：

- canonical subject info
- method bodies
- literal pool
- capability summaries
- reverse indexes
- per-method referenced literal lists
- pretty-printed operation dumps
- 调试统计 / 辅助缓存

### 优点

- 下游“想看什么都能拿到”
- 某些分析或调试工具实现更省事

### 缺点

- 很快分不清什么是 authoritative contract
- downstream 容易误依赖某些本来只是派生视图的数据
- 一旦内部缓存或索引重构，外部依赖面就会被拖着走

### 我的判断

- **不推荐**

## 3.2 方案 B：最小 canonical output + 明确分离 derived/debug views

### 方案描述

做法是：

- `SemanticWorld` 只冻结最小、稳定、下游真正需要的 canonical output
- 所有统计、索引、pretty-print、debug projection 都明确降级为 derived/debug views

### 优点

- authoritative 边界清楚
- `Linker`/`CodeGen` 的可依赖面稳定
- 未来内部重构空间更大
- 最符合我们前面一路都在收紧的 artifact contract 思路

### 缺点

- 需要 upfront 决定“哪些块真的属于 canonical”
- 某些调试场景不能再直接把所有内部数据都当正式接口

### 我的判断

- **推荐**

## 3.3 方案 C：极薄输出面，只放 method bodies，其余全靠隐式 lookup

### 方案描述

做法是：

- `SemanticWorld` 几乎只输出 method bodies
- 类型/成员 shape、literal pool、capability 等都尽量不做显式正式块
- 下游需要时再回到 loader facts 或内部 lookup 补读

### 优点

- 表面上输出面最小

### 缺点

- 很多本该被 contract 固化的 semantic identity / shape 又退回成隐含约定
- `Linker` 会被迫重新猜“哪些 lookup 是 authoritative”
- 和前面已经拍板的强边界路线冲突

### 我的判断

- **不推荐**

## 4. 我的建议

如果这一轮要拍板，我建议采用：

- **方案 B：最小 canonical output + 明确分离 derived/debug views**

一句话定义：

- `SemanticWorld` 的正式输出只包含“semantic layer 真正承诺给下游的 canonical artifacts”
- 其他一切辅助索引、统计、可视化、pretty dump 都不是正式 contract

## 5. 我建议冻结的 canonical output 块

当前 `Phase A` 我建议把 `SemanticWorld` 的最小 canonical output 收成下面六块：

1. `CanonicalSubjects`
2. `SemanticShapes`
3. `MethodBodies`
4. `CapabilityBundles`
5. `LiteralPool`
6. `Diagnostics`

## 5.1 `CanonicalSubjects`

回答：

- 当前 `SemanticWorld` 内有哪些 canonical subject identity

这里承载的应是：

- `TypeId`
- `MethodId`
- `FieldId`
- `MemberId`
- owner 关系
- canonical subject kind

一句话说：

- **先回答“谁是谁”**

## 5.2 `SemanticShapes`

回答：

- 这些 canonical subjects 在当前 semantic slice 里是什么语义形状

例如可包括：

- type 是 `reference type` 还是别的当前支持形状
- field 是 `instance field` 还是别的形状
- method 是 `constructor / instance method / static method / imported method`
- method 是否拥有 canonical body

一句话说：

- **再回答“这些 subject 在当前切片里意味着什么”**

这里我建议把“是否应该存在 `MethodSemanticBody`”这样的信息也归到 shape 层，
而不是让下游靠“body map 里有没有”去猜。

## 5.3 `MethodBodies`

回答：

- 哪些 method 在当前 semantic slice 中拥有 canonical semantic body

每个 body 继续采用前一轮已经冻结的最小结构：

- `MethodId`
- `Inputs`
- `Spans`
- `Operations`

这里 method body 是：

- method-local semantic execution body

而不是：

- loader dump
- lowering plan
- debug pretty view

## 5.4 `CapabilityBundles`

回答：

- 当前 semantic slice 对下游暴露的 capability / requirement summaries 是什么

这是我建议明确列入 canonical output 的一块，
而不是把它降级成纯派生调试信息。

原因是前面已经拍板：

- `CodeGen` 的 family selection 主信号之一就是 `semantic subject structure + capability bundle`
- `Linker` 也需要基于 semantic requirements 冻结 dependency obligations

所以 capability bundle 虽然是 summary，
但它不是“可有可无的 debug summary”，而是：

- **semantic layer 正式承诺给下游的归纳结果**

## 5.5 `LiteralPool`

回答：

- `SemanticWorld` 级、按 `kind + value` 去重的 `LiteralRef` 条目集合

这里继续沿用前一轮已经拍板的边界：

- `LiteralRef` 是 world-level constant identity
- method body 里的 `load-const-*` 只引用这些 literal entries

## 5.6 `Diagnostics`

回答：

- 当前 `SemanticWorld` 是否已经成功产出了一个自洽、可消费的 semantic world

这里继续沿用上一轮已经拍板的 diagnostics contract：

- semantic-scope structured diagnostics
- 只要存在 `Severity = error`
  - `Linker` 就不应继续消费该 `SemanticWorld`

## 6. 我为什么把 `CapabilityBundles` 放进 canonical output

这一点我想单独强调。

因为表面上看，capability bundle 很像“从 body 派生出来的摘要”，
容易被误以为应该和统计/索引一起降级为 derived view。

但我当前不建议这么做。

原因有三点：

### 1. 它是正式下游消费面的一部分

前面已经拍板：

- `CodeGen` 用它做 family selection 主信号之一

如果它只是“调试视图”，
那就等于我们又把 `CodeGen` 逼回去自己重扫 method body。

### 2. 它表达的是 semantic layer 的归纳结论

例如：

- `requires-string-concat`
- `requires-console-string-output`
- `uses-instance-field-state`

这些不是实现细节，也不是展示层缓存，
而是 semantic layer 对 method/world 的稳定总结。

### 3. 它和 raw operation stream 承担不同层次的职责

- operation stream 讲“发生了哪些语义动作”
- capability bundle 讲“这组语义动作对下游提出了哪些能力要求”

这两层都应该是正式 contract。

## 7. 我建议明确降级为 derived/debug views 的东西

当前我建议把下面这些明确排除在 canonical output 之外：

### A. pretty-printed operation dump

例如：

- 文本化 method body
- 可视化 operation listing

这些适合作为调试视图，不适合作为正式 contract。

### B. reverse indexes

例如：

- `LiteralRef -> methods using it`
- `Capability -> methods requiring it`
- `ValueRef -> use-sites`

这些都更像分析辅助索引。

### C. local summaries that can be derived from bodies

例如：

- `ReferencedLiteralRefs[]`
- `ReferencedSubjects[]`
- per-method operation statistics

### D. source/provenance display views

例如：

- 代码位置展示
- loader record 反查视图
- source-ish explanation text

这些都可能有价值，
但不应升级成下游 authoritative 输入。

## 8. 我建议的 `Linker` 消费白名单

如果 `SemanticWorld` 输出 contract 按推荐方案冻结，
那么 `Linker` 我建议只允许正式消费下面几块：

1. `Diagnostics`
   - 用于 gating
2. `CanonicalSubjects`
   - 用于 canonical identity / ownership / subject resolution
3. `SemanticShapes`
   - 用于理解 subject semantic shape
4. `MethodBodies`
   - 用于 method-level semantic traversal、retention、obligation discovery
5. `CapabilityBundles`
   - 用于 semantic requirement / dependency obligation 归纳

对 `LiteralPool`：

- `Linker` 可以在 body 语义确实需要解析 literal 时读取
- 但 literal value 不应成为 retention/dependency policy 的主驱动面

一句话说：

- **`Linker` 消费的是 semantic facts，不是 debug projections**

## 9. 我建议明确禁止 `Linker` 消费的东西

为了防止边界重新回流，
我建议把下面这些明确写成禁区：

### 禁区 1：raw loader facts 重新成为 `Linker` 的事实来源

例如：

- raw IL instruction sequence
- metadata blob 细节
- token 级未归一化记录

如果 `SemanticWorld` 已经给出了 canonical semantic output，
`Linker` 就不应再回头把 raw loader 输入当成主事实来源。

### 禁区 2：derived/debug views 成为保留或依赖决策主依据

例如：

- pretty dump
- usage stats
- reverse indexes

这些可以帮助调试，
但不应成为 retention / dependency freeze 的 authoritative source。

### 禁区 3：下游通过“body 缺失”猜语义，而不读 shape

例如：

- “body 不存在，所以它一定是 imported method”

我建议禁止这种隐式推理。

应该通过：

- `SemanticShapes`

明确判断 method 的 semantic role 和 body availability。

## 10. 用一个例子看推荐的整体输出面

以 `Greeter.Build()` / `Main()` 这条线为例，
推荐理解成：

```text
SemanticWorld {
  CanonicalSubjects {
    TypeId(Greeter)
    FieldId(Greeter::_name)
    MethodId(Greeter::.ctor)
    MethodId(Greeter::Build)
    MethodId(Program::Main)
  }

  SemanticShapes {
    Greeter = reference-type
    Greeter::_name = instance-field
    Greeter::.ctor = constructor-with-body
    Greeter::Build = instance-method-with-body
    Program::Main = static-entry-method-with-body
  }

  MethodBodies {
    body(Greeter::Build) = ...
    body(Program::Main) = ...
  }

  CapabilityBundles {
    Greeter::Build => requires-string-concat
    Program::Main => requires-console-string-output
    world-slice => uses-instance-field-state + requires-string-concat + requires-console-string-output
  }

  LiteralPool {
    lit0 = string("Hello, ")
    lit1 = string("Alice")
  }

  Diagnostics {
    ...
  }
}
```

这里：

- `Linker` 真正依赖的是上面六块 canonical output
- pretty dump / usage index / debug text 都不属于主线 contract

## 11. 我建议的整体判断

如果把这一步压成一句很短的话，
我建议把 `SemanticWorld` 输出面记成：

- **identity**
- **shape**
- **body**
- **capability**
- **literal**
- **diagnostics**

这六块刚好对应：

- 谁是谁
- 它们在当前切片里是什么
- method 内部做了什么
- 对下游要求什么
- 用到了哪些 world-level constants
- 当前 world 是否可被继续消费

## 12. 我的最终建议

这一轮我建议正式拍板下面几点：

1. **`SemanticWorld` 的正式输出 contract 采用“最小 canonical output + derived/debug views 明确分离”路线**
2. **首轮 canonical output 冻结为六块：`CanonicalSubjects / SemanticShapes / MethodBodies / CapabilityBundles / LiteralPool / Diagnostics`**
3. **`CapabilityBundles` 明确属于 canonical output，而不是 debug summary**
4. **`Linker` 只允许消费 canonical output，不应依赖 raw loader facts 或 derived/debug views**
5. **method body 是否存在，不应用隐式猜测表达，而应由 `SemanticShapes` 明确给出 body availability / method role**
6. **pretty dump、reverse indexes、usage summaries、source/provenance display views 全部降级为 derived/debug views**

## 13. 这一步之后最值得继续讨论的点

如果你认可这个方向，下一步最自然的问题就是：

- **`CapabilityBundles` 自身的 contract 到底怎么冻结**

也就是继续往下收：

- capability 的附着粒度是 method、subject，还是 world-slice
- 哪些 capability 是 canonical vocabulary
- capability bundle 和 operation stream 的职责边界如何更精确地切开

