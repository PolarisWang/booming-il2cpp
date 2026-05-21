# 完整 IL2CPP 总方案与 iOS 热更 Brainstorm

## 背景

当前仓库主线已经明确收口为 `Phase A performance-first core mainline`，重点是：

- 把 `SemanticWorld -> Linker -> CodeGen -> native proof/runtime` 的核心链路做稳
- 用 `MainlineFeaturePack` 验证 `dispatch`、`generics/layout`、`arrays/boxing`、`delegates`、`exceptions` 等能力批次
- 把 `engine binding` 放在主线尾阶段

但你这次提出的目标更大，已经超出当前主线边界：

1. 做一套长期可用的高性能 `C# project -> C++ -> native binary` 总方案
2. 与引擎层接口稳定衔接
3. 在发布 IL2CPP binary 后，还能继续做 C# 热更
4. 热更方案要特别考虑 iOS

这意味着现在要同时回答两类问题：

- 还缺哪些系统级能力，才能从“proof-oriented mainline”走到“完整产品级 IL2CPP”
- 热更体系要不要并入主线，以及它和 iOS 约束如何兼容

## 关键矛盾

### 1. 性能最优 与 热更灵活性天然冲突

主线 IL2CPP 想追求的是 generated native 的 `C++` 性能上限，而热更天然要求：

- 动态装载新程序集
- 允许运行时补充元数据
- 允许 AOT 与非 AOT 代码共存

如果把两者混成一套执行模型，主线很容易被热更约束反向污染。

### 2. “完整 C# project” 不是“把单个 DLL 翻译得更好”

当前仓库已经有 `Loader`、`SemanticWorld`、`MetadataWriter`、`CodeGen`、`Pipeline` 等模块，但从契约和文档看，主线仍然更接近：

- proof subject
- 单输入程序集闭包
- 能力批次补齐

离“任意工程、多程序集、复杂项目图、完整 BCL/平台语义”还有明显距离。

### 3. iOS 不适合把 JIT 当热更前提

iOS 目标不能把“发布后再生成并执行新的本地代码”当默认方案。更现实的技术方向是：

- AOT 主线
- 解释器或 AOT+Interpreter 混合执行
- 预生成桥接与元数据补充

此外，远程下发会改变功能的代码，还会碰到 App Store 审核与发行边界，不只是技术问题。

## 备选路径

### 方案 A：先只做 IL2CPP 主线，热更完全后置

优点：

- 主线最聚焦
- 不会过早引入解释器和动态装载复杂度

缺点：

- 将来补热更时，很可能要返工 metadata、bridge、engine ABI、package layout
- iOS 热更边界会在后期集中爆雷

### 方案 B：双轨设计，单核主线

做法：

- 主线继续坚持 `performance-first AOT`
- 热更作为独立运行模式，不反向污染主线热路径
- 两条线共享 metadata contract、engine ABI、bridge stub、packaging 约束

优点：

- 最符合“高性能 IL2CPP + 将来热更”的长期目标
- 技术上也最接近 HybridCLR 这类 `AOT + Interpreter` 思路

缺点：

- 需要从现在就预留 bridge、metadata supplement、runtime mode 切换点
- roadmap 必须拆成多阶段推进

### 方案 C：热更直接依赖 Mono/CoreCLR 侧车

优点：

- 原型快

缺点：

- 架构会分裂成“两套 runtime”
- 引擎接入、对象模型、线程、异常、P/Invoke、调试都更复杂
- 不符合你想做自有高性能 IL2CPP 主线的长期目标

## 结论

推荐方案 B：

- 用一条 `performance-first AOT mainline` 继续做主产品
- 同时把热更定义为“共享 contract 的第二执行平面”
- iOS 热更默认按 `AOT + Interpreter` 思路设计，不按 JIT 思路设计

这已经不是单次实现计划，而是明确需要进入 `roadmap` 的多阶段产品路线问题。
