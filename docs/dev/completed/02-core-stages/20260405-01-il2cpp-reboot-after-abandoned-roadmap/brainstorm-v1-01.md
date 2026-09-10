# IL2CPP-Like C# to C++ 重新 Brainstorm（废弃路线复盘）

Date: 2026-04-05 20:51:20 +08:00
Status: in_progress

## 本轮目标

这次 brainstorm 不是在旧 roadmap 上做局部修补，而是要完成两件事：

1. 把已废弃路线的全过程重新梳理清楚。
2. 在这个复盘基础上，提出新的计划入口，而不是沿用旧父级 roadmap 直接往下走。

## 输入来源

- 已废弃父任务 `20260329-01-il2cpp-like-cs-to-cpp`
- 其下历史文档：
  - `brainstorm-v1-01.md`
  - `design-v1-01.md`
  - `roadmap-v1-01.md`
  - `plan-2026-03-31-23-il2cpp-like-cs-to-cpp-implementation-roadmap.md`
  - `finding-2026-03-29-23-il2cpp-like-cs-to-cpp.md`
- 已派生但未启动的子任务骨架：
  - `20260404-03-roadmap-1-assembly-and-typed-il-ir`
  - `20260404-04-roadmap-3-runtime-core-v1`

## 废弃方案全过程梳理

### 1. 立项阶段：先定义终局，再定义阶段

旧任务起点是一个非常大的目标：构建一套 IL2CPP-like 的 `C#/.NET -> C++` 系统。

这一阶段确认了几项大方向：

- 采用 staged roadmap，而不是一次性追求完整实现
- 面向一般 .NET 项目，而不是 Unity-only 子集
- 采用 `IL-first` 而不是 source-first
- 采用完全自研 runtime，而不是直接站在 CoreCLR/Mono 内部实现上
- 从第一天就按跨平台架构思考

这一步的价值在于把终局边界拉得很清楚，但代价是从一开始就引入了很大的系统体量。

### 2. 架构展开阶段：形成大而完整的体系蓝图

旧任务随后形成了完整的双平面结构：

- 翻译流水线
- 运行时平台

并逐步收敛到显式中间层：

- `Metadata Graph`
- `Typed IL IR`
- `Runtime-Contract IR`

同时又把运行时、AOT、metadata、ABI、interop、PAL、GC 等一级问题全部纳入主设计。

这一步的好处是：

- 架构术语和模块边界越来越清楚

这一步的问题是：

- 设计越来越“全”，但执行入口越来越分散

### 3. 对标阶段：从官方 .NET runtime 和 Unity IL2CPP 吸收思路

旧任务系统性比较了三条路线：

- 官方 `.NET runtime`
- Unity `IL2CPP`
- 当前自研方案

最终形成的判断是：

- 长期统一 AOT 架构上，更应向 Unity `IL2CPP` 学习
- 宿主边界、BCL 工程化与平台成熟度方面，仍应吸收官方 `.NET runtime` 的经验
- 当前自研路线方向可行，但当时仍停留在“方向正确的蓝图”

这一阶段直接推动了编译系统骨架的收敛：

- `Contracts`
- `Driver`
- `Loader`
- `Semantic World`
- `Pipeline`
- `Analyzer`
- `CodeGen`
- `Metadata Writer`
- `Runtime`
- `Build`
- `Linker`

### 4. 技术栈与依赖讨论阶段：逐步冻结外围实现选型

旧任务在较长一段时间里对实现栈做了逐项确认，形成了如下历史结论：

- `System.Reflection.Metadata + PEReader` 作为前端主实现
- `Mono.Cecil` 只做测试/对照基线
- `Scriban` 作为 codegen 模板方案
- `BDWGC` 作为 `GC facade` 背后的 Phase 1 backend
- `NuGet.Frameworks` 作为 TFM/兼容性辅助库

同时还讨论了：

- logging/config/CLI 解析
- performance baseline 与比较对象
- 一些后来被判定为非关键的外围库

这一阶段的收获是技术栈更清晰，但也暴露出一个问题：

- 计划在还没有打穿第一条执行闭环之前，就已经开始消耗大量精力在外围依赖收敛上

### 5. 首条 vertical slice 冻结阶段：`HelloWorld`

旧任务后期最有价值的一步，是把第一条真正的 proof slice 冻结为 object-based `HelloWorld`。

它不再是“只打印字面量”的最小样例，而是明确要求证明：

- managed entry
- object allocation
- constructor invocation
- field store/load
- instance method call
- string narrow path
- `Console.WriteLine(string)`
- runtime bootstrap
- metadata registration
- generated-code execution

并进一步冻结了五个实现检查点：

1. `Loader / Typed IL IR`
2. `Linker / AOT manifest`
3. `Metadata Writer`
4. `Runtime bootstrap / minimum helpers`
5. `CodeGen first emission`

这是旧路线里最值得保留的成果之一，因为它把“第一证明点”从抽象 roadmap 重新拉回到了可验证样例。

### 6. 文档同步与子任务派生阶段

旧任务随后做了几件事：

- 把 design / roadmap / implementation plan 三份主文档全部同步到统一口径
- 从父级 roadmap 派生出两个 ready 状态的子任务骨架：
  - `Roadmap 1`
  - `Roadmap 3`

这一步说明旧任务已经接近“准备进入实现”的门槛。

但问题也恰恰出在这里：

- 旧父级 roadmap 同时承担了历史设计总览、当前执行入口、未来多阶段路线图和子任务派生母体四种职责
- 结果是它文档上越来越完整，执行上却缺少一个真正被重新压缩后的主线入口

## 旧方案的核心价值

以下结论不应随旧任务废弃而被一起丢掉：

- `IL-first`
- 闭世界 AOT
- 自研 runtime 长期方向
- `Metadata Graph / Typed IL IR / Runtime-Contract IR` 三层显式模型
- `Linker / Metadata Writer / CodeGen / Runtime` 分层
- `HelloWorld` 作为第一 proof slice 的定义
- `SRM + PEReader` 主实现、`Mono.Cecil` 测试基线、`Scriban` 模板、`BDWGC behind GC facade` 这些历史选型

## 旧方案暴露的问题

旧方案被废弃，不是因为它“完全错误”，而是因为它作为当前执行主线有几个明显问题：

### 问题 1：父级 roadmap 过重

它同时承载：

- 终局架构
- 阶段路线
- 当前入口
- 子任务派生逻辑

这会让它越来越像“总蓝图”，越来越不像“今天真正要从哪里开工”。

### 问题 2：执行主线不够单一

虽然旧路线已经冻结了 `HelloWorld`，但父级 roadmap 仍然保留了较强的“大模块并行展开”惯性。

结果就是：

- 文档上说要 proof-first
- 结构上却仍然容易把工作拆回 `Roadmap 1 / 2 / 3 / 4` 的平行推进

### 问题 3：子任务派生过早

`Roadmap 1` 和 `Roadmap 3` 子任务骨架已经派生出来，但在新的主线重组前，这种派生容易制造“已经可以开干”的错觉。

实际上，当父级主线本身需要重启时，这些 ready 子任务并不稳。

### 问题 4：外围讨论过早扩张

旧路线在某些阶段把过多精力消耗在：

- 第三方库确认
- logging/config/CLI 细节
- 远期 runtime 组件候选

这类话题对长期架构有帮助，但对第一条 proof slice 的打穿帮助有限。

## 废弃原因总结

旧任务被废弃，根本原因不是“方向错了”，而是：

- 旧路线已经积累出很多正确资产
- 但这些资产没有被重新组织成一个更窄、更硬、更适合直接执行的新入口

换句话说：

- 旧路线更像一套完成度很高的历史架构蓝图
- 它不再适合作为下一轮真正执行的唯一母文档

## 重新 brainstorm 的三个选项

### 方案 A：继续沿用旧父级 roadmap 往下做

优点：

- 不需要重建任何文档入口
- 既有 `Roadmap 1 / Roadmap 3` 子任务可以直接继续

缺点：

- 会把旧路线的体量、惯性和混合职责继续带下去
- 很容易再次回到“大模块视角”而不是 proof-first 视角

结论：

- 不推荐

### 方案 B：彻底推翻旧路线，从空白重新定义

优点：

- 最干净
- 不会再被旧文档结构牵引

缺点：

- 会丢掉大量已经确认过的高价值架构收敛结果
- 重复成本太高

结论：

- 不推荐

### 方案 C：保留稳定资产，但重建执行主线

核心思路：

- 把旧任务整体降级为“历史资产包”
- 新任务只继承已经证明有价值的稳定结论
- 重新围绕 proof-first 主线设计新的计划入口

优点：

- 既不浪费旧成果
- 又能真正把执行入口重新压缩

缺点：

- 需要先完成一次明确的“保留 / 重开 / 废弃”三分法

结论：

- 推荐

## 推荐的新计划方向

推荐采用方案 C，并把新的主线重新组织成下面五段。

当前已和用户对齐的顺序是：

1. 先做执行入口与工程骨架重整
2. 再进入 `HelloWorld proof-first`

用户进一步选择了“重量版”重整。

这里的“先重整骨架”不再只是轻量入口整理，而是要在进入 proof 之前，重新审视当前主 contract 与执行骨架是否足够稳定，至少覆盖：

- `typed-il-ir` artifact 边界
- `runtime ABI` 边界
- `codegen bridge` 边界
- `AOT manifest / metadata registration` 是否属于 Stage 1 契约冻结范围
- smoke 样例与 proof 样例的角色边界

但它仍然不等于原样恢复旧 `Roadmap 0`。本轮重量版重整的目标是“为第一条 proof 重新冻一套更可信的入口契约”，而不是再次平铺整套长期基础设施。

### 第 0 段：历史资产冻结包

目标：

- 明确哪些旧结论直接继承
- 哪些只是候选
- 哪些彻底退出当前主线

产物：

- 保留项列表
- 重开项列表
- 废弃项列表

### 第 1 段：重量版执行入口与契约重整包

目标：

- 不再让旧父级 roadmap 继续扮演当前执行入口
- 重新整理现有 contract / smoke / runtime skeleton 的角色边界
- 对当前 contract 主表面做一次显式重开和重新冻结
- 明确哪些现有骨架直接复用，哪些只作为历史底座，哪些需要替换掉当前入口

产物：

- 新的执行主线说明
- 现有骨架资产映射表
- Stage 1 contract reset 清单
- 重新冻结后的 artifact / ABI / bridge 边界
- `HelloWorld` 当前 smoke 样例与 proof 样例之间的差异表
- 不进入当前主线的话题清单

重量版 Stage 1 建议纳入的重开面：

- `typed-il-ir` 的最小稳定形状是否足以支撑 object-based `HelloWorld`
- `runtime ABI v0` 是否已经覆盖 bootstrap、对象分配、字段访问、异常边界和最小反射查询
- `codegen bridge v0` 是否应继续承载当前定义的高层 helper 集
- `AOT manifest`、`code registration`、`metadata registration` 是否应在 proof 之前一起冻结为第一批契约
- 现有 smoke 样例命名、角色和 gate 是否会干扰 proof 主线

当前已与用户确认：

- `AOT manifest`
- `code registration`
- `metadata registration`

这三项一并纳入重量版 Stage 1，不再延后到 `HelloWorld` proof 阶段之后再讨论。

重量版 Stage 1 明确不纳入：

- 新一轮第三方依赖扩张
- 多平台大规模验证矩阵
- `HelloWorld` 之外的新 feature slice
- 长期 runtime 远期能力的重新铺陈

### 第 2 段：`HelloWorld` 规格与执行包

目标：

- 在执行入口已经被压缩之后，再围绕第一条 proof slice 开工
- 把 object-based `HelloWorld` 重新固定为唯一第一证明点

产物：

- `HelloWorld` 样例规格
- 最小运行时落点
- 最小 metadata/code registration
- 最小 generated-code landing path

### 第 3 段：参考平台端到端证明

目标：

- 先打穿单参考平台
- 明确输出、退出码和无手工 rescue path 的证明标准

产物：

- 参考平台构建/run 证明
- snapshot/diff 基线
- 最小 smoke gate

### 第 4 段：证明后再拆 roadmap

目标：

- 只有在 `HelloWorld` 真正跑通后，才重新拆分后续 roadmap

这样可以避免：

- 在 proof 之前就过早分裂为多个平行子任务

## 本轮建议冻结

### 直接保留

- `IL-first`
- 闭世界 AOT
- 三层显式 IR
- `Linker / Metadata Writer / Runtime / CodeGen` 分层
- `HelloWorld` 作为第一条 proof slice

### 重新打开

- 旧 7 条 roadmap 是否仍是最佳主拆分
- `Roadmap 1 / Roadmap 3` 是否应该继续作为第一层子任务
- 第三方库讨论的优先级
- 多平台验证的具体进入时点

### 明确废弃

- 旧父级 roadmap 作为当前执行入口的地位
- 旧 `Roadmap 1 / Roadmap 3` ready 子任务骨架的有效性
- 在 proof slice 之前继续大规模扩展外围依赖讨论
- 把当前仓库里的 contract 文档直接视为无需复核的终局答案

## 本轮输出结论

本次重新 brainstorm 的核心结论是：

- 旧路线应整体作为历史资产保留
- 新计划不应直接继承旧父级 roadmap 的执行入口地位
- 用户当前选择的顺序是“先做重量版执行入口/契约重整，再进入 `HelloWorld` proof”
- 应改为“复盘旧方案全过程 -> 冻结保留项 -> 做一轮面向 proof 的重量版 contract reset -> 围绕 `HelloWorld` 建立 proof-first 主线 -> 证明后再拆 roadmap”
