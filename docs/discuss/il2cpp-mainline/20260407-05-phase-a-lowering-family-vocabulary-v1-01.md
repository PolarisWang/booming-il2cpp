# Phase A Lowering Family Vocabulary v1.01

Date: 2026-04-07
Status: design-discuss

## 1. 问题

在当前代码里，`NativeReferenceLoweringPlanArtifact.PlanKind` 已经存在，但它的命名体系并不统一：

- `constructorThenInstanceCall`
  - 描述的是指令/调用顺序
- `staticCallCtorGetter`
  - 描述的是局部动作序列
- `reflectionQueryMinimal`
  - 描述的是语义领域 + scope
- `pinvokeDllImportMinimal`
  - 描述的是 interop 机制 + scope

这会带来三个直接问题：

1. 同一层字段混合了不同维度
2. 名字仍然过于贴近当前 proof 形状，而不是 sample-agnostic 的主线术语
3. `CodeGen` 和 `Emitter` 的边界很难稳定，因为我们还没定义清楚“能力提示”和“lowering 选择”是不是同一件事

所以这一轮的核心问题不是“给四个 plan kind 换个更好听的名字”，而是：

- `Phase A` 应该用怎样的 vocabulary 表达当前 lowering families
- 这个 vocabulary 要不要和 capability hints 分层
- 它如何为后续 `Phase B` 继续扩语义留接口

## 2. 这一轮要回答什么

这一轮只解决 `Phase A` 的 vocabulary 问题，不扩新语义家族。

目标只有三个：

1. 让当前四类 proof 切片有统一、稳定、样例无关的术语
2. 让 `SemanticWorld -> CodeGen -> Emitter` 的接口可以围绕这个 vocabulary 稳定下来
3. 让未来新增 lowering family 时，不需要重新发明命名规则

## 3. 三种方案

## 3.1 方案 A：继续保留单层 `PlanKind`，只把当前四个名字改干净

### 方案描述

- 仍然只保留一个主字段，例如 `PlanKind`
- 不额外引入 capability set / template kind
- 只是把现有四个名字从 proof-first 风格，改成更语义化的名字

例如：

- `constructorThenInstanceCall`
  - 改成 `captured-instance-message-minimal`
- `staticCallCtorGetter`
  - 改成 `static-forwarder-getter-minimal`
- `reflectionQueryMinimal`
  - 改成 `reflection-query-closed-type-minimal`
- `pinvokeDllImportMinimal`
  - 改成 `pinvoke-direct-call-minimal`

### 优点

- 改动最小
- 对现有 artifact contract 冲击最小
- 便于快速把当前 proof-first 标签收口成 sample-agnostic 名称

### 缺点

- 仍然把“能力集合”和“lowering 选择”混在同一个词里
- 后续一旦一个 capability set 可以映射到多个 lowering family，就会开始摇摆
- `Emitter` 将来如果有多模板变体，也没有自然落点

### 判断

- 可用，但不推荐作为长期主线 vocabulary

它适合短期减轻命名混乱，但还不够支撑 `Phase A` 后续 contract 冻结。

## 3.2 方案 B：分成两层 vocabulary，`CapabilityHints + LoweringFamily`，必要时再挂 `TemplateKind`

### 方案描述

把命名体系拆成两层，必要时保留第三层模板变体：

- `CapabilityHints`
  - 表达“当前 method/world 里有什么语义能力与运行需求”
- `LoweringFamily`
  - 表达“针对当前 backend，选择了哪一类 lowering 路径”
- `TemplateKind`
  - 只在确实需要区分 emitter 模板时出现

一句话说：

- capability 负责“它是什么”
- lowering family 负责“我们决定怎么降”
- template kind 负责“最后用哪种模板渲染”

### 优点

- 与上一份边界文档完全对齐
- `SemanticWorld` 可以只负责 capability hints，不越权决定 lowering
- `CodeGen` 可以明确承担 lowering family 选择
- `Emitter` 可以只消费 family/template，不再反推语义
- 更利于后续 `Phase B` 继续扩语义而不打乱命名体系

### 缺点

- 相比方案 A，会多出一层术语
- 需要纪律性：不能再把 capability hint 和 lowering family 写成同义词
- 首轮设计时要先约束命名规则，否则会很快长歪

### 判断

- 推荐

这是当前最稳的方案。  
它既避免过度设计，又能把“语义能力”和“后端 lowering 决策”清楚拆开。

## 3.3 方案 C：完全放弃 family 枚举，只保留 capability tags，运行时动态组装 lowering

### 方案描述

- 不定义 `LoweringFamily`
- 只保留一组 capability/requirement tags
- `CodeGen` 或 `Emitter` 根据 tags 动态决定模板、助手函数和 plan 内容

### 优点

- 理论上最通用
- 最接近“未来 full semantic engine + generalized lowering” 的终局

### 缺点

- 这不是 `Phase A`
- 当前四类 proof 还远没到“纯 capability 组合即可自动导出 lowering”的阶段
- 会把当前需要冻结的路径重新拉回抽象不落地
- `Emitter` 很容易重新长回一个组合式 shape detector

### 判断

- 明确不建议

这条路可以作为远期方向思考，但不适合作为当前主线 vocabulary 的落地方案。

## 4. 推荐结论

推荐采用：

- **方案 B：`CapabilityHints + LoweringFamily` 两层 vocabulary，必要时再挂 `TemplateKind`**

推荐原因很直接：

- `SemanticWorld` 负责 capability hints
- `CodeGen` 负责 selecting lowering family
- `Emitter` 负责 consuming lowering family / template kind

这样每一层都能说清楚自己在“解释语义”还是“选择实现路线”。

## 5. 推荐的命名规则

## 5.1 `CapabilityHint` 命名规则

建议使用动词前缀，保持语义导向：

- `uses-*`
  - 表达语义形状或结构性使用
- `requires-*`
  - 表达运行时、helper、icall、host glue 需求

例如：

- `uses-instance-construction`
- `uses-instance-field-state`
- `uses-field-getter`
- `uses-closed-generic-static-call`
- `requires-string-concat-pair`
- `requires-console-string-output`
- `requires-reflection-query`
- `requires-imported-call`

## 5.2 `LoweringFamily` 命名规则

建议采用三段式语义名：

- `<domain>.<pattern>.<scope>`

规则建议如下：

- `domain`
  - 表达大类，例如 `managed-object`、`managed-generic`、`reflection`、`interop`
- `pattern`
  - 表达当前 family 的核心 lowering 模式
- `scope`
  - 当前阶段建议保留 `minimal`
  - 明确说明这只是 `Phase A` / 当前 proof 切片的收口，而不是完整语义承诺

同时要求：

- 不出现 sample 名
- 不直接写 opcode 序列
- 不把模板名混进 family 名
- 不把 backend 细节写进 family 名，除非 family 本身就是 backend-specific

## 5.3 `TemplateKind` 命名规则

如果后续需要把 emitter 模板显式建模，建议使用：

- `<backend>.<family>`

例如：

- `native-reference.managed-object.captured-state-instance-message`
- `native-reference.reflection.closed-type-query`

但在 `Phase A` 当前阶段，可以暂时不把 `TemplateKind` 单独冻成正式字段，只先把它作为后续预留概念。

## 6. 推荐的首轮 vocabulary 骨架

## 6.1 当前 `constructorThenInstanceCall`

它表达的核心不是“先 ctor 再 instance call”，而是：

- 构造对象
- 把字符串字面量捕获到实例字段
- 通过实例方法读取字段并拼接字符串
- 最终输出到 console

建议拆成：

- capability hints
  - `uses-instance-construction`
  - `uses-instance-field-state`
  - `uses-virtual-instance-call`
  - `requires-string-concat-pair`
  - `requires-console-string-output`
- lowering family
  - `managed-object.captured-state-instance-message.minimal`

## 6.2 当前 `staticCallCtorGetter`

它表达的核心不是“static call + ctor + getter”这个动作顺序，而是：

- 调用闭合泛型的静态 forwarder
- 构造对象并捕获状态
- 通过 getter 读回字段
- 最终输出到 console

建议拆成：

- capability hints
  - `uses-closed-generic-static-call`
  - `uses-instance-construction`
  - `uses-field-getter`
  - `uses-instance-field-state`
  - `requires-console-string-output`
- lowering family
  - `managed-generic.static-forwarder-captured-getter.minimal`

## 6.3 当前 `reflectionQueryMinimal`

它表达的核心是：

- 在当前最小支持切片内
- 对 closed type 的 field / property / method / parameter / generic type definition 做 reflection query
- 最终输出规范化结果

建议拆成：

- capability hints
  - `requires-reflection-query`
  - `requires-type-handle-query`
  - `requires-field-metadata-query`
  - `requires-property-metadata-query`
  - `requires-method-metadata-query`
  - `requires-generic-definition-query`
  - `requires-console-string-output`
- lowering family
  - `reflection.closed-type-query.minimal`

## 6.4 当前 `pinvokeDllImportMinimal`

它表达的核心是：

- 调用 imported static method
- 依赖 import module / entrypoint metadata
- 完成最小 direct-call interop proof

建议拆成：

- capability hints
  - `requires-imported-call`
  - `requires-import-metadata`
  - `requires-native-import-resolution`
- lowering family
  - `interop.pinvoke-direct-call.minimal`

## 7. 对当前 artifact contract 的直接建议

如果沿推荐方案推进，当前 `LoweringPlan` 层建议按下面的顺序收口：

1. 先保留现有 `PlanKind` 字段
2. 但从语义上把它视为 `LoweringFamily`
3. 在后续 contract 轮次里，再讨论是否显式新增：
   - `CapabilityHints`
   - `TemplateKind`

也就是说，短期可以不急着改字段名，但必须先改我们对这个字段的理解。

## 8. 这份 vocabulary 对 roadmap 的意义

这一步一旦冻结，后面 `Phase A` 的主线会更顺：

1. `SemanticWorld`
   - 负责产出 capability hints
2. `Linker`
   - 负责 retained/dependency policy
3. `CodeGen`
   - 负责 capability set 到 lowering family 的映射
4. `Emitter`
   - 负责按 lowering family/template 渲染

这样后面讨论：

- dependency policy
- lowering plan role contract
- emitter template boundary

都会有共同语言，不会再混用 proof 名、样例名和动作顺序名。

## 9. 当前建议

如果沿这条线继续细化，下一份最自然的文档建议是二选一：

1. `Phase A lowering-plan role contract`
   - 每一种 lowering family 需要哪些显式 role
   - 例如 entry / ctor / getter / instance / import / field / literal / expected-output
2. `Phase A dependency policy contract`
   - capability hints 如何映射成 retained/dependency reasons

如果只能先选一个，我建议先写：

- **`Phase A lowering-plan role contract`**

因为 vocabulary 一旦定下来，下一步最需要补齐的就是：

- `CodeGen` 究竟要把哪些“角色化输入”显式写进 plan
- `Emitter` 才能真正停止从 typed-il 里反推语义
