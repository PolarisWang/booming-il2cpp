# Phase A Lowering-Plan Role Contract v1.01

Date: 2026-04-07
Status: design-discuss

## 1. 问题

上一轮我们已经把 vocabulary 收敛到：

- `CapabilityHints`
- `LoweringFamily`
- 必要时预留 `TemplateKind`

接下来的关键问题是：

- `CodeGen` 选完 lowering family 之后，到底要把哪些信息显式写进 plan
- `Emitter` 应该消费怎样的 role contract，才能真正停止从 `typed-il` 里反推语义

从当前 `NativeReferenceLoweringPlanArtifact` 看，plan 已经存在，但还是一个“稀疏字段袋”：

- 公共头信息
  - `PlanKind`
  - `AssemblyName`
  - `EntrySubjectId`
  - `IncludeHeader`
  - `ProofFunctionName`
- 方法与符号
  - `EntrySymbol`
  - `ConstructorSymbol`
  - `InstanceMethodSymbol`
  - `EchoMethodSymbol`
  - `GetterSymbol`
- token
  - `ReferenceTypeToken`
  - `CapturedFieldToken`
  - `EntryMethodToken`
  - `ConstructorMethodToken`
  - `InstanceMethodToken`
  - `EchoMethodToken`
  - `GetterMethodToken`
- import / icall / literal / expected output
  - 混在同一个 record 里，由大量 nullable 字段拼起来

这带来三个直接问题：

1. 字段存在，但角色语义不够显式
2. family-specific 信息和公共 contract 混在一起
3. 每新增一个 lowering family，都倾向于继续往 record 里加新的 nullable 字段

所以这一轮的核心不是“再补几个字段”，而是：

- `Phase A` 应该把 lowering plan 冻结成怎样的 role contract

## 2. 这一轮要回答什么

这一轮不讨论新增语义家族，只讨论 contract 形状。

目标只有三个：

1. 让 `CodeGen` 输出的 plan 成为显式角色绑定，而不是模板参数散包
2. 让 `Emitter` 消费 plan 时，不再需要二次理解业务语义
3. 让后续新增 lowering family 时，优先是补 role binding，而不是继续扩 nullable bag

## 3. 三种方案

## 3.1 方案 A：继续沿用单 record 稀疏字段袋，只统一命名

### 方案描述

- 保留当前 `NativeReferenceLoweringPlanArtifact` 的总体形状
- 继续使用大量 nullable 字段
- 只把命名与分组文档化，例如：
  - 哪些字段属于 method roles
  - 哪些字段属于 type/field roles
  - 哪些字段属于 import/literal/output

### 优点

- 改动最小
- 对现有 emitter/template 最友好
- 短期就能把“字段语义”讲清楚

### 缺点

- 仍然是“字段名知道角色”，而不是“contract 显式表达角色”
- family 一多，record 会继续膨胀
- 很难阻止 `CodeGen` 和 `Emitter` 再次通过“有没有某字段”来隐式猜 family
- 对后续多个 backend/template 变体不够稳

### 判断

- 不推荐

这条路能暂时稳住现状，但它不会真正完成 `Phase A` 的 contract 收口。

## 3.2 方案 B：改成“公共头 + 结构化 role bindings + 少量 family extension”

### 方案描述

把 lowering plan 拆成三层：

1. 公共头
   - `LoweringFamily`
   - `AssemblyName`
   - `EntrySubjectId`
   - `IncludeHeader`
   - `ProofFunctionName`
   - 可选 `TemplateKind`
2. 结构化 role bindings
   - subject roles
   - runtime roles
   - literal bindings
   - output contract
3. 少量 family extension
   - 只有确实无法落在通用 roles 里的字段才进入 extension

其中 role bindings 建议是显式的“role -> binding”关系，而不是平铺成 `ConstructorSymbol` 这种专门字段。

### 优点

- 角色语义清楚，和上一轮 vocabulary 完全对齐
- `CodeGen` 的职责变成“选择 family + 填 role bindings”
- `Emitter` 的职责变成“按 family/template 读取既定 roles”
- 对新增 family 最友好：优先补 role，而不是补平铺字段
- 更适合以后把同一 family 适配到不同宿主/平台模板

### 缺点

- 比方案 A 更需要 upfront 设计
- 需要先约束 role naming 规则
- 过渡期需要接受现有模板和 plan model 之间有一次中间映射

### 判断

- 推荐

这条路最符合 `Phase A`：既把 contract 真正做显式，又不需要立刻把所有 family 拆成完全不同的 artifact 类型。

## 3.3 方案 C：每个 lowering family 单独定义一套专属 plan payload

### 方案描述

- 不再追求统一 plan record
- 每个 lowering family 都有独立 payload，例如：
  - `ManagedObjectCapturedStateInstanceMessagePlan`
  - `ManagedGenericStaticForwarderCapturedGetterPlan`
  - `ReflectionClosedTypeQueryPlan`
  - `InteropPInvokeDirectCallPlan`

### 优点

- 每个 family 的字段最直观
- 类型约束最强
- emitter/template 对应关系最直接

### 缺点

- 这更像 `Phase B` 甚至更后面的精细化工程化
- 当前 family 数量不多时看起来清楚，但后面会很快膨胀
- 公共字段与共用角色会被重复定义
- 不利于先冻结主线 contract 语言

### 判断

- 暂不建议作为 `Phase A` 主路径

它可以作为未来更强类型化的演进方向，但现在还太重。

## 4. 推荐结论

推荐采用：

- **方案 B：公共头 + 结构化 role bindings + 少量 family extension**

一句话定义：

- `LoweringFamily` 决定“这是哪类 lowering”
- `RoleBindings` 决定“这类 lowering 需要的角色分别绑定到谁”
- `Emitter` 只负责按 family/template 消费这些角色

## 5. 推荐的 role contract 骨架

## 5.1 公共头

建议每个 lowering plan 至少有一组稳定公共头：

- `LoweringFamily`
- `AssemblyName`
- `EntrySubjectId`
- `IncludeHeader`
- `ProofFunctionName`
- 可选 `TemplateKind`

这部分回答的是：

- 这份 plan 属于哪个 family
- 它要驱动哪个 proof 入口
- 后端需要哪个模板/头文件上下文

## 5.2 Subject Roles

建议把所有“来自 managed semantic world 的角色绑定”统一放到 subject roles 里。  
每个 binding 至少包含：

- `RoleId`
- `SubjectKind`
  - `type`
  - `field`
  - `property`
  - `method`
  - `parameter`
- `SubjectId`
- 可选 `Token`
- 可选 `Symbol`
- 可选 `DisplayName/Name`

例如当前 family 里已经隐式存在的 roles：

- `entry-method`
- `constructor-method`
- `instance-message-method`
- `echo-forwarder-method`
- `getter-method`
- `imported-method`
- `reference-type`
- `closed-type`
- `generic-type-definition`
- `captured-state-field`
- `queried-field`
- `queried-property`
- `queried-method`
- `queried-parameter`

这样做之后，`ConstructorSymbol`、`GetterSymbol`、`ClosedTypeSubjectId` 这种字段都不再是 contract 的第一表达，而只是 role binding 的不同属性。

## 5.3 Runtime Roles

建议把运行时解析需求单独建模，避免继续混进业务角色里。  
每个 runtime role 至少包含：

- `RoleId`
- `RuntimeKind`
  - `icall`
  - `helper`
  - `host-import`
- `Identifier`
- 可选附加 metadata

当前已存在的 runtime roles 例如：

- `console-write-line-string`
- `string-concat-pair`
- `native-import-resolution`

这层回答的是：

- emitter/template 在运行时还需要解析哪些 helper / icall / host binding

## 5.4 Literal Bindings

建议把字面量和常量输入单独归档：

- `RoleId`
- `Value`
- 可选 `ByteCount`
- 可选 `ValueKind`

当前已存在的 literal roles 例如：

- `constructor-argument-literal`
- `echo-argument-literal`
- `message-prefix-literal`
- `message-suffix-literal`
- `output-prefix-literal`
- `expected-output-literal`

这层回答的是：

- 哪些模板输入是“值”
- 哪些输入是“subject”

二者不要再混在一起。

## 5.5 Output Contract

建议把 proof 的输出预期单独建模，而不是继续作为零散字符串字段：

- `OutputMode`
  - 例如 `console-string`
- `ExpectedValue`
- 可选 `ExpectedByteCount`
- 可选 `ExitCodePolicy`

当前 `ReflectionLite` 和 `PInvokeLite` 已经明显需要这层 contract。

## 5.6 Family Extension

只有通用 roles 无法合理容纳的内容，才进入 extension。

例如当前 `reflection.closed-type-query.minimal` 可能需要的少量扩展：

- method parameter count
- queried parameter index

但原则上：

- 不要把所有 family-specific 字段都偷渡进 extension
- extension 只用于少量补充，不应取代 role bindings

## 6. 当前四类 family 的 role contract 轮廓

## 6.1 `managed-object.captured-state-instance-message.minimal`

建议最少包含：

- subject roles
  - `entry-method`
  - `constructor-method`
  - `instance-message-method`
  - `reference-type`
  - `captured-state-field`
- runtime roles
  - `console-write-line-string`
  - `string-concat-pair`
- literal bindings
  - `constructor-argument-literal`
  - `message-prefix-literal`
  - `message-suffix-literal`
- output contract
  - `console-string`

## 6.2 `managed-generic.static-forwarder-captured-getter.minimal`

建议最少包含：

- subject roles
  - `entry-method`
  - `echo-forwarder-method`
  - `constructor-method`
  - `getter-method`
  - `reference-type`
  - `captured-state-field`
- runtime roles
  - `console-write-line-string`
- literal bindings
  - `echo-argument-literal`
- output contract
  - `console-string`

## 6.3 `reflection.closed-type-query.minimal`

建议最少包含：

- subject roles
  - `entry-method`
  - `closed-type`
  - `generic-type-definition`
  - `queried-field`
  - `queried-property`
  - `queried-method`
  - `queried-parameter`
- runtime roles
  - `console-write-line-string`
- literal bindings
  - `expected-output-literal`
- output contract
  - `console-string`
- family extension
  - `queried-method-parameter-count`
  - `queried-parameter-index`

## 6.4 `interop.pinvoke-direct-call.minimal`

建议最少包含：

- subject roles
  - `entry-method`
  - `imported-method`
- runtime roles
  - `console-write-line-string`
  - `native-import-resolution`
- literal bindings
  - `output-prefix-literal`
- family extension
  - `import-module-name`
  - `import-entry-point-name`
  - `import-argument0`
  - `import-argument1`
  - `import-argument2`
- output contract
  - `console-string`

## 7. 对当前 contract 的迁移解释

如果按推荐方案推进，当前 artifact 可以这样迁移理解：

- `PlanKind`
  - 语义上升级为 `LoweringFamily`
- `EntrySymbol / ConstructorSymbol / GetterSymbol / ...`
  - 迁移到 `SubjectRoles[].Symbol`
- `ReferenceTypeToken / CapturedFieldToken / ...`
  - 迁移到 `SubjectRoles[].Token`
- `ConsoleWriteLineStringIcall / StringConcatPairIcall`
  - 迁移到 `RuntimeRoles`
- `ConstructorLiteral / EchoLiteral / MessagePrefixLiteral / ...`
  - 迁移到 `LiteralBindings`
- `ExpectedOutput / ExpectedOutputByteCount`
  - 迁移到 `OutputContract`
- `ImportModuleName / ImportEntryPointName / ImportArgument*`
  - 暂时可以进 `FamilyExtension`
  - 如果后续 interop family 继续扩张，再升级为正式 `HostBindings`

## 8. 为什么这对移动平台适配也重要

这份 contract 不只是为了当前 Windows proof 看起来更整齐。  
它对后续 Android / iOS / macOS 的意义也很直接：

- 当 backend/template 变化时
  - 最好变化的是 `TemplateKind` 或 emitter 实现
  - 而不是重新定义 plan 里“需要哪些角色”
- 当 host binding 变化时
  - 最好变化的是 runtime/host roles
  - 而不是重新把语义角色和宿主细节绑回一起

也就是说，这一步其实是在给后面的跨平台适配打 contract 地基。

## 9. 当前建议

如果沿这条线继续细化，下一份最自然的文档建议是二选一：

1. `Phase A dependency policy contract`
   - capability hints 如何映射成 retained/dependency reasons
2. `Phase A emitter template boundary`
   - template kind 是否需要进入正式 contract
   - emitter 允许保留哪些 defensive validation

如果只能先选一个，我建议先写：

- **`Phase A dependency policy contract`**

因为现在：

- vocabulary 已经有了
- lowering family 也有了
- role contract 也有骨架了

下一步最缺的是：

- `SemanticWorld / Linker` 到 `CodeGen` 之间，dependency reason 到底如何从 capability hints 规约出来
