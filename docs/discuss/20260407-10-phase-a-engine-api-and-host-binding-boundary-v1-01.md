# Phase A Engine API And Host-Binding Boundary v1.01

Date: 2026-04-07
Status: design-discuss

## 1. 问题点

当前主线设计已经隐含覆盖了三类东西：

- `managed external dependency`
- `runtime/helper/icall`
- `host/platform binding`

但“IL2CPP 后续嵌入自研引擎，并由引擎向 C# 暴露 API”这件事，还没有被正式冻结成单独的话题。

这会带来一个很实际的问题：

- 我们现在虽然已经讨论了 `runtime roles`、`host bindings`、`emitter/backend boundary`
- 但还没有明确回答：
  - 引擎暴露给 C# 的接口，到底属于哪一层
  - 它和普通 `managed dependency`、通用 `runtime helper`、平台 `host glue` 的边界分别是什么
  - 后续 Android / iOS 适配时，引擎接口差异应该落在哪一层

换句话说，这个问题不是“有没有考虑到引擎接入”这么简单，而是：

- **要不要把 engine-facing API contract 作为主线的一等边界显式冻结下来。**

## 2. 问题的具体描述

如果后续 IL2CPP 不是只做独立 proof，而是要进入自研引擎，那么实际会出现至少四类接口：

1. 引擎向 C# 暴露的类型和方法
   - 例如 `EngineObject`、`Component`、`Transform`、`Log`、`Asset`、`Scene`
2. C# 反过来回调引擎的入口
   - 例如生命周期、调度、事件、主线程派发、资源加载
3. 托管对象和引擎对象之间的身份/句柄/生命周期关系
   - 例如 object handle、native pointer、ownership、GC rooting
4. 宿主和平台层的差异
   - 例如 Windows/macOS/Android/iOS 上的加载方式、ABI、主线程规则、打包与运行入口

这四类东西目前只被“部分覆盖”：

- 前面的文档已经覆盖了：
  - `LoweringPlan` 可以承载 runtime/host 相关 binding
  - `Emitter` 应该退回 backend，不负责重新发现语义
  - `Linker` 不应该把 host/runtime 问题混进 `LinkedWorld.Dependencies`
- 但还没有正式覆盖：
  - `engine binding` 是否应单列
  - engine API surface 的 owner 是谁
  - engine API 是不是只是 `interop` 的一种别名
  - engine object model / callback / threading / ownership 应该写进哪类 contract

如果这个边界不先讲清楚，后面很容易出现三种坏结果：

1. 把 engine API 错塞进 `LinkedWorld.Dependencies`
2. 把 engine 语义退化成零散的 `host-import` 或 `pinvoke` 特例
3. 把 Android / iOS 上真正的 engine 接入差异重新污染到 `Linker`、`CodeGen family` 甚至 `Emitter`

所以，当前设计对这个场景的真实状态是：

- **已经部分包含**
- **但还没有显式冻结为正式 contract**

## 3. 有哪些可解决的路径

### 路径 A：继续把 engine API 混在 `runtime roles / host bindings` 里，不新增正式概念

做法：

- 不引入 `engine binding` 这个正式概念
- 引擎暴露给 C# 的接口，一律按：
  - `runtime role`
  - 或 `host-import`
  - 或少量 `family extension`
  处理

优点：

- 改动最小
- 最贴近当前已有的 `interop` 与 `host binding` 讨论
- 短期内能让少量 engine proof 先跑起来

缺点：

- 引擎接口和通用 runtime/helper 混层
- object handle、生命周期、回调、线程归属这些核心问题会没有正式位置
- 后面一旦 engine API 面扩大，很容易退化成“到处都是特例 binding”
- 不利于后续 Android / iOS 的差异治理

判断：

- 不推荐作为主路径

### 路径 B：把 `engine binding` 升成一等 contract 边界，但仍留在 `Phase A` 现有主线里推进

做法：

- 显式区分四层概念：
  - `managed external dependency`
  - `runtime/helper/icall`
  - `engine binding`
  - `host/platform binding`
- `Linker`
  - 继续只负责 `managed external dependency`
- `CodeGen / LoweringPlan`
  - 正式表达 `engine binding` 与 `host binding`
- `Emitter`
  - 只消费 plan，不负责重新识别 engine 语义

优点：

- 最符合前面已经收敛出来的主线边界
- engine API 不会再被错误混进 `LinkedWorld.Dependencies`
- 能把“通用 runtime 能力”和“项目自定义 engine 能力”分层
- 对后续 Windows/macOS/Android/iOS 的 engine 接入差异更稳
- 既能覆盖未来引擎嵌入，又不需要现在就新开一条独立工程主线

缺点：

- 需要先冻结一轮 vocabulary
- 要额外定义 engine-facing role/binding 的最小骨架
- 如果收不住，很容易过度设计成大型 binding 平台

判断：

- 推荐

### 路径 C：暂时不正式讨论 engine binding，等当前 reference spine 更稳定后再补

做法：

- 当前继续只按 `managed dependency / runtime role / host binding` 推进
- 把 engine 接入视为未来专项
- 等 `Phase A` 或 `Phase B` 更多基础能力稳定后，再新开 contract

优点：

- 当前讨论焦点最集中
- 不会立刻把主线扩大

缺点：

- engine 接入如果是确定目标，这其实是在推迟必答题
- 很容易导致后面已有 contract 被二次返工
- 一旦先出现若干 engine proof，早期形状就可能变成事实标准

判断：

- 可以作为节奏上的短暂拖后
- 但不适合作为正式主路径

## 4. 你的方案和理由

我推荐：

- **路径 B：把 `engine binding` 升成一等 contract 边界，但仍作为 `Phase A` contract 收口的一部分推进**

理由有五个：

1. 它最符合我们现在已经确定的主线方向
   - `SemanticWorld` 提供语义切片与 capability hints
   - `Linker` 只承载 retained world 与 managed external dependency policy
   - `CodeGen / LoweringPlan` 负责表达真正要被后端消费的 binding
   - `Emitter` 退回 backend

2. 它能把“自研引擎接口”从“通用 runtime helper”里分离出来
   - `console-write-line-string` 这类是通用 runtime/helper
   - `engine-object-handle-resolve`、`engine-main-thread-dispatch`、`engine-log-write` 这类是 engine binding

3. 它比“全部退化成 interop/pinvoke”更稳
   - `pinvoke` 更像调用机制
   - engine API 更像宿主语义模型的一部分
   - 两者不能长期混成一个概念

4. 它对移动平台适配更合理
   - engine 语义最好尽量稳定
   - 平台差异尽量落在 host/backend/packaging
   - 而不是让 Android/iOS 差异去污染 engine role 本身

5. 它不会过早把问题做重
   - 现在只需要先冻结 contract 语言
   - 不需要马上做完整 binding generator 或大型 engine SDK 框架

如果按这个方案推进，我建议先冻结四条规则：

### 规则 1：`LinkedWorld.Dependencies` 仍然不承载 engine binding

`LinkedWorld.Dependencies` 继续只表示：

- 真实的 managed external dependency

例如：

- `System.Object::.ctor()`
- `System.Console::WriteLine(System.String)`
- 某个实际参与闭包的 managed facade assembly 成员

但下面这些不应直接塞回 `LinkedWorld.Dependencies`：

- `engine-log-write`
- `engine-object-handle-resolve`
- `engine-main-thread-dispatch`
- `engine-component-query`

因为它们不是“外部 managed 依赖”，而是“引擎绑定需求”。

### 规则 2：`engine binding` 不应退化成泛化的 `runtime role`

建议在 contract 语义上把三类东西显式区分：

- `runtime roles`
  - 通用 helper / icall / runtime support
- `engine bindings`
  - 自研引擎向 C# 暴露的类型、方法、回调、句柄、生命周期桥接
- `host bindings`
  - 平台加载、ABI、线程硬约束、打包和运行时宿主条件

这样后续看一个 lowering plan 时，我们能清楚区分：

- 这是通用 IL2CPP runtime 需要的东西
- 这是 engine integration 需要的东西
- 这是平台/宿主条件

### 规则 3：engine API 的首轮正式落点应在 `LoweringPlan`，而不是在 `Emitter` 现场推理

建议 engine 相关需求由 `CodeGen` 明确写进 plan，例如首轮可以允许出现这类 binding/role：

- `engine-log-write`
- `engine-object-handle-create`
- `engine-object-handle-resolve`
- `engine-component-get`
- `engine-scene-load`
- `engine-main-thread-dispatch`
- `engine-callback-register`

`Emitter` 只做两件事：

- 校验这些 binding 在当前 backend/host 下是否完整
- 渲染对应模板

而不再允许它根据 `typed-il` 或特定调用序列反推：

- 这是不是一个 engine callback
- 这是不是一个 engine object 访问
- 这是不是一个生命周期入口

### 规则 4：跨平台差异优先落在 `host binding / backend template`，不要回流污染 engine 语义

例如同样一个能力：

- `engine-log-write`

在 Windows/macOS/Android/iOS 上可能：

- C++ glue 不同
- main-thread 限制不同
- loader/bootstrap 入口不同
- 打包与导出形式不同

但这些优先应理解为：

- `host binding` 或 backend/template 差异

而不是：

- Windows 有一套 engine log family
- Android 又分叉一套 engine log family

否则后面 `LoweringFamily` 和 engine vocabulary 会被平台维度快速污染。

从执行路径看，我建议这个话题后续按下面顺序继续细化：

1. 先冻结 terminology
   - `engine binding`
   - `engine type`
   - `engine method`
   - `engine callback`
   - `engine object handle`
   - `engine thread affinity`
2. 再冻结 contract 落点
   - 哪些属于 subject roles
   - 哪些属于 runtime roles
   - 哪些属于 engine bindings
   - 哪些属于 host bindings
3. 再定义首轮 proof 子集
   - 一个最小 engine log/console bridge
   - 一个最小 engine object handle roundtrip
   - 一个最小 engine callback/lifecycle entry
4. 最后才讨论工程化
   - 是不是需要 facade assembly
   - 是不是需要 binding generator
   - 是不是需要 engine sdk manifest

## 5. 审查下这个问题本身，是否合理，以及是否有其他方式让这个方案在整体框架中更合理地得以解决

我认为这个问题本身非常合理，而且不是边角问题，而是主线必须提前回答的问题。

原因很直接：

- 如果 IL2CPP 的最终落点之一就是“嵌入自研引擎，给 C# 提供运行与 API 暴露能力”
- 那么 engine binding 其实不是后期附加 feature
- 而是会反过来影响：
  - contract 怎么冻结
  - interop 怎么分类
  - runtime/host/engine 三层边界怎么切
  - 移动平台适配时哪些变化算平台问题，哪些算引擎问题

不过，这个问题也需要被重新提问得更精准。

更精准的问法不应该只是：

- “有没有包含引擎接口暴露这个情况？”

而应该拆成：

1. 自研引擎暴露给 C# 的 API，属于 `managed dependency`、`runtime role`、`engine binding`、`host binding` 中的哪一类？
2. 哪些 engine 语义需要正式写进 lowering plan？
3. 哪些差异属于平台宿主，哪些属于 engine contract 本身？

为了让这个方案在整体框架里更合理，我建议再补三条约束：

### A. 不要把 engine binding 直接等同于 `interop`

`interop` 只是机制视角。  
而 engine binding 更像：

- 语义视角
- 宿主模型视角
- 生命周期与对象模型视角

如果二者直接画等号，后续 contract 会越来越像“所有东西都是 pinvoke 的变体”。

### B. 不要让每个业务模块各自定义零散 engine API 暴露方式

更合理的长期方向应该是：

- 主线先冻结统一的 engine binding contract
- 后续再决定是：
  - facade assembly
  - generated wrapper
  - native import bridge
  - 混合策略

这样不会让不同 subsystem 把 binding 方式做散。

### C. 先讨论“最小 engine 子集”，不要一开始就讨论完整引擎 SDK

`Phase A` 现在只需要回答：

- engine binding 在主线里的位置
- 最小 contract 骨架
- 最小 proof 如何建

不需要现在就回答：

- 全部引擎模块怎么暴露
- 编辑器 API 怎么暴露
- 资源系统/渲染系统/物理系统如何一次性覆盖

所以，对你这个问题的最终结论是：

- **现有设计已经部分覆盖了“IL2CPP 进入自研引擎并向 C# 暴露接口”的场景**
- **但还不够，必须把它补成正式的 `engine binding` 边界**
- **而且这件事应该在 `Phase A` 期间先把 contract 冻住，不要拖到移动平台阶段再回头补**
