# Phase A CodeGen Input Consumption Contract v1.01

Date: 2026-04-08
Status: design-discuss

## 1. 问题点

到上一轮为止，`Linker` 的输出面已经基本接近闭合：

- retained closure
- semantic-layer reasons
- `EmissionOrder`
- `Diagnostics`

而且我们已经明确：

- 只要 `LinkedWorld.Diagnostics` 中存在 `error`
- `CodeGen` 就不应继续消费该 `LinkedWorld`

那接下来的核心问题就自然变成：

- **当 `CodeGen` 拿到一个 error-free `LinkedWorld` 之后，它到底允许读哪些输入，不允许再去哪里“补理解”。**

如果这一步不冻结，前面辛苦收住的 `Linker` contract 很容易被重新冲掉：

1. `CodeGen` 回头直接扫 `typed-il` 再猜一次语义
2. `CodeGen` 重新自己造 dependency/order/reason 逻辑
3. `CodeGen` 用 proof-shape/sample-shape 作为事实来源

这样一来，表面上是 `Linker -> CodeGen` 分层了，实际上还是：

- 语义理解散落在多个阶段

所以这一轮真正要回答的是：

- **`CodeGen` 的正式输入消费边界应该长什么样。**

## 2. 这一轮真正要分开的东西

这里最容易混淆的是四类输入来源：

### A. `LinkedWorld`

这是已经冻结好的 semantic closure artifact，包含：

- entries
- retained types / members
- dependencies
- reasons
- emission order
- diagnostics

### B. `SemanticWorld` 的 canonical lookup

这是按 canonical id 可查询的语义层信息，例如：

- subject kind / member shape
- semantic operation stream
- capability / requirement hints
- canonical semantic identity

### C. metadata/token/signature lookup

这是 `CodeGen` 在组 plan 时仍然需要的稳定元数据，例如：

- token
- name
- signature
- arity
- owner type

### D. raw/late-discovery 输入

这类输入包括：

- 原始 `typed-il`
- raw instruction sequence
- proof-shape detector
- sample-specific shape heuristic

这类输入一旦重新变成 `CodeGen` 的事实来源，边界就会回流。

所以真正要比较的不是：

- “`CodeGen` 需不需要信息”

而是：

- **哪些信息应通过正式 contract 进入 `CodeGen`**
- **哪些信息绝不能再作为 `CodeGen` 的重新发现入口**

## 3. 三种可选方案

## 3.1 方案 A：宽松消费型

### 方案描述

`CodeGen` 可以读取：

- `LinkedWorld`
- `SemanticWorld`
- metadata lookup
- 必要时直接回看 `typed-il` / raw instruction shape

也就是说：

- `LinkedWorld` 是重要输入
- 但不是 `CodeGen` 的唯一权威边界

### 优点

- 迁移成本最低
- 最接近当前代码现实
- 某些短期 case 容易“先跑通”

### 缺点

- `CodeGen` 仍然是第二个语义解释器
- `LinkedWorld` 的 contract 权威性会被削弱
- 很容易重新长出：
  - instruction normalization
  - shape detection
  - fallback dependency reasoning
- 后续 review 很难判断某个决策到底来自哪一层

### 适用场景

- 只追求短期迁移落地速度
- 暂时接受主线边界继续模糊

### 我的判断

- 不推荐作为主线方案

这条路适合过渡，但不适合 `il2cpp-mainline` 的核心层收口。

## 3.2 方案 B：平衡消费型

### 方案描述

`CodeGen` 的正式输入分成三层：

1. `LinkedWorld`
   - 作为唯一的 closure/dependency/order/diagnostics 权威来源
2. `SemanticWorld` canonical lookup
   - 只能按 canonical id 查询已归一化的 semantic info
3. stable metadata lookup
   - 只能读取 token/signature/name/arity 这类元数据

同时明确禁止：

- 从 raw `typed-il` / instruction shape 重新推断语义
- 从 proof-shape / sample-shape 重新识别 lowering family
- 自己重新发明 dependency reasons / member ordering

也就是说：

- `CodeGen` 可以“查询”
- 但不能“重新发现”

### 优点

- 边界清楚，但不过度理想化
- `LinkedWorld` 成为权威的 semantic closure 输入
- `CodeGen` 仍然保留足够信息来选择 lowering family、绑定 roles、组装 plan
- 最符合前面已经拍板的：
  - `SemanticWorld` 讲清语义
  - `Linker` 冻结 world
  - `CodeGen` 选择 lowering

### 缺点

- 需要明确 canonical lookup 的白名单
- 需要克制，不让“lookup”重新演化成“任意回看上游”

### 适用场景

- 想让主线边界真正成立
- 同时又不想把所有信息都硬塞进 `LinkedWorld`

### 我的判断

- 推荐

这是我认为当前 `Phase A` 最平衡、最稳的一条路。

## 3.3 方案 C：强约束切片型

### 方案描述

`CodeGen` 不再自己面向 `SemanticWorld` 做自由查询。  
上游直接为它准备一个更强约束的 `LoweringReadySlice`，例如包含：

- family candidate set
- subject role candidates
- required semantic operations summary
- dependency summary
- metadata bundle

此时 `CodeGen` 主要做：

- 在候选 family 中做最终选择
- 组装 `LoweringPlan`

### 优点

- 边界最硬
- deterministic 最强
- `CodeGen` 最容易被测试与断言

### 缺点

- 很容易把 `Linker` 或上游辅助层做厚
- `CodeGen` 会变薄，逐步退化成 plan serializer
- 当前 `Phase A` 来说偏重，容易提前工程化

### 适用场景

- 如果你最在意长期强约束、可验证性和多 backend 一致性
- 并且愿意接受上游 contract 明显变厚

### 我的判断

- 现在不建议作为主线方案
- 但可以作为未来在 family 激增后的升级方向保留

## 4. 我给你的综合建议

如果你是要在这三条路里选一个主线方向，我的建议是：

- **主线选方案 B**
- **把方案 A 只当迁移过渡**
- **把方案 C 当未来升级目标，不要现在就上**

原因很直接：

### 为什么不是方案 A

因为它最容易让我们表面上讨论完 contract，实际上 `CodeGen` 还在偷看原始输入。

### 为什么不是方案 C

因为它虽然最干净，但当前会把太多 lowering-ready 语义预处理压回上游，容易让 `Phase A` 过重。

### 为什么是方案 B

因为它同时守住了三件事：

1. `LinkedWorld` 是权威 closure 输入
2. `CodeGen` 仍然是 lowering 选择层，而不是被动序列化层
3. 不需要现在就把上游做成“半个 codegen”

## 5. 如果按方案 B，建议冻结的正式输入白名单

我建议把 `CodeGen` 可依赖输入明确收成下面三组。

### A. 必须依赖的权威输入

- `Entries`
- `RetainedTypes`
- `RetainedMembers`
- `Dependencies`
- `RetainedReasons`
- `DependencyReasons`
- `EmissionOrder`
- `Diagnostics`

这里面最关键的是：

- closure/dependency/order/diagnostics 只能认 `LinkedWorld`

### B. 允许查询的 canonical semantic lookup

- subject canonical identity
- member shape / subject kind
- semantic operation stream
- capability / requirement hints

这里的约束是：

- 只能按 canonical id 查询
- 只能读已归一化结果
- 不能回去读 raw IL 重新解释

### C. 允许查询的 stable metadata lookup

- token
- name
- signature
- arity
- owner type

这些信息是为了：

- 绑定 `LoweringPlan` 的 subject roles
- 生成 symbol / literal / output contract 所需字段

## 6. 如果按方案 B，建议明确禁止的行为

我建议把下面这些写成明确禁区：

### 禁区 1：禁止用 raw `typed-il` / instruction shape 重新识别 lowering family

例如不应再出现这种思路：

- “如果 instruction 第 3 个是 `newobj`、第 5 个是 `callvirt`，那我猜它是 HelloWorldObject family”

这类识别已经越过 contract 边界了。

### 禁区 2：禁止 `CodeGen` 重新发明 dependency policy

例如不应再出现：

- 看到某个 member，就临时决定要补一个 external dependency
- 或重新改写 dependency reason

dependency freeze 已经属于 `Linker`。

### 禁区 3：禁止 `CodeGen` 重新排序 semantic closure

`CodeGen` 可以在内部组织自己的 lowering traversal，  
但不能改写：

- 哪些 retained objects 存在
- 它们的权威 stable order 是什么

### 禁区 4：禁止 proof-shape/sample-shape 成为事实来源

proof 样例可以辅助测试，但不能成为：

- lowering family 识别入口
- role 绑定入口
- 语义理解入口

## 7. 如果按方案 B，`CodeGen` 自己必须承担哪些决策

这部分也很重要。  
方案 B 不是让 `CodeGen` 只做机械搬运，而是让它把真正属于 lowering 的决策承担起来。

我建议明确归给 `CodeGen` 的决策有：

- 选择 `LoweringFamily`
- 决定同一 semantic slice 的具体 lowering policy
- 组装 `LoweringPlan`
- 填充 subject roles / runtime roles / literal bindings / output contract
- 必要时引入 family-specific extension

用前面的经典例子说就是：

- `semantic.string-concat`
  - 是 `Linker`/reason 层的语义义务
- `pair concat chain` 还是别的 lowering
  - 是 `CodeGen` 的决策

## 8. 更具体的三案对比建议

如果你现在想快速综合评估，我建议用下面四个维度看：

### 维度 1：边界清晰度

- 方案 A：低
- 方案 B：高
- 方案 C：最高

### 维度 2：短期迁移成本

- 方案 A：最低
- 方案 B：中
- 方案 C：最高

### 维度 3：`CodeGen` 是否仍然像真正的 lowering 层

- 方案 A：中，但容易继续兼职语义解释器
- 方案 B：高
- 方案 C：中偏低，容易变薄

### 维度 4：未来多 family / 多 backend 演进性

- 方案 A：低
- 方案 B：高
- 方案 C：高，但早期成本偏大

## 9. 我的最终推荐

如果你让我直接给一个当前最适合主线的建议，我会这样拍板：

1. **主线采用方案 B**
2. `CodeGen` 的权威输入以 `LinkedWorld` 为核心
3. 允许 `CodeGen` 通过 canonical id 查询 `SemanticWorld` 和稳定 metadata
4. 明确禁止 `CodeGen` 从 raw `typed-il`、instruction shape、proof-shape 重新发现语义
5. 把方案 C 记作未来升级方向，而不是现在的基线

### 当前拍板

- **当前讨论已拍板：这里采用方案 B。**
- 也就是：
  - `CodeGen` 的权威输入以 `LinkedWorld` 为核心
  - 允许按 canonical id 查询 `SemanticWorld` 的已归一化语义信息
  - 允许读取稳定 metadata lookup
  - 不允许从 raw `typed-il`、instruction shape、proof-shape 重新发现语义
- 对应影响是：
  - `CodeGen` 仍然保持 lowering 选择层地位
  - `LinkedWorld` 的 closure/dependency/order/diagnostics 真正成为权威 contract
  - 下一步讨论应转向：在这个输入边界下，`CodeGen` 用什么最小决策面来选择 `LoweringFamily`

## 10. 这一步之后最值得继续讨论的点

如果这一步你认可方案 B，下一轮最自然该继续细化的是：

- **`CodeGen` 如何从这些输入中选择 `LoweringFamily`**

也就是从“能读哪些输入”继续走到：

- “面对一个 error-free `LinkedWorld`，family selection 的最小决策面是什么”

这会比直接跳去实现更稳，因为它正好卡在：

- `CodeGen` 是 lowering 选择层
- 但还没有掉进具体代码细节里
