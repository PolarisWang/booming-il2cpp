# Phase A CodeGen Lowering-Family Selection Surface v1.01

Date: 2026-04-08
Status: design-discuss

## 1. 问题点

上一轮已经拍板：

- `CodeGen` 输入边界采用方案 B
- `LinkedWorld` 是 closure/dependency/order/diagnostics 的权威输入
- `CodeGen` 允许按 canonical id 查询 `SemanticWorld` 的已归一化语义信息
- `CodeGen` 不允许从 raw `typed-il`、instruction shape、proof-shape 重新发现语义

那接下来的核心问题就很自然：

- **在这个输入边界下，`CodeGen` 到底用什么最小决策面来选择 `LoweringFamily`。**

如果这一步不收口，虽然我们已经说清了“不能从哪里偷看”，但仍然没有说清：

1. `CodeGen` 究竟根据哪些正式信号做 family selection
2. 哪些信号只是辅助信息，不能直接决定 family
3. 什么情况下应该得到“无法选择 family”的 `CodeGen` 级 diagnostics

所以这一轮真正要回答的不是：

- “现在支持哪几个 family”

而是：

- **family selection 的正式决策面应该长什么样。**

## 2. 问题的具体描述

在方案 B 下，`CodeGen` 手上大致会有三类信息：

### A. `LinkedWorld` 级信息

例如：

- `Entries`
- `RetainedTypes`
- `RetainedMembers`
- `Dependencies`
- `RetainedReasons / DependencyReasons`
- `EmissionOrder`

### B. canonical semantic lookup

例如：

- subject kind / member shape
- semantic operation stream
- capability / requirement hints

### C. stable metadata lookup

例如：

- token
- signature
- owner type
- arity

但这里真正难的点是：

- **哪一层信息应该成为 family selection 的“主信号”。**

因为如果主信号选错了，架构就会偏：

- 如果过度依赖 dependency 列表，family selection 会被 BCL 形状牵着走
- 如果过度依赖某几个具体 member/token，family selection 会退回 sample-driven
- 如果过度依赖 operation stream 细节，又可能把 `CodeGen` 做成“第二个语义归一化器”

## 3. 三种可选方案

## 3.1 方案 A：以 dependency/reason 侧信号为主

### 方案描述

`CodeGen` 主要根据：

- `DependencyReasons`
- `RetainedReasons`
- external dependency 组合

来判断应该选择哪个 `LoweringFamily`。

例如：

- 看到了 `semantic.console-string-output`
- 看到了 `semantic.string-concat`
- 看到了 `semantic.object-construction`

于是判断大概率属于某个 object/message family。

### 优点

- 直接利用已经冻结好的 `Linker` 输出
- 看起来很 declarative
- 对 diagnostics/reporting 也比较顺手

### 缺点

- reason/dependency 更适合回答“为什么保留/为什么依赖”，不太适合独立回答“这是什么 lowering family”
- 很容易出现多个 family 共享同一批 semantic reasons
- family selection 会被 external dependency 形状牵着走，而不是被 semantic subject structure 驱动

### 我的判断

- 不推荐作为主路径

它更像辅助证据，不适合作为主决策面。

## 3.2 方案 B：以 semantic subject structure + capability bundle 为主，reason/dependency 为辅助约束

### 方案描述

`CodeGen` 主要根据两类主信号做 family selection：

1. semantic subject structure
   - entry method shape
   - retained subject kinds
   - subject role候选
   - canonical semantic operation pattern
2. capability bundle
   - 当前 world 需要哪些 capability / requirement hints

而：

- `RetainedReasons / DependencyReasons`
- `Dependencies`

主要作为辅助约束与一致性校验，不作为第一决策面。

换句话说：

- family 是由“这个 linked semantic slice 本质上是什么结构”决定的
- 不是由“它恰好依赖了哪些外部成员”决定的

### 优点

- 最符合前面已经拍板的分层
- family selection 仍然停留在 semantic/lowering 边界
- 不容易被具体 BCL subject 或宿主差异带偏
- 对未来 family 增长最稳

### 缺点

- 需要明确什么叫“semantic subject structure”的最小表达
- 需要克制，避免把 `CodeGen` 重新做成复杂 pattern engine

### 我的判断

- 推荐

这是当前最平衡的方案。

## 3.3 方案 C：由上游直接提供 family candidate set，`CodeGen` 只做最后判定

### 方案描述

上游在进入 `CodeGen` 前就给出：

- family candidate set
- candidate confidence / rationale

`CodeGen` 只负责：

- 在候选集中选最终 family
- 或报告无法判定

### 优点

- `CodeGen` 决策面最小
- 测试与断言更容易
- family selection 更 deterministic

### 缺点

- 很容易把 family reasoning 压回上游
- `CodeGen` 逐渐变薄
- 当前 `Phase A` 会过早增加一层“半个 codegen”

### 我的判断

- 现在不建议作为主线路径
- 可以作为未来升级方向保留

## 4. 我的建议

如果你要我在这三条路里给一个当前最适合主线的选择，我建议是：

- **主线采用方案 B**

也就是：

- **family selection 的主决策面 = semantic subject structure + capability bundle**
- **reason/dependency = 辅助约束，不是主决策面**

## 5. 为什么我推荐方案 B

### 原因 1：它最符合“`CodeGen` 是 lowering 选择层”

`CodeGen` 本来就应该回答：

- 这组已保留语义该走哪一类 lowering

那它最自然的主输入就应是：

- 语义结构
- 能力需求组合

而不是：

- 某个恰好被保留下来的外部依赖成员名字

### 原因 2：它不会把 family 绑死到当前 BCL 细节

如果主信号来自：

- `System.String::Concat(...)`
- `System.Console::WriteLine(...)`

那 family selection 很容易被当前框架/BCL 形状绑住。  
但真正稳定的东西应该是：

- “这是一个带实例状态的对象构造 + 实例消息 + 字符串输出切片”

而不是：

- “因为看到了某个具体 dependency，所以它属于某 family”

### 原因 3：它给 reason/dependency 留在了正确的位置

我们前面已经花了很多轮才收住：

- reason 是 semantic/linking 解释面
- dependency 是 external managed closure 面

如果现在又让它们变成 family selection 的主入口，就等于把这两层重新抬成 lowering classifier。

### 原因 4：它对未来扩 family 更稳

将来新增 family 时，最健康的扩展方式应是：

- 增一个新的 semantic subject structure 模式
- 增一组 capability bundle 组合规则

而不是：

- 再加一批 dependency 特征匹配

## 6. 如果按方案 B，建议冻结的最小决策面

我建议把 family selection 的主决策面收成下面三组信号。

### A. primary signal 1：entry-centered semantic subject structure

回答：

- 这组 retained subjects 围绕 entry 构成了什么样的 semantic slice

例如可以关注：

- entry method 是否驱动对象构造
- 是否存在 reference type + captured state field
- 是否存在 instance-message method
- 是否存在 getter-forwarder
- 是否存在 closed-type reflection query subject
- 是否存在 imported method subject

这类信号本质上回答的是：

- “当前 world 的主语义骨架是什么”

### B. primary signal 2：capability bundle

回答：

- 这个 semantic slice 需要哪些能力组合

例如：

- `requires-console-string-output`
- `requires-string-concat`
- `uses-instance-field-state`
- `requires-reflection-query`
- `requires-imported-call`

这些信号本质上回答的是：

- “当前骨架需要哪些语义能力支撑”

### C. secondary signal：reason/dependency consistency

用于：

- 检查当前候选 family 是否与 linked world 一致

例如：

- 候选 family 需要 `semantic.string-concat`
- 但 dependency/reason 侧完全没有对应 obligation

那就说明：

- 不是 family 直接选错了
- 就是上游 contract 没对齐

这类信息很重要，但我建议把它放在：

- **consistency check**

而不是：

- **primary classifier**

## 7. 一个更具体的理解方式

你可以把方案 B 下的 family selection 近似理解成一个两步判断：

### 第一步：先看 semantic slice 骨架

例如：

- 是“对象构造 + captured state + instance message”
- 还是“generic static forwarder + captured getter”
- 还是“closed-type reflection query”
- 还是“imported direct call”

这一步先把候选范围缩到某个 family 区域。

### 第二步：再看 capability bundle 是否闭合

例如：

- object/message family 是否同时具备：
  - `uses-instance-field-state`
  - `requires-console-string-output`
  - 需要时具备 `requires-string-concat`

如果 capability bundle 不闭合，就说明：

- 要么 family 不成立
- 要么应产出 `CodeGen` diagnostics

## 8. 明确不建议作为主信号的东西

我建议把下面这些明确降级为“不能单独决定 family”：

### A. 具体 dependency id

例如：

- `System.String::Concat(System.String,System.String)`
- `System.Console::WriteLine(System.String)`

这些只能辅助说明 semantic obligation 已被满足，  
不能单独决定 family。

### B. 单个 reason 名字

例如：

- `semantic.string-concat`
- `semantic.console-string-output`

单个 reason 太弱，也太通用，不适合直接分类。

### C. token / symbol / owner type 这类 metadata

这些信息适合：

- role binding
- plan assembly

但不适合直接用来判 family。

### D. raw instruction 细节

这一条最重要：

- instruction index
- opcode 微模式
- sample/proof shape

都不应回到 family selection 主路径里。

## 9. 我的最终建议

如果这一步要直接拍板，我建议定成：

1. **当前 `CodeGen` lowering-family selection 使用方案 B**
2. `LoweringFamily` 的主决策面是：
   - semantic subject structure
   - capability bundle
3. `RetainedReasons / DependencyReasons / Dependencies` 只作为辅助约束与一致性校验
4. metadata lookup 只用于后续 role binding / plan assembly，不作为 family classifier
5. raw `typed-il` / instruction shape / proof-shape 明确禁止进入 family selection 主路径

### 当前拍板

- **当前讨论已拍板：这里采用方案 B。**
- 也就是：
  - `LoweringFamily` 的主决策面 = `semantic subject structure + capability bundle`
  - `RetainedReasons / DependencyReasons / Dependencies` 只作为辅助约束与一致性校验
  - metadata lookup 只用于 role binding / plan assembly
  - raw `typed-il`、instruction shape、proof-shape 明确禁止进入 family selection 主路径
- 对应影响是：
  - `CodeGen` 的 family selection 继续保持 semantic-to-lowering 边界
  - family 不会被当前 BCL dependency 形状绑死
  - 下一步讨论应转向：这些规则本身到底采用 `打分式`、`优先级式` 还是 `唯一匹配式`

## 10. 这一步之后最值得继续讨论的点

如果你认可这一步，那下一轮最自然的继续点就是：

- **`LoweringFamily` 的 family selection rule 应该是“打分式”“优先级式”还是“唯一匹配式”**

也就是继续往下问：

- 当一个 semantic slice 看起来同时像两个 family 时，`CodeGen` 应该怎么裁决
- 是允许候选竞争，还是要求规则天然唯一
