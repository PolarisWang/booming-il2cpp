# Phase A Semantic Operation Stream Granularity v1.01

Date: 2026-04-08
Status: design-discuss

## 1. 问题点

上一轮已经把 `IL-first` 下的边界收成了：

- `Loader` 负责读出 assembly/metadata/IL 的结构事实
- `SemanticWorld` 负责把这些结构事实归一化成当前支持切片里的语义

但这里还有一个关键问题没冻住：

- **`SemanticWorld` 输出的 canonical semantic operation stream，到底应该细到什么粒度。**

如果这一步不收住，边界还是会继续晃：

1. operation 太贴近 raw IL，`SemanticWorld` 就还是半个 passthrough
2. operation 太高层，`SemanticWorld` 就会偷偷侵入 lowering / family 选择
3. `Linker / CodeGen` 会重新因为 operation 粒度不稳而各自补理解

所以这一步真正要回答的是：

- **operation stream 应该停在“哪种语义粒度”才最适合当前 `Phase A`。**

## 2. 这里讨论的不是哪件事

这里讨论的不是：

- `LoweringFamily` 怎么选
- `LoweringPlan` 里绑哪些 roles
- C++ 最后怎么写

这里讨论的是：

- `SemanticWorld` 给下游看的 method-level 语义动作，应该是什么粒度

也就是：

- `Loader` 读出来的 raw IL，经过 `SemanticWorld` 之后，到底该变成什么样的“语义动作流”

## 3. 三种可选方案

## 3.1 方案 A：贴近 IL 的语义化 opcode 流

### 方案描述

做法是：

- 基本保留 IL 的动作拆分
- 但把 opcode 名换成稍微语义化的 operation 名

例如：

- `ldstr` -> `load-string`
- `ldarg` -> `load-arg`
- `newobj` -> `new-object`
- `call` -> `call-static`
- `callvirt` -> `call-instance`
- `ldfld` -> `read-field`
- `stfld` -> `write-field`
- `ldtoken` -> `load-token`

### 优点

- 最接近当前输入
- 最容易从 loader 过渡
- 实现和验证成本最低

### 缺点

- evaluation stack / opcode shape 的味道仍然太重
- 下游还是要继续把多个 operation 重新拼成“更高一层语义”
- `SemanticWorld` 很容易继续像“更漂亮的 IL dump”

### 我的判断

- 不推荐作为主线路径

这条路虽然安全，但它没有真正把 `Loader -> SemanticWorld` 的边界拉开。

## 3.2 方案 B：语义动作级 operation stream

### 方案描述

做法是：

- operation 不再追逐 opcode
- 而是表达当前支持切片里真正稳定的语义动作

它仍然是 method-level、可组合的动作流，  
但已经不再暴露 stack shuffle 或 opcode 微模式。

例如可以落成这样的操作类型：

- `construct-object`
- `read-instance-field`
- `write-instance-field`
- `invoke-static-method`
- `invoke-instance-method`
- `invoke-imported-method`
- `string-concat`
- `emit-console-string`
- `query-closed-type-member`
- `query-generic-type-definition`
- `return`

### 优点

- 真正把语义层和 opcode 层分开
- 足够服务 `Linker`、`CodeGen`
- 仍然没有提前侵入 lowering / backend
- 最符合当前 `Phase A` 的目标

### 缺点

- 需要 upfront 约束 operation vocabulary
- 需要克制，不让 operation 继续长成宏语义

### 我的判断

- 推荐

这是当前最平衡的方案。

## 3.3 方案 C：proof/family 宏级 operation stream

### 方案描述

做法是：

- 直接把一整段 method 识别成更高层宏动作

例如：

- `captured-state-instance-message`
- `generic-forwarder-getter`
- `closed-type-reflection-query`
- `pinvoke-direct-call`

### 优点

- 下游看起来最轻
- family selection 似乎会变得很容易

### 缺点

- `SemanticWorld` 会直接越界到 family/lowering 层
- operation stream 会和 proof spine/sample shape 绑定
- 很难扩展，也很难复用

### 我的判断

- 明确不推荐

这已经不是 semantic normalization，而是在做 lowering classification。

## 4. 我的建议

如果要在这三条路里选一个当前最适合主线的方案，我建议：

- **主线采用方案 B：语义动作级 operation stream**

一句话定义：

- operation 要比 opcode 更语义化
- 但要比 lowering family 更细、更中性

## 5. 为什么我推荐方案 B

### 原因 1：它刚好把 `Loader` 和 `SemanticWorld` 拉开

`Loader` 负责：

- `ldstr`
- `newobj`
- `callvirt`

而 `SemanticWorld` 则负责：

- `construct-object`
- `invoke-instance-method`
- `string-concat`

这样两层的语言已经明显不同。

### 原因 2：它不会提前侵入 `CodeGen`

如果 operation 已经变成：

- `captured-state-instance-message`

那 `SemanticWorld` 就已经开始替 `CodeGen` 选 family 了。  
方案 B 避开了这一点。

### 原因 3：它足够支撑 capability hints

像下面这些 hints：

- `uses-instance-field-state`
- `requires-string-concat`
- `requires-console-string-output`

都可以从语义动作级 stream 稳定导出，  
不需要再从 raw IL 或 proof-shape 里猜。

### 原因 4：它对未来扩语义更稳

后面新增语义时，最健康的扩展方式是：

- 增 operation vocabulary
- 增 capability vocabulary

而不是：

- 继续把 operation 直接做成 family 名

## 6. 用例子看三种粒度的差别

继续用这个例子：

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

### 方案 A 会长什么样

更接近：

- `load-string("Alice")`
- `new-object(Greeter::.ctor)`
- `call-instance(Greeter::Build)`
- `call-static(Console.WriteLine)`

`Build()` 里则是：

- `load-string("Hello, ")`
- `read-field(Greeter::_name)`
- `call-static(String.Concat)`
- `return`

问题是：

- 下游还是得自己理解 “这是实例状态 + 实例消息 + string concat”

### 方案 B 会长什么样

更接近：

- `construct-object(type=Greeter, ctor=Greeter::.ctor, args=[...])`
- `invoke-instance-method(method=Greeter::Build)`
- `emit-console-string(value=...)`

`Build()` 里则是：

- `read-instance-field(field=Greeter::_name)`
- `string-concat(inputs=[...])`
- `return(value=...)`

这时下游看到的已经是：

- 语义动作

而不是：

- opcode 影子

### 方案 C 会长什么样

更接近直接写成：

- `captured-state-instance-message`
- `console-output-proof`

这就已经太高了，  
基本等于提前做 family classification。

## 7. 我建议冻结的 operation 粒度原则

如果按方案 B，我建议先冻结下面几条原则。

### 原则 1：不暴露纯 stack-shuffle 细节

例如：

- `dup`
- `pop`
- 某些只服务 evaluation stack 的微动作

不应直接作为 canonical semantic operation 的一等对象。

### 原则 2：operation 应回答“语义动作是什么”

例如：

- `construct-object`
- `read-instance-field`
- `invoke-imported-method`
- `query-closed-type-member`

而不是继续回答：

- “第几个 opcode 是什么”

### 原则 3：operation 不应直接回答“最终怎么降”

例如当前不应出现：

- `string-concat-pair-chain`
- `console-write-line-icall`
- `pinvoke-host-bridge`

这些都已经越过 semantic layer 了。

### 原则 4：operation 仍应保持可组合

也就是说：

- 一个 method 仍然是由多个 semantic operations 组成
- 不能直接塌成一个 family 名

## 8. 我建议的首轮最小 operation vocabulary 方向

当前 `Phase A` 可以先围绕下面这些动作族收：

- value/materialization
  - `load-const-string`
  - `load-const-int32`
- object/state
  - `construct-object`
  - `read-instance-field`
  - `write-instance-field`
- call/message
  - `invoke-static-method`
  - `invoke-instance-method`
  - `invoke-imported-method`
- semantic helpers
  - `string-concat`
  - `emit-console-string`
- reflection/query
  - `load-type-handle`
  - `query-closed-type-member`
  - `query-generic-type-definition`
- exit
  - `return`

这里我故意没有把名字做得过多、过细，  
因为当前要先稳边界，不是先做一张巨大的操作码表。

## 9. operation stream 和 capability hints 的关系

我建议把两者关系理解成：

- operation stream
  - 是 method-level 语义动作证据
- capability hints
  - 是对这些动作的摘要归纳

例如：

- method 里出现 `read-instance-field`
  - 可以汇总成 `uses-instance-field-state`
- method 里出现 `string-concat`
  - 可以汇总成 `requires-string-concat`
- method 里出现 `emit-console-string`
  - 可以汇总成 `requires-console-string-output`

这样两者就不会打架，而是：

- 一个讲动作
- 一个讲摘要

## 10. 我的最终建议

如果这一步要直接拍板，我建议定成：

1. **当前 `Phase A` 的 canonical semantic operation stream 采用方案 B：语义动作级**
2. operation 要比 raw IL opcode 更语义化
3. operation 要比 lowering family 更细、更中性
4. 纯 stack-shuffle 细节不进入 canonical operation stream
5. lowering/runtime/backend 决策名明确禁止进入 operation vocabulary

## 11. 这一步之后最值得继续讨论的点

如果你认可这里，那么下一步最自然的继续点就是：

- **首轮 canonical semantic operation vocabulary 到底具体冻结哪些 operation 名字**

也就是从“粒度”继续走到：

- “词表本身的最小集合”
