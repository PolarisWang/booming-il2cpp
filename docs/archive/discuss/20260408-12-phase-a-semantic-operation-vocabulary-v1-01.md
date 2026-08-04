# Phase A Semantic Operation Vocabulary v1.01

Date: 2026-04-08
Status: design-discuss

## 1. 问题点

上一轮已经基本拍板：

- `SemanticWorld` 的 canonical semantic operation stream 采用语义动作级粒度
- operation 要比 raw IL opcode 更语义化
- 但不能直接塌成 lowering family / proof 宏

那下一步真正必须收口的就不是“operation 要多高层”，而是：

- **首轮 canonical semantic operation vocabulary 到底具体冻结哪些名字。**

如果这一步不冻结，后面很容易继续漂：

1. 不同语义切片各自发明 operation 名
2. operation 名开始一部分像 opcode、一部分像 family
3. payload 里本来可以表达的差异，被错误地写进 operation 名本身

所以这一步的核心不是把词表做大，而是：

- **先收出一套最小、稳定、不会泄漏 lowering 的 canonical semantic operation vocabulary。**

## 2. 这一轮真正要解决的三个小问题

### A. 词表要不要一开始就铺很大

可选方向大致有三种：

- 做一套尽量完整、面向未来 Phase B 的大词表
- 做一套只覆盖当前 proof spine 的最小词表
- 每个 family 自己有一套局部 operation 词表

### B. operation 名里应该放多少差异

例如下面这些差异，到底该放在 operation 名里，还是放到 payload 里：

- `field` / `property` / `method` / `parameter`
- imported call 的 module / entrypoint
- reflection query 目标是 field 还是 property

### C. 哪些名字必须明确禁止

和 reason vocabulary 一样，这里最大的风险不是不够细，而是：

- operation 名开始偷偷表达 lowering/runtime/backend 决策

例如：

- `pair-concat-chain`
- `console-write-line-icall`
- `pinvoke-host-bridge`
- `virtual-slot-dispatch`

## 3. 我的建议

我建议当前把 operation vocabulary 收成：

- **一套共享词表**
- **覆盖当前 proof spine 的最小集合**
- **operation 名保持语义动作级，差异尽量下沉到 payload**

也就是：

- **选“最小共享词表”，而不是“大而全词表”或“family 私有词表”。**

## 4. 为什么我推荐这条路

### 原因 1：当前 Phase A 要先稳边界，不是先做百科全书

现在更重要的是让：

- `Loader`
- `SemanticWorld`
- `Linker`
- `CodeGen`

都说同一门稳定语言，  
而不是一开始就预支未来所有语义。

### 原因 2：operation 名应优先表达“做了什么语义动作”

而不是：

- “这个动作最后会怎么 lowering”
- “它属于哪条 proof spine”
- “它是哪个 backend/runtime helper”

### 原因 3：很多差异更适合放到 payload

比如：

- `query-closed-type-member`

这个名字已经足够表达“闭合类型成员查询”这件事。  
至于是：

- field
- property
- method
- parameter

更适合由 payload 去表达，  
不需要把 operation 名膨胀成四五个近义词。

## 5. 我建议冻结的首轮最小 operation vocabulary

我建议当前 `Phase A` 先冻结下面这批最小 operation 名字。

### A. object / state

- `construct-object`
- `read-instance-field`
- `write-instance-field`

### B. invocation

- `invoke-static-method`
- `invoke-instance-method`
- `invoke-imported-method`

### C. semantic helpers

- `string-concat`
- `emit-console-string`

### D. reflection / metadata query

- `load-type-handle`
- `query-closed-type-member`
- `query-generic-type-definition`

### E. constants / exit

- `load-const-string`
- `load-const-int32`
- `return`

## 6. 为什么是这几个

这批 operation 基本正好覆盖当前已讨论和已证明过的主线切片：

- object construction / instance state / instance message
- generic static forwarder / getter
- reflection closed-type query
- imported direct call
- console string output
- string concat

也就是说，它们覆盖的是：

- 当前 proof spine 的“共用语义动作”

而不是：

- 某个具体样例的局部技巧

## 7. 哪些名字我建议现在先不进词表

### A. 过于贴近 IL 的名字

例如：

- `load-arg`
- `dup`
- `pop`
- `box`
- `unbox`

这些不是当前 canonical semantic operation 该暴露的一等语言。

### B. 过于细碎、其实更适合放 payload 的名字

例如当前我不建议一开始就拆成：

- `query-closed-type-field`
- `query-closed-type-property`
- `query-closed-type-method`
- `query-closed-type-parameter`

更推荐先统一成：

- `query-closed-type-member`

然后由 payload 表达 member kind。

### C. 会泄漏 lowering/runtime/backend 的名字

例如：

- `string-concat-pair-chain`
- `console-write-line-icall`
- `host-bridge-log-write`
- `native-import-resolution`

这些都已经越过 semantic layer 了。

## 8. 用例子看这套词表怎么落

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

### `Main()` 的 operation stream

可以近似理解成：

- `load-const-string`
- `construct-object`
- `invoke-instance-method`
- `emit-console-string`
- `return`

### `Build()` 的 operation stream

可以近似理解成：

- `load-const-string`
- `read-instance-field`
- `string-concat`
- `return`

这里最关键的是：

- 我们已经不再看到 raw IL opcode
- 但也还没有直接看到：
  - `captured-state-instance-message`
  - `string-concat-pair-chain`

这说明粒度刚好停在 semantic layer。

## 9. 再看 reflection / imported call

### reflection query

对于 query-only reflection path，method-level stream 可以近似理解成：

- `load-type-handle`
- `query-closed-type-member`
- `query-generic-type-definition`
- `emit-console-string`
- `return`

### imported direct call

对于 imported direct call path，可以近似理解成：

- `invoke-imported-method`
- `emit-console-string`
- `return`

这里同样能看到：

- `invoke-imported-method`
  只表达“这是 imported call 语义”
- 它不表达：
  - 最后走哪种 native resolution
  - 用哪种 host bridge

这些都应留给后面的 lowering/runtime 层。

## 10. operation 名和 payload 的分工

我建议当前冻结一个很重要的原则：

- **operation 名表达“语义动作类别”**
- **payload 表达“这个动作具体作用到谁、带什么值、带什么子种类”**

例如：

- `construct-object`
  - payload 可带：
    - target type
    - ctor method
    - argument refs
- `query-closed-type-member`
  - payload 可带：
    - queried member kind
    - queried subject id
- `invoke-imported-method`
  - payload 可带：
    - imported method id
    - argument refs

这样可以避免 operation 名继续爆炸。

## 11. 我建议明确禁止进入 operation 名的类别

我建议当前把下面这些类别明确列入禁区：

- `lowering.*`
- `runtime-role.*`
- `host-binding.*`
- `template.*`
- `proof-shape.*`

对应到具体 operation 名，当前不应出现：

- `captured-state-instance-message`
- `generic-forwarder-getter`
- `pair-concat-chain`
- `console-write-line-icall`
- `pinvoke-host-bridge`

一句话标准可以定成：

- **如果这个名字回答的是“最后怎么实现”，它就不属于 canonical semantic operation vocabulary。**
- **只有回答“这里发生了什么语义动作”，它才属于当前 vocabulary。**

## 12. 我的最终建议

这一轮我建议正式拍板下面几件事：

1. 当前 `Phase A` 的 canonical semantic operation vocabulary 使用一套共享词表，而不是按 family 各自发明局部语言
2. 当前先冻结覆盖 proof spine 的最小集合，而不是预支 Phase B 的大词表
3. 首轮最小 operation 集合建议为：
   - `construct-object`
   - `read-instance-field`
   - `write-instance-field`
   - `invoke-static-method`
   - `invoke-instance-method`
   - `invoke-imported-method`
   - `string-concat`
   - `emit-console-string`
   - `load-type-handle`
   - `query-closed-type-member`
   - `query-generic-type-definition`
   - `load-const-string`
   - `load-const-int32`
   - `return`
4. member kind / target subject / literal value / argument refs 等差异优先进入 payload，而不是继续拆 operation 名
5. 一切会泄漏 lowering/runtime/backend/template/proof-shape 的名字，明确禁止进入当前 operation vocabulary

## 13. 这一步之后最值得继续讨论的点

如果这套 operation vocabulary 方向认可，下一步最自然的继续点就是：

- **这些 operation 的 payload contract 到底最少应该长什么样**

因为到这里为止：

- 动作名字已经基本有了
- 接下来缺的是：
  - 每个动作最少需要携带哪些 canonical subject refs / literal refs / kind tags

也就是说，operation vocabulary 稳住之后，下一层自然就是：

- **operation payload contract**
