# Phase A Dependency Policy Contract v1.01

Date: 2026-04-07
Status: design-discuss

## 1. 问题点

当前 `Phase A` 在 dependency policy 上的核心问题有三个：

1. `Linker` 还在直接用具体 `subject id` 做 dependency reason 映射
2. managed external dependency、runtime binding、host/platform binding 还没有彻底分层
3. dependency reason 还不是从 `SemanticWorld` 的 capability hints 显式推导出来的 contract

## 2. 问题的具体描述

从当前实现看，`LinkerStage` 还在做两件明显不该长期停留在这一层的事情：

- `NormalizeDependencySubjectId(...)`
  - 例如把 `System.String::Concat(string,string,string)` 直接改写成 pair concat
- `ResolveDependencyReason(...)`
  - 例如把具体 subject id 直接映射成：
  - `base-ctor`
  - `narrow-concat-path`
  - `stdout-path`
  - `reflection-query`

这会带来五个具体问题：

1. dependency reason 的来源还是“某个具体 BCL 成员”，而不是“当前语义切片需要什么能力”
2. 如果后续 `.NET 8 / .NET 10` 输入差异导致 subject shape 变化，reason 体系会一起摇晃
3. 如果后续 Android / iOS / macOS 的宿主绑定不同，当前模型很容易把 host 问题和 managed dependency 问题重新混在一起
4. `CodeGen` 和 `Emitter` 虽然在上几轮已经开始收边界，但 `Linker` 这一侧还没有一个同等显式的 contract
5. 当前“依赖”这个词本身也有歧义，它至少混着三类东西：
   - managed external dependency
   - runtime helper / icall binding
   - host/platform binding

换句话说，这个问题不是简单的“把 switch-case 改漂亮”，而是要回答：

- `LinkedWorld.Dependencies` 到底只装什么
- 哪些依赖应由 `Linker` 负责
- 哪些依赖应转去 `LoweringPlan` 的 runtime/host roles

## 3. 有哪些可解决的路径

### 路径 A：继续把 dependency policy 留在 `Linker`，只是整理成更干净的映射表

做法：

- 保留 `Linker` 为唯一 dependency policy 入口
- 把当前 `ResolveDependencyReason(...)` 这类逻辑从硬编码 `switch` 收成集中配置表
- `SemanticWorld` 暂时不新增 capability-to-dependency contract

优点：

- 改动最小
- 最接近当前代码
- 短期能把现有四个 proof 切片整理得更规整

缺点：

- 仍然是 subject-id 驱动，不是 capability 驱动
- `.NET 10` 输入兼容和移动平台差异会继续把复杂度压回 `Linker`
- 只是在整理代码，不是在冻结主线 contract

判断：

- 不推荐

### 路径 B：建立“两段式 dependency contract”，由 capability hints 驱动 managed dependency policy，runtime/host binding 继续留在 lowering plan

做法：

- `SemanticWorld`
  - 输出 capability hints
- `Linker`
  - 只负责把 capability hints 映射成 managed external dependencies
  - 给出 declarative dependency reasons
- `CodeGen / LoweringPlan`
  - 继续负责 runtime roles / host bindings

也就是说：

- `LinkedWorld.Dependencies`
  - 只表达 external managed dependencies
- `LoweringPlan`
  - 表达 runtime/host binding

优点：

- 和前面三份讨论文档完全对齐
- `Linker` 的职责清楚，回到 retained/dependency policy
- runtime/helper/host 相关需求不会再次混回 `LinkedWorld`
- 对后续 Android / iOS / macOS 适配更稳

缺点：

- 需要先冻结 capability hints 和 reason taxonomy
- 某些能力会同时投影到两边：
  - 一边是 managed external dependency
  - 一边是 runtime role
- 首轮设计要格外注意不要把 contract 做成“大而全 DSL”

判断：

- 推荐

### 路径 C：单独再引入一个 `DependencyPolicy` 层或 stage

做法：

- `SemanticWorld` 之后新增独立 `DependencyPolicy` 层
- 专门负责：
  - capability -> requirement bundle
  - requirement bundle -> managed dependencies
  - reason taxonomy
- `Linker` 只消费已经决议好的 dependency set

优点：

- 架构上最干净
- 依赖策略成为独立可演进对象
- 长期看更适合 family 数量继续增长后的治理

缺点：

- 对 `Phase A` 来说偏重
- 当前 family 数量还不大，直接新开 stage 容易把问题提前工程化
- 会让我们在 contract 还没冻住前先多出一个物理层

判断：

- 现在不建议作为主路径
- 但可以作为未来演进方向保留

## 4. 我的方案和理由

我推荐：

- **路径 B：建立“两段式 dependency contract”**

理由有四个：

1. 它最符合我们前面已经收敛出的主线边界
   - `SemanticWorld` 负责 capability hints
   - `Linker` 负责 managed external dependencies
   - `CodeGen / LoweringPlan` 负责 runtime/host binding

2. 它能直接把 `LinkedWorld.Dependencies` 的语义收窄
   - 不再把所有“运行需要的东西”都塞进 dependency
   - 只保留 external managed dependency

3. 它对移动平台适配更合理
   - Android / iOS / macOS 差异更多体现在 host binding 和 runtime glue
   - 这些本来就不该塞回 `Linker`

4. 它是 `Phase A` 可执行、但不过度设计的解法
   - 不需要马上新开一个 stage
   - 但已经能把 contract 语言冻出来

如果按这个方案推进，我建议先冻结三条规则：

### 规则 1：`LinkedWorld.Dependencies` 只承载 external managed dependencies

例如：

- `System.Object::.ctor()`
- `System.String::Concat(System.String,System.String)`
- `System.Console::WriteLine(System.String)`
- `System.Type::GetField(System.String)`

这类对象留在 `LinkedWorld.Dependencies` 里是合理的。

但下面这些不应直接作为 `LinkedWorld.Dependencies` 主表达：

- `console-write-line-string` 这样的 runtime role
- `native-import-resolution` 这样的 host role
- 平台 loader / bootstrap glue

### 规则 2：dependency reason 必须由 capability hints 或 requirement bundle 导出

建议 reason taxonomy 在 `Phase A` 先收成语义导向，而不是成员名导向。  
例如首轮可以按下面的风格：

- `semantic.object-base-ctor`
- `semantic.console-string-output`
- `semantic.string-concat-pair`
- `semantic.reflection.closed-type-query`
- `semantic.imported-method-reference`

也就是说，不再问：

- “因为看到了哪个 subject id，所以给什么 reason”

而是改成：

- “因为当前 world 需要哪种 capability，所以展开成哪些 dependencies 与 reasons”

### 规则 3：runtime/host binding 仍由 lowering plan 表达

例如同一个能力：

- `requires-console-string-output`

会投影成两类结果：

1. `Linker`
   - external managed dependency:
   - `System.Console::WriteLine(System.String)`
   - reason:
   - `semantic.console-string-output`
2. `CodeGen / LoweringPlan`
   - runtime role:
   - `console-write-line-string`

类似地：

- `requires-string-concat-pair`

也可以同时投影成：

1. external managed dependency
2. runtime role `string-concat-pair`

这不是重复，而是两个层次的同一需求投影。

## 5. 审查下这个问题本身，是否合理，以及是否有其他方式让这个方案在整体框架中更合理地解决

我认为这个问题本身是合理的，而且是 `Phase A` 现在必须讨论的点。  
如果 dependency policy 不冻结，我们前面讨论的：

- capability hints
- lowering family
- lowering-plan role contract

都会少一块中间连接层。

但这个问题也需要被重新框一下，否则很容易问得过大。  
更准确的问法不应是：

- “所有依赖策略到底怎么做？”

而应拆成：

1. `Linker` 负责的 managed external dependency policy 怎么做
2. `LoweringPlan` 负责的 runtime/host binding policy 怎么做

也就是说，原问题是对的，但需要在整体框架里被更精确地切开。

我对这个问题本身的反审查结论是：

- 合理
- 但不能把它做成一个包打天下的“总依赖系统”

让这个方案在整体框架里更合理的做法有三条：

### A. 先冻结 contract，再决定是否物理拆 stage

也就是：

- 现在先用路径 B 冻结 contract 语言
- 将来如果 family 继续增多，再考虑是否演进成路径 C 的独立 `DependencyPolicy` stage

### B. 保持“最小 declarative”，不要过早做 DSL

`Phase A` 最需要的是：

- reason taxonomy
- capability -> dependency bundle 映射
- managed dependency 与 runtime/host binding 的边界

不需要现在就引入一套复杂规则引擎。

### C. 让跨平台差异优先落在 runtime/host binding，而不是回流到 linker

这条很关键。  
如果后续移动平台适配出现差异，优先应该落在：

- runtime roles
- host bindings
- template/backend boundary

而不是重新把平台特化逻辑塞回 `Linker` 的 dependency reason 里。

所以这份文档的真正意义是：

- 把 dependency policy 从“linker 里的特例代码”提升成“主线 contract 的一部分”
- 同时又不把它扩张成一个过重的新系统
