# Phase A CodeGen Lowering-Family Selection Rule Shape v1.01

Date: 2026-04-08
Status: design-discuss

## 1. 问题点

上一轮已经拍板：

- `CodeGen` family selection 的主决策面采用方案 B
- 主信号是：
  - `semantic subject structure`
  - `capability bundle`
- `reason/dependency` 只作为辅助约束

那接下来真正的问题就从“看什么输入”变成了：

- **这些输入到底用什么规则形状来裁决 `LoweringFamily`。**

因为即使主决策面已经收住，仍然还有一个很关键的架构问题没有回答：

1. 当多个 family 看起来都像时，怎么裁决
2. 当没有 family 完整匹配时，怎么失败
3. 规则体系到底应追求“灵活容错”，还是“严格唯一”

所以这一轮真正要回答的不是：

- “当前有哪些 family”

而是：

- **family selection rule 本身应该是什么形状。**

## 2. 问题的具体描述

在当前 `Phase A` 语境下，family selection rule 至少要能回答三件事：

### A. 匹配条件如何表达

例如一个 family 可能会要求：

- 存在某类 subject structure
- 同时具备某组 capability
- 不存在某些排斥性信号

### B. 多候选冲突如何裁决

例如某个 semantic slice 可能同时满足：

- 一个更具体的 family
- 一个更泛化的 family

### C. 失败如何变成 diagnostics

例如：

- 没有任何 family 匹配
- 多个 family 同时成立
- capability bundle 不闭合

如果这一层不收清楚，后面 `CodeGen` 很容易退化成：

- case-by-case if/else
- 隐式顺序依赖
- “看起来像哪个就先用哪个”

这对核心层是很危险的。

## 3. 三种可选方案

## 3.1 方案 A：打分式

### 方案描述

每个 `LoweringFamily` 定义一组特征分值，例如：

- 匹配某个 subject structure 加多少分
- 匹配某个 capability 加多少分
- 缺某个必要条件减多少分

最后：

- 分数最高者获胜
- 分数低于阈值则失败

### 优点

- 最灵活
- 能处理模糊和不完整输入
- 看起来对未来 family 增长适应性强

### 缺点

- 对核心编译层来说太“软”
- 很容易变成经验调参系统
- 诊断解释不够直接
- 同一输入稍改一个权重，结果就可能漂
- 很难形成稳定 contract

### 我的判断

- 不推荐

这类方案更像检索/推荐系统，不像核心 lowering 选择规则。

## 3.2 方案 B：优先级式

### 方案描述

每个 family 都有：

- 一组 required predicates
- 一组 optional support signals
- 一组 forbidden predicates

然后 family 规则按显式优先级顺序评估：

1. 从更具体的 family 开始
2. 第一个满足 required 且不触犯 forbidden 的 family 获胜
3. 如果没有任何 family 成立，则产出 `CodeGen` diagnostics

一句话说：

- **先按语义具体度排顺序，再按顺序裁决。**

### 优点

- deterministic
- 易解释
- 比打分式稳定得多
- 比唯一匹配式更容易在 `Phase A` 落地
- 很适合当前 family 数量少、边界相对明确的阶段

### 缺点

- 优先级本身会成为 contract 的一部分
- 如果顺序设计不好，可能把“更具体/更泛化”的关系偷偷固化成隐藏规则
- 后续新增 family 时需要小心插入位置

### 我的判断

- 推荐

这是当前最平衡的方案。

## 3.3 方案 C：唯一匹配式

### 方案描述

每个 family 只定义：

- required predicates
- forbidden predicates

然后要求：

- 成功时必须“恰好一个 family 匹配”
- `0` 个匹配 = diagnostics
- `2+` 个匹配 = diagnostics

也就是说：

- 规则体系天然要求 families 之间互斥或足够可分

### 优点

- contract 最硬
- 最容易测试
- overlap 会立刻暴露
- 长期最利于体系治理

### 缺点

- 当前 `Phase A` 过于严格
- 要求我们现在就把 family 边界雕得非常干净
- 一旦未来 family 增长，前期会频繁碰到 overlap diagnostics
- 会拖慢主线收口节奏

### 我的判断

- 现在不建议作为主线路径
- 但很适合作为未来升级方向

## 4. 我的综合建议

如果你现在要选一个最适合 `Phase A` 主线的规则形状，我建议：

- **当前主线采用方案 B：优先级式**

同时保留一个演进方向：

- **长期目标可以向方案 C：唯一匹配式演进**

## 5. 为什么我推荐方案 B

### 原因 1：它最符合 `Phase A` 的成熟度

当前 family 数量不多，但也不是小到可以完全不考虑冲突。  
优先级式正好处在：

- 不像打分式那样过软
- 也不像唯一匹配式那样过硬

### 原因 2：它的可解释性足够强

当一个 family 被选中时，我们可以直接解释：

- 因为它满足了哪些 required predicates
- 没触犯哪些 forbidden predicates
- 并且在优先级上先于更泛化的 family

这比“得分最高”要清楚得多。

### 原因 3：它适合当前 family 的天然层次

当前 families 本身就有一个从更具体到更一般的感觉，例如：

- `interop.pinvoke-direct-call.minimal`
- `reflection.closed-type-query.minimal`
- `managed-generic.static-forwarder-captured-getter.minimal`
- `managed-object.captured-state-instance-message.minimal`

这里面某些 family 天然更窄、更具体，  
用优先级式能比较自然地表达这一点。

### 原因 4：它给未来升级留了路

优先级式并不妨碍未来演进到唯一匹配式。  
相反，它可以先帮助我们看清：

- 哪些 overlap 是真实存在的
- 哪些 family 其实已经足够互斥

等规则面稳定后，再升级为唯一匹配式更稳。

## 6. 如果按方案 B，建议冻结的最小规则骨架

我建议每个 family rule 至少包含三组条件：

### A. `RequiredPredicates`

回答：

- 成为该 family 至少必须满足什么

例如：

- 存在 imported method subject
- 存在 reflection query structure
- 存在 captured state field + instance-message method

### B. `ForbiddenPredicates`

回答：

- 出现哪些信号时，该 family 明确不成立

例如：

- 这是 imported direct call，就不应落到 object/message family
- 这是 closed-type reflection query，就不应落到 generic forwarder family

### C. `SupportSignals`

回答：

- 哪些 capability/reason/dependency consistency 可以用来增强置信度与可解释性

这里要注意：

- `SupportSignals` 只能辅助解释与校验
- 不应重新滑回打分系统

## 7. 如果按方案 B，建议的裁决规则

我建议当前先冻结下面这组简单规则：

1. families 按“从更具体到更泛化”的顺序排列
2. 依次检查每个 family：
   - required 全满足
   - forbidden 全不触发
3. 第一个成立的 family 获胜
4. 如果没有任何 family 成立：
   - 产出 `CodeGen` diagnostics

这一轮我不建议再加更复杂的竞争机制。

## 8. 当前不建议做得太复杂的点

### A. 不建议引入分数权重

因为这会把规则系统立刻变成调参系统。

### B. 不建议现在就强制“多匹配即错误”

这更适合未来升级到唯一匹配式时再做。  
当前先让优先级顺序承担裁决职责更实际。

### C. 不建议把 dependency/reason consistency 升格成单独仲裁层

它们更适合作为：

- support signals
- diagnostics explanation

而不是再单独拉出一层“二次仲裁器”。

## 9. 这里的 rule 方案到底在讨论什么

这里讨论的不是：

- `LoweringFamily` 叫什么名字
- 也不是每个 family 里面具体绑哪些 role

这里真正讨论的是：

- **当 `CodeGen` 已经拿到一个 semantic slice，并且它“看起来可能像多个 family”时，系统到底用什么裁决规则选出最终 family。**

也就是说，rule 方案讨论的是：

- family selection engine 的裁决机制

而不是：

- family vocabulary 本身

## 10. 用一个具体例子看三种 rule

为了把问题说清楚，可以先假设未来除了当前的：

- `managed-object.captured-state-instance-message.minimal`

之外，我们又新增了一个更泛化的 family：

- `managed-object.instance-message.minimal`

这个更泛化的 family 只要求：

- 有 reference type
- 有 instance-message method
- 有 console string output

而更具体的 family 还额外要求：

- 有 captured state field
- 有 object construction
- 需要时具备 string concat

现在看这个例子：

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

这个 semantic slice 同时满足：

- 泛化 family
  - `managed-object.instance-message.minimal`
- 更具体 family
  - `managed-object.captured-state-instance-message.minimal`

这时候三种 rule 方案的区别就出来了。

### A. 打分式会怎么做

系统可能这样打分：

- `managed-object.instance-message.minimal`
  - 有 instance-message: `+3`
  - 有 console output: `+2`
  - 总分 `5`
- `managed-object.captured-state-instance-message.minimal`
  - 有 instance-message: `+3`
  - 有 console output: `+2`
  - 有 captured state field: `+3`
  - 有 object construction: `+2`
  - 总分 `10`

于是系统选：

- `managed-object.captured-state-instance-message.minimal`

问题是：

- 这个结果依赖权重设计
- 权重一变，结果可能就变

所以它更像调参，而不像硬 contract。

### B. 优先级式会怎么做

系统先定义顺序：

1. `managed-object.captured-state-instance-message.minimal`
2. `managed-object.instance-message.minimal`

然后按顺序检查：

- 第一个 family 的 required predicates 全满足
- forbidden predicates 也没触发

于是直接选：

- `managed-object.captured-state-instance-message.minimal`

这里的关键是：

- 不是因为它“分更高”
- 而是因为它被定义为“更具体、优先级更高”的 family

这就是我当前推荐的规则形状。

### C. 唯一匹配式会怎么做

如果两个 family 都匹配成功，那么系统不会自动挑一个，而是直接报错：

- 这说明 family 规则有 overlap
- 规则体系还不够互斥

于是你必须回去改规则，例如：

- 给泛化 family 增一个 `ForbiddenPredicate`
  - `has-captured-state-field`

这样它就不再和更具体 family 重叠。

这类方案最干净，但现在会比较硬。

## 11. 再看一个“0 个匹配”的例子

假设有个 semantic slice：

- 既不是 imported direct call
- 也不是 reflection query
- 也不满足当前 object/message family
- 也不满足 generic forwarder family

那三种方案里：

- 打分式
  - 可能会试图选一个“分数最高但其实不太像”的 family
- 优先级式
  - 会得到“没有任何 family 成立”的明确 diagnostics
- 唯一匹配式
  - 也会得到明确 diagnostics

这也是为什么我不推荐打分式：

- 它太容易在“其实不该匹配”时也硬选出一个看起来最像的结果

## 12. 一个更具体的建议顺序

如果按当前 family 直觉，我建议先按“更具体 -> 更一般”理解成：

1. `interop.pinvoke-direct-call.minimal`
2. `reflection.closed-type-query.minimal`
3. `managed-generic.static-forwarder-captured-getter.minimal`
4. `managed-object.captured-state-instance-message.minimal`

这不是说这个顺序现在就永久冻结，  
而是建议把“优先级本身是一等规则”这个思想先冻结下来。

## 13. 我的最终建议

如果这一步要直接拍板，我建议定成：

1. **当前 `Phase A` 的 family selection rule 采用方案 B：优先级式**
2. 每个 family rule 至少包含：
   - `RequiredPredicates`
   - `ForbiddenPredicates`
   - `SupportSignals`
3. family 按“更具体 -> 更一般”的显式顺序裁决
4. `reason/dependency consistency` 只用于辅助解释和一致性校验
5. 打分式明确不采用
6. 唯一匹配式保留为未来升级方向

## 14. 这一步之后最值得继续讨论的点

如果你认可这里，那么下一步最自然的继续点就是：

- **把当前几类 `LoweringFamily` 的 `RequiredPredicates / ForbiddenPredicates` 具体列出来**

也就是从“规则形状”继续走到：

- “这 4 个当前 family 各自到底怎么匹配”
