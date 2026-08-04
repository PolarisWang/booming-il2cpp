# Phase A / Phase B 边界细化 v1.01

Date: 2026-04-07
Status: design-discuss

## 1. 目标

本文件专门回答一个会直接影响主线顺序的问题：

- 什么算 `Phase A` 的“核心层稳定化”
- 什么又算 `Phase B` 的“.NET 8 核心语义扩张”

如果这条边界不提前讲清楚，后续很容易出现两个问题：

- 把真正属于“扩语义”的工作伪装成“补内核”
- 让 `Phase A` 无限膨胀，迟迟无法进入 `.NET 10`、`macOS`、Android、iOS

## 2. 当前代码现实

从当前 `src/managed/Chaos.IL2CPP.*` 实现看，pipeline 已经存在，但“核心层”和“语义扩张层”还没有完全分开。

当前事实包括：

- `Loader`
  - 已能读取真实程序集 metadata、方法体、字段、属性、部分泛型实例化。
- `SemanticWorld`
  - 当前基本还是把 `LoadedAssemblyModel` 原样转成 `SemanticWorldModel`，更像投影层而不是强语义归一化层。
- `Linker`
  - 已能形成最小 retained world，但仍内嵌了对现有 proof 子集的 dependency reason 映射，例如：
    - `stdout-path`
    - `narrow-concat-path`
    - `reflection-query`
- `CodeGen`
  - 当前还承担一部分对现有 proof 子集的 canonicalization，例如把 `String.Concat(string,string,string)` 折成两次 pair concat。
- `NativeReferenceProofEmitter`
  - 仍明显带有“按已知样例指令形状选 lowering plan”的特征。
  - 现有 `constructorThenInstanceCall`、`staticCallCtorGetter`、`reflectionQueryMinimal`、`pinvokeDllImportMinimal` 都是 proof-first 切片，不是完整通用 lowering 体系。

也就是说：

- 当前主线已经有“能跑”的 managed spine
- 但还没有彻底做到“样例无关的核心层”和“新增语义能力”之间的清晰切分

## 3. 边界判定原则

建议后续都按下面这条总原则判断：

- `Phase A`
  - 只解决“让当前已支持切片成为稳定、样例无关、层次清晰的核心能力”
- `Phase B`
  - 才开始解决“把尚未支持的新 C# 语义真正纳入支持矩阵”

换句话说：

- 不增加新的语言承诺，只是把现有能力收口成可扩主线的，算 `Phase A`
- 一旦开始新增 IL opcode 家族、metadata 家族、runtime helper 或 BCL 语义承诺，就算 `Phase B`

## 4. 属于 Phase A 的工作

## 4.1 清理 sample-shaped 假设

以下工作属于 `Phase A`：

- 把 `src/**` 中仍然绑定当前 proof 形状的逻辑抽离出来。
- 明确哪些 lowering plan 是“现有能力族”的表达，而不是“特定样例名字”的表达。
- 把现有 helper 依赖、icall 依赖、registration 依赖固化成可声明 contract，而不是零散散布在 `Linker` / `CodeGen` / `Emitter` 内。

判断标准：

- 如果改动后 `HelloWorldObject / GenericEcho / ReflectionLite / PInvokeLite` 仍然是唯一通过的样例，但生产代码变得更 sample-agnostic，这仍然算 `Phase A`。

## 4.2 把现有切片的 contract 讲清楚

这些也属于 `Phase A`：

- 明确 `Loader -> SemanticWorld -> Linker -> MetadataWriter -> CodeGen` 各阶段各自产出什么、绝不产出什么。
- 把 typed IL、AOT manifest、metadata registration、code registration、lowering plan 的职责再冻结一层。
- 明确 runtime-core / bootstrap / support 面向 managed pipeline 的正式消费边界。

判断标准：

- 如果只是把当前已有 proof spine 的边界讲清、接口做稳，而不是新增语义子集，仍算 `Phase A`。

## 4.3 对现有语义切片做“最小必要归一化”

`SemanticWorld` 现在很薄，因此下面这种工作建议也归到 `Phase A`：

- 把当前已经存在于 proof 子集里的语义实体真正提升为 world 级 canonical object，而不是简单搬运 loader 输出。
- 对现有已支持切片做 identity / ownership / member binding / dependency reason 的规范化。

但边界要卡住：

- 只收口当前已经进入 proof 的切片
- 不借机把异常、接口分发、虚调用、数组、boxing 一次性吞进去

## 4.4 诊断与阶段定位能力

这些也应属于 `Phase A`：

- 让现有 proof spine 的失败更稳定地定位到 `source / host-input / analysis / generated / build / runtime`
- 把“为什么 retained world 引入这个依赖”“为什么 lowering plan 选择这条路径”变成显式诊断信息

因为这仍然是在提高现有主线的稳定性，而不是扩张语言能力。

## 5. 不属于 Phase A、应归到 Phase B 的工作

## 5.1 新的控制流和异常语义

以下一旦开始，就应明确视为 `Phase B`：

- try/catch/finally
- throw / rethrow
- 多基本块 / 分支 / CFG 级 lowering

原因：

- 这已经不是“把当前 proof 切片收口”，而是在新增一个新的语义家族

## 5.2 新的对象模型与分发语义

这些也属于 `Phase B`：

- interface dispatch
- virtual dispatch
- delegates / closures
- boxing / unboxing
- arrays

原因：

- 它们需要新的 metadata、runtime helper、lowering 约束与验证样例
- 本质上已经是“支持矩阵新增能力”，不再只是内核清理

## 5.3 更宽的泛型能力

当前如果只是把现有 closed generic proof 切片从 sample-shape 收口成稳定 contract，可以算 `Phase A`。

但以下应归 `Phase B`：

- 更宽的 generic instantiation 支持
- generic sharing
- generic virtual/interface 组合
- 更复杂的 generic reflection 语义

## 5.4 新的运行时/BCL 承诺

这些都应算 `Phase B`：

- 新增 BCL helper surface
- 新增 reflection capability
- 新增 interop 能力
- 新增 runtime object model 能力

因为它们直接改变“当前 IL2CPP 已支持什么”的答案。

## 6. 按模块切分的边界判断

## 6.1 Loader

`Phase A`：

- 清理当前 loader 的输入/输出 contract
- 收口当前 proof 切片已经涉及的 metadata 解析
- 明确哪些 opcode / metadata handle 是“当前正式支持子集”

`Phase B`：

- 新增 opcode 家族
- 新增异常、数组、接口、虚调用等语义所需的输入解析

## 6.2 SemanticWorld

`Phase A`：

- 让 world 真正承接当前已支持切片的 canonical semantic objects
- 不再只是简单 passthrough

`Phase B`：

- 让 world 承接新的语义家族，例如接口图、虚方法分发、异常模型、数组/boxing 语义

## 6.3 Linker

`Phase A`：

- 把现有 dependency reason / normalization / retained world 逻辑收口为 declarative contract
- 服务于当前 proof spine 的样例无关化

`Phase B`：

- 扩 reachability / retention 规则去支持新的语义能力

## 6.4 MetadataWriter

`Phase A`：

- 稳定当前 registration kinds 和 token 约束
- 明确现有 proof 切片需要的 registration completeness

`Phase B`：

- 新增异常、接口、数组、泛型扩张等所需的 metadata/registeration 面

## 6.5 CodeGen / NativeReferenceProofEmitter

`Phase A`：

- 把现有 proof emitter 从“按样例形状硬编码”收口成“按能力切片 / lowering family”驱动
- 不新增新的语言能力，只整理当前已有切片

`Phase B`：

- 新增新的 lowering family
- 新增面向新语义家族的 codegen/runtime glue

## 6.6 runtime-core / bootstrap / support

`Phase A`：

- 只整理当前 proof spine 已经需要的 helper surface
- 把现有 helper 的 contract 稳定下来

`Phase B`：

- 新增为异常、虚调用、接口、数组、boxing 等服务的新 helper surface

## 7. 临时结论

当前建议把 `Phase A` 严格收窄为下面这句话：

- **让现有 proof spine 成为样例无关、层次清晰、可长期扩张的核心层，但不新增新的 C# 语义承诺**

而 `Phase B` 则从下面这句话开始：

- **开始新增 `.NET 8` 核心语义子集，并把它们接入支持矩阵**

## 8. 当前最值得继续细化的下一层议题

基于这份边界判断，下一轮最值得继续细化的是：

1. `Phase A` 下 `SemanticWorld` 应该真正承担哪些职责
2. `Phase A` 下 `Linker + CodeGen + NativeReferenceProofEmitter` 如何从 proof-shape 收口成 capability/lowering family
3. `Phase B` 的第一批语义波次到底先选：
   - exceptions
   - delegates
   - interface/virtual
   - boxing/arrays

## 9. 当前建议

如果沿这条线继续推进，我建议下一份讨论直接写：

- `Phase A semantic-world responsibility`

因为现在最明显的空层就是 `SemanticWorld` 仍太薄；它不先补清楚，`Linker`、`MetadataWriter`、`CodeGen` 的边界讨论会一直反复。
