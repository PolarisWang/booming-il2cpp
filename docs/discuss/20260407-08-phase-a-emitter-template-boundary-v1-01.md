# Phase A Emitter Template Boundary v1.01

Date: 2026-04-07
Status: design-discuss

## 1. 问题点

当前 `Phase A` 在 emitter/template boundary 上的核心问题有四个：

1. `NativeReferenceProofEmitter` 还在自己识别 lowering，而不是只消费 lowering
2. 模板选择还和当前 `PlanKind`、具体模板文件名、proof 形状耦合在一起
3. emitter 还保留了大量“语义发现式 validation”
4. `TemplateKind` 是否应该立即进入正式 contract 还没有定论

## 2. 问题的具体描述

从当前代码看，`NativeReferenceProofEmitter` 还没有真正退回一个纯后端：

- 它现在不仅负责：
  - `GetTemplateForPlan(...)`
  - `CreateTemplateModel(...)`
  - 模板渲染
- 它还负责：
  - `CreateLoweringPlan(...)`
  - `TryCreatePInvokeDllImportMinimalLoweringPlan(...)`
  - `TryCreateReflectionQueryMinimalLoweringPlan(...)`
  - `IsStaticCallCtorGetterEntryPointShape(...)`
  - 多个 `Validate...Shape(...)`

也就是说，当前 emitter 还在做三层事情：

1. 识别这到底是哪种 lowering family
2. 组装 lowering plan
3. 选择模板并渲染

这和前面几份讨论已经形成冲突：

- `SemanticWorld`
  - 应负责 capability hints
- `Linker`
  - 应负责 managed dependency policy
- `CodeGen`
  - 应负责 lowering family 与 role contract
- `Emitter`
  - 应只负责 template/backend rendering

此外，当前模板边界还有两个现实问题：

1. `GetTemplateForPlan(...)` 直接用 `PlanKind -> Template` 做硬映射
2. `CreateTemplateModel(...)` 还是平铺式把大量 nullable 字段灌进 `ScriptObject`

这意味着目前还没有回答清楚：

- emitter 的正式输入到底是 `LoweringFamily` 还是 `TemplateKind`
- emitter 允许保留哪些 validation
- 什么样的差异属于 template/backend 差异，什么不属于

## 3. 有哪些可解决的路径

### 路径 A：维持当前 hybrid emitter，只把边界写进文档

做法：

- 保留 emitter 内部的 lowering 识别逻辑
- 保留 `Is...Shape(...)`、`Validate...Shape(...)`
- 保留 `PlanKind -> Template` 的现状
- 只是把“哪些是过渡行为”文档化

优点：

- 改动最小
- 对现有四条 proof 脊柱影响最小
- 短期最省事

缺点：

- emitter 仍然不是纯后端
- 会和我们已经冻结的 `CapabilityHints / LoweringFamily / RoleContract` 持续冲突
- 后续平台适配时，平台差异很容易再次被塞回 emitter 的识别逻辑

判断：

- 不推荐

### 路径 B：以 `LoweringFamily` 作为 emitter 的正式输入，template 选择仍留在 emitter 内部映射；emitter 只允许做 plan 完整性与 backend invariant validation

做法：

- `CodeGen`
  - 负责选择 `LoweringFamily`
  - 负责输出 `LoweringPlan Role Contract`
- `Emitter`
  - 只消费 lowering plan
  - 通过内部映射选择模板
  - 只保留两类 validation：
    - plan completeness validation
    - backend invariant validation
- `TemplateKind`
  - 先保留为内部概念
  - 暂不进入正式 contract

这里的 backend invariant validation，指的是这类问题：

- 缺少必须 role
- token/symbol/literal 不完整
- 当前 backend 明确不支持某个 lowering family
- 当前 host 不满足 template 所要求的硬前提

它不再允许做这类事情：

- 从 `typed-il` 重新判断 family
- 从 instruction index 反推 method role
- 重新验证 ctor/getter/instance-call 的业务形状是否成立

优点：

- 和前面几份主线文档完全对齐
- `Emitter` 真正退回后端位置
- 不会过早把 `TemplateKind` 工程化成一个新 contract 字段
- 对当前 `Phase A` 最稳

缺点：

- 需要明确区分“plan 完整性校验”和“语义发现式校验”
- family 与 template 的映射仍然先留在 emitter 内部
- 如果很快出现同一 family 下的多模板变体，就要再讨论 `TemplateKind`

判断：

- 推荐

### 路径 C：现在就把 `TemplateKind` 升级为正式 contract，由 `CodeGen` 同时决定 `LoweringFamily + TemplateKind`，emitter 完全只做渲染

做法：

- 在 lowering plan 里引入正式 `TemplateKind`
- `CodeGen` 明确输出：
  - `LoweringFamily`
  - `TemplateKind`
- `Emitter`
  - 完全按 `TemplateKind` 找模板
  - 不再保留任何 template 选择逻辑

优点：

- 边界最明确
- 如果后续很快出现多 backend、多平台模板变体，会更好扩
- emitter 最接近纯 renderer

缺点：

- 对 `Phase A` 来说偏重
- 当前还只有单 backend、每个 family 基本一对一对应单模板
- 现在就把 `TemplateKind` 升成正式 contract，容易提前冻结不必要的实现细节

判断：

- 现在不建议作为主路径
- 但适合作为后续演进方向保留

## 4. 我的方案和理由

我推荐：

- **路径 B：`LoweringFamily` 作为 emitter 的正式输入，template 选择暂时留在 emitter 内部映射，emitter 只保留 plan 完整性与 backend invariant validation**

理由有五个：

1. 它最符合 `Phase A` 的节奏
   - 先把职责收清楚
   - 不急着把所有内部机制都抬成正式 contract

2. 它和现有 repo 的成熟度匹配
   - 当前只有 `native-reference` 一个明确 backend
   - 当前 family 和 template 基本还是一对一关系
   - 现在就正式引入 `TemplateKind`，收益还不够大

3. 它能明确切掉 emitter 的越权范围
   - 不再做 lowering 识别
   - 不再做 shape 推理
   - 不再从 `typed-il` 中重新发现语义

4. 它对后续移动平台适配更稳
   - 平台差异优先落在 template/backend 实现
   - 而不是回流到 lowering family 或 emitter shape detection

5. 它为后续升级保留了空间
   - 当同一 family 出现多个 template/backend 变体时
   - 再把 `TemplateKind` 升成正式 contract，时机更合理

如果按这个方案推进，我建议先冻结三条规则：

### 规则 1：emitter 的正式输入是 lowering plan，不是 typed-il + metadata + methodPointers 的混合原料

这意味着长期方向应该是：

- `CodeGen`
  - 负责 `CreateLoweringPlan(...)`
- `Emitter`
  - 只消费已成型 plan

也就是说，当前 `NativeReferenceProofEmitter.CreateLoweringPlan(...)` 这类逻辑应视为过渡实现，而不是长期职责。

### 规则 2：emitter 允许的 validation 必须白名单化

推荐允许的 validation：

- plan 必填字段是否完整
- role contract 是否齐全
- token/symbol/literal/output contract 是否完整
- 当前 backend/template 是否支持该 family
- 当前宿主是否满足硬约束
  - 例如 `pinvoke` 当前模板要求 Windows host

不再允许的 validation：

- 重新验证 entry method 的 IL 形状
- 重新通过 instruction 序列推断 ctor/getter/instance-call 角色
- 重新发现 reflection query 的语义结构

### 规则 3：`TemplateKind` 暂时不进正式 contract，但要保留升级闸门

建议当前只冻结一个升级条件：

- 只要出现以下任一情况，就考虑把 `TemplateKind` 正式升格：
  - 同一 `LoweringFamily` 需要多个模板变体
  - 多 backend 开始并存
  - 同一 family 的不同宿主模板差异不再只是局部实现差异

换句话说：

- 现在不升
- 但不是永远不升

## 5. 审查下这个问题本身，是否合理，以及是否有其他方式让这个方案在整体框架中更合理地得以解决

我认为这个问题本身是合理的，而且时机也对。  
因为如果不把 emitter boundary 讲清楚，前面已经沉淀下来的：

- capability hints
- lowering family
- lowering-plan role contract

都会在最后一跳重新退化成“emitter 自己再猜一遍”。

但这个问题也要避免问偏。  
更准确的问法不应该是：

- “模板系统要不要现在一次性设计完整？”

而应该是：

- “`Phase A` 里 emitter 允许做什么，不允许做什么；什么东西值得现在就进正式 contract，什么先保留为内部实现细节？”

我对这个问题本身的反审查结论是：

- 合理
- 但不应该被扩张成一个完整模板平台设计问题

让这个方案在整体框架里更合理地解决，我建议三条补充原则：

### A. 先冻结 emitter 的职责负面清单

也就是先明确：

- emitter 不负责 lowering 识别
- emitter 不负责语义发现
- emitter 不负责重建 role contract

这比先定义一个复杂模板体系更重要。

### B. 把跨平台差异优先解释为 template/backend 差异，而不是 family 差异

这条对移动平台尤其关键。  
比如未来 Android / iOS / macOS 的某些输出模板不同，优先应理解为：

- 同一 family 的不同 backend/template 实现

而不是：

- 因为平台不同，所以 family 重新分叉

否则 `LoweringFamily` 很快会被平台维度污染。

### C. 等第二个真实变体出现，再正式引入 `TemplateKind`

这会让整体框架更稳。  
因为 contract 最怕“提前冻结错误维度”，而 `TemplateKind` 目前还更像实现细节，不像已经被验证过的主线边界。

所以这份文档真正要解决的不是“模板文件怎么命名”，而是：

- 让 emitter 从“解释器”退回“后端”
- 同时不给当前主线引入过早的 contract 负担
