# Phase A Engine-Binding Lowering-Plan Contract v1.01

Date: 2026-04-07
Status: design-discuss

## 1. 问题点

上一轮我们已经确认：

- 自研引擎向 C# 暴露 API 这件事，不能继续只作为隐含场景存在
- `engine binding` 应该成为正式 contract 边界

那么下一步必须回答的问题就变成了：

- **`engine binding` 在 `LoweringPlan` 里到底应该怎么落。**

更具体地说，要回答四个点：

1. `engine binding` 是不是要在 plan 里有独立结构
2. 引擎 facade 的 managed 类型/方法，与真正的 engine native binding，边界怎么切
3. object handle / lifecycle / callback / thread affinity 这些语义，写进哪一层
4. 平台差异到底落在 `engine binding`、`host binding`，还是继续退回 `runtime role`

如果这一层不先冻结，前一轮“把 `engine binding` 提成正式边界”的决定，其实还没有真正落地。

## 2. 问题的具体描述

当前已有的 lowering-plan 讨论，正式收口到的是：

- `CommonHeader`
- `SubjectRoles`
- `RuntimeRoles`
- `LiteralBindings`
- `OutputContract`
- `FamilyExtension`

这套骨架对已有 proof 是够用的，因为当前主要还是：

- managed object proof
- generic proof
- reflection proof
- pinvoke/interp proof

但一旦进入自研引擎接入，就会出现一个新的复杂度来源：

### 第一层：managed facade 本身

如果引擎对 C# 暴露 API，通常会出现一层 managed facade，例如：

- `EngineObject`
- `Component`
- `Transform`
- `Engine.Log`
- `SceneManager`

这些东西从程序集视角看，仍然是 managed subject。

也就是说：

- 它们的 type/method/field identity 仍然应该出现在 `SubjectRoles`
- 如果它们来自外部 managed facade assembly，它们也可能出现在 `LinkedWorld.Dependencies`

### 第二层：facade 背后的 engine 语义

但 facade 后面真正需要 native backend 去绑定的，不是“一个普通 managed method”，而是：

- engine service call
- engine object handle create/resolve
- engine callback register
- engine lifecycle dispatch
- engine thread lane
- engine ownership / lifetime bridge

这些东西如果只塞进 `RuntimeRoles` 或 `FamilyExtension`，会有三个问题：

1. `RuntimeRoles` 会被污染成“通用 runtime + 项目引擎能力”的混合桶
2. `FamilyExtension` 会重新退化成一个“什么都能塞”的大口袋
3. `Emitter` 和 backend 又会被迫重新猜：
   - 这是一个普通 helper
   - 还是 engine callback
   - 还是 object handle 规则

### 第三层：平台/宿主差异

引擎能力真正进入 Windows/macOS/Android/iOS 后，还会出现另一层差异：

- 主线程模型不同
- loader/bootstrap 入口不同
- 打包、ABI、导出方式不同
- callback / message pump / run loop 规则不同

这层差异如果不小心，也会重新污染 `engine binding` 本身。

所以这一轮真正要回答的，不是“要不要支持引擎”，而是：

- `LoweringPlan` 应该怎样同时表达：
  - managed facade subject
  - engine semantic binding
  - host/platform binding

而且这三者要能清楚分层。

## 3. 有哪些可解决的路径

### 路径 A：继续把 engine 信息塞进 `RuntimeRoles + FamilyExtension`

做法：

- 不新增新的正式 section
- facade type/method 继续进 `SubjectRoles`
- engine native 需求：
  - 能落进 `RuntimeRoles` 的就落进去
  - 不能落进去的就塞进 `FamilyExtension`

例如可能会出现这种表达：

- `RuntimeRoles`
  - `engine-log-write`
  - `engine-object-handle-resolve`
  - `engine-callback-register`
- `FamilyExtension`
  - `ownership-policy`
  - `lifetime-policy`
  - `thread-affinity`

优点：

- 改动最小
- 和当前 `Phase A` 既有 plan skeleton 最接近
- 短期最容易做出第一批 engine proof

缺点：

- `RuntimeRoles` 的语义会变脏
- `FamilyExtension` 会重新膨胀成逃逸口
- object handle / callback / lifecycle 这些关键语义没有正式位置
- 后续 Android / iOS 接入时，平台差异很容易再倒灌回来

判断：

- 不推荐

### 路径 B：在 lowering plan 中正式引入 `EngineBindings` 与 `HostBindings` 两个结构化层

做法：

- 保留现有整体方向：
  - `CommonHeader`
  - `SubjectRoles`
  - `RuntimeRoles`
  - `LiteralBindings`
  - `OutputContract`
  - `FamilyExtension`
- 在中间显式加入：
  - `EngineBindings`
  - `HostBindings`

也就是说，plan 结构升级为：

1. `CommonHeader`
2. `SubjectRoles`
3. `RuntimeRoles`
4. `EngineBindings`
5. `HostBindings`
6. `LiteralBindings`
7. `OutputContract`
8. `FamilyExtension`

其中：

- `SubjectRoles`
  - 继续表达 managed 侧的类型/字段/方法身份
- `RuntimeRoles`
  - 只表达通用 runtime/helper/icall
- `EngineBindings`
  - 表达 engine-facing semantic contract
- `HostBindings`
  - 表达平台/宿主硬条件与落地方式

优点：

- 与上一轮 engine 边界讨论完全一致
- engine 语义有正式归宿，不再挤占 `RuntimeRoles`
- host/platform 差异也有正式归宿，不再污染 engine binding 本身
- 对后续多平台接入、自研引擎扩面都更稳

缺点：

- 比路径 A 多一轮 upfront contract 设计
- 需要补一套 naming / field 规则
- 现有 lowering-plan 骨架要做一次增量修订

判断：

- 推荐

### 路径 C：每类 engine family 单独定义专属 plan payload

做法：

- 不再追求统一的 engine binding section
- 直接为不同 engine family 定义单独 payload，例如：
  - `EngineLogWritePlan`
  - `EngineObjectHandleRoundtripPlan`
  - `EngineLifecycleCallbackPlan`

优点：

- 每类场景字段最直观
- 类型约束最强
- 对单个 proof 很容易落地

缺点：

- 太早进入“engine SDK 工程化”思维
- 很容易让 family/payload 数量爆炸
- 会重复定义公共字段
- 不利于 `Phase A` 先冻结统一 contract 语言

判断：

- 暂不建议作为主路径

## 4. 你的方案和理由

我推荐：

- **路径 B：在 lowering plan 中正式引入 `EngineBindings` 与 `HostBindings` 两个结构化层**

理由有六个：

1. 它和前一轮边界结论完全一致
   - 既然我们已经明确 `engine binding` 不等于 `runtime role`
   - 那就应该在 contract 结构上把它真正分出来

2. 它能把 facade 身份和 native binding 关系切开
   - facade type/method 留在 `SubjectRoles`
   - facade 背后的 engine 能力进入 `EngineBindings`
   - 不再把两种信息写成一种东西

3. 它能给 object handle / lifecycle / callback 一个正式归宿
   - 这些东西不是简单 helper
   - 也不是单纯平台约束
   - 它们就是 engine semantic binding

4. 它能让 host 差异有单独落点
   - Android / iOS / macOS / Windows 的差异优先落在 `HostBindings`
   - 而不是让 `EngineBindings` 本身不断分叉

5. 它对 `Emitter` 最友好
   - `Emitter` 不需要重建 engine 语义
   - 只需要消费：
     - subject roles
     - runtime roles
     - engine bindings
     - host bindings

6. 它仍然属于 `Phase A` 能承受的收口范围
   - 这是 contract 加层
   - 不是现在就做完整 binding generator / engine manifest 系统

如果按这个方案推进，我建议先冻结四条结构规则。

### 规则 1：managed facade 成员仍然属于 `SubjectRoles`

例如：

- `Engine.Log::Write(string)`
- `EngineObject::.ctor(...)`
- `Component::GetTransform()`

这类成员如果存在于 managed facade assembly 中，那么它们依然首先是：

- `SubjectRoles`

必要时它们也可能参与：

- `LinkedWorld.Dependencies`

但这不等于它们背后的 native engine 语义已经被表达完了。

也就是说：

- facade 的“managed 身份”与背后的“engine binding”必须拆开

### 规则 2：`EngineBindings` 只表达 engine semantic binding，不表达通用 runtime helper

建议 `EngineBindings` 的最小字段至少包含：

- `BindingId`
- `BindingKind`
  - 例如：
    - `service-call`
    - `object-handle-create`
    - `object-handle-resolve`
    - `callback-register`
    - `lifecycle-dispatch`
    - `data-bridge`
- `EngineCapabilityId`
- `FacadeSubjectRoleId`
- 可选 `TargetSubjectRoleId`
- 可选 `OwnershipPolicy`
- 可选 `LifetimePolicy`
- 可选 `ExecutionLane`

这里最关键的是：

- `FacadeSubjectRoleId`
  - 把 `EngineBindings` 和 `SubjectRoles` 接上
- `OwnershipPolicy / LifetimePolicy / ExecutionLane`
  - 给 engine object model、callback、线程模型一个正式位置

### 规则 3：`HostBindings` 只表达平台/宿主实现与硬条件

建议 `HostBindings` 的最小字段至少包含：

- `BindingId`
- `BindingKind`
  - 例如：
    - `thread-contract`
    - `loader-contract`
    - `bootstrap-contract`
    - `abi-contract`
    - `artifact-observe-contract`
- `HostCapabilityId`
- 可选 `EngineBindingId`
- 可选 `RequiredHostFamily`
- 可选 `ThreadContract`
- 可选 `AbiContract`

这里的边界是：

- `EngineBindings`
  - 说“我需要 engine main thread callback”
- `HostBindings`
  - 说“当前 Windows/macOS/Android/iOS 宿主如何实现并保证这件事”

换句话说：

- engine 语义负责“要什么”
- host 语义负责“在哪儿、怎么落”

### 规则 4：`FamilyExtension` 只用于少量补充，不能重新吞回 engine/host 主体语义

例如未来某个 engine family 可能需要少量特殊字段：

- callback priority
- marshaling mode
- observer channel name

这些可以暂时进入 `FamilyExtension`。

但下面这些不应该再藏进去：

- object ownership model
- handle lifetime
- thread lane
- host bootstrap contract

因为这些已经是正式边界，不该再退回 extension。

## 4.1 推荐的 plan 骨架

在当前 lowering-plan role contract 基础上，我建议把骨架更新成：

- `CommonHeader`
- `SubjectRoles`
- `RuntimeRoles`
- `EngineBindings`
- `HostBindings`
- `LiteralBindings`
- `OutputContract`
- `FamilyExtension`

其中每层回答不同问题：

- `SubjectRoles`
  - 这次 lowering 涉及哪些 managed subject
- `RuntimeRoles`
  - 这次 lowering 依赖哪些通用 runtime/helper/icall
- `EngineBindings`
  - 这次 lowering 依赖哪些 engine semantic capability
- `HostBindings`
  - 当前宿主怎样满足这些 engine/runtime 需求
- `LiteralBindings`
  - 模板需要哪些值
- `OutputContract`
  - proof 期望什么输出

## 4.2 三个最小示例

### 示例 A：`engine.log-write.minimal`

建议最少包含：

- `SubjectRoles`
  - `entry-method`
  - `engine-log-facade-method`
- `RuntimeRoles`
  - `string-marshaling-utf16`
- `EngineBindings`
  - `engine-log-write`
    - `BindingKind = service-call`
    - `EngineCapabilityId = engine.log.write`
    - `FacadeSubjectRoleId = engine-log-facade-method`
- `HostBindings`
  - `engine-log-observer`
    - `BindingKind = artifact-observe-contract`
    - `HostCapabilityId = engine.log.observe`
- `LiteralBindings`
  - `message-literal`

这里表达的是：

- managed 侧调的是 facade method
- 真正需要 backend 接上的，是 engine log service
- proof 怎么观察这个结果，由 host binding 决定

### 示例 B：`engine.object-handle.roundtrip.minimal`

建议最少包含：

- `SubjectRoles`
  - `entry-method`
  - `engine-object-wrapper-type`
  - `engine-object-handle-field`
  - `engine-object-name-method`
- `EngineBindings`
  - `engine-object-handle-create`
    - `BindingKind = object-handle-create`
    - `EngineCapabilityId = engine.object.handle.create`
    - `FacadeSubjectRoleId = engine-object-wrapper-type`
    - `OwnershipPolicy = shared-handle`
    - `LifetimePolicy = persistent`
  - `engine-object-handle-resolve`
    - `BindingKind = object-handle-resolve`
    - `EngineCapabilityId = engine.object.handle.resolve`
    - `FacadeSubjectRoleId = engine-object-name-method`
    - `OwnershipPolicy = shared-handle`
    - `LifetimePolicy = persistent`

这里表达的是：

- wrapper type 是 managed subject
- handle create/resolve 是 engine semantic binding
- ownership/lifetime 是 binding 元数据，不应再藏在 extension

### 示例 C：`engine.lifecycle.callback.minimal`

建议最少包含：

- `SubjectRoles`
  - `entry-method`
  - `engine-update-callback-method`
- `EngineBindings`
  - `engine-callback-register`
    - `BindingKind = callback-register`
    - `EngineCapabilityId = engine.callback.register`
    - `FacadeSubjectRoleId = engine-update-callback-method`
    - `ExecutionLane = engine-main-thread`
  - `engine-lifecycle-dispatch`
    - `BindingKind = lifecycle-dispatch`
    - `EngineCapabilityId = engine.lifecycle.update`
    - `FacadeSubjectRoleId = engine-update-callback-method`
    - `ExecutionLane = engine-main-thread`
- `HostBindings`
  - `engine-main-thread-contract`
    - `BindingKind = thread-contract`
    - `HostCapabilityId = engine.main-thread.dispatch`
    - `EngineBindingId = engine-lifecycle-dispatch`
    - `ThreadContract = required`

这里表达的是：

- callback 的业务语义属于 engine binding
- “当前宿主如何保证 main thread 回调成立”属于 host binding

## 5. 审查下这个问题本身，是否合理，以及是否有其他方式让这个方案在整体框架中更合理地得以解决

我认为这个问题本身非常合理，而且它就是前一轮边界文档之后最自然的下一问。

因为前一轮已经回答了：

- engine binding 要不要作为正式边界存在

那这一轮就必须回答：

- 既然它是正式边界，那么在 lowering plan 里具体放哪儿、长什么样

否则上一轮结论就还是概念性的，没有真正进入可执行 contract。

不过，这个问题也要防止问得过大。

更精确的问法不应该是：

- “引擎集成的全部 contract 怎么设计？”

而应该是：

- “在 `Phase A`，为了让 `CodeGen -> Emitter` 这条链真正可收口，`engine binding` 最小应该如何进入 lowering plan？”

这样问题规模就对了。

为了让这个方案在整体框架里更合理，我建议再加三条补充约束：

### A. 先冻结最小字段，不要一开始做大而全的 engine schema

当前最小必需的是：

- `BindingKind`
- `EngineCapabilityId`
- `FacadeSubjectRoleId`
- `OwnershipPolicy`
- `LifetimePolicy`
- `ExecutionLane`

不要现在就把：

- 完整序列化规则
- 完整 marshaling DSL
- 全量 engine sdk registry

一起做进来。

### B. 先围绕 2 到 3 个最小 engine proof 建模

建议首轮只围绕：

1. `engine.log-write.minimal`
2. `engine.object-handle.roundtrip.minimal`
3. `engine.lifecycle.callback.minimal`

这样能覆盖：

- service call
- object handle / lifetime
- callback / thread lane

也最能验证这套 contract 骨架是不是稳。

### C. 为后续更强类型化保留升级口，但现在不提前工程化

如果将来 engine family 很快膨胀，完全可以再演进到：

- 专门的 engine payload type
- engine manifest
- binding generator

但那应该是：

- contract 先站稳之后的下一阶段

而不是现在就提前把 `Phase A` 做重。

所以，这一轮的最终结论是：

- **`engine binding` 既然已经被提升成正式边界，就应该在 lowering plan 里拥有自己的结构化层**
- **推荐做法是：在现有 role contract 上增量加入 `EngineBindings` 与 `HostBindings`**
- **这样既不会污染 `RuntimeRoles`，也不会让平台差异重新反向污染 engine 语义**
