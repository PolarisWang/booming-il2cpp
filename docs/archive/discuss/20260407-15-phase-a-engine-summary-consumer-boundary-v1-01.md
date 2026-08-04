# Phase A Engine Summary Consumer Boundary v1.01

Date: 2026-04-07
Status: design-discuss

## 1. 问题点

`20260407-14` 已经把 engine proof 的四类 summary 落点收口成了：

- `subject.manifest.json.engineProofProfile`
- `analysis.manifest.json.engineContractSummary`
- `generated.manifest.json.engineEmissionSummary`
- `runtime.manifest.json.engineObservationSummary`
- `report.json.engineProofSummary`

下一步真正高影响的问题是：

- **这些字段到底该由谁消费，谁只能引用，谁完全不应该依赖它们做执行决策。**

如果这个 consumer boundary 不先定下来，后面很容易出现三类退化：

1. `planner` 开始读取 `engineContractSummary` 去推断语义，重新变成半个 engine 解释器
2. `executor` 开始读取 `engineObservationSummary` 决定怎么调度 worker，执行层重新长出领域知识
3. `reporting` 反过来去重建 `LoweringPlan` 级别语义，summary 再次变成假真源

也就是说，字段本身定下来了还不够，必须继续定下：

- **它们的主消费者是谁，以及哪些层明确禁止把它们当执行真源。**

## 2. 问题的具体描述

这一轮需要回答的不是“这些字段有没有用”，而是“五个字段分别服务哪一层”：

1. `engineProofProfile`
   - 是给 registry 做发现与分类，还是给 planner 做执行决策
2. `engineContractSummary`
   - 是给 reporting 做结果解释，还是给 generated worker 当实际输入
3. `engineEmissionSummary`
   - 是给 diagnostics/reporting 看，还是给 runtime worker 用来反推观测目标
4. `engineObservationSummary`
   - 是给 matrix report assembler 汇总，还是给 executor 直接拿来判断后续 stage
5. `engineProofSummary`
   - 是不是 engine proof 对外公开的唯一高层入口

这里最容易出现的错位有四种：

- 把 `summary` 当执行真源
- 把 `summary` 写进 planner output，导致执行计划和报告摘要耦合
- 让 executor 根据 `engine*Summary` 再次推理 worker 行为
- 让 reporting 绕过 `LoweringPlan` 与局部 report，自行重建领域结论

所以，这一轮真正要收口的是：

- **engine summary 字段的“主消费者”与“禁止消费边界”**

## 3. 有哪些可解决的路径

### 路径 A：允许多层自由消费，谁需要谁读

做法：

- `registry / planner / executor / reporting` 都可以按需读取 `engine*Summary`
- planner output 也允许把这些字段直接带下去
- 由各层自行决定这些字段是做展示、校验，还是做执行判断

优点：

- 最灵活
- 短期实现阻力最小
- 遇到新需求时不用先回到 contract 层改边界

缺点：

- 最容易失控
- 同一个字段会被不同层重复解释
- engine 语义会重新渗回 planner / executor
- 后续一旦字段含义变化，会造成多层联动回滚

判断：

- 不推荐

### 路径 B：严格主消费者模型，summary 主要服务 reporting / diagnostics，planner 与 executor 保持通用

做法：

- 为每个 engine 字段明确一个主消费者
- registry 只读静态 subject 级别信息
- planner 继续只消费通用 `goal / matrix / pipeline / stage graph` 信息
- executor 继续只消费 `stage.kind + selection + stagePlan`，不读 engine summary 做语义决策
- reporting / report assembler 成为 bucket-level engine summary 的主要消费者

优点：

- 最符合前面已经收紧好的主线边界
- 不会让 planner / executor 再次回到“解释 engine 语义”的角色
- 方便后续 public surface 统一围绕 `report.json.engineProofSummary`
- 最利于长期维护

缺点：

- 需要对“哪些层禁止依赖哪些字段”写得更死
- 某些看起来方便的捷径需要主动拒绝

判断：

- 推荐

### 路径 C：由 planner 做中心化归一，其他层主要消费 planner 归一后的 engine context

做法：

- planner 读取 `engineProofProfile` 与 bucket-level engine summary
- planner 输出一份归一化 engine context 给 executor / reporting
- executor 与 reporting 尽量不再直接看 manifest 中的 engine summary

优点：

- 看起来“中心化”
- executor 侧逻辑可能更薄
- reporting 能得到统一格式的 planner 产出

缺点：

- planner 会被迫理解 analysis / generated / runtime 的产物细节
- planner 和 artifact chain 耦合过深
- 一旦 engine 证据形态变化，planner 也要跟着改
- 会让规划层和结果层边界重新糊掉

判断：

- 不推荐

## 4. 你的方案和理由

我推荐：

- **路径 B：严格主消费者模型，summary 主要服务 reporting / diagnostics，planner 与 executor 保持通用**

原因有六个：

1. 它最符合前面关于 `Emitter`、`LoweringPlan`、`subject + matrix + artifact-chain` 的边界收敛方向
2. 它能防止 engine summary 从“结果摘要”退化回“执行真源”
3. 它把 engine-specific 逻辑尽量关在 stage worker 与 reporting 之间，不污染 registry / planner / executor 主路径
4. 它让 public surface 可以稳定围绕 `report.json.engineProofSummary`
5. 它不会迫使 planner output 膨胀成领域上下文快照
6. 它为后续移动平台扩张保留了通用执行骨架

如果按这条路线推进，我建议先冻结六条规则。

### 规则 1：`engineProofProfile` 的主消费者是 registry，reporting 可以次级引用，planner 不应依赖它做执行分流

推荐分工：

- 主消费者：
  - `registry`
- 次级消费者：
  - `reporting`
- 不应成为主消费者：
  - `planner`
  - `executor`

原因是：

- `engineProofProfile` 本质上是 subject 的静态身份说明
- 它回答“这个 subject 计划证明什么”
- 它不回答“这次到底该跑哪条 graph”

换句话说：

- planner 选路仍然应基于：
  - `defaultGoal`
  - `defaultMatrix`
  - `pipelineId`
  - `supportedGoals`
  - `executionPipelines`
- 而不是基于：
  - `focusArea = service-call/object-handle/lifecycle-callback`

### 规则 2：`engineContractSummary / engineEmissionSummary / engineObservationSummary` 的主消费者都是 reporting / diagnostics，不是 planner / executor

推荐分工：

- 主消费者：
  - `report assembler`
  - 人工排障流程
- 次级消费者：
  - 对应阶段之后的局部校验逻辑
- 不应成为主消费者：
  - `planner`
  - `executor`

这三类 summary 的职责分别是：

- `engineContractSummary`
  - 告诉 reporting：analysis 最终冻结了哪一类 engine contract
- `engineEmissionSummary`
  - 告诉 reporting：generated 阶段实际发射了哪类 bridge / glue
- `engineObservationSummary`
  - 告诉 reporting：runtime 阶段留下了哪些观测证据入口

但它们都不应该替代真实执行输入：

- analysis / generated worker 的真实真源仍然是 `LoweringPlan`
- runtime worker 的真实真源仍然是 generated output 与自身 stage request

### 规则 3：executor 只允许按 `stage.kind + selection.executionContext + stagePlan` 调度，不允许读取 engine summary 做行为分流

这条规则必须说死。

executor 可以依赖：

- `stage.kind`
  - 例如：
    - `generated-engine-proof`
    - `runtime-engine-observe`
    - `runtime-engine-trace-compare`
- `selection.executionContext`
- `validationIntent`
- `paths`

executor 不应依赖：

- `engineProofProfile.focusArea`
- `engineContractSummary.resolvedCapabilityIds`
- `engineEmissionSummary.emittedCapabilityIds`
- `engineObservationSummary.evidenceItems`

原因非常直接：

- executor 的职责是 orchestration，不是 engine 语义理解
- 一旦它开始分支读取这些字段，就会重新变成“半个 engine runtime planner”

### 规则 4：planner output 不复制任何 bucket-level engine summary，只保留通用 selection 与路径

第一批明确不建议 planner output 新增：

- `engineProofProfile`
- `engineContractSummary`
- `engineEmissionSummary`
- `engineObservationSummary`

planner output 继续只保留：

- `request`
- `selection`
- `artifactsRoot`
- `stagePlan`

原因是：

- planner output 的职责是“可执行计划”
- 不是“报告预备上下文”

reporting 如果需要 engine 信息，更合理的方式是：

- 通过 `stageResults[].manifestPath / reportPaths / primaryEvidencePaths`
- 回到真实 bucket manifest 与局部 report 读取

而不是让 planner 预先复制一份摘要带下去。

### 规则 5：`engineProofSummary` 只能由 matrix report assembler 生成，且它是 engine proof 对外公开的唯一高层入口

推荐分工：

- 生成者：
  - `matrix report assembler`
- 主要消费者：
  - `TUI`
  - `subject summary assembler`
  - `session summary bridge`
  - 后续 public/reporting surface

它的输入来源应固定为：

1. `subject.manifest.json.engineProofProfile`
2. `analysis.manifest.json.engineContractSummary`
3. `generated.manifest.json.engineEmissionSummary`
4. `runtime.manifest.json.engineObservationSummary`
5. 局部 `*.report.json`
   - `contract-validate.report.json`
   - `engine-observe.report.json`
   - `trace-compare.report.json`

它不应被：

- stage worker 直接写
- planner 预先生成
- executor 手工拼接

这样才能保证：

- `engineProofSummary` 是真正从既有产物链汇总出来的结果摘要
- 不是任何中间层顺手写出来的“半成品结论”

### 规则 6：对外展示只看 `report.json.engineProofSummary`，不让 public surface 直接跨层依赖 bucket summary

推荐对外消费顺序固定为：

- `TUI / watch / public command`
  - 先看 `report.json.engineProofSummary`
- `subject summary / session summary`
  - 继续通过 matrix `report.json` 聚合
- 深入排障时
  - 再沿着 `engineProofSummary.localReportPaths`
  - `stageResults[].primaryEvidencePaths`
  - `artifactResults[].manifestPath`
    回到下层

不推荐的方式是：

- TUI 直接去读 `runtime.manifest.json.engineObservationSummary`
- session summary 直接跨过 matrix report 去聚合 bucket 级 engine 数据

因为这样会破坏：

- `matrix -> subject -> session`

这条已经固定好的 reporting 真源链路。

## 4.1 推荐的主消费者映射

为了让边界更可执行，我建议把第一批映射先固定成下面这样：

- `subject.manifest.json.engineProofProfile`
  - 主消费者：`registry`
  - 次级消费者：`reporting`
- `analysis.manifest.json.engineContractSummary`
  - 主消费者：`report assembler`
  - 次级消费者：人工 diagnostics
- `generated.manifest.json.engineEmissionSummary`
  - 主消费者：`report assembler`
  - 次级消费者：人工 diagnostics
- `runtime.manifest.json.engineObservationSummary`
  - 主消费者：`report assembler`
  - 次级消费者：人工 diagnostics
- `report.json.engineProofSummary`
  - 主消费者：`TUI / summary / public reporting surface`

对应地，第一批明确禁止的依赖是：

- `planner` 依赖 bucket-level engine summary 做 graph 选择
- `executor` 依赖 engine summary 做 worker 分流
- stage worker 依赖前一层 summary 替代 canonical inputs
- `subject/session summary` 直接跨过 matrix report 聚合 engine bucket 数据

## 5. 审查下这个问题本身，是否合理，以及是否有其他方式让这个方案在整体框架中更合理地得以解决

我认为这个问题本身非常合理，而且它就是 `20260407-14` 之后必须紧跟着补上的一环。

因为上一轮只是解决了：

- engine summary 放在哪里

但还没有解决：

- 这些 summary 到底归谁所有
- 谁能用它们做什么

如果只定字段，不定消费者，最后的结果通常不是“灵活”，而是：

- 所有人都能读
- 所有人都在解释
- 最后没人能说清哪个对象才是真源

不过，这个问题也需要被问得更精确。更合理的问法不是：

- “engine 字段怎样让所有层都更方便？”

而应该是：

- “如何让 engine summary 只在真正该消费它们的层发挥作用，同时不把 engine 语义重新灌回 planner / executor？”

为了让这个方案在整体框架里更合理，我建议再补三条审查结论。

### A. 这一步的核心目标不是“提高复用率”，而是“阻止边界回流”

很多时候最危险的不是字段没人用，而是字段太方便，导致每层都顺手用一点。

所以这一轮真正应该防的是：

- planner 回流成 engine 解释器
- executor 回流成 engine 调度语义层
- reporting 回流成 pseudo-lowering-plan

### B. 这一步比字段命名更重要

像下面这些问题：

- `engineObservationSummary` 以后要不要改名
- `engineProofSummary` 将来要不要抽成通用 `proofSummary`

都不是当前最高优先级。

当前真正高影响的是：

- consumer boundary 先稳住

命名和抽象层级可以后续再优化，但如果消费者先失控，后面再改名也救不回来。

### C. 这一步的成功标准是“执行层继续通用化”

真正成功的标志应该是：

1. `registry` 仍然偏静态发现与分类
2. `planner` 仍然偏 graph / goal / matrix / reuse 决策
3. `executor` 仍然偏 orchestration 与 stage dispatch
4. `reporting` 仍然偏结果汇总与公开展示

只要这四点没有被 engine summary 重新打乱，这一轮方案就在整体框架里是合理的。

所以，这一轮的最终结论是：

- **推荐采用严格主消费者模型**
- **推荐把 bucket-level engine summary 主要收敛到 reporting / diagnostics 使用**
- **推荐禁止 planner / executor 依赖 engine summary 做执行分流**
- **推荐把 `report.json.engineProofSummary` 固定为 engine proof 对外公开的唯一高层入口**
