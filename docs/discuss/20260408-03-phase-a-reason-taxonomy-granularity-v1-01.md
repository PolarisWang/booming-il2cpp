# Phase A Reason Taxonomy Granularity v1.01

Date: 2026-04-08
Status: design-discuss

## 1. 问题点

在上一轮讨论里，关于 `RetainedReasons / DependencyReasons` 的粒度，我们已经自然分出了两条更值得比较的路径：

- 方案 2：纯语义层 reason
- 方案 3：双层 reason

这里的核心问题不是“哪个更优雅”，而是一个更硬的架构问题：

- **方案 3 是否能做到方案 2 做不到的事？**

如果答案是“不能”，那方案 3 很可能只是更重的写法。  
如果答案是“能”，那我们就要继续问：

- 它多出来的能力到底属于“核心编译能力”
- 还是“跨层追踪、诊断和演进能力”

## 2. 问题的具体描述

先明确这里的两个方案：

### 方案 2：纯语义层 reason

- `Linker` 只产出语义层 reason，例如：
  - `semantic.string-concat`
  - `semantic.console-string-output`
  - `semantic.object-construction`
- `CodeGen` 后续如何 lowering，不再被 reason 体系显式表达

### 方案 3：双层 reason

- `Linker` 仍只产出语义层 reason
- 但在 `CodeGen / LoweringPlan / diagnostics` 一侧，再正式引入一层 implementation / lowering reason
- 两层之间保持可追踪映射

也就是说，方案 3 不是让 `Linker` 变厚，而是：

- **在不污染 linker 的前提下，再给 lowering 层一个正式的“为什么这样降”的表达面。**

所以比较的关键不是：

- “谁更简洁”

而是：

- “是否有一些架构能力，方案 2 只能隐式做到，而方案 3 可以显式做到”

## 3. 我的判断

我的判断分成两句：

### 结论 1：从“核心编译能力”看，方案 3 并没有解锁方案 2 绝对做不到的事

也就是说：

- 只要 `CodeGen` 能看懂 `LinkedWorld`
- 只要 `LoweringPlan` 能表达最终 role bindings

那么从“能不能把 C# 正确降到 C++”这个角度，方案 2 已经足够。

例如：

- `semantic.string-concat`
- `semantic.console-string-output`

在方案 2 下，`CodeGen` 仍然可以决定：

- `String.Concat` 是 direct lowering
- 还是 pair concat chain

`Emitter` 也仍然可以按 plan 正确输出 C++。

所以：

- **方案 3 不是为了解锁新的核心语义支持。**

### 结论 2：从“架构层追踪与演进能力”看，方案 3 能做到方案 2 做不到的事

这里的“做不到”，更准确地说是：

- 方案 2 可以靠局部实现或文档约定“勉强做到”
- 但不能把它提升成一等、稳定、可查询的正式 contract

换句话说，方案 3 多出来的能力，不是“编译能力”，而是：

- **semantic -> implementation 的一等可追踪性**

## 4. 方案 3 真正多出来的能力

我认为至少有四类。

### A. 同一语义 reason 对应多个 lowering 策略时，方案 3 可以显式表达“这次到底选了哪一个”

例子：

```csharp
Console.WriteLine(string.Concat(a, b, c));
```

在方案 2 里，`Linker` 可以很干净地只留下：

- `semantic.string-concat`

然后 `CodeGen` 继续自己决定：

- 直接 lowering
- 或拆成 pair concat chain

这当然能工作。

但方案 2 没有一个一等对象去表达：

- “本次之所以最终这么生成，是因为选了 `concat.pair-chain` 这条实现策略”

方案 3 则可以显式表达：

- semantic reason:
  - `semantic.string-concat`
- implementation reason:
  - `lowering.concat.pair-chain`

这件事在单 backend、单 lowering 时不显眼，但一旦同一语义家族有多种 lowering 路线，它就会很重要。

### B. 同一语义在不同 backend / host / template 下走不同实现时，方案 3 可以保留稳定语义 reason，同时显式记录实现分歧

例子：

```csharp
Console.WriteLine(message);
```

可能的语义 reason 始终是：

- `semantic.console-string-output`

但实现层可能分成：

- `lowering.runtime.icall.console-write-line`
- `lowering.host.bridge.console-log`
- `lowering.engine.binding.log-write`

方案 2 当然也能支持这些不同实现：

- 只要 `CodeGen` 和 `LoweringPlan` 自己各做各的就行

但方案 2 没有一等的“implementation reason”层，因此：

- 后续的 reporting / compare / perf attribution
- 都得从 `LoweringPlan` 或 backend-specific diagnostics 里反推

方案 3 则能让这层差异变成正式对象。

### C. 当同一语义支持开始做 A/B lowering 或 perf 策略切换时，方案 3 更容易支撑正式对比

例子：

- `semantic.virtual-dispatch`

未来可能对应：

- `lowering.virtual.slot-call`
- `lowering.virtual.devirtualized-direct-call`
- `lowering.virtual.helper-dispatch`

如果只用方案 2：

- 系统仍然能工作
- 但“为什么这次选了 helper-dispatch 而不是 direct-call”没有一等表达

方案 3 则可以让这种对比变成结构化信息：

- 语义上都是 `virtual-dispatch`
- 实现上这次走了 `helper-dispatch`

这对将来的 perf baseline、backend comparison、traceability 都更友好。

### D. 方案 3 更适合把“语义 reason”和“实现 reason”分别给不同消费者

例如：

- `Linker / architecture docs`
  - 主要看 semantic reasons
- `CodeGen / emitter diagnostics / perf tooling`
  - 更关心 implementation reasons

方案 2 的问题不是完全不能满足这两个消费者，而是：

- 两边会开始靠不同局部对象说话
- 缺少一个正式的桥梁

方案 3 则天然适合分消费层：

- 语义层看语义
- lowering 层看实现
- 两者仍然能稳定回链

## 5. 方案 2 做不到的事，准确说是什么

为了说得更严谨，我建议把“做不到”换成下面这句话：

- **方案 2 做不到把 semantic -> implementation 映射提升成一等、稳定、跨层可查询的 contract。**

它不是完全没有能力表达，而是只能通过：

- `LoweringFamily`
- `TemplateKind`
- backend-specific diagnostics
- 局部 report

这些分散对象来“拼出来”。

所以如果系统未来的目标只是：

- 一条主 backend
- 每个 semantic slice 基本只有一条 lowering 路
- 暂时不强调 compare/perf/host-variant traceability

那么方案 2 完全够用。

但如果系统未来要支持：

- 一个 semantic family 对应多个 lowering strategy
- backend / host / engine binding 的实现分岔
- perf A/B 比较
- 公开报告里回答“为什么这次这样降”

那方案 3 的价值就会开始变大。

## 6. 我的推荐

所以我的最终建议不是简单地说“方案 3 更强，所以现在就上”，而是：

### 推荐结论

- **现在的 `Phase A` 基线仍然应优先采用方案 2**
- **但架构上应明确预留方案 3 的升级方向**

### 当前拍板

- **当前讨论已拍板：这里使用方案 2。**
- 也就是：
  - `Linker` 只输出 semantic-layer `RetainedReasons / DependencyReasons`
  - 不在当前阶段引入正式的 implementation / lowering reason 层
- 对应影响是：
  - `semantic -> implementation` 的正式双层可追踪性先不进入 `Phase A` 基线 contract
  - `CodeGen` 继续通过 `LoweringFamily + LoweringPlan` 表达实现决策
  - 方案 3 只保留为未来在“一语义多 lowering / 多 backend / perf compare”明显增长时的升级方向

原因是：

1. 方案 2 足以支撑当前核心层收口
2. 方案 3 的增量价值主要在 traceability / reporting / compare / perf，而不是当前最紧迫的 correctness contract
3. 如果现在就把方案 3 做成正式全局 contract，`Phase A` 很容易过重

### 更具体地说

既然已经拍板方案 2，现在基线应固定为：

- `Linker`
  - 只输出 semantic-layer `RetainedReasons / DependencyReasons`
- `CodeGen`
  - 先继续通过 `LoweringFamily + LoweringPlan` 表达实现决策
- 文档层
  - 明确记录：未来若出现“一语义多 lowering”显著增长，可升级到方案 3 的双层 reason

这样能同时守住两件事：

- 不让 `Linker` 回流进 lowering
- 也不给未来的 traceability 演进封死路

## 7. 这一步之后最值得继续讨论的点

如果沿这个推荐方向继续，下一步最值得细化的是：

1. 方案 2 下 semantic reasons 的最小 vocabulary
2. 哪些场景一旦出现，就应该触发从方案 2 升级到方案 3

如果只能先选一个，我更建议先聊：

- **semantic reasons 的最小 vocabulary**

因为 vocabulary 不稳，方案 2 本身就还没冻住；  
而方案 3 的升级条件，可以在方案 2 稳下来之后再补。
