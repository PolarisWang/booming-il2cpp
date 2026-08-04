# Phase A Semantic Reason Vocabulary v1.01

Date: 2026-04-08
Status: design-discuss

## 1. 问题点

上一轮已经拍板：

- 当前 `Phase A` 使用方案 2
- `Linker` 只输出 semantic-layer `RetainedReasons / DependencyReasons`
- 不在当前基线里引入正式的 implementation/lowering reason 层

那下一步真正必须收口的就不是“还要不要双层 reason”，而是：

- **方案 2 下，`RetainedReasons / DependencyReasons` 的最小 vocabulary 到底该怎么冻结。**

如果这一步不冻结，`LinkedWorld` 虽然形式上已经有了 reasons，但后续仍然会继续漂：

1. 不同语义切片各自发明 reason 名字
2. `RetainedReasons` 和 `DependencyReasons` 逐渐长成两套彼此脱节的话语体系
3. lowering/template/runtime 决策重新借 reason 名字偷偷回流进 `Linker`

所以这一步的核心不是“先把词汇表做大”，而是：

- **先收出一套最小、稳定、不泄漏 lowering 的 semantic reason vocabulary。**

## 2. 问题的具体描述

这里其实有三个紧挨着的小问题：

### A. `RetainedReasons` 和 `DependencyReasons` 要不要完全拆成两套 vocabulary

可选方向大致有三种：

- 完全共用一套 vocabulary
- 完全拆成两套 vocabulary
- 共用同一命名空间，但按 surface 限制允许子集

### B. 结构性 reason 和语义性 reason 要怎么分

目前我们已经自然遇到了两种不同性质的“原因”：

- 结构性原因
  - 例如：它是 entry root，或它从 entry root 可达
- 语义性原因
  - 例如：当前 world 需要 object construction、string concat、console string output

这两类如果不分开，reason vocabulary 很快会变成一团：

- 一半在讲 graph closure
- 一半在讲 semantic obligations
- 还会混进实现层名字

### C. 哪些 reason 名字必须明令禁止

方案 2 的最大风险不是“reason 不够多”，而是：

- `Linker` 重新开始用 reason 偷渡 lowering 决策

例如下面这些词一旦被 reason 体系吸收，就说明边界已经开始回流：

- `pair-concat`
- `direct-call`
- `template-*`
- `icall-*`
- `host-bridge-*`
- `engine-binding-*`

## 3. 我的方案和理由

我建议把这一轮 vocabulary 收成：

- **一套共享命名空间**
- **两类 reason family**
- **两个 surface 各自允许不同子集**

也就是第三种路径：

- **共用同一命名空间，但按 surface 限制允许子集**

### 推荐结论 1：不要把 `RetainedReasons` 和 `DependencyReasons` 做成两套完全独立 vocabulary

如果完全拆开，短期看起来清爽，但长期很容易出现：

- 同一 semantic obligation 在 retained 侧和 dependency 侧各有一套不同命名
- 文档、diagnostics、下游消费方要维护两套映射
- 一旦未来要做 cross-surface reasoning，就得重新补桥

而我们当前真正需要的不是两门语言，而是：

- 一门共享的 semantic language
- 再约束不同 surface 分别能说哪些句子

### 推荐结论 2：reason family 先只分成 `structural.*` 和 `semantic.*`

这一步我建议非常克制，不要再细分第三层第四层：

- `structural.*`
  - 回答“它为什么在 closure 里”
- `semantic.*`
  - 回答“当前 linked world 为什么需要这个 managed 语义义务”

这样做的好处是：

1. 结构性和语义性被明确拆开
2. vocabulary 仍然很小
3. 还不会提前引入 implementation/lowering 层

### 推荐结论 3：`DependencyReasons` 在当前基线里尽量只吃 `semantic.*`

我的建议是：

- `RetainedReasons`
  - 允许 `structural.*`
  - 允许 `semantic.*`
- `DependencyReasons`
  - 当前基线只允许 `semantic.*`

原因是：

- retained world 需要说明 graph closure 中的位置
- external managed dependency 更应该说明“它被引入是为了满足什么 semantic obligation”

如果把太多 structural reason 也灌进 `DependencyReasons`，很容易退化成：

- “因为某个 retained member 引用了它，所以保留它”

这句话虽然没错，但解释力很弱，而且会把 reason 重新拉回图遍历视角，而不是 semantic contract 视角。

## 4. 推荐的最小 vocabulary

### A. `structural.*` 首批只冻结两个

我建议当前 `Phase A` 基线先只冻结：

- `structural.entry-root`
- `structural.entry-reachable`

含义分别是：

- `structural.entry-root`
  - 该对象是当前 linked world 的显式 entry 根
- `structural.entry-reachable`
  - 该对象不是显式 entry，但因 entry closure 被保留

这里故意不继续扩成：

- `structural.owner-type-required`
- `structural.signature-required`
- `structural.base-chain-required`

因为这些一旦展开太早，reason vocabulary 很容易重新长成“图边类型枚举表”。  
当前基线先只需要回答两件事：

1. 它是不是 root
2. 它是不是 root 导出的 closure 一部分

这就够了。

### B. `semantic.*` 首批建议冻结的最小集

我建议当前先冻结下面这批最小 semantic vocabulary：

- `semantic.object-construction`
- `semantic.instance-field-state`
- `semantic.instance-message`
- `semantic.string-concat`
- `semantic.console-string-output`
- `semantic.reflection.closed-type-query`
- `semantic.imported-method-reference`
- `semantic.imported-type-reference`

这些名字对应的不是 lowering 决策，而是 semantic obligation：

- `semantic.object-construction`
  - 当前 linked world 需要对象构造语义
- `semantic.instance-field-state`
  - 当前 linked world 需要实例字段状态读写/持有语义
- `semantic.instance-message`
  - 当前 linked world 需要实例方法消息发送语义
- `semantic.string-concat`
  - 当前 linked world 需要字符串拼接语义
- `semantic.console-string-output`
  - 当前 linked world 需要控制台字符串输出语义
- `semantic.reflection.closed-type-query`
  - 当前 linked world 需要对闭合类型执行反射查询语义
- `semantic.imported-method-reference`
  - 当前 linked world 需要一个 external managed method reference
- `semantic.imported-type-reference`
  - 当前 linked world 需要一个 external managed type reference

这里面前五个更像“行为语义义务”，后两个更像“跨程序集引用义务”，但它们都仍然停留在 semantic layer。

### C. surface 允许子集

我建议当前先把允许子集收成：

#### `RetainedReasons`

允许：

- `structural.entry-root`
- `structural.entry-reachable`
- 上述 `semantic.*` 最小集

#### `DependencyReasons`

允许：

- `semantic.object-construction`
- `semantic.instance-field-state`
- `semantic.instance-message`
- `semantic.string-concat`
- `semantic.console-string-output`
- `semantic.reflection.closed-type-query`
- `semantic.imported-method-reference`
- `semantic.imported-type-reference`

不建议当前让 `DependencyReasons` 使用：

- `structural.entry-root`
- `structural.entry-reachable`

因为 dependency 的解释重点应该是 semantic requirement，而不是 closure 图位置。

## 5. 用例子看这套 vocabulary 怎么落

继续用前面的例子：

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

如果按当前推荐 vocabulary，大致可以这样理解：

### `RetainedReasons`

- `Program::Main()`
  - `structural.entry-root`
- `Greeter`
  - `structural.entry-reachable`
  - `semantic.object-construction`
- `Greeter::.ctor(string)`
  - `structural.entry-reachable`
  - `semantic.object-construction`
- `Greeter::Build()`
  - `structural.entry-reachable`
  - `semantic.instance-message`
- `Greeter::_name`
  - `structural.entry-reachable`
  - `semantic.instance-field-state`

### `DependencyReasons`

- `System.String::Concat(...)`
  - `semantic.string-concat`
- `System.Console::WriteLine(System.String)`
  - `semantic.console-string-output`
- 某个 external managed method ref
  - `semantic.imported-method-reference`

这里最关键的是：

- `String.Concat(a,b,c)` 即使最终被 `CodeGen` 选成 pair-chain lowering
- `DependencyReasons` 里仍然只写 `semantic.string-concat`

而不是：

- `semantic.string-concat-pair`
- `lowering.concat.pair-chain`

因为后两者都已经越过了方案 2 的边界。

## 6. 明确禁止进入当前 vocabulary 的名字

我建议把下面这些类别明确列入当前禁区：

- `lowering.*`
- `template.*`
- `runtime-role.*`
- `host-binding.*`
- `engine-binding.*`
- `literal.*`

对应到具体例子，当前不应出现的 reason 名字包括：

- `pair-concat`
- `concat-pair-chain`
- `direct-call`
- `virtual-slot-call`
- `template-hello-world-object`
- `icall-console-write-line`
- `host-bridge-console-log`
- `engine-binding-log-write`

一句话标准可以定成：

- **如果这个名字回答的是“怎么实现”，那它就不属于当前 `Linker` reason vocabulary。**
- **只有回答“为什么在 linked semantic closure 里需要它”，才属于当前 vocabulary。**

## 7. 我的最终结论

这一轮我建议正式拍板下面几件事：

1. 当前 `Phase A` 的 semantic reason vocabulary 使用共享命名空间，而不是 `RetainedReasons / DependencyReasons` 两套完全独立语言
2. reason family 先只冻结两类：
   - `structural.*`
   - `semantic.*`
3. `structural.*` 当前基线只冻结：
   - `structural.entry-root`
   - `structural.entry-reachable`
4. `semantic.*` 当前基线先冻结最小集：
   - `semantic.object-construction`
   - `semantic.instance-field-state`
   - `semantic.instance-message`
   - `semantic.string-concat`
   - `semantic.console-string-output`
   - `semantic.reflection.closed-type-query`
   - `semantic.imported-method-reference`
   - `semantic.imported-type-reference`
5. `DependencyReasons` 当前基线只使用 `semantic.*`
6. 一切会泄漏 lowering/template/runtime/host/engine 决策的命名，明确禁止进入当前 vocabulary

## 8. 这一步之后最值得继续讨论的点

如果这套 vocabulary 方向认可，下一步最值得继续收的是：

- **`EmissionOrder` 的 contract 到底只需要“稳定顺序”，还是还需要更强的 grouping / phase boundary 语义。**

因为到这里为止：

- `Linker` 输出“为什么保留/为什么依赖”已经基本有了语言
- 接下来缺的是“这些冻结后的对象该如何稳定交给 `CodeGen` 消费”

也就是说，reason vocabulary 稳住之后，下一层自然就是：

- **`EmissionOrder` contract**
