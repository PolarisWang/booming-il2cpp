# Phase A Linker Diagnostics Contract v1.01

Date: 2026-04-08
Status: design-discuss

## 1. 问题点

到目前为止，`LinkedWorld` 的输出面基本已经收出三块：

- retained closure
- semantic-layer reasons
- stable `EmissionOrder`

但如果 `Diagnostics` 这一块还不冻结，整个 `Linker` contract 仍然有一个明显缺口：

1. `Linker` 发现的问题会继续以零散字符串、日志或异常的形式四处飘
2. `CodeGen` 和 `Emitter` 很容易被迫替 `Linker` 补做一部分 closure/dependency 排障
3. 同样的问题在不同阶段会被用不同术语重复报一遍

所以这一轮真正要回答的不是：

- “系统要不要报错”

而是：

- **`Linker` 这一层到底该对哪些问题负责诊断，以及这些诊断应如何成为正式 contract。**

## 2. 问题的具体描述

这里最关键的是先把三层问题切开。

### A. semantic understanding 问题

这类问题本质上属于：

- `Loader`
- `SemanticWorld`

例如：

- 输入 IL/metadata 不能被当前支持切片正确理解
- 某个语义切片尚未被当前 `SemanticWorld` 支持

这类问题不应伪装成 linker diagnostics。

### B. closure/dependency freezing 问题

这类问题才真正属于：

- `Linker`

例如：

- entry root 无法解析
- retained closure 某个 prerequisite 缺失
- 某个 semantic obligation 无法映射成 external managed dependency
- 某个 external managed dependency 解析失败
- `EmissionOrder` 无法稳定导出

### C. lowering/render/backend 问题

这类问题本质上属于：

- `CodeGen`
- `Emitter`

例如：

- 找不到合适的 `LoweringFamily`
- runtime role / host binding 无法满足
- backend/template 不支持当前 lowering
- lowering plan 缺 role、symbol、literal

如果这三层不切开，`Diagnostics` 很快就会重新变成一个大杂烩：

- 一部分在讲语义支持缺口
- 一部分在讲 managed dependency policy
- 一部分已经在讲 backend/template

这会直接把边界重新冲垮。

## 3. 有哪些可解决的路径

### 路径 A：`Linker` 不定义正式 diagnostics contract，继续靠异常、日志和下游兜底

做法：

- `Linker` 发现问题就抛异常或写文本日志
- 是否能继续往下走，由调用方自己判断
- `CodeGen` / `Emitter` 发现问题时再补报各自的错误

优点：

- 设计成本最低
- 最接近很多现有实现习惯

缺点：

- 没有稳定消费面
- 排障信息不可组合、不可比较
- 很容易出现同一个 closure/dependency 问题在多个阶段重复报错
- `LinkedWorld` 很难成为一个可解释 artifact

判断：

- 不推荐

### 路径 B：冻结“linker-scope structured diagnostics”，只覆盖 closure/dependency/order/invariant 问题

做法：

- `Linker` 只对自己边界内的问题产出结构化 diagnostics
- diagnostics 成为 `LinkedWorld` 正式输出的一部分
- 但明确不诊断：
  - lowering family 选择
  - runtime/host binding
  - template/backend 支持性

优点：

- 跟前面已经冻结的层次边界完全一致
- `LinkedWorld` 变成真正可解释的 semantic artifact
- 能支撑 deterministic reporting、测试断言和后续排障
- 不会把 `Linker` 重新做成“总错误中心”

缺点：

- 需要 upfront 收一个最小 diagnostics shape 和分类面
- 需要明确错误严重度与下游可消费性的关系

判断：

- 推荐

### 路径 C：让 `Linker` 预先汇总跨层 diagnostics，连 lowering/backend 风险一起报

做法：

- `Linker` 除了 closure/dependency 问题外，还尝试预测：
  - 哪些 lowering family 可能不支持
  - 哪些 host/backend 可能失败
  - 哪些 template 可能缺约束

优点：

- 表面上看起来“一次报全”
- 某些场景下对用户可能更省心

缺点：

- `Linker` 会再次越界侵入 `CodeGen/Emitter`
- 预测性 diagnostics 很容易和真实 lowering/backend 结论不一致
- 多 backend / 多 host 后会迅速失控

判断：

- 不推荐

## 4. 我的方案和理由

我推荐：

- **路径 B：冻结 linker-scope structured diagnostics。**

一句话定义：

- `Linker.Diagnostics` 只回答：
  - `Linker` 是否成功冻结了一个自洽、可排序、依赖可满足的 linked semantic world

它不回答：

- 这个 world 后面应该如何 lowering
- 哪个 backend/template 最终能不能生成

我更推荐它的原因有四个：

1. 它刚好补齐 `LinkedWorld` 的最后一块输出面
2. 它让 closure/dependency 级错误停留在最该负责的层
3. 它能避免 `CodeGen`/`Emitter` 重复替 `Linker` 报相同问题
4. 它对未来多 backend、多 host 更稳，因为 diagnostics 仍然停留在 semantic/linking 边界

## 5. `Linker` 该负责哪些 diagnostics

我建议当前先把 `Linker` 负责的诊断范围冻结成四类。

### A. `entry` 类

回答：

- entry root 本身是否可解析、可纳入当前 linked world

典型例子：

- 指定 entry method 不存在
- entry 指向的对象不属于当前可链接输入集

### B. `closure` 类

回答：

- retained closure 是否自洽

典型例子：

- 某 retained member 的 owner type 缺失
- 某 retained object 的 prerequisite 未满足
- closure 中出现内部不一致的 retained subject

### C. `dependency` 类

回答：

- semantic obligations 是否能被 external managed dependencies 满足

典型例子：

- 某个 `semantic.*` obligation 当前没有 dependency policy 映射
- 映射出的 external managed dependency 无法解析
- dependency kind 与实际 subject 不匹配

### D. `order/invariant` 类

回答：

- `LinkedWorld` 是否能稳定导出自洽的 `EmissionOrder`
- `LinkedWorld` 整体结构是否满足最小不变量

典型例子：

- canonical id 冲突导致顺序不稳定
- `MemberOrder` 无法满足 owner grouping 约束
- 某个 retained subject 同时以冲突身份出现

## 6. 明确不属于 `Linker` diagnostics 的东西

我建议把下面这些内容明确排除在当前 `Linker.Diagnostics` 之外：

- `LoweringFamily` 无法选择
- 当前 semantic slice 没有 codegen 支持路径
- runtime role 缺失
- host binding 缺失
- backend/template 不支持
- lowering plan role 不完整
- literal/symbol/token 在 lowering plan 中缺失

一句话说就是：

- **只要问题开始回答“怎么降”“怎么生成”“哪个 backend 支持”，它就已经不属于 `Linker` diagnostics。**

## 7. 推荐的最小 diagnostics shape

这一轮我建议 diagnostics shape 先冻结得很克制，只保留最必要字段：

- `Code`
- `Severity`
- `Category`
- `SubjectKind`
- `SubjectId`
- 可选 `RelatedSubjectIds[]`
- 可选 `ReasonRefs[]`
- `Message`

### A. `Code`

回答：

- 这是哪一类稳定可断言的 linker diagnostic

建议使用稳定 code，而不是只用 message 文本。

### B. `Severity`

当前基线我建议先只冻结两级：

- `error`
- `warning`

不建议现在就引入更多层级。

### C. `Category`

当前基线我建议先只冻结：

- `entry`
- `closure`
- `dependency`
- `order`
- `invariant`

### D. `SubjectKind / SubjectId`

回答：

- 这条 diagnostic 主要指向哪个对象

当前可以先按下面几类理解：

- `world`
- `entry`
- `type`
- `member`
- `dependency`

### E. `RelatedSubjectIds[]`

用于表达：

- 这条 diagnostic 还和哪些 retained/dependency/entry 对象有关

### F. `ReasonRefs[]`

我建议允许 diagnostics 引用：

- `RetainedReasons`
- `DependencyReasons`

而不是自己重新发明一套“为什么”的文字体系。

### G. `Message`

保留给人看的解释文本。  
但 contract 的稳定消费应该优先靠：

- `Code`
- `Severity`
- `Category`
- `SubjectId`

而不是靠 message 文案。

## 8. 推荐冻结的最小 diagnostics code family

我建议当前基线先冻结下面这批最小 code family：

- `linker.entry.unresolved-root`
- `linker.closure.missing-prerequisite`
- `linker.closure.inconsistent-retained-subject`
- `linker.dependency.policy-gap`
- `linker.dependency.unresolved-external`
- `linker.order.nondeterministic-order`
- `linker.invariant.invalid-linked-world`

这些 code 的定位分别是：

- `linker.entry.unresolved-root`
  - entry root 无法解析或无法纳入当前 world
- `linker.closure.missing-prerequisite`
  - closure 某个必要前置对象缺失
- `linker.closure.inconsistent-retained-subject`
  - retained subject 自身存在结构冲突
- `linker.dependency.policy-gap`
  - 某个 semantic obligation 当前没有 dependency policy 映射
- `linker.dependency.unresolved-external`
  - external managed dependency 解析失败
- `linker.order.nondeterministic-order`
  - 无法稳定导出 `EmissionOrder`
- `linker.invariant.invalid-linked-world`
  - world 级最小不变量被破坏

这里我刻意没有继续细分太多 code，原因和前面的 reason vocabulary 一样：

- `Phase A` 先要稳定 contract
- 不要过早长成一张巨大错误码表

## 9. 下游可消费性规则

我建议当前明确冻结一条很重要的规则：

- **只要 `Diagnostics` 中存在 `Severity = error`，该 `LinkedWorld` 就不应被 `CodeGen` 继续消费。**

相对地：

- `warning`
  - 可以继续消费
  - 但应进入 report/assertion surface

这条规则的价值很大，因为它把：

- “linker 只是报了点问题，但下游还能不能继续猜着跑”

这种模糊状态彻底切开了。

也就是说：

- `error` = `Linker` contract 未成立
- `warning` = `Linker` contract 成立，但存在需要暴露的 caveat

## 10. 用例子看这层边界

### 例子 A：缺失 managed external dependency

如果 `LinkedWorld` 中存在：

- `semantic.string-concat`

但当前输入集中无法解析出所需 external managed dependency，那么这里应是：

- `linker.dependency.unresolved-external`

而不是：

- `codegen.unsupported-concat-lowering`

因为问题还没走到 lowering，那时 `Linker` 就已经没能完成自己的 dependency freeze。

### 例子 B：无法选择 lowering family

如果 `LinkedWorld` 本身 closure、dependency、order 都正常，但：

- `CodeGen` 不知道该把这组 semantic objects 归到哪个 `LoweringFamily`

那这不是 linker diagnostic，而应属于：

- `CodeGen` diagnostics

### 例子 C：backend/template 不支持

如果 lowering plan 已形成，但当前 backend/template 不支持，那么这应属于：

- `Emitter` 或 backend invariant diagnostics

而不应提前回灌成：

- `linker.*`

## 11. 我的最终结论

这一轮我建议正式拍板下面几件事：

1. `Diagnostics` 是 `LinkedWorld` 正式 contract 的一部分，而不是实现细节日志
2. 当前 `Linker` 只对四类问题负责：
   - `entry`
   - `closure`
   - `dependency`
   - `order/invariant`
3. 当前 `Linker` 明确不负责任何 lowering/runtime/host/backend/template diagnostics
4. diagnostics 最小 shape 先冻结为：
   - `Code`
   - `Severity`
   - `Category`
   - `SubjectKind`
   - `SubjectId`
   - 可选 `RelatedSubjectIds[]`
   - 可选 `ReasonRefs[]`
   - `Message`
5. severity 当前先只冻结两级：
   - `error`
   - `warning`
6. 只要存在 `error` 级 linker diagnostic，`CodeGen` 就不应继续消费该 `LinkedWorld`

## 12. 这一步之后最值得继续讨论的点

到这里为止，`LinkedWorld` 的正式输出面已经基本接近闭合：

- retained closure
- reasons
- emission order
- diagnostics

所以接下来最自然的下一话题，我认为会从 “`Linker` 输出什么” 切到：

- **`CodeGen` 的输入消费契约**

也就是继续回答：

- `CodeGen` 在拿到一个 error-free `LinkedWorld` 之后，允许依赖哪些输入
- 哪些决策必须由 `CodeGen` 自己做
- 哪些东西绝不能再回头从 `typed-il` 或 proof-shape 里重建
