# IL2CPP Core Structure And Scriban Governance Brainstorm v1.01

Date: 2026-04-17 13:20:00 +08:00
Status: approved-for-roadmap

## 1. 背景

当前 `IL2CPP` 主线已经进入持续扩展 `AOT Core IR` 与 `NativeAOT` 能力的阶段，但核心代码的组织形态开始成为新的主阻力。问题不再只是“某个 feature 还没支持”，而是：

- `Chaos.IL2CPP.CodeGen` 体量明显失衡，单项目承载过多职责。
- `NativeAotLoweringPlanner.cs` 已超过一万行，规划、helper 建模、最终文本拼接混在一起。
- `NativeReferenceLoweringPlanner.cs` 和 `NativeReferenceProofEmitter.cs` 在 family 常量、模板路径、模板加载上存在职责重叠。
- `LoaderStage.cs`、`LinkerStage.cs`、`ManagedClosureContracts.cs` 也已经超出合理文件尺寸。
- `Scriban` 已接入，但更多像“模板外壳”，复杂生成规则仍然在 C# 里用 `StringBuilder.AppendLine(...)` 手写。

如果继续在现有结构上叠新能力，后果会非常直接：

- 新 feature 的接入成本越来越高。
- planner / emitter 的边界持续模糊。
- 代码生成规则越来越难验证。
- 测试框架、运行时支持、AOT lowering 的职责更容易相互污染。

## 2. 现状证据

本轮审计得到的结构热点如下：

- `src/managed/Chaos.IL2CPP.CodeGen/NativeAotLoweringPlanner.cs`: 14052 行
- `src/managed/Chaos.IL2CPP.CodeGen/NativeReferenceLoweringPlanner.cs`: 3024 行
- `src/managed/Chaos.IL2CPP.Loader/LoaderStage.cs`: 2613 行
- `src/managed/Chaos.IL2CPP.Contracts/ManagedClosureContracts.cs`: 2006 行
- `src/managed/Chaos.IL2CPP.CodeGen/NativeReferenceProofEmitter.cs`: 1399 行
- `src/managed/Chaos.IL2CPP.Linker/LinkerStage.cs`: 1391 行

按项目统计，`Chaos.IL2CPP.CodeGen` 已达 `19767` 行，远高于其它 managed 项目，说明问题主要集中在 codegen 层而不是仓库全局平均分布。

## 3. 设计目标

- 让 `IL2CPP` 核心代码结构恢复成可持续扩展的状态。
- 把 `Scriban` 提升为核心代码层的一部分，而不是 emitter 的可选实现细节。
- 明确“什么必须在 C# 里做，什么必须进模板层”。
- 建立文件拆分、目录分层、模板治理、验收阈值的长期规范。
- 最终把规范沉淀到 `wiki/`，让后续 feature 接入不再依赖会话上下文。

## 4. 方案对比

### 方案 A：直接按 assembly 大拆分

做法：

- 把 `Chaos.IL2CPP.CodeGen` 继续拆成多个新的 `.csproj`。
- 让 `Planning / Lowering / Emission / RuntimeSupport / Templating` 变成独立程序集。

优点：

- 物理边界最强。
- 引用关系更容易被编译器约束。

问题：

- 当前主问题先是“类内职责混杂”，不是“程序集还不够多”。
- 第一刀直接拆 assembly，迁移成本高，引用/构建/项目图复杂度会上升。
- 在职责还没梳理清楚时先拆 assembly，容易把坏结构原样分散出去。

结论：

- 这不是第一阶段推荐路径。

### 方案 B：先做 `CodeGen` 内部平衡分层，再评估 assembly 拆分

做法：

- 保持 `Chaos.IL2CPP.CodeGen` 暂时还是一个 assembly。
- 先在项目内建立清晰目录层级：
  - `CoreIr/`
  - `Planning/`
  - `Lowering/`
  - `RuntimeSupport/`
  - `Emission/`
  - `ReferenceProof/`
  - `Templating/`
  - `Templates/`
- 先收口职责，再视边界稳定程度决定是否拆独立项目。

优点：

- 风险最低，最适合当前仓库状态。
- 先解决“超大文件 + 职责混杂”的真实问题。
- 不会在第一步引入额外项目图复杂度。

问题：

- 编译期边界不如多 assembly 强。
- 需要用规范和代码评审持续守边界。

结论：

- 推荐作为主方案。

### 方案 C：只做 codegen 模板化，不先碰结构

做法：

- 优先把 `StringBuilder` 迁到 `Scriban`。
- 暂时容忍 `NativeAotLoweringPlanner.cs` 继续作为大 orchestrator 存在。

优点：

- 短期内能较快减少一部分手写文本。

问题：

- 不能解决真正的根因。
- 规划、helper 建模、模板输入构造仍然会继续糅合在一起。
- 后续模板模型会继续被大文件控制，最终还是难维护。

结论：

- 不推荐单独采用，只能作为方案 B 中的一个阶段。

## 5. 推荐方向

采用方案 B：

- 第一层先做 `IL2CPP` 核心代码的内部平衡分层。
- 第二层把 `Scriban` 正式纳入核心代码层治理。
- 第三层再以治理后的目录结构为基础，推进 planner/emitter 分离、模板能力扩展和其它 stage 的文件拆分。

## 6. 本轮冻结的关键判断

- `Scriban` 不是“外围模板工具”，而是 `IL2CPP` 核心 codegen 层的标准组成部分。
- 只要产物是结构化文本文件，例如 `.cpp/.h/.g.cs/.json`，就应该优先经模板层输出。
- 如果模板能力不够，应优先扩展 `Templating` 支撑层或 `Scriban` 可用函数，而不是回退到新的大块字符串拼接。
- `Chaos.TestFramework` 与 `AOT Core IR` 必须继续解耦，测试代码不能混入 core lowering 逻辑。
- 第一阶段先做 `CodeGen` 内部分层，不直接拆更多 managed project。

## 7. 进入 Roadmap 的原因

这不是一个单次 patch 能结束的工作，原因很明确：

- 需要分阶段推进结构治理、模板治理、运行时支持建模收口、其它 stage 拆分和 wiki 沉淀。
- 每个阶段都要有独立的 exit criteria，避免“拆了一半就继续堆新能力”。
- 后续执行必须按子任务推进，不能把父任务既当 roadmap 又当实现日志。

因此本任务应进入 `roadmap`，而不是直接写单份执行 plan。
