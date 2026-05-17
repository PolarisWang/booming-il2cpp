# IL2CPP-Like C# to C++ System Findings

Date: 2026-03-29 23:37:57 +08:00
Status: active

## Repository Findings

- Current workspace contains process/skill documentation only.
- No existing translator, runtime, or compiler codebase was found to constrain the architecture.

## Design Findings

- The request implies a platform-scale system rather than a single compiler stage.
- A faithful IL2CPP-like solution will require both a translation pipeline and a managed runtime support layer.
- User prefers a phased program: full target architecture plus staged execution roadmap, not a single-step production-equivalent delivery plan.
- User prefers general .NET project coverage over a Unity-only subset. This pushes the architecture toward stronger BCL/runtime fidelity and away from a narrowly optimized game-script pipeline.
- User chose layered dynamic-feature support: first-class architecture for Reflection, exceptions, generics, P/Invoke, and common serialization; defer dynamic runtime code generation and late-bound assembly loading to later phases.
- User confirmed priority ordering: engineering deliverability first, performance second. Semantic completeness remains important, but early architecture should optimize for “can translate, compile, and run real projects” before deeper optimization work.
- User chose an initial cross-platform matrix. This means early architecture cannot assume a single ABI, allocator model, threading primitive set, or native compiler family. Platform abstraction must exist from day one.
- User chose a fully self-built runtime. This increases implementation scope substantially, but it also keeps the long-term architecture independent from Mono/CoreCLR internals and licensing/runtime coupling.

## Q&A Findings

### 2026-03-29 23:51:53 +08:00 - Difference between source-first and IL-first frontends

**Question:** What is the difference between supporting source input and IL input?

**Answer summary:**

- **IL-first** means the system consumes compiled assemblies (`.dll` / `.exe`) and CIL metadata after the normal C# compiler has already run.
- **Source-first** means the system consumes C# source code directly from Roslyn syntax/semantic models before CIL exists.

**Practical difference:**

1. **Compiler boundary**
   - IL-first reuses the standard C# compiler as the front door.
   - Source-first must either embed or tightly couple to Roslyn compilation behavior.

2. **Semantic stability**
   - IL-first sees normalized, compiler-resolved semantics.
   - Source-first must track language-version features, compiler lowering, and source-level corner cases itself or through Roslyn.

3. **Fidelity to IL2CPP**
   - IL-first is much closer to Unity IL2CPP, which fundamentally works from IL/metadata.
   - Source-first is a different product shape, more like a language compiler than an IL AOT backend.

4. **Optimization visibility**
   - Source-first can preserve richer source constructs and developer intent earlier.
   - IL-first loses some source-form information, but gains a simpler and more uniform intermediate contract.

5. **Tooling compatibility**
   - IL-first naturally supports any frontend that can produce valid .NET assemblies, including multiple C# project shapes.
   - Source-first is more tightly coupled to C# source compilation and project-system behavior.

6. **Engineering complexity**
   - IL-first is usually simpler and more robust for a first production architecture.
   - Source-first increases scope because the system must reason about source compilation, analyzer-like semantics, language evolution, and project loading.

**Architecture implication:**

- If the goal is “IL2CPP-like,” IL-first is the cleaner primary path.
- Supporting both is possible, but the most practical design is usually:
  - canonical pipeline = source -> Roslyn/CSC -> IL assemblies -> translator -> C++
  - optional source-aware layer = extra diagnostics / source mapping / richer tooling

### 2026-03-29 23:54:46 +08:00 - Frontend choice confirmed

- User selected IL/assembly-first as the canonical system boundary.
- Source-level support should therefore be treated as an adjunct layer for diagnostics, symbol mapping, and developer experience rather than as an equal translation frontend.

### 2026-03-29 23:56:53 +08:00 - Phase-1 project coverage

- Phase 1 mainline should support:
  - class libraries
  - console applications
  - background jobs / CLI tools
  - general server applications
  - native-host embedding scenarios
- Higher-level framework-heavy app models such as ASP.NET full-stack hosting, desktop UI stacks, and MAUI-like frameworks are not part of the initial mainline commitment.

### 2026-03-30 00:06:35 +08:00 - Visual companion status

- Browser-based visual companion was not used successfully in this session.
- Continue the brainstorming flow in console/text mode.

### 2026-04-05 16:35:01 +08:00 - IL2CPP-like 功能定义与实现顺序

**Question:** How should this task define the “IL2CPP-like C# to C++” feature, and how should implementation proceed?

**Answer summary:**

- 在本任务语境里，“IL2CPP-like” 不等于把 C# 源码直接翻译成 C++ 文本。
- 更准确的定义是：构建一个以 IL/程序集为规范入口的闭世界 AOT 系统，把 `.NET` 程序集、元数据和 IL 分析为稳定中间产物，再生成 C++，并由自研 runtime 承接托管语义。
- 这个功能的交付边界同时包括两部分：
  - 翻译流水线：`assemblies + metadata -> metadata graph -> typed IL IR -> AOT manifest -> runtime-contract IR -> generated C++ -> native build`
  - 运行时平台：bootstrap、对象模型、class init、异常、线程 attach/detach、基础反射、泛型基础、P/Invoke、宿主嵌入与平台抽象
- 因此，这个项目的本质是“IL AOT backend + managed runtime + native packaging system”，而不是单一转换器。

**Implementation conclusion:**

1. `Roadmap 0` 已经完成的工作，本质上是在冻结真正会约束后续实现的边界：
   - schema pack
   - runtime ABI v0
   - codegen bridge v0
   - smoke 输入项目与平台 gate 骨架
2. 当前仓库现状说明实现尚未进入前端、runtime、codegen 主体：
   - `analysis/` 下已有 schema 与 examples
   - `contracts/native/v0/` 下已有 ABI/bridge 头文件与文档
   - `frontend/`、`runtime/`、`codegen/` 目录尚未正式落地
3. 这意味着最合理的开工顺序不是先写 C++ 发射器，而是：
   - 先做 `Roadmap 1`：程序集加载、元数据图、typed IL IR
   - 并行做 `Roadmap 3`：runtime bootstrap、对象模型、class init、线程、异常、PAL 基线
   - 之后做 `Roadmap 2`：AOT 闭包、保留规则与 manifest
   - 在 `Roadmap 2 + Roadmap 3` 最小闭环可用后，再做 `Roadmap 4`：runtime-contract IR 到 C++ 的 lowering 与端到端链路
4. 这条顺序的核心理由是：
   - 如果先写 codegen，后续泛型、反射、异常、线程、P/Invoke 语义都会反向牵引接口返工
   - 如果先稳定 IR 与 runtime contract，codegen 只需要消费已经收敛的托管语义动作

**Scope clarification:**

- 第 1 阶段主线目标是“能翻译、编译并运行真实 .NET 项目”，不是“立刻达到 Unity IL2CPP 的全部覆盖面”。
- 第 1 阶段非目标保持不变：
  - `dynamic`
  - `System.Reflection.Emit`
  - `Assembly.Load` 式动态延迟加载
  - 重量级桌面 UI 技术栈
  - 全量框架级托管生态兼容

**Record-keeping agreement:**

- 从本次讨论开始，凡是围绕本任务产生的功能定义、实现顺序、范围边界、阶段取舍与关键技术判断，都应持续追加到当前 finding 记录中，而不是只停留在对话里。

### 2026-04-05 16:42:11 +08:00 - 自研方案、官方 .NET runtime、Unity IL2CPP 的多平台架构对比

**Question:** Between the current self-built IL2CPP-like roadmap, the official `.NET runtime` approach, and the Unity `IL2CPP` approach, which is more reliable when cross-platform architecture is the primary concern?

**Answer summary:**

- 这三个方案各自“更靠谱”的前提不同，不能只给一个绝对结论。
- 如果按“今天直接拿来交付真实多平台产品”的成熟度排序，结论是：
  - 官方 `.NET runtime` 方案最靠谱
  - Unity `IL2CPP` 方案次之
  - 当前自研方案最不靠谱
- 如果按“是否拥有统一、从一开始就面向 AOT 和多平台限制的平台级架构”排序，结论更接近：
  - Unity `IL2CPP` 架构最靠谱
  - 当前自研方案的方向基本正确，但尚未被实现验证
  - 官方 `.NET runtime` 更像一组 runtime 家族和宿主体系，不是单一统一的跨平台执行架构

**Evidence-backed comparison:**

1. 官方 `.NET runtime` 的强项是成熟度和现成平台覆盖：
   - `corehost -> hostfxr -> hostpolicy -> coreclr_* ABI` 的宿主链条非常稳定
   - CoreCLR 与 Mono 在宿主侧共享同一套 Hosting ABI
   - 对桌面平台和宿主嵌入场景非常成熟
2. 但官方 `.NET runtime` 在跨平台执行模型上不是“一条统一路线”：
   - Windows/Linux/macOS 更自然的是 `hostfxr + CoreCLR`
   - iOS 更偏 `Mono library mode`、`Interpreter`、`ReadyToRun` 或 `NativeAOT`
   - WASM 也是 Mono 路线
   - 这说明它是“按平台切 runtime 家族”的体系，而不是单一运行时产品形态
3. Unity `IL2CPP` 的强项是统一 AOT 架构：
   - 输入边界统一为 IL/程序集
   - 前端链路清晰分为 `UnityLinker -> IL2CPP converter -> Bee BuildLogic -> libil2cpp`
   - 平台差异主要沉淀在 `BuildLogic.*`、`libil2cpp/os`、底层平台依赖，而不是污染前端主流程
   - 这对 iOS、移动端和其他受限平台尤其友好，因为它默认不把 JIT 当核心前提
4. Unity `IL2CPP` 的弱点是：
   - 当前可见资料里前端 converter 的完整源码树不可见
   - 体系和 Unity 生态绑定很深
   - 它虽然是成熟参考实现，但不等于低成本可复刻
5. 当前自研方案在架构方向上明显更接近 Unity `IL2CPP`，而不是官方 `.NET runtime`：
   - 也是 IL-first
   - 也是闭世界 AOT
   - 也是“翻译流水线 + runtime 平台”的双平面
   - 还额外把 `metadata graph / typed IL IR / AOT manifest / runtime ABI / codegen bridge` 做成了显式 contract
6. 但当前自研方案的最大问题不是方向，而是验证状态：
   - 目前只完成 `Roadmap 0` 的 contract freeze 和骨架
   - `Roadmap 1`、`Roadmap 3` 仍未进入真实实现
   - `frontend/`、`runtime/`、`codegen/` 目录尚未正式落地
   - 因此它今天只能算“方向正确的蓝图”，不能算“更靠谱的现成方案”

**Cross-platform architecture conclusion:**

- 如果“靠谱”指的是**现阶段最容易把产品做出来并在多平台跑起来**，官方 `.NET runtime` 方案胜出，因为它已经把 host、runtime、BCL、packaging 和测试体系做成了工业级产品仓库。
- 如果“靠谱”指的是**长期最适合作为统一的多平台 AOT 架构蓝本**，Unity `IL2CPP` 胜出，因为它天然围绕：
  - IL-first
  - linker + converter 分层
  - code registration / metadata registration
  - AOT + native build + runtime bootstrap
  这套模型来组织平台差异。
- 当前自研方案最适合被理解为：
  - 以 Unity `IL2CPP` 为主要架构参考
  - 借鉴官方 `.NET runtime` 在宿主边界、BCL 和平台工程化上的经验
  - 但在真正做出 `Roadmap 1 + Roadmap 3` 的最小闭环前，不应宣称其比现有两套方案更靠谱

**Practical recommendation:**

- 如果目标是“短中期交付多平台产品”，应优先站在官方 `.NET runtime` 或其混合变体上交付，而不是直接豪赌全自研。
- 如果目标是“长期拥有一套不依赖 CoreCLR/Mono 内部实现、且更适合 iOS/移动端/受限平台的统一架构”，应继续沿着当前自研方案推进，但它的参考母体应优先是 Unity `IL2CPP`，不是官方 `.NET runtime`。

### 2026-04-05 16:50:05 +08:00 - 是否需要修改原计划

**Question:** Based on the comparison above, does the original roadmap need to change?

**Answer summary:**

- 需要修改，但不是推翻重做。
- 更准确地说：**目标架构不改，执行策略要改，验证策略要补，风险表述要收紧。**

**What should remain unchanged:**

1. 顶层产品方向不需要改：
   - 仍然是 IL-first
   - 仍然是闭世界 AOT
   - 仍然是“翻译流水线 + runtime 平台”的双平面
   - 仍然以自研 runtime 为长期目标
2. 父 roadmap 的主顺序基本不用推翻：
   - `Roadmap 1`
   - `Roadmap 3`
   - `Roadmap 2`
   - `Roadmap 4`
   - `Roadmap 5`
   - `Roadmap 6`
3. `Roadmap 0` 已冻结的 schema / ABI / bridge 方向仍然成立，不需要因为这次比较而回退。

**What should be adjusted:**

1. 计划中应明确：官方 `.NET runtime` 不是目标架构，而是参考实现与验证基线。
   - 用它来理解 host、BCL、平台工程化和行为基线
   - 不把它当作我们最终的跨平台统一执行模型
2. 计划中应明确：Unity `IL2CPP` 是主要架构母体。
   - 尤其在 linker + converter 分层
   - registration / metadata bootstrap
   - AOT + native build + runtime 对接
   这些点上应更主动对齐
3. 需要补一条新的“影子验证线”：
   - 在 `Roadmap 1`、`Roadmap 3`、`Roadmap 5` 中，增加与官方 runtime/现有 IL2CPP 方案的差分验证
   - 把它当作 semantic oracle，而不是实现依赖
4. 需要把多平台承诺写得更分层，而不是默认齐头并进：
   - reference desktop：完整端到端与语义主验证面
   - Android/iOS：前期以 compile/package/startup smoke 为主
   - 受限平台：从一开始按 AOT 假设设计，不借 JIT 兜底
5. 需要把风险表述收紧：
   - 当前自研方案只能说“方向正确”
   - 在 `Roadmap 1 + Roadmap 3` 最小闭环跑通前，不能把它表述成“比官方 runtime / IL2CPP 更靠谱”

**Concrete planning impact:**

- 应修改的是 roadmap 的“执行说明”和“验证策略”，而不是重写整个阶段划分。
- 下一版计划里，至少应新增或强化下面几类内容：
  - reference runtime / IL2CPP 差分验证机制
  - 平台分层 gate 说明
  - 自研 runtime 与官方 runtime 的边界说明
  - 为什么我们借鉴官方 `.NET runtime`，但不复制它的多 runtime 家族模式

**Current recommendation:**

- 目前最合理的动作不是推翻原计划重写，而是对现有 roadmap 做一次“校准版修订”。
- 修订重点应放在：
  - 参考系
  - 验证线
  - 多平台分层承诺
  - 风险措辞
  这四个方面。

### 2026-04-05 16:59:49 +08:00 - 各平台何时开始实测 IL2CPP 功能

**Question:** At which roadmap stage can the IL2CPP-like functionality be tested on actual target platforms?

**Answer summary:**

- 这件事不能只回答一个阶段，因为 roadmap 里把“平台实测”分成了 4 层：
  - runtime self-test
  - 端到端 compile/package/startup smoke
  - 语义子集验证
  - 正式平台 gate
- 如果问题指的是“第一次在目标平台上真的跑到自研链路”，答案是：
  - `Roadmap 3` 开始能做 runtime self-test
  - `Roadmap 4` 开始能做 IL2CPP 端到端 smoke
- 如果问题指的是“开始比较像真正验证 IL2CPP 功能正确性”，答案是：
  - `Roadmap 5` 开始进入平台上的语义子集验证
- 如果问题指的是“平台已经成为正式验收门槛”，答案是：
  - `Roadmap 6`

**Layered interpretation:**

1. `Roadmap 0`
   - 只有平台 gate 骨架与最小 smoke 入口约束
   - 还不算真正实测 IL2CPP 功能
2. `Roadmap 3`
   - 运行时与 PAL 先能以 self-test 形式在移动端编译和启动
   - 这时测到的是 runtime core，不是完整的“程序集 -> C++ -> native build”链路
3. `Roadmap 4`
   - 端到端流水线增加 Android/iOS 编译与打包 smoke
   - 这是第一次能在平台上实测自研 IL2CPP 主链路
   - 但主要还是 smoke 级别，不代表语义覆盖已经足够
4. `Roadmap 5`
   - Android 与 iOS 各至少有一个移动端友好样例通过“编译/打包/启动 smoke”
   - 并对一个语义子集完成行为验证
   - 这是第一次进入“平台上的功能正确性验证”
5. `Roadmap 6`
   - Android emulator/device 与 iOS simulator/device 要具备自动 smoke 能力
   - 且至少一组语义子集测试可稳定运行
   - 到这时 Android/iOS 才从附加实验目标提升为正式平台 Gate

**Practical conclusion:**

- 想尽早在平台上看到“东西跑起来”，看 `Roadmap 3/4`
- 想验证“IL2CPP 功能在平台上基本正确”，看 `Roadmap 5`
- 想把平台实测变成正式发布门槛，看 `Roadmap 6`

### 2026-04-05 17:02:55 +08:00 - Codegen 必须采用第三方 Scriban 模板方案

**Question:** What concrete codegen implementation constraint was added for the C++ emission stage?

**Answer summary:**

- 用户新增明确约束：进入 codegen 阶段时，生成 C++ 不采用手写字符串拼接作为主方案，而是采用第三方 `Scriban` 模板方案。
- 当前仓库已存在 vendored 依赖：`third_party/scriban`，因此这不是“未来再决定是否引入”的开放项，而是正式实现约束。

**Planning implication:**

1. `Roadmap 4` 的 C++ 发射器需要显式拆成三层：
   - `Runtime-Contract IR -> emission DTO/model`
   - `Scriban template rendering`
   - 输出文件组织、格式化与构建对接
2. 模板本身应被视为 codegen 的正式资产：
   - 可版本化
   - 可 snapshot
   - 可 diff
   - 可按目标产物拆分，例如 type/method/module/registration/driver
3. 这意味着 codegen 的主要风险不再只是 lowering 正确性，还包括：
   - 模板输入模型稳定性
   - 模板渲染结果可预测性
   - 模板变更的回归可追溯性

**Architecture conclusion:**

- `Scriban` 应作为可移植发射层的标准渲染引擎，而不是一个可选工具。
- 生成逻辑应坚持“语义 lowering 与文本渲染分层”，避免把托管语义判断塞进模板文本本身。

### 2026-04-05 17:06:09 +08:00 - 基于当前架构还需要引入哪些大模块

**Question:** Based on the current architecture, which additional large modules still need to be referenced or introduced?

**Answer summary:**

- 如果按“真正会影响架构与实现边界的大模块”来排，当前最值得明确的是 4 类：
  - 程序集/元数据读取模块
  - Linker / preserve / descriptor 模块
  - GC backend 模块
  - codegen 模板模块
- 其中 `Scriban` 已经确认并 vendored，剩下真正需要决策的是前 3 类。

**Recommended dependency set:**

1. **程序集/元数据读取：`System.Reflection.Metadata` 为主，`Mono.Cecil` 为对照基线**
   - 适用阶段：`Roadmap 1`
   - 原因：
     - `System.Reflection.Metadata` 更轻、更贴近底层 metadata/PE 读取，适合做我们自己的世界模型与 typed IL IR 前端
     - `Mono.Cecil` 更适合做结构对照、差分验证和 tricky case 的语义校验
   - 结论：
     - 不建议把前端核心直接绑定在 `Mono.Cecil` 的对象模型上
     - 更建议“`SRM/PEReader + 自有模型`”为主，`Mono.Cecil` 为 oracle/test helper

2. **Linker / preserve / descriptor：需要一套正式 linker 能力，优先参考/复用 `ILLink` 语义模型**
   - 适用阶段：`Roadmap 2`
   - 原因：
     - 当前路线里已经把 `descriptor/preserve`、reflection keep、serialization keep、unsupported diagnostics 明确成正式交付物
     - 这类能力如果完全从零拍脑袋实现，后面很容易和真实 .NET 生态脱节
   - 结论：
     - 需要引入一个“大模块级参考系”，最合理的是 `ILLink`/trimmer 语义
     - 但未必需要整包直接嵌入主实现；可以先做格式兼容、规则对齐和差分基线

3. **GC backend：需要一个现成 collector backend，首选 `bdwgc` 这类成熟实现**
   - 适用阶段：`Roadmap 3`
   - 原因：
     - 当前设计明确说 Phase 1 不应被“自研高阶精确 GC”阻塞
     - runtime ABI 已经冻结了 handle/root/write barrier/safepoint 这些边界
   - 结论：
     - 需要引入一个成熟 GC backend 来承接早期对象存活与分配
     - 最合理的方向是 `bdwgc` 或同量级 collector backend
     - 这样我们能把精力放在 runtime 语义而不是先写收集器

4. **模板 codegen：`Scriban`**
   - 适用阶段：`Roadmap 4`
   - 状态：
     - 已确认
     - 已 vendored 到 `third_party/scriban`
   - 结论：
     - 这是当前唯一已经从“候选依赖”升级为“正式方案约束”的大模块

**What should remain reference-only for now:**

- 官方 `.NET runtime`
  - 作为宿主边界、BCL 行为和平台工程化参考
  - 不作为整包依赖引入
- Unity `IL2CPP`
  - 作为架构母体与启动/registration/codegen 参考
  - 不作为可直接复用的大模块引入

**What should not be introduced wholesale right now:**

- 整个 `Mono` runtime
- 整个 `CoreCLR` / `dotnet/runtime`
- 整个 `libil2cpp`
- Unity 的 `Bee BuildLogic` 整包

原因：

- 这些模块一旦整包引入，会直接破坏当前“自研 runtime + 自有 contract + 可控多平台边界”的核心路线。

**Practical prioritization:**

1. 先确认 `Roadmap 1` 的前端依赖策略：
   - `System.Reflection.Metadata` 主实现
   - `Mono.Cecil` 测试/对照
2. 再确认 `Roadmap 3` 的 GC backend：
   - `bdwgc` 或等价 collector
3. 然后为 `Roadmap 2` 明确 linker 参考边界：
   - `ILLink` 语义兼容还是局部复用
4. `Roadmap 4` 已按 `Scriban` 执行，不再是开放项

### 2026-04-05 17:12:12 +08:00 - 依赖确认流程按大模块逐项收敛

**Question:** How should the dependency review be conducted as the entry to this complex task?

**Answer summary:**

- 用户要求改为“逐库逐项确认”，不再一次性抽象讨论全部依赖。
- 当前最合适的确认顺序为：
  1. `Roadmap 1`：程序集/元数据读取主实现
  2. `Roadmap 2`：linker / preserve / descriptor 参考模块
  3. `Roadmap 3`：GC backend
  4. 其余模块仅在前三项收敛后再补充
- 这样做的原因是：
  - 前端读取层会直接影响 `Metadata Graph`、`Typed IL IR` 和测试基线
  - linker 与 GC 都依赖前端输出边界
  - `Scriban` 已确认，不再需要占用第一轮决策带宽

### 2026-04-05 17:15:43 +08:00 - `System.Reflection.Metadata + PEReader` 与 `Mono.Cecil` 的主要差别

**Question:** What is the real difference between using `System.Reflection.Metadata + PEReader` as the main implementation and using `Mono.Cecil`, and what can `Mono.Cecil` not do well enough for this architecture?

**Answer summary:**

- 严格说，`Mono.Cecil` 不是“做不到读取程序集和 IL”，它当然能做。
- 真正的区别是：
  - `System.Reflection.Metadata + PEReader` 更偏底层、按 handle/表/heap/PE 结构工作
  - `Mono.Cecil` 更偏高层、按 Assembly/Type/Method/Instruction 对象模型工作
- 因此，核心问题不是“Cecil 能不能读”，而是“我们愿不愿意把前端主实现绑在它的对象模型和重写风格上”。

**Main difference:**

1. `System.Reflection.Metadata + PEReader`
   - 更接近 PE 和 ECMA-335 metadata 的原始结构
   - 直接面向 metadata tables、heaps、handles、RVA、PE headers、sections、debug directory 等底层信息
   - 更适合作为“自有世界模型 + 自有 IR”的底层读取层
2. `Mono.Cecil`
   - 提供更方便的高层对象模型
   - 程序集遍历、成员访问、方法体读取、IL 指令操作都更直接
   - 更适合快速做分析、原型、重写和测试对照

**What `Mono.Cecil` is weaker at for our architecture:**

1. 不适合作为低层 PE/metadata fidelity 的主边界
   - 我们当前架构想自己定义 `Metadata Graph` 和 `Typed IL IR`
   - 如果主实现直接建在 Cecil 对象图上，前端边界会天然耦合到第三方抽象层
2. 不擅长“最小抽象、最小分配”的底层读取路线
   - Cecil 的优势是易用，不是低分配和贴近原始表结构
   - 对大型程序集图和后续可控优化来说，这会更重
3. 不如 `PEReader` 直接胜任 PE 级细节入口
   - 例如 PE headers、sections、原始映像、debug directory、RVA 到方法体的低层控制
   - 这些在 `PEReader`/SRM 路线上更自然
4. 不利于把“读取层”和“改写层”彻底分开
   - Cecil 天然鼓励以可编辑对象模型工作
   - 而我们的前端主线更希望先把读取、建模、IR 规范化做成偏只读、可验证、可快照的底座

**What `Mono.Cecil` can do that SRM does not conveniently give us:**

- 高层程序集导航更方便
- IL 指令级改写更方便
- 写回程序集更方便

**Current conclusion for this task:**

- 对当前架构来说，`Mono.Cecil` 最大的问题不是“能力不够”，而是“抽象层级太高，不适合做主实现边界”。
- 更合适的组合是：
  - `System.Reflection.Metadata + PEReader`：主实现底座
  - `Mono.Cecil`：对照基线、tricky case oracle、必要时的测试辅助

### 2026-04-05 17:15:43 +08:00 - Roadmap 1 依赖策略已确认并登记到第三方依赖

**Question:** What was the final dependency decision for `Roadmap 1`, and how was it recorded?

**Answer summary:**

- 用户确认：
  - `System.Reflection.Metadata + PEReader` 做主实现
  - `Mono.Cecil` 只做测试/对照基线
- 该决策已同步到：
  - `third_party/INDEX.md`
  - `third_party/system.reflection.metadata`
  - `third_party/mono.cecil`
  - `roadmap-v1-01.md`
  - `plan-2026-03-31-23-il2cpp-like-cs-to-cpp-implementation-roadmap.md`

**Vendor status:**

- `System.Reflection.Metadata`：以本地 `.NET runtime` 源码快照方式 vendored
- `Mono.Cecil`：以本地 IL2CPP 自带 `netstandard` 二进制快照方式 vendored，并在 vendor metadata 中标记为 binary snapshot

### 2026-04-05 17:31:57 +08:00 - third_party 索引补充源码仓库地址与版本精度

**Question:** How should `third_party/INDEX.md` be normalized so dependency provenance is easier to audit?

**Answer summary:**

- `third_party/INDEX.md` 应显式增加“源码仓库”列，而不是只记录本地快照来源路径。
- 对能够从本地来源恢复精确版本的依赖，应优先登记 upstream commit，而不是继续保留笼统的 `local snapshot` 描述。
- 对当前 snapshot 无法恢复上游 tag/commit 的依赖，应明确标注为 “upstream tag/commit unavailable”，同时保留本仓库导入锚点。

**Applied normalization:**

- `System.Reflection.Metadata (incl. PEReader)`
  - 源码仓库：`https://github.com/dotnet/runtime`
  - 版本：本地来源仓库 commit `ab2f538fca8e2d278e592da1dd6e6ad82954e91c`
- `Mono.Cecil`
  - 源码仓库：`https://github.com/Unity-Technologies/cecil`
  - 版本：`unity-master` @ `55142e20f3972f9d83f2c18644a5d4093ad2f526`
  - 额外说明：当前 vendored 内容仍是本地 IL2CPP 附带的 `netstandard` 二进制快照
- `Scriban`
  - 源码仓库：`https://github.com/scriban/scriban`
  - 版本：当前 snapshot 未携带可恢复的上游 tag/commit，保留本仓库导入锚点 `booming import commit 3ecd20e`

### 2026-04-05 17:36:08 +08:00 - `bdwgc` 是做什么的，以及在什么情况下用

**Question:** What is `bdwgc` used for, and under what conditions would it be used in this project?

**Answer summary:**

- `bdwgc` 指的通常是 **Boehm-Demers-Weiser Garbage Collector**。
- 它本质上是一个给 `C/C++` 运行时使用的现成垃圾回收器后端，用来负责：
  - 堆对象分配
  - 存活对象追踪
  - 回收不可达对象
  - 线程参与 GC 时的基本配合
- 它最常见的价值不是“帮我们实现托管语义本身”，而是“先替我们接管对象生命周期管理”，这样我们不用在 `Roadmap 3` 一开始就自研完整 GC。

**In this roadmap, what it would be used for:**

1. 作为 `Runtime Core v1` 的早期 GC backend
   - 承接引用对象、字符串、数组、装箱对象等堆分配
   - 支撑最小可运行的对象存活模型
2. 让 runtime 团队先集中实现更关键的语义层
   - runtime bootstrap
   - `TypeInfo` / `MethodInfo`
   - class init / cctor
   - 异常
   - 线程 attach/detach
   - P/Invoke 边界
3. 作为 GC 抽象层后面的临时或阶段性实现
   - 我们前面已经把 root、handle、write barrier、safepoint 这类边界列进 runtime ABI
   - 所以可以先接一个成熟 collector backend，再决定后面是否替换

**When it is a good fit:**

- 当目标是尽快做出 `Runtime Core v1` 的最小闭环时
- 当重点是“先把托管语义跑起来”，不是“先把 GC 做到最强”
- 当我们接受 Phase 1 的 GC 方案以工程可交付优先，而不是一次性做到高阶精确移动式收集
- 当 runtime 主要还在打基础，对象布局、线程模型、异常边界都还在收敛时

**When it is not an ideal fit:**

- 当目标已经变成高性能、低内存、移动端强约束优化
- 当我们需要更强的精确对象扫描、压缩/移动、分代、特化写屏障策略
- 当后续要深度优化二进制体积、停顿时间、缓存局部性或平台级调优时

**Project-level conclusion:**

- 在本项目里，`bdwgc` 更像是 `Roadmap 3` 的“工程加速器”，不是最终 runtime 架构本身。
- 它适合用来帮助我们尽快验证：
  - 对象模型是否成立
  - runtime contract 是否够稳定
  - 线程/异常/反射/PInvoke 与堆对象生命周期能否协同工作
- 一旦项目进入更高阶段，如果 conservative collector 的行为、性能或平台约束开始成为瓶颈，再评估替换为更强的 GC backend 才是合理节奏。

### 2026-04-05 17:36:08 +08:00 - `bdwgc` 已登记到 third_party

**Question:** How was `bdwgc` added to the repository as a third-party dependency?

**Answer summary:**

- 用户要求将 `bdwgc` 也纳入 `third_party` 管理。
- 已按与前面依赖一致的方式完成落点：
  - vendored 源码快照：`third_party/bdwgc`
  - vendor metadata：`third_party/bdwgc/booming-vendor.md`
  - 本地随附许可摘要：`third_party/bdwgc/LICENSE.txt`
  - 第三方索引登记：`third_party/INDEX.md`

**Recorded provenance:**

- 当前快照来源：`E:\agent\dotnet\il2cpp\builds\external\bdwgc`
- 本地 IL2CPP 许可证清单把它登记为：`https://github.com/Unity-Technologies/bdwgc/`
- 自带 `README.md` 报告版本：`7.7.0 (next release development)`
- 当前本地快照无法恢复精确 upstream/fork commit，因此以 `local source snapshot + reported version string` 记录

**Scope note:**

- 这次动作表示 `bdwgc` 已进入仓库的第三方依赖资产清单
- 但它当前仍应理解为 `Roadmap 3` 的 GC backend 候选/首选参考，不等同于已经完成 runtime 集成或最终锁死后续 GC 路线

### 2026-04-05 17:36:08 +08:00 - 参考 Unity IL2CPP 整体目录架构后，对自研设计框架的吸收建议

**Question:** After reviewing the `E:\agent\dotnet\.findings\il2cpp-src` architecture notes, should the project absorb more of Unity IL2CPP's overall directory/system design rather than only isolated technical ideas?

**Answer summary:**

- 结论是：**要参考，而且应该上升到“骨架级参考”，不是只参考局部实现技巧。**
- 但参考的重点不应该是照搬 Unity 的项目拆分名称或 Bee 生态，而应该是吸收下面这组更本质的组织方式：
  - 薄 CLI / 厚转换内核
  - 稳定请求契约层
  - 显式语义世界模型
  - 显式 phase 边界
  - `Context + Scheduler + Step + Collector + Results`
  - Linker / CodeGen / Native Build 三兄弟分层
  - runtime metadata / code registration / startup flow 正式产物化

**What Unity IL2CPP clarified especially well:**

1. `il2cpp` 外壳应当很薄
   - 参数解析、模式选择、异常包装放在入口层
   - 复杂状态和实际转换都沉到内核上下文里
2. “前端语义世界”必须是一等公民
   - Unity 用的是 `TypeContext`
   - 对我们来说，对应的是 `Metadata Graph + Typed IL IR`，但地位应进一步提升成真正的编译系统中心
3. phase 之间必须有显式结果边界
   - Unity 的 `AssemblyConversionResults` 非常值得借鉴
   - 我们现在虽然已经有 `Metadata Graph / Typed IL IR / AOT manifest / Runtime-Contract IR`，但还缺“phase 结果对象”这个组织层
4. 并行模型不应靠共享全局状态硬顶
   - Unity 的 `ForkedContextScope + Collector Merge` 非常像一个成熟的扩展点
   - 即使我们现在不做并行，也值得先预留这个骨架
5. Linker 和 build 系统不应该并进 codegen 内核
   - Unity 把 `UnityLinker` 放在兄弟层，把 `Building/Bee` 放在后段
   - 这和我们当前“Linker / preserve 作为 Roadmap 2、Build 作为后段”的方向是一致的，应继续加强，而不是回退合并

**What our current design already got right:**

- IL-first，而不是 source-first
- 闭世界 AOT 主线
- `Metadata Graph -> Typed IL IR -> Runtime-Contract IR` 的多层中间产物
- Linker / preserve 单独成层
- runtime 和 codegen 分层
- 先 `Roadmap 1 + Roadmap 3`，再进 `Roadmap 4`

**What is still missing in our design skeleton:**

1. 缺一个正式的“请求契约层”
   - 类似 `ConversionRequest / CompilationRequest / SharedSettings`
   - 现在我们的契约更偏产物 schema，缺少“驱动引擎的请求对象图”
2. 缺一个正式的“Conversion Context 骨架”
   - 不只是 ABI/bridge，而是前端/分析/codegen 共享但按能力裁剪的总上下文
3. 缺一个正式的“Phase Results / Collectors / Services”分层
   - 当前我们定义了产物，但还没定义 phase 内部如何安全组织状态与过渡结果
4. 缺一个“收集阶段 vs 写出阶段”的强约束
   - Unity 的 `PrimaryCollection / PrimaryWrite / SecondaryCollection / SecondaryWrite` 非常有启发
   - 我们当前 roadmap 有分析和 codegen，但在“先收集、后落盘”的骨架上还可以更明确
5. 缺一个更正式的 startup / registration 总图
   - 我们已经识别到 `init -> registration -> metadata cache -> runtime world`
   - 但这部分还没有像 Unity 那样成为贯通 CLI、conversion、runtime、build 的系统级视图

**What should not be copied directly:**

- `Bee` 相关项目分层和图构建生态
- WinRT / Tiny / debugger / analytics 等高阶专项系统
- Full per-assembly fork 执行模式
- Unity 现有目录树命名本身
- 把 `TypeContext` 单点等同于我们的全部 IR 体系

原因是：

- 我们的设计已经比 Unity 更显式地区分了 `Metadata Graph / Typed IL IR / Runtime-Contract IR`
- 这是我们的优势，不应该为了“像 IL2CPP”而退化回单块语义模型

**Three candidate integration options:**

1. **方案 A：只吸收骨架模式，不重切现有路线图**
   - 吸收内容：
     - 薄 CLI
     - 请求契约层
     - `ConversionContext`
     - `Collectors / Services / Results`
     - 收集/写出分段
   - 优点：
     - 与当前 roadmap 兼容性最好
     - 返工最小
     - 适合边做边收敛
   - 缺点：
     - 目录/模块层次提升会比较渐进
   - 适合：
     - 当前阶段

2. **方案 B：把当前设计升级成“Unity 风格编译系统骨架”**
   - 吸收内容：
     - 新增 `Api / Driver / Loader / IR / Pipeline / CodeGen / Metadata / RuntimeBridge / Build / Linker` 十字分层
     - 在 `Roadmap 1` 前就补齐 `Context + Results + Collectors`
   - 优点：
     - 长期最稳
     - 一开始就把系统边界拉齐
   - 缺点：
     - 会触发一轮设计文档重构
     - 早期推进速度会慢一点
   - 适合：
     - 如果我们愿意先做一次架构校准，再进入正式实现

3. **方案 C：只参考启动链路和 phase 命名，不引入骨架抽象**
   - 吸收内容：
     - `Initialize / Setup / Collection / Write / Completion` 这套阶段命名
     - startup / registration 总图
   - 优点：
     - 变更最少
   - 缺点：
     - 很容易只学到表面，后面在并行、增量、结果可追踪性上继续吃亏
   - 结论：
     - 不推荐作为主方案

**Current recommendation:**

- 推荐选 **方案 A 向 B 靠拢**：
  - 现在不要推翻现有 `Metadata Graph / Typed IL IR / Runtime-Contract IR` 体系
  - 但应尽快把下面这些骨架补成正式设计项：
    - `ConversionRequest / CompilationRequest`
    - `AssemblyConversionContext` 对应物
    - `Services / StatefulServices`
    - `Collectors`
    - `PhaseResults`
    - `PrimaryCollection / PrimaryWrite / SecondaryCollection / SecondaryWrite` 风格的 phase 分段

**Design integration proposal:**

### 2026-04-05 17:36:28 +08:00 - Implementation stack alignment and performance baseline

**Decision summary:**

- User agreed with these implementation-stack defaults:
  - choose one reference desktop platform first for the first end-to-end vertical slice
  - lock project input boundary to `project/sln -> Driver -> dotnet build/publish -> assemblies -> converter core`
  - lock native build baseline to `CMake + Ninja + toolchain files`
- User requested that the validation baseline must not stop at correctness-only testing.
- A formal `performance reference` line must be added, covering:
  - `CPU performance benchmarks`
  - `memory performance benchmarks`
  - comparison reports against reference runtimes and previous snapshots

**Architecture implication:**

- The validation system should be split into two explicit baselines:
  - `correctness baseline`
    - `xUnit`
    - `golden snapshots`
    - `differential harness`
  - `performance baseline`
    - `CPU benchmark harness`
    - `memory benchmark harness`
    - diff/comparison reports
- Early performance work should not block semantic-correctness bring-up, but benchmark harnesses and measurement contracts should be reserved from the start.
- Performance validation is now part of the planned engineering baseline, not a late optional addition.

**Recommended measurement scope:**

- `CPU`
  - method-call overhead
  - virtual/interface dispatch
  - generic instantiation hot paths
  - array/string operations
  - exception hot paths where relevant
- `Memory`
  - allocation throughput
  - steady-state heap size
  - peak memory
  - long-lived object retention
  - GC-trigger frequency and pause characteristics
  - metadata-cache and startup resident memory

**Reference-comparison direction:**

- Benchmark results should be comparable against:
  - `dotnet 8`
  - `dotnet 10`
  - `Mono`
  - our own previous snapshots/builds
- Reports should support at least:
  - per-sample comparison
  - per-platform comparison
  - per-runtime-profile comparison

### 2026-04-05 17:44:12 +08:00 - Benchmark stack recommendation for performance reference

**Question:** After locking the comparison set to `dotnet 8`, `dotnet 10`, `Mono`, and our generated artifacts, what benchmark stack should be used?

**Recommendation summary:**

- Use a split benchmark architecture rather than forcing every runtime path through one tool.
- For managed reference runtimes:
  - use `BenchmarkDotNet` as the primary benchmark harness for `dotnet 8`, `dotnet 10`, and `Mono`
- For our generated/native-runtime artifacts:
  - use a project-owned benchmark host/runner that consumes the same benchmark case manifest and emits the same normalized result schema
- Keep the benchmark case definitions, parameters, and expected output schema shared across both sides.

**Why this is the recommended shape:**

- `BenchmarkDotNet` is a strong fit for managed runtime comparison and standard benchmark reporting.
- Our generated artifacts will eventually run through a native host/runtime path that should not be distorted to look like a normal managed benchmark process.
- A shared case-manifest plus normalized result schema lets us compare:
  - `dotnet 8`
  - `dotnet 10`
  - `Mono`
  - our generated/native artifacts
  without forcing identical harness internals.

**Recommended output contract:**

- machine-readable:
  - `benchmark-results.json`
  - optional `benchmark-results.csv`
- minimum dimensions:
  - benchmark case id
  - runtime id
  - platform id
  - profile id
  - mean / median / p95 where applicable
  - allocation metrics
  - peak / resident memory metrics where available
  - benchmark build commit or snapshot id

**Planning implication:**

- `BenchmarkDotNet` should be treated as the managed-side performance baseline tool, not as the single universal benchmark runtime for the whole system.
- The native/generated side still needs a first-class benchmark harness owned by this project.

### 2026-04-05 17:49:36 +08:00 - Performance comparison set corrected and benchmark split approved

**Decision summary:**

- User corrected the performance comparison set to:
  - `dotnet 8`
  - `dotnet 10`
  - `Mono`
  - our own previous `snapshot/build` results as regression references
- The current generated/native artifact remains the measurement target under test, but is no longer listed as a peer baseline entry in the comparison set.
- User approved the split benchmark architecture:
  - managed reference side uses `BenchmarkDotNet`
  - generated/native side uses a project-owned benchmark host
  - both sides share benchmark case definitions and normalized result schemas

**Architecture implication:**

- Performance reports should be organized as:
  - current target-under-test result
  - comparison columns for `dotnet 8`, `dotnet 10`, `Mono`
  - regression columns for previous project snapshots/builds
- This keeps "reference-runtime comparison" and "self-regression tracking" in the same system without confusing the current generated artifact with the baseline set.

### 2026-04-05 17:56:18 +08:00 - Next third-party candidate: BenchmarkDotNet

**Question:** What should be the next library candidate after locking the performance baseline shape?

**Recommendation summary:**

- The next library to confirm is `BenchmarkDotNet`.
- Role in this project:
  - managed-side performance baseline tool
  - benchmark runner for `dotnet 8`, `dotnet 10`, and `Mono`
  - standard report producer for CPU and allocation-oriented benchmark data
- It should not be treated as the universal benchmark runtime for the whole project.
- The generated/native side still needs a project-owned benchmark host that consumes the same case manifest and emits the same normalized result schema.

**Current repository observation:**

- `BenchmarkDotNet` is not yet registered in `third_party/INDEX.md` as a project dependency.
- A vendored dependency already present in the repository, `Scriban`, contains its own benchmark project that references `BenchmarkDotNet`, but that does not count as this project's benchmark-stack decision.

**Why this candidate is worth confirming now:**

- It is directly tied to the already approved performance baseline.
- It helps stabilize the reference-runtime side before we design the project-owned native benchmark host.
- It keeps managed reference benchmarking on a mature, standard tool instead of inventing a custom harness too early.

### 2026-04-05 18:01:22 +08:00 - BenchmarkDotNet approved as managed-side performance baseline dependency

**Decision summary:**

- User approved continuing with `BenchmarkDotNet` as the managed-side performance baseline dependency.
- Confirmed scope:
  - benchmark execution for `dotnet 8`
  - benchmark execution for `dotnet 10`
  - benchmark execution for `Mono`
  - standard benchmark result export feeding the project's normalized comparison reports
- It remains scoped to the managed reference side only.
- The generated/native side still requires a project-owned benchmark host and is not replaced by `BenchmarkDotNet`.

### 2026-04-05 18:06:14 +08:00 - Next third-party candidate: xUnit

**Question:** What should be the next library candidate after `BenchmarkDotNet` for the managed-side baseline?

**Recommendation summary:**

- The next library to confirm is `xUnit`.
- Role in this project:
  - managed-side correctness baseline test framework
  - unit/integration test runner for:
    - `Loader`
    - `Metadata Graph`
    - `Typed IL IR`
    - `AOT manifest`
    - differential test orchestration
  - execution host for project-owned golden snapshot assertions
- `xUnit` should be treated as the test execution/assertion framework, not as the snapshot format or diff system itself.

**Current repository observation:**

- `xUnit` is not yet registered in `third_party/INDEX.md` as a project dependency.
- Only incidental references were found inside vendored dependency files; they do not represent this project's testing-stack decision.

**Why this candidate is worth confirming now:**

- It pairs naturally with the already approved correctness baseline shape:
  - `xUnit`
  - `golden snapshots`
  - `differential harness`
- It lets us keep snapshot layout and semantic diff logic project-owned while still using a standard test framework.

### 2026-04-05 18:12:40 +08:00 - Next native-side testing decision: CTest plus project-owned hosts vs GoogleTest

**Question:** After the managed-side baseline, what should be the next native-side testing-stack decision?

**Recommendation summary:**

- Do not add `GoogleTest` yet.
- Keep the native-side testing baseline as:
  - `CTest`
  - project-owned `runtime self-test host`
  - project-owned `smoke host`
  - project-owned `benchmark host`
- Treat `CTest` as the orchestration/execution layer, not as a replacement for the project-specific test hosts.

**Current repository observation:**

- No native-side test framework such as `GoogleTest`, `Catch2`, or `doctest` is currently registered in `third_party/INDEX.md`.
- There is also no evidence that the repository has already committed to a C++ unit-test framework stack.

**Why not add GoogleTest now:**

- The earliest native-side validation work is dominated by:
  - runtime bootstrap
  - ABI bridge checks
  - startup smoke
  - host attach/detach
  - packaging and launch verification
- Those are closer to process-level harnesses than to classic in-process C++ unit tests.
- Adding `GoogleTest` now would increase dependency and build-surface complexity before the native runtime has stabilized enough to benefit from a large C++ unit-test framework.

**Planning implication:**

- Native-side correctness and benchmark infrastructure should first be designed as project-owned executable hosts with stable result contracts.
- If the runtime core later grows enough internal pure-C++ logic to justify fine-grained in-process unit tests, `GoogleTest` can be reconsidered as a later dependency rather than a phase-1 default.

### 2026-04-05 18:22:05 +08:00 - Logging abstraction candidate: Microsoft.Extensions.Logging.Abstractions

**Question:** Should `Microsoft.Extensions.Logging.Abstractions` be introduced for the managed toolchain layers, and what are its tradeoffs versus alternatives?

**Current repository observation:**

- No existing logging stack such as `Microsoft.Extensions.Logging`, `Serilog`, `NLog`, or `log4net` is currently committed in the repository.

**Recommendation summary:**

- Recommended: **yes, but only for operational logging**
- Scope if adopted:
  - `Driver`
  - `Pipeline`
  - `Loader`
  - `Analyzer`
  - `CodeGen`
  - build orchestration and benchmark runners
- Not recommended as the primary representation for compiler/user-facing diagnostics.
- User-facing diagnostics should remain project-owned contracts such as:
  - `Diagnostic`
  - `DiagnosticBag`
  - `DiagnosticReporter`

**Advantages:**

- very standard in the `.NET` ecosystem
- low adoption friction for tool/CLI code
- supports categories, levels, scopes, and pluggable providers
- easy to start with `NullLogger` / console logging and expand later
- works well with structured logging patterns and `LoggerMessage`-style optimization
- easier future integration with host apps, tests, and benchmark runners than a one-off custom logger

**Disadvantages:**

- log levels are generic and do not model compiler diagnostics well
- easy for teams to overuse logging where structured diagnostics should exist
- careless usage can create formatting/allocation overhead in hot paths
- scopes and provider behavior can become noisy if logging policy is not kept disciplined
- it is a managed-toolchain abstraction only; it does not solve native runtime logging design

**Key architectural rule:**

- `logging != diagnostics`
- Logging answers:
  - what the pipeline/runtime-host is doing
  - timing, phases, orchestration, tracing, benchmarking, debug info
- Diagnostics answer:
  - what is wrong with the input or unsupported in the translation/runtime contract
  - file/member/method context
  - error code, severity, fix guidance

**Alternatives considered:**

1. `Custom ILogSink / ILogger` abstraction
   - pros:
     - smallest surface
     - fully project-shaped semantics
     - zero ecosystem coupling
   - cons:
     - we must reinvent providers, adapters, and testing helpers
     - tends to grow ad hoc over time
   - fit:
     - good only if we want extreme minimalism

2. `Serilog` as the main abstraction
   - pros:
     - very strong structured logging story
     - rich sink ecosystem
   - cons:
     - heavier opinion set than we need right now
     - less suitable as a thin common abstraction than `Microsoft.Extensions.Logging.Abstractions`
   - fit:
     - better as an optional provider/adapter later than as the baseline dependency

3. `System.Diagnostics` only (`TraceSource`, `EventSource`, `ActivitySource`)
   - pros:
     - close to platform/runtime primitives
     - useful for telemetry and tracing
   - cons:
     - not ergonomic enough as the main day-to-day compiler/CLI logging API
     - does not replace a simple logger abstraction cleanly
   - fit:
     - good as a future telemetry/tracing layer, not as the only baseline logging API

4. No shared logging abstraction, only project-owned diagnostics
   - pros:
     - simplest conceptual model
   - cons:
     - operational insight becomes inconsistent across driver/pipeline/build/benchmark flows
     - harder to observe phase timing and integration issues
   - fit:
     - too weak for this project's orchestration-heavy architecture

**Current recommendation:**

- Introduce `Microsoft.Extensions.Logging.Abstractions` for managed operational logging.
- At the same time, explicitly keep compiler diagnostics separate as project-owned contracts.
- Do not introduce a heavier logging stack such as full `Serilog` as a baseline dependency at this phase.

### 2026-04-05 18:31:18 +08:00 - Logging abstraction confirmed; next candidate is DI abstraction

**Decision summary:**

- User confirmed the recommendation to adopt `Microsoft.Extensions.Logging.Abstractions`.
- Confirmed boundary:
  - use it for managed operational logging
  - do not use it as the main compiler-diagnostic representation
  - keep diagnostics project-owned

### 2026-04-05 18:31:18 +08:00 - DI abstraction candidate: Microsoft.Extensions.DependencyInjection.Abstractions

**Question:** After confirming logging, should `Microsoft.Extensions.DependencyInjection.Abstractions` be introduced for service wiring in `Pipeline` and related managed layers?

**Current repository observation:**

- No existing DI/container stack such as `Microsoft.Extensions.DependencyInjection`, `Autofac`, `DryIoc`, `Lamar`, or `SimpleInjector` is currently committed in the repository.

**Recommendation summary:**

- Recommended: **do not introduce it as a baseline dependency right now**
- Preferred phase-1 shape:
  - explicit project-owned `ServiceRegistry`
  - explicit `ServiceFactory`
  - explicit per-phase/context service views
- Reconsider a DI abstraction later only if composition complexity clearly justifies it.

**Advantages if adopted:**

- standard `.NET` ecosystem abstraction
- familiar registration and lifetime model
- easy integration with host apps and generic infrastructure
- can reduce boilerplate in object graph construction for tool/CLI code

**Disadvantages:**

- encourages hidden dependencies and service-locator style access if used carelessly
- makes phase/context capability boundaries easier to blur
- container lifetimes do not map cleanly to this project's explicit `Context / Services / StatefulServices / Collectors / PhaseResults` architecture
- can make compiler-pipeline wiring feel more magical than explicit, which hurts debuggability in an architecture-heavy system
- adds an extra abstraction layer before we have enough implementation pressure to justify it

**Alternatives considered:**

1. Project-owned lightweight `ServiceRegistry` / `ServiceFactory`
   - pros:
     - explicit dependency graph
     - aligns better with phase-scoped contexts
     - simplest to debug and evolve with our pipeline model
   - cons:
     - more manual wiring
     - fewer out-of-the-box ecosystem integrations
   - fit:
     - best current fit for phase 1

2. `Microsoft.Extensions.DependencyInjection.Abstractions`
   - pros:
     - familiar and standard
     - good for operational tooling composition
   - cons:
     - weaker fit for strict compiler-phase capability boundaries
   - fit:
     - possible later adapter, not recommended as baseline now

3. Heavier containers such as `Autofac` / `DryIoc`
   - pros:
     - powerful composition features
   - cons:
     - too heavy for the current stage
     - would increase architecture and debugging complexity
   - fit:
     - not recommended

**Current recommendation:**

- Keep service composition explicit and project-owned in phase 1.
- Do not add `Microsoft.Extensions.DependencyInjection.Abstractions` as a formal third-party dependency yet.

### 2026-04-05 18:39:42 +08:00 - CLI parsing is required; candidate is System.CommandLine

**Decision summary:**

- User confirmed that the project does need real CLI parsing.
- The `Driver` layer should not rely on ad hoc/manual argument parsing once the tool surface starts covering:
  - conversion mode
  - build mode
  - platform/architecture selection
  - profile selection
  - preserve/descriptor inputs
  - output/layout selection
  - benchmark and smoke-run options

**Current repository observation:**

- No existing CLI parsing library such as `System.CommandLine`, `Spectre.Console.Cli`, `CommandLineParser`, `McMaster.Extensions.CommandLineUtils`, or `Cocona` is currently committed in the repository.

**Recommendation summary:**

- Recommended baseline dependency: `System.CommandLine`
- Intended scope:
  - `Driver`/CLI argument parsing
  - subcommands
  - options and arity
  - validation/binding
  - help text generation
  - command routing into project-owned request objects

**Why this is the current recommendation:**

- It is a good fit for a tool that will likely grow multiple commands and non-trivial option sets.
- It keeps parsing/help/validation logic out of the conversion core.
- It aligns naturally with the already chosen `Driver` layer boundary.

**Alternatives considered:**

1. Manual parsing
   - pros:
     - zero dependency
     - full control
   - cons:
     - quickly becomes fragile and noisy once subcommands and validation rules grow
   - fit:
     - too weak for the expected CLI surface

2. `Spectre.Console.Cli`
   - pros:
     - nice command model
     - good user-facing CLI experience
   - cons:
     - more opinionated toward console app UX
     - less ideal if we want a thin parsing layer without richer console styling concerns
   - fit:
     - possible, but not the best baseline

3. `CommandLineParser` / `McMaster.Extensions.CommandLineUtils` / `Cocona`
   - pros:
     - usable for many CLI apps
   - cons:
     - less preferred than `System.CommandLine` for this project's expected subcommand-driven tool shape
   - fit:
     - acceptable alternatives, not current first choice

**Current recommendation:**

- Introduce `System.CommandLine` for the `Driver` layer.
- Keep the output of the CLI layer normalized into project-owned request contracts such as `ConversionRequest` and `CompilationRequest`.

### 2026-04-05 18:46:42 +08:00 - Configuration abstraction confirmed

**Decision summary:**

- User requested to introduce `Microsoft.Extensions.Configuration.Abstractions`.
- Confirmed role:
  - managed-side configuration abstraction for tool/driver/config loading
  - configuration access for CLI defaults, profile defaults, environment overlays, and file-backed settings
- Confirmed boundary:
  - configuration ingestion stays outside the conversion core
  - normalized project-owned request objects remain the boundary into the pipeline

**Current repository observation:**

- No existing `Microsoft.Extensions.Configuration` stack is currently committed in the repository.

**Architecture implication:**

- `Microsoft.Extensions.Configuration.Abstractions` should be treated as the configuration interface layer only.
- This decision does **not** yet automatically lock the provider set.
- Provider packages still need to be decided separately if we want support for:
  - JSON config files
  - environment variables
  - command-line configuration overlays
  - binding strongly-typed option objects

**Current recommendation:**

- Introduce `Microsoft.Extensions.Configuration.Abstractions`.
- Keep config-to-request normalization project-owned.
- Decide provider packages in a later step instead of pulling the full configuration stack by default all at once.

### 2026-04-05 18:51:34 +08:00 - JSON configuration provider confirmed

**Decision summary:**

- User confirmed introduction of `Microsoft.Extensions.Configuration.Json`.
- Confirmed role:
  - default file-backed configuration provider for the managed tool/driver side
  - support for stable project/tool configuration files without pushing file-format parsing into the conversion core

**Architecture implication:**

- JSON configuration becomes the default file-based config source on the driver/config side.
- The effective boundary remains:
  - config providers load raw settings
  - project-owned normalization maps them into `ConversionRequest` / `CompilationRequest`
- This still does not imply that environment-variable overlays, command-line overlays, or typed binding helpers are automatically included; those remain separate decisions.

### 2026-04-05 18:54:49 +08:00 - Environment-variable configuration provider confirmed

**Decision summary:**

- User confirmed introduction of `Microsoft.Extensions.Configuration.EnvironmentVariables`.
- Confirmed role:
  - environment-based configuration overlays for CI, local toolchain path injection, platform SDK selection, and experimental/benchmark overrides

**Architecture implication:**

- Environment variables are now part of the planned configuration stack on the driver/config side.
- Recommended precedence shape remains:
  - base JSON config
  - environment overlays
  - later command-line overlays if adopted
- Effective settings still must be normalized into project-owned request contracts before entering the pipeline.

### 2026-04-05 18:57:31 +08:00 - Command-line configuration provider confirmed

**Decision summary:**

- User confirmed introduction of `Microsoft.Extensions.Configuration.CommandLine`.
- Confirmed role:
  - final command-line configuration overlays on top of file and environment settings
  - support for a standard precedence chain between config file defaults, environment overrides, and final CLI overrides

**Architecture implication:**

- The planned configuration precedence is now explicitly:
  - `JSON config`
  - `environment variables`
  - `command-line overlays`
- CLI parsing still belongs to `System.CommandLine` in the `Driver` layer.
- Configuration overlays and CLI command parsing are related but remain separate responsibilities.
- Effective settings still must be normalized into project-owned request objects before entering the pipeline.

### 2026-04-05 19:03:12 +08:00 - Binder application scenarios in this project

**Question:** What concrete application scenarios justify `Microsoft.Extensions.Configuration.Binder` in this project?

**Example scenarios:**

1. Tool-level default config file
   - Example file:
     - `booming.il2cpp.json`
   - Example shape:
     - `input.entryAssembly`
     - `input.references`
     - `output.directory`
     - `build.targetPlatform`
     - `build.targetArchitecture`
     - `runtime.profile`
   - Binder value:
     - bind the config tree into a `ToolOptions` object first, then normalize into `ConversionRequest`

2. Platform toolchain configuration
   - Example shape:
     - `toolchains.android.ndkRoot`
     - `toolchains.android.apiLevel`
     - `toolchains.ios.xcodeRoot`
     - `toolchains.windows.vsInstallPath`
   - Binder value:
     - map nested config into a `ToolchainOptions` object instead of manually pulling dozens of keys

3. Benchmark and performance profiles
   - Example shape:
     - `benchmarks.enabled`
     - `benchmarks.runtimes`
     - `benchmarks.iterationCount`
     - `benchmarks.memory.collectPeak`
   - Binder value:
     - convenient mapping into `BenchmarkOptions` for the benchmark runner and comparison pipeline

4. Linker/preserve policy configuration
   - Example shape:
     - `linker.mode`
     - `linker.descriptorFiles`
     - `linker.preserve.serialization`
     - `linker.preserve.reflection`
   - Binder value:
     - bind nested policy objects before translating them into analyzer/linker request contracts

5. Runtime and build profiles
   - Example shape:
     - `runtime.gcBackend`
     - `runtime.enableThreadChecks`
     - `build.compiler`
     - `build.lto`
     - `build.stripSymbols`
   - Binder value:
     - makes profile presets much easier to express and load

**Where Binder helps most:**

- nested configuration trees
- repeated option groups
- environment and command-line overlays on top of the same object shape
- keeping config-loading code short in the `Driver` layer

**Where Binder should not become the architecture boundary:**

- do not bind configuration directly into core pipeline/domain contracts
- preferred flow remains:
  - configuration sources
  - bind into tool/options DTOs
  - validate
  - normalize into `ConversionRequest` / `CompilationRequest`

**Practical conclusion:**

- `Binder` is valuable mainly on the driver/config side when the config shape becomes nested and profile-heavy.
- If the project stayed extremely small, manual mapping would be acceptable.
- For the current planned architecture, Binder is likely to save a lot of repetitive mapping code.

### 2026-04-05 19:07:18 +08:00 - Binder confirmed

**Decision summary:**

- `Microsoft.Extensions.Configuration.Binder` is now treated as the recommended next configuration-stack dependency.
- Confirmed boundary:
  - use it for config-to-options DTO binding on the driver/config side
  - do not bind directly into core pipeline/domain contracts
  - keep validation and normalization into `ConversionRequest` / `CompilationRequest` project-owned

### 2026-04-05 19:13:42 +08:00 - FileSystemGlobbing confirmed

**Decision summary:**

- User agreed to introduce `Microsoft.Extensions.FileSystemGlobbing`.
- Confirmed intended scope:
  - `Driver`-side input discovery
  - descriptor/preserve file collection
  - benchmark/sample set discovery
  - include/exclude pattern handling for tooling workflows
- Confirmed boundary:
  - keep it out of `Loader`, `Analyzer`, and `CodeGen` semantic cores
  - use it only in tooling/config/input-discovery layers

### 2026-04-05 19:16:28 +08:00 - Logging.Console confirmed

**Decision summary:**

- User agreed to introduce `Microsoft.Extensions.Logging.Console`.
- Confirmed role:
  - default development-time and tool-host console logging provider
  - immediate operational visibility for `Driver`, `Pipeline`, `Build`, benchmark runners, and related managed orchestration layers
- Confirmed boundary:
  - use it as a logging provider only
  - do not treat it as the compiler-diagnostic channel

### 2026-04-05 19:20:41 +08:00 - Logging.Debug rejected to keep the logging stack lean

**Decision summary:**

- User rejected introducing `Microsoft.Extensions.Logging.Debug`.
- Rationale:
  - the logging stack should stay small
  - avoid accumulating too many logging providers early
  - `Microsoft.Extensions.Logging.Console` is sufficient as the default provider for the current phase

**Architecture implication:**

- Managed logging baseline remains intentionally minimal:
  - `Microsoft.Extensions.Logging.Abstractions`
  - `Microsoft.Extensions.Logging.Console`
- Additional logging providers should require explicit justification rather than being added by default.

### 2026-04-05 19:29:18 +08:00 - Logging group simplification review

**Question:** After revisiting the logging group, what can still be simplified further?

**Conclusion summary:**

- The current managed logging group is already near the minimum practical baseline.
- Recommended retained set:
  - `Microsoft.Extensions.Logging.Abstractions`
  - `Microsoft.Extensions.Logging.Console`
- Recommended explicit exclusions:
  - `Microsoft.Extensions.Logging.Debug`
  - `Serilog`
  - `NLog`
  - `log4net`
  - `Microsoft.Extensions.Hosting` as a logging-driven dependency
  - telemetry/tracing stacks such as OpenTelemetry for phase 1

**What can still be simplified:**

1. Simplify by boundary, not by adding replacement systems
   - keep logging only for operational/tooling concerns
   - keep compiler diagnostics separate
   - avoid building a second project-owned logging subsystem just to replace `Logging.Console`

2. Simplify provider count
   - stop at one provider: `Console`
   - do not add extra development-only providers

3. Simplify usage policy
   - avoid using logging as a substitute for diagnostics
   - avoid deep logging inside hot-path semantic transforms unless gated
   - prefer phase-level, build-level, benchmark-level, and orchestration-level logs

**What is not recommended as a simplification:**

- Dropping `Microsoft.Extensions.Logging.Console` and replacing it with a custom console logger
  - this would reduce a package but create a project-owned logging subsystem, which is not actually simpler
- Dropping `Microsoft.Extensions.Logging.Abstractions`
  - this would push the project either toward ad hoc logging calls or toward inventing a custom abstraction too early

**Practical result:**

- The logging group should now be considered closed for phase 1 unless a future need is strong enough to justify reopening it.

### 2026-04-05 19:36:54 +08:00 - Logging group closure confirmed; next candidate is Microsoft.Build.Locator

**Decision summary:**

- User confirmed the current logging simplification plan.
- Logging group is now treated as closed for phase 1 with the retained set:
  - `Microsoft.Extensions.Logging.Abstractions`
  - `Microsoft.Extensions.Logging.Console`

### 2026-04-05 19:36:54 +08:00 - MSBuild integration candidate: Microsoft.Build.Locator

**Question:** After closing the logging group, should `Microsoft.Build.Locator` be introduced as a baseline dependency?

**Current repository observation:**

- The project repository itself does not yet commit to an `MSBuild` integration stack.
- A vendored dependency already present in the repository, `Scriban`, references `Microsoft.Build.Locator` internally for its own codegen tooling, but that does not count as this project's MSBuild decision.

**Architecture context:**

- The already aligned project-input boundary is:
  - `project/sln -> Driver -> dotnet build/publish -> assemblies -> converter core`
- The conversion core is explicitly not supposed to depend on the `MSBuild` object model.

**Recommendation summary:**

- Recommended: **do not introduce `Microsoft.Build.Locator` as a phase-1 baseline dependency**
- Preferred phase-1 strategy:
  - keep project/sln handling in the `Driver`
  - use `dotnet build` / `dotnet publish` subprocess orchestration
  - consume the produced assemblies and related artifacts from there

**Why not add it now:**

- `Microsoft.Build.Locator` is mainly valuable when we need in-process `MSBuild` discovery and binding.
- That usually goes together with:
  - `MSBuildWorkspace`
  - `ProjectCollection`
  - in-process project evaluation/loading
- Those are useful, but they also move the tool closer to the `MSBuild` object model and increase environment-coupling complexity.
- For the current architecture, the simpler and safer boundary is still:
  - external build orchestration
  - internal assembly-based translation

**When it may become justified later:**

- if the `Driver` needs richer in-process project graph inspection
- if we need finer control over multi-targeting resolution than subprocess `dotnet build/publish` gives us
- if future source-aware tooling requires `MSBuildWorkspace` integration

**Alternatives considered:**

1. No `Microsoft.Build.Locator`, subprocess-only build orchestration
   - pros:
     - keeps the boundary clean
     - lower complexity
     - avoids early `MSBuild` object-model coupling
   - cons:
     - less in-process introspection and control
   - fit:
     - best current fit

2. Introduce `Microsoft.Build.Locator` now
   - pros:
     - enables future in-process project loading
     - can help if we later adopt `MSBuildWorkspace`
   - cons:
     - brings in complexity before the project actually needs it
   - fit:
     - not recommended as a baseline now

**Current recommendation:**

- Keep `MSBuild` integration minimal in phase 1.
- Do not add `Microsoft.Build.Locator` yet.
- Revisit only if the `Driver` outgrows subprocess-based `dotnet build/publish` orchestration.

### 2026-04-05 19:41:52 +08:00 - Microsoft.Build.Locator deferred

**Decision summary:**

- User decided not to introduce `Microsoft.Build.Locator` for now.
- Policy:
  - keep phase-1 `MSBuild` integration minimal
  - continue using subprocess-based `dotnet build/publish` orchestration at the `Driver` boundary
  - reconsider only when a concrete need appears

**Architecture implication:**

- The project remains intentionally decoupled from in-process `MSBuild` object-model integration in phase 1.
- Richer project-graph inspection or `MSBuildWorkspace`-style scenarios are explicitly deferred until they are justified by real requirements.

### 2026-04-05 19:49:58 +08:00 - Important-library discussion reset and reprioritization

**Question:** Which libraries are actually important enough to matter for the architecture, and which previously discussed items are only convenience/tooling dependencies?

**Conclusion summary:**

- The discussion should be reset around architecture-critical dependencies, not general tooling convenience packages.
- Previously discussed logging/config/CLI support libraries are mostly **non-critical tooling dependencies**.
- The project should now prioritize only libraries that materially affect:
  - IL/metadata ingestion correctness
  - C++ code generation architecture
  - runtime/GC viability
  - cross-platform build/runtime portability
  - correctness/performance verification at system level

**Tier 0 - already identified as architecture-critical:**

1. `System.Reflection.Metadata + PEReader`
   - primary IL/metadata ingestion foundation
   - affects the entire frontend and semantic-world correctness

2. `Scriban`
   - primary codegen templating foundation
   - affects how `Runtime-Contract IR` becomes generated C++

3. `BDWGC`
   - early GC backend/reference for runtime bring-up
   - materially affects whether the runtime can become viable before a self-built GC exists

**Tier 1 - likely important enough to discuss next:**

1. `NuGet.Frameworks`
   - useful if the `Driver` must reason about TFM parsing, compatibility, and multi-targeting rules
   - more relevant than deeper `MSBuild` integration for the current architecture boundary

2. `BenchmarkDotNet`
   - not core translation architecture, but important enough for the already approved managed-side performance baseline

3. `xUnit`
   - not core translation architecture, but important enough for the already approved managed-side correctness baseline

**Tier 2 - useful but not architecture-defining for phase 1:**

- `System.CommandLine`
- `Microsoft.Extensions.Configuration.*`
- `Microsoft.Extensions.Logging.*`
- `Microsoft.Extensions.FileSystemGlobbing`

These improve driver/tooling ergonomics but do not determine whether the IL2CPP-like system architecture can succeed.

**Tier 3 - explicitly deferred unless a concrete need appears:**

- `Microsoft.Build.Locator`
- in-process `MSBuildWorkspace` stacks
- heavier DI containers
- extra logging providers
- richer telemetry/tracing stacks

**Process implication:**

- Future library discussions should focus first on `Tier 1` and on any missing runtime/build/linker-critical dependencies, rather than continuing to expand tooling-support packages.

### 2026-04-05 20:02:26 +08:00 - Tier-1 library discussion: NuGet.Frameworks

**Question:** Is `NuGet.Frameworks` important enough to introduce, and why is it more relevant than deeper `MSBuild` integration for the current architecture?

**Current repository observation:**

- The project itself has not yet adopted `NuGet.Frameworks`.
- A vendored dependency already present in the repository, `Scriban`, references `NuGet.Frameworks` for its own tooling, but that does not decide this project's boundary.

**What problem it actually solves in this project:**

- `NuGet.Frameworks` is not a package-download library here.
- Its value is as a lightweight `TFM` and framework-compatibility utility.
- Concretely, it helps with:
  - parsing `TargetFramework` values such as `net8.0`, `net10.0`, `netstandard2.0`
  - normalizing framework identifiers
  - comparing framework compatibility
  - reasoning about multi-targeting choices
  - selecting the nearest applicable framework rule/profile when the `Driver` has to choose among multiple targets

**Why it matters more than MSBuild integration right now:**

- The agreed project-input boundary is still:
  - `project/sln -> Driver -> dotnet build/publish -> assemblies -> converter core`
- That means the current problem is not "load the whole MSBuild object model".
- The nearer problem is:
  - once the `Driver` sees target-framework data, how should it parse, compare, and validate it correctly?
- `NuGet.Frameworks` helps solve exactly that smaller but real problem without dragging in in-process `MSBuild` coupling.

**Concrete application scenarios:**

1. Multi-target project output selection
   - Example:
     - a project targets `net8.0;netstandard2.0`
   - Need:
     - decide which target is the translation candidate
     - reject or warn on ambiguous selection

2. Input validation
   - Example:
     - user asks to translate assemblies built for a framework/profile the current phase does not support well
   - Need:
     - emit a clean capability/compatibility diagnostic rather than relying on string matching

3. Reference/runtime-pack/profile coordination
   - Example:
     - target platform profile or runtime profile has a minimum framework expectation
   - Need:
     - compare requested target against project/framework inputs in a principled way

4. Benchmark and smoke matrix labeling
   - Example:
     - benchmark/smoke runs need stable labels for `net8.0`, `net10.0`, `netstandard2.0`
   - Need:
     - avoid ad hoc parsing and comparison logic spread across tooling

**Advantages:**

- small, focused, and much lighter than deeper `MSBuild` integration
- solves a real correctness problem rather than only a tooling comfort problem
- keeps framework parsing/compatibility logic out of ad hoc string code
- aligns well with the `Driver`-side project normalization boundary

**Disadvantages:**

- still another dependency to carry
- does not solve project loading, restore, or build orchestration
- easy to overestimate and accidentally treat as a full project-system solution when it is not

**Alternatives considered:**

1. Manual TFM parsing/comparison
   - pros:
     - zero dependency
   - cons:
     - fragile
     - reinvents framework parsing and compatibility rules poorly
   - fit:
     - not recommended

2. In-process `MSBuild`/workspace integration
   - pros:
     - richer project-system knowledge
   - cons:
     - much heavier than the actual problem we need to solve now
   - fit:
     - deferred

3. Heavier `NuGet.Client` stacks
   - pros:
     - more package-graph features
   - cons:
     - too heavy if the real need is just framework parsing and compatibility
   - fit:
     - not recommended now

**Current recommendation:**

- `NuGet.Frameworks` is important enough to adopt.
- Recommended scope:
  - `Driver`
  - project normalization
  - target-framework validation/selection
  - benchmark/smoke labeling where framework identity matters
- Not recommended as a core dependency of:
  - `Loader`
  - `Semantic World`
  - `Analyzer`
  - `CodeGen`

### 2026-04-05 20:07:41 +08:00 - NuGet.Frameworks confirmed

**Decision summary:**

- User confirmed introducing `NuGet.Frameworks`.
- Confirmed intended scope:
  - `Driver`
  - project normalization
  - target-framework parsing
  - target-framework validation/selection
  - benchmark and smoke labeling where framework identity matters
- Confirmed boundary:
  - do not treat it as a project-system or package-restore solution
  - do not pull it into `Loader`, `Semantic World`, `Analyzer`, or `CodeGen`

### 2026-04-05 20:18:12 +08:00 - Whether ICU is required for IL2CPP-like functionality

**Question:** Does the IL2CPP-like feature actually need `ICU`, and where would it be used?

**Conclusion summary:**

- `ICU` is **not required** to make the minimal IL2CPP-like pipeline exist.
- It is **not** a frontend/codegen dependency.
- It becomes important when the runtime claims broader `.NET` compatibility for:
  - globalization
  - culture-aware string behavior
  - collation/comparison
  - casing rules
  - normalization
  - locale data
  - time-zone and formatting related culture data paths, where applicable

**Where ICU would be used:**

- in the `Runtime` plane, not the translation plane
- more specifically in runtime subsystems such as:
  - globalization backend
  - culture/locale services
  - culture-aware string comparison and sorting helpers
  - Unicode casing/normalization helpers
  - parts of formatting/parsing behavior that depend on locale data

**Where ICU would NOT be used:**

- not in `Loader`
- not in `Metadata Graph`
- not in `Typed IL IR`
- not in `Analyzer`
- not in `CodeGen`
- not in `Metadata Writer`

**Architecture implication:**

- Without `ICU`, the project can still build an IL2CPP-like system that:
  - reads IL/metadata
  - performs AOT analysis
  - generates C++
  - boots a native runtime
  - runs a meaningful subset of real projects
- But without `ICU`, the runtime should narrow its compatibility claim in phase 1 to something like:
  - `globalization-lite`
  - invariant or limited-culture behavior
  - not full parity for culture-sensitive APIs

**Practical phase-1 recommendation:**

- Do **not** treat `ICU` as a phase-1 must-have dependency for proving the IL2CPP-like architecture.
- Do reserve a runtime globalization abstraction boundary now, so the project can later choose between:
  - invariant/globalization-lite mode
  - `ICU`-backed mode

**Decision heuristic:**

- If phase 1 target programs are mostly:
  - console tools
  - services
  - background jobs
  - AOT smoke/golden projects
  then `ICU` can be deferred.
- If the project soon wants to claim stronger compatibility for:
  - `CultureInfo`
  - culture-sensitive string operations
  - non-trivial formatting/parsing across locales
  then `ICU` becomes a serious candidate.

### 2026-04-05 20:21:54 +08:00 - ICU strategy confirmed

**Decision summary:**

- User agreed with the proposed `ICU` strategy.
- Confirmed phase-1 policy:
  - do not treat `ICU` as a must-have dependency
  - keep phase 1 at `invariant / globalization-lite`
  - reserve a runtime globalization-backend abstraction so `ICU` can be introduced later without redesigning the runtime surface

### 2026-04-05 20:33:18 +08:00 - Whether libffi is required for IL2CPP-like functionality

**Question:** Does the IL2CPP-like feature need `libffi`, and where would it be used?

**Conclusion summary:**

- `libffi` is **not required** for the minimal IL2CPP-like architecture.
- It is not part of the IL/metadata frontend or the C++ code-generation core.
- It becomes relevant only if the runtime wants a more generic/dynamic native-call bridge than static generated wrappers.

**Where libffi would be used:**

- in the `Runtime` plane, specifically around interop/call-bridge facilities such as:
  - generalized `P/Invoke` dispatch when the call shape is not fully hardwired at compile time
  - callback/delegate trampolines for native-to-managed or managed-to-native bridge cases
  - runtime-generated call stubs for reflection-style invocation or generic call adapters, where applicable

**Where libffi would NOT be used:**

- not in `Loader`
- not in `Metadata Graph`
- not in `Typed IL IR`
- not in `Analyzer`
- not in `CodeGen` core lowering logic
- not in `Metadata Writer`

**Key architectural distinction:**

1. Static interop path
   - generated C++ wrappers are emitted from known signatures
   - marshaling glue is code-generated ahead of time
   - no `libffi` is needed

2. Dynamic/generalized interop path
   - runtime needs to adapt arbitrary signatures more generically
   - runtime may need a portable foreign-function bridge
   - `libffi` becomes a meaningful candidate

**Architecture implication:**

- The already aligned phase-1 scope favors the static path:
  - AOT
  - closed world
  - generated wrappers
  - explicit runtime/codegen bridge
- Under that scope, `P/InvokeLite` and similar early interop scenarios should be handled by:
  - compile-time signature analysis
  - generated marshaling glue
  - project-owned bridge helpers

**Practical phase-1 recommendation:**

- Do **not** treat `libffi` as a phase-1 required dependency.
- Prefer:
  - code-generated wrappers
  - project-owned marshaling helpers
  - explicit trampoline/bridge surfaces in runtime contracts
- Reserve an interop-backend abstraction so `libffi` can be revisited later if dynamic/generalized invocation pressure appears.

**When libffi may become justified later:**

- broader reflection-driven invocation support
- more dynamic callback/trampoline requirements
- platform-portable generic foreign-call bridging beyond a generated-wrapper strategy
- if project-owned call-bridge code becomes too complex or too platform-specific to maintain cleanly

### 2026-04-05 20:38:44 +08:00 - libffi strategy confirmed

**Decision summary:**

- User agreed with the proposed `libffi` strategy.
- Confirmed phase-1 policy:
  - do not treat `libffi` as a must-have dependency
  - prefer code-generated wrappers and project-owned marshaling helpers
  - reserve an interop-backend abstraction so `libffi` can be reconsidered later if dynamic/generalized invocation pressure appears

### 2026-04-05 20:38:44 +08:00 - Next focus should shift from more libraries to self-built critical modules

**Conclusion summary:**

- After reviewing `ICU` and `libffi`, the set of truly architecture-relevant third-party libraries is mostly converged for phase 1.
- Further progress is more likely to depend on self-built subsystem design than on introducing many more third-party libraries.
- The next discussion focus should be the project-owned critical modules that no external library should replace, especially:
  - `Linker`
  - `Metadata Writer`
  - `Runtime ABI`
  - `PAL`
  - `GC facade`
  - `Interop/marshaling bridge`

### 2026-04-05 20:50:26 +08:00 - Linker formal definition and phase-1 scope

**Question:** How should `Linker` be defined in this architecture, and what should phase 1 require from it?

**Definition summary:**

- `Linker` is not just a shrinker/pruner.
- In this project, `Linker` should be defined as the subsystem that computes the closed-world executable set and the retention policy needed for AOT.
- More concretely, it is responsible for:
  - root discovery
  - reachability closure
  - virtual/interface dispatch closure
  - generic instantiation discovery
  - metadata retention decisions
  - preserve-policy application
  - production of the canonical `AOT manifest`

**Recommended inputs:**

- entry assemblies and entry points
- explicit root descriptors / preserve files
- framework preserve profiles
- host-embedding exports and native callback roots
- reflection / serialization hints and policy inputs
- project/runtime profile settings that affect retention strictness

**Recommended outputs:**

1. canonical product
   - `AOT manifest`

2. supporting products
   - preserve report
   - uncertain-closure report
   - unsupported-feature diagnostics
   - metadata-retention decisions or retention sections referenced by the manifest

**Important design point:**

- The primary canonical artifact should still be `AOT manifest`.
- But `Linker` should not be modeled as producing only one opaque blob.
- Diagnostics and retention reporting need first-class outputs so later verification and debugging remain possible.

**What Linker is not:**

- not just file-level trimming
- not the native binary linker
- not the `CodeGen` stage
- not the `Metadata Writer`

**Relationship to Analyzer:**

- architecture-level boundary:
  - `Linker` should remain a sibling subsystem, not conceptually merged into `Analyzer`
- implementation-level allowance for phase 1:
  - early logic may temporarily live in analyzer-owned code paths
  - but outward-facing contracts and artifacts should still be named and modeled as `Linker` outputs

**Why this boundary matters:**

- otherwise `Analyzer`, `Linker`, and `Metadata Writer` will collapse into one mixed subsystem
- that would make reflection/preserve behavior, closure debugging, and later platform/profile evolution much harder to reason about

**Phase-1 minimum commitment:**

- phase 1 `Linker` should support at least:
  - direct-call reachability
  - virtual/interface closure for the supported subset
  - delegate roots for the supported subset
  - generic instantiation discovery for the supported subset
  - explicit descriptor/preserve roots
  - metadata retention decisions required by codegen/runtime bootstrap
  - clean diagnostics for unsupported or uncertain closure cases

**Phase-1 non-goals:**

- complete ecosystem-wide trimming sophistication
- perfect modeling of every reflection-heavy dynamic pattern
- silent best-effort behavior with no diagnostics

**Practical conclusion:**

- `Linker` should be treated as the AOT-closure and retention subsystem that defines what the rest of the pipeline is allowed to assume exists.
- In phase 1, the most important thing is not maximum trimming aggressiveness, but a stable manifest + explicit diagnostics + predictable retention behavior.

### 2026-04-05 21:02:18 +08:00 - Metadata Writer formal definition and phase-1 scope

**Question:** How should `Metadata Writer` be defined in this architecture, and what should phase 1 require from it?

**Definition summary:**

- `Metadata Writer` is not the `CodeGen` stage and not the `Linker`.
- In this project, `Metadata Writer` should be defined as the subsystem that materializes the retained managed world into the runtime-readable metadata and registration artifacts required for bootstrap, reflection-lite, class initialization, and runtime lookup.
- More concretely, it is responsible for:
  - emitting `code registration`
  - emitting `metadata registration`
  - emitting metadata blobs/binaries/tables consumed by the runtime
  - emitting lookup maps needed by runtime/bootstrap/debug-support surfaces

**Key architectural distinction:**

1. `Linker`
   - decides what must exist
   - computes closure and retention policy

2. `CodeGen`
   - emits executable code bodies and helper calls from `Runtime-Contract IR`

3. `Metadata Writer`
   - emits the data model and registration structures that let the runtime find, initialize, and reason about those retained types/methods/fields at runtime

**Recommended inputs:**

- `AOT manifest`
- metadata-retention decisions
- retained type/method/field/property/event sets
- generic-instantiation retention data for the supported subset
- codegen symbol inventory or code-body identity map
- runtime profile and ABI/profile settings that affect metadata layout
- debug/source/map inputs where the phase-1 runtime or tooling needs them

**Recommended outputs:**

1. canonical bootstrap/runtime products
   - `code registration`
   - `metadata registration`
   - metadata binary/blob/table package

2. supporting products
   - token-to-runtime lookup maps
   - string/literal/metadata index sections as needed by runtime lookup
   - optional debug/line/symbol maps for supported tooling scenarios

**Important design point:**

- `Metadata Writer` should be modeled as an explicit artifact-producing subsystem, not as "some extra arrays incidentally emitted by codegen templates".
- Even if early implementations choose to emit some registration artifacts as generated C++ translation units, the contracts and ownership should still remain with `Metadata Writer`.

**What Metadata Writer is not:**

- not reachability analysis
- not preserve-policy computation
- not method-body lowering
- not native-linker behavior
- not a general serialization system for all tooling data

**Relationship to CodeGen:**

- `CodeGen` and `Metadata Writer` are sibling subsystems.
- `CodeGen` may provide symbol inventories and body identities that `Metadata Writer` references.
- `Metadata Writer` should not own body-lowering logic.
- `CodeGen` should not own metadata layout policy.

**Relationship to Runtime:**

- `Metadata Writer` defines the persisted/bootstrap-facing metadata products that the runtime consumes.
- Runtime startup should conceptually follow a flow like:
  - bootstrap
  - load/attach registration artifacts
  - initialize metadata caches
  - expose runtime world

**Why this boundary matters:**

- otherwise generated code, registration tables, and runtime metadata layout will collapse into one tightly coupled implementation blob
- that would make:
  - reflection-lite evolution
  - metadata debugging
  - profile-specific runtime layouts
  - startup-flow changes
  much harder to manage

**Phase-1 minimum commitment:**

- phase 1 `Metadata Writer` should support at least:
  - type metadata needed for runtime bootstrap and class lookup
  - method metadata needed for supported reflection-lite/runtime lookup scenarios
  - code registration entries for generated method bodies in the supported subset
  - metadata registration entries for retained assemblies/types/methods needed by the runtime
  - string/literal/identifier sections needed by the supported subset
  - clean alignment with `runtime ABI` and startup flow

**Phase-1 non-goals:**

- full reflection parity
- maximal metadata compactness/optimization
- every advanced debug-symbol scenario
- fully general metadata servicing for unsupported dynamic features

**Practical conclusion:**

- `Metadata Writer` should be treated as the subsystem that turns the retained semantic world into the runtime's loadable metadata universe.
- In phase 1, the most important thing is not ultra-compact metadata encoding, but stable registration contracts, predictable startup behavior, and metadata outputs that are explicit enough to debug and evolve.

### 2026-04-05 21:13:42 +08:00 - Runtime ABI formal definition and phase-1 scope

**Question:** How should `Runtime ABI` be defined in this architecture, and what should phase 1 require from it?

**Definition summary:**

- `Runtime ABI` is the low-level binary/runtime contract between:
  - generated code
  - metadata/registration artifacts
  - runtime core
- It is not the same thing as the higher-level `CodeGen bridge`.
- In this project, `Runtime ABI` should define the stable callable and layout-visible surface that generated/native code is allowed to rely on across runtime startup, object access, method dispatch, class initialization, exception flow, threading, GC roots, and interop entry points.

**Key architectural distinction:**

1. `Runtime ABI`
   - low-level contract
   - calling conventions
   - object/type/runtime handle shapes
   - registration/bootstrap entry points
   - helper function signatures visible at the binary/native layer

2. `CodeGen bridge`
   - higher-level helper API presented to code generation
   - can wrap and simplify ABI details
   - can evolve more ergonomically as long as it still lowers onto the stable ABI

**What Runtime ABI should cover:**

- object model access points
  - object/type handles
  - array/string/object access helpers where layout must stay abstracted
- allocation entry points
- class initialization entry points
- method/code registration attach points
- metadata registration attach points
- exception throw/catch boundary helpers visible to generated code
- thread attach/detach and runtime-context entry points
- GC root registration/barrier-related callable surface for the supported subset
- interop/PInvoke callable surface for the supported subset

**What Runtime ABI should NOT cover directly:**

- high-level policy decisions from `Linker`
- semantic IR concerns from `Typed IL IR`
- template layout concerns from `Scriban`
- every convenience helper that codegen may want

**Recommended inputs to ABI design:**

- runtime object model requirements
- startup/bootstrap flow
- metadata registration requirements
- code registration requirements
- cross-platform calling-convention and toolchain constraints
- minimum interop and exception semantics promised in phase 1

**Recommended outputs/artifacts:**

- native header contracts such as:
  - runtime ABI headers
  - registration struct definitions
  - exported/imported helper signatures
- versioned ABI surface identifiers
- ABI conformance tests for generated/runtime integration

**Relationship to Runtime Core:**

- Runtime Core implements the ABI.
- Runtime ABI should remain smaller and more stable than the full internal runtime implementation.
- Internal refactors inside the runtime should ideally not force broad changes in generated code as long as the ABI contract remains intact.

**Relationship to CodeGen:**

- `CodeGen` should target the `CodeGen bridge`, not hardcode runtime internals.
- `CodeGen bridge` then maps to the stable `Runtime ABI`.
- This reduces churn when runtime internals evolve.

**Why this boundary matters:**

- otherwise generated code will directly learn too much about runtime internals
- that would cause:
  - large regeneration churn when runtime internals change
  - cross-platform fragility
  - harder bootstrap/versioning evolution
  - tighter coupling between codegen templates and runtime implementation details

**Phase-1 minimum commitment:**

- phase 1 `Runtime ABI` should support at least:
  - object allocation
  - type/class lookup handles needed by the supported subset
  - class initialization
  - method/code registration hookup
  - metadata registration hookup
  - exception throw/propagation helpers for the supported subset
  - thread attach/detach
  - GC root and object-lifetime interaction points required by the supported subset
  - `PInvokeLite` entry points for the supported subset

**Phase-1 non-goals:**

- perfect ABI stability across all future runtime generations
- full reflection or dynamic-code surfaces
- maximal platform specialization from day one
- exposing broad internal runtime implementation details as ABI

**Practical conclusion:**

- `Runtime ABI` should be treated as the minimal stable native contract that allows generated code, metadata artifacts, and runtime core to meet safely.
- In phase 1, the most important thing is not a huge ABI surface, but a small, versioned, explicit ABI that is sufficient for bootstrap, generated-code execution, and controlled runtime evolution.

### 2026-04-05 21:24:36 +08:00 - PAL formal definition and phase-1 scope

**Question:** How should `PAL` be defined in this architecture, and what should phase 1 require from it?

**Definition summary:**

- `PAL` should be defined as the `Platform Abstraction Layer` inside the runtime plane.
- It is the layer that hides OS/toolchain/platform differences from the rest of the runtime.
- It is not the same as:
  - `Runtime ABI`
  - `CodeGen bridge`
  - `Build`
- In this project, `PAL` should be the runtime-owned substrate for cross-platform services such as:
  - threading
  - synchronization
  - time/clock
  - file/path
  - environment/process queries
  - dynamic-library loading where needed
  - memory/page protection helpers where needed by the supported subset

**Key architectural distinction:**

1. `Runtime ABI`
   - contract between generated code, metadata artifacts, and runtime core

2. `Runtime Core`
   - managed-object/runtime semantics implementation

3. `PAL`
   - OS/platform-facing abstraction used by runtime core

4. `Build`
   - toolchain selection, packaging, target compilation, app/container/bundle layout

**What PAL should cover:**

- threads
  - current-thread identity
  - attach/detach support hooks
  - thread creation/join primitives as needed
- synchronization
  - mutex/monitor primitives
  - condition/event primitives as needed
- time
  - wall clock
  - monotonic clock
  - timers/sleep helpers as needed
- filesystem/path
  - directory/file existence
  - enumeration basics
  - path separators and normalization utilities where needed
- environment/process
  - environment variables
  - process identity/basic process info
- dynamic library loading
  - platform loader wrappers for the supported `PInvokeLite` subset
- virtual memory helpers
  - page allocation/protection primitives if required by the supported subset/runtime internals

**What PAL should NOT cover directly:**

- closure analysis from `Linker`
- metadata layout from `Metadata Writer`
- generated-code helper selection from `CodeGen`
- project-system or package-management concerns
- high-level runtime policy decisions that are not inherently platform-facing

**Relationship to Runtime Core:**

- Runtime Core should call into `PAL`, not directly spray OS-specific APIs everywhere.
- `PAL` should be narrower than "all native utilities" and focused on platform variance.
- Runtime Core policy should stay above `PAL`; `PAL` should mainly provide capability-normalized primitives.

**Relationship to Build:**

- `Build` selects target platform/toolchain and assembles artifacts.
- `PAL` is the runtime implementation surface that must work on those targets.
- These two layers influence each other but should remain separate:
  - `Build` is about producing the app
  - `PAL` is about how the runtime talks to the OS once the app runs

**Why this boundary matters:**

- otherwise platform-specific code will leak into runtime core, codegen assumptions, and even metadata/bootstrap logic
- that would make:
  - multi-platform support slower to expand
  - platform-specific bug isolation harder
  - desktop/mobile divergence more painful
  - future constrained-platform adaptations harder to reason about

**Phase-1 minimum commitment:**

- phase 1 `PAL` should support at least:
  - thread attach/detach related primitives
  - basic synchronization needed by class init/monitor support in the supported subset
  - wall-clock and monotonic-time helpers
  - filesystem/path helpers needed by the supported subset and driver/runtime interaction points
  - environment-variable access
  - dynamic-library load/lookup for the supported `PInvokeLite` subset
  - platform error propagation/translation into runtime-diagnostic forms where needed

**Phase-1 non-goals:**

- every OS facility under one giant abstraction
- early support for all possible platform specialties
- abstracting away all performance-sensitive platform differences
- turning `PAL` into a generic utility dumping ground

**Practical conclusion:**

- `PAL` should be treated as the runtime's cross-platform substrate, not as a catch-all native helper library.
- In phase 1, the most important thing is a small, disciplined PAL that supports the runtime subset cleanly across the first target platforms without leaking OS-specific details upward.

### 2026-04-05 21:36:58 +08:00 - GC facade formal definition and phase-1 scope

**Question:** How should `GC facade` be defined in this architecture, and what should phase 1 require from it?

**Definition summary:**

- `GC facade` should be defined as the runtime-owned abstraction layer between:
  - runtime object model / runtime core
  - concrete garbage-collection backend implementation
- It is not the same thing as:
  - object layout itself
  - `Runtime ABI`
  - a specific GC backend such as `BDWGC`
- In this project, `GC facade` should be the place where allocation, root registration, object scanning assumptions, pinning semantics for the supported subset, and GC lifecycle hooks are normalized for the rest of the runtime.

**Key architectural distinction:**

1. `Object model`
   - defines what managed objects, arrays, strings, headers, and type links conceptually look like

2. `Runtime ABI`
   - exposes stable callable/native contract points to generated code

3. `GC facade`
   - runtime-internal abstraction that translates runtime needs into a concrete GC backend

4. concrete GC backend
   - `BDWGC` in phase 1, or a future self-built collector later

**What GC facade should cover:**

- object allocation entry points used by runtime/object construction paths
- root registration/unregistration for the supported subset
- stack/thread registration hooks as needed by the chosen backend
- write-barrier/card-mark hooks if future backends need them, even if phase 1 backend does not
- pinning/handle semantics for the supported subset
- finalization-related hooks where the supported subset needs them
- GC lifecycle hooks:
  - startup/shutdown attach points
  - collection trigger requests
  - backend statistics/telemetry hooks as needed

**What GC facade should NOT cover directly:**

- high-level metadata decisions
- closure/reachability logic from `Linker`
- codegen template policy
- platform abstraction that belongs to `PAL`
- pretending all backends have identical capabilities when they do not

**Relationship to BDWGC:**

- `BDWGC` should be treated as the initial backend/reference implementation behind the facade.
- The architecture should not let `BDWGC` become the de facto runtime object-model spec.
- If `BDWGC` limitations require temporary compromises, those compromises should be recorded as backend-specific policy, not silently promoted to permanent core semantics.

**Relationship to Runtime Core:**

- Runtime Core should allocate and manage roots through the `GC facade`, not directly through backend APIs.
- This keeps future collector replacement feasible and helps isolate backend-specific assumptions.

**Relationship to Runtime ABI / CodeGen:**

- Generated code should target stable allocation/root-related ABI helpers.
- Those helpers are implemented by runtime core and ultimately delegated through the `GC facade`.
- Generated code should not know concrete backend APIs such as `BDWGC` symbols directly.

**Why this boundary matters:**

- otherwise:
  - generated code
  - runtime core
  - object layout
  - backend collector
  will all become coupled to one backend's assumptions
- that would make future collector replacement or even backend-tuning much more expensive

**Phase-1 minimum commitment:**

- phase 1 `GC facade` should support at least:
  - managed object allocation for the supported subset
  - root registration needed by the supported subset and runtime bootstrap
  - thread/stack registration hooks required by the chosen backend
  - enough handle/pinning semantics for the supported `PInvokeLite` subset
  - collection trigger hooks and backend startup/shutdown integration
  - diagnostic/stat reporting hooks sufficient for bring-up and basic verification

**Phase-1 non-goals:**

- a perfect collector-neutral abstraction for every future GC style
- full finalization/reference-queue sophistication
- maximum performance tuning across all target platforms
- pretending `BDWGC` and a future moving collector have identical semantics

**Practical conclusion:**

- `GC facade` should be treated as the runtime's collector abstraction boundary, with `BDWGC` as the initial backend rather than the architecture center.
- In phase 1, the most important thing is to keep allocation/rooting/backend lifecycle interactions explicit and replaceable, even if the first backend is conservative and the abstraction is still incomplete.

### 2026-04-05 21:49:32 +08:00 - Current GC options comparison for this project

**Question:** Are there currently better GC options than `BDWGC` for this IL2CPP-like project?

**Conclusion summary:**

- There is no single universally "better" GC choice; it depends on whether the priority is:
  - fastest phase-1 bring-up
  - long-term runtime quality/performance
  - lowest integration risk across platforms
- For this project's current phase, `BDWGC` still remains the strongest phase-1 baseline.
- The best long-term direction is still a project-owned precise collector behind the `GC facade`.

**Option comparison:**

1. `BDWGC`
   - best for:
     - fastest phase-1 bring-up
     - low integration friction
     - broad portability
   - weaknesses:
     - conservative GC limitations
     - not the ideal long-term collector model for high-fidelity managed-runtime evolution
   - project fit:
     - best current baseline backend

2. project-owned precise collector
   - best for:
     - long-term control
     - better managed-runtime fidelity
     - future generational/moving collector evolution
   - weaknesses:
     - highest engineering cost
     - too expensive as a phase-1 prerequisite
   - project fit:
     - best long-term target, not best phase-1 bootstrap strategy

3. `MMTk`
   - best for:
     - modern GC architecture exploration
     - access to multiple collector strategies behind one toolkit
   - weaknesses:
     - not yet the safest deployment-grade choice for this project's phase-1 baseline
     - would introduce a new Rust-based integration surface and binding effort
   - project fit:
     - strong experimental/phase-2 candidate, not current first choice

4. `Mono SGen`
   - best for:
     - showing what a generational collector for a managed runtime can look like
   - weaknesses:
     - tightly coupled to Mono runtime embedding model and internals
     - poor fit as a drop-in collector for this project's self-built runtime architecture
   - project fit:
     - reference inspiration, not recommended as the backend choice

5. `CoreCLR GC`
   - best for:
     - high-performance production managed-runtime GC in the `.NET` ecosystem
   - weaknesses:
     - deeply tied to the broader CoreCLR runtime implementation
     - not a practical phase-1 reusable collector component for this architecture
   - project fit:
     - reference baseline/inspiration, not recommended as an integration target

**Practical recommendation:**

- phase 1:
  - keep `BDWGC` behind `GC facade`
  - design the facade so future moving/generational collectors remain possible
- phase 2:
  - decide whether to:
    - begin a project-owned precise collector
    - or evaluate `MMTk` on an isolated experimental branch
- do not couple the runtime object model directly to `BDWGC` assumptions

**Decision heuristic:**

- If the question is "what is the best phase-1 backend right now?"
  - answer: `BDWGC`
- If the question is "what is the best long-term architecture?"
  - answer: a project-owned precise collector, with `GC facade` already prepared for it
- If the question is "what is the strongest external modern research/engineering candidate beyond BDWGC?"
  - answer: `MMTk`, but as an experiment rather than the mainline default

### 2026-04-05 21:56:28 +08:00 - MMTk candidate explicitly deferred

**Decision summary:**

- User decided not to record `MMTk` as a future third-party candidate for now.
- Policy:
  - keep the current decision focused on `BDWGC` as the phase-1 backend behind `GC facade`
  - keep long-term direction aimed at a project-owned precise collector
  - revisit `MMTk` only if a concrete later need justifies reopening the comparison

### 2026-04-05 22:02:44 +08:00 - Interop / Marshaling bridge formal definition and phase-1 scope

**Question:** How should the `Interop / Marshaling bridge` be defined in this architecture, and what should phase 1 require from it?

**Definition summary:**

- `Interop / Marshaling bridge` should be defined as the project-owned subsystem that translates between managed calling/data semantics and native ABI/platform calling semantics.
- It is not:
  - `Runtime ABI` itself
  - `PAL`
  - a generic dynamic foreign-function engine
  - a replacement for generated wrappers
- In this project, it is the layer that owns:
  - marshaling rules for the supported subset
  - generated wrapper contracts
  - native-library entry resolution for the supported subset
  - callback/trampoline boundaries for the supported subset

**Key architectural distinction:**

1. `Runtime ABI`
   - stable low-level contract exposed to generated code

2. `PAL`
   - platform-facing substrate for dynamic library loading and related OS services

3. `Interop / Marshaling bridge`
   - policy and helper layer that turns managed signatures into native-callable bridges and back

4. concrete native calls
   - resolved target functions and platform ABI details at runtime/build time

**What the Interop / Marshaling bridge should cover:**

- `PInvokeLite` signature mapping for the supported subset
- parameter marshaling helpers for the supported subset
- return-value marshaling helpers for the supported subset
- string and blittable-structure policies for the supported subset
- library/symbol resolution flow built on top of `PAL`
- callback/delegate trampoline surfaces for the supported subset
- handle/pinning coordination with `GC facade` where needed

**What it should NOT cover directly:**

- generalized reflection-driven arbitrary signature invocation
- full dynamic invocation engines
- deep platform API surface modeling unrelated to marshaling
- direct backend-specific GC logic
- metadata retention or closure analysis

**Relationship to libffi:**

- phase 1 path:
  - no `libffi`
  - prefer generated wrappers and project-owned marshaling helpers
- future path:
  - `libffi` can be reconsidered only if generalized/dynamic invocation pressure becomes strong enough

**Relationship to CodeGen:**

- `CodeGen` should emit wrapper stubs and helper calls for the supported interop subset.
- The interop subsystem owns the semantics/policies of those helpers.
- This keeps `CodeGen` from becoming the place where marshaling policy is hardcoded in templates alone.

**Relationship to PAL and GC facade:**

- `PAL` provides:
  - dynamic library load/lookup
  - platform-facing primitives
- `GC facade` provides:
  - rooting/pinning/handle coordination where needed
- `Interop / Marshaling bridge` sits above both and composes them for the supported interop subset

**Phase-1 minimum commitment:**

- phase 1 should support at least:
  - `PInvokeLite`
  - blittable primitive arguments/returns
  - basic string marshaling for the chosen supported subset
  - native-library load and symbol lookup
  - explicit diagnostics for unsupported signatures or marshaling cases
  - minimal delegate/callback support only if required by the agreed smoke/golden subset

**Phase-1 non-goals:**

- arbitrary signature adaptation at runtime
- full COM/WinRT-style interop stacks
- broad custom marshaler support
- full reflection-driven invoke support
- pretending all supported platforms have identical native ABI corner cases

**Practical conclusion:**

- `Interop / Marshaling bridge` should be treated as an explicit runtime/codegen sibling subsystem, not as a pile of ad hoc helper functions.
- In phase 1, the most important thing is a small, explicit, diagnosable `PInvokeLite` bridge built on generated wrappers, `PAL`, and `GC facade`, not a general-purpose dynamic interop engine.

### 2026-04-05 22:15:36 +08:00 - Phase-1 implementation priority reordering after module closure

**Question:** After defining the critical self-built modules, what should the actual phase-1 implementation order be?

**Conclusion summary:**

- Phase 1 should now be treated as a vertical-slice program, not as parallel construction of all big modules at once.
- The correct priority is to first stabilize contracts and execution skeletons, then build the minimal semantic/runtime loop, and only after that let full codegen expand.
- `CodeGen` is important, but it is not the first major implementation target.

**Recommended critical-path order:**

1. `Contracts + shared execution skeleton`
   - freeze:
     - request objects
     - phase boundaries
     - context/views
     - results/collectors
     - runtime ABI headers
     - codegen bridge headers
   - reason:
     - every later subsystem depends on these boundaries staying stable enough to iterate on

2. `Loader + Metadata Graph + Typed IL IR`
   - establish:
     - assembly loading
     - metadata graph
     - typed method-level IR
   - reason:
     - the whole system needs a trustworthy semantic world before any serious AOT or codegen work

3. `Runtime bootstrap minimum`
   - establish:
     - runtime ABI implementation shell
     - startup flow
     - PAL minimum
     - GC facade minimum with `BDWGC`
     - class-init/object-allocation/thread-attach basics
   - reason:
     - without a real runtime landing zone, generated outputs and metadata products have nowhere stable to attach

4. `Linker minimum`
   - establish:
     - root discovery
     - direct-call closure
     - supported-subset virtual/interface/generic closure
     - preserve diagnostics
     - canonical `AOT manifest`
   - reason:
     - codegen and metadata emission should consume a stable retained world rather than infer retention ad hoc

5. `Metadata Writer minimum`
   - establish:
     - metadata registration
     - code registration model
     - minimum metadata blobs/tables
     - startup-consumable registration artifacts
   - reason:
     - runtime bootstrap depends on these products just as much as it depends on generated method bodies

6. `CodeGen minimum vertical slice`
   - establish:
     - `Runtime-Contract IR -> emission model`
     - `Scriban` templates for a very small supported subset
     - generated wrappers and method bodies for `HelloWorld`-class scenarios
   - reason:
     - by this point generated code can target a real ABI, real retained world, and real registration flow

7. `Single-platform end-to-end slice`
   - goal:
     - one reference desktop platform
     - build, launch, bootstrap, execute, and verify a minimal golden sample
   - reason:
     - this is the first moment where the architecture is proven as a system rather than as isolated subsystems

8. `Semantic expansion pass`
   - extend:
     - exceptions
     - more generics
     - delegate paths
     - reflection-lite
     - `PInvokeLite`
   - reason:
     - expand coverage only after the smallest loop works

9. `Platform expansion pass`
   - extend:
     - Android/iOS compile/package/startup smoke
     - later semantic subsets on those platforms
   - reason:
     - keep multi-platform from day one architecturally, but do not demand full platform parity before the first vertical slice exists

**What can run in parallel after step 1:**

- `Loader/IR` work and `Runtime bootstrap minimum` can overlap to some extent
- `Linker minimum` can start once enough semantic world exists
- `Metadata Writer minimum` can start once manifest shape and runtime registration shape are stable enough

**What should NOT be pulled forward too early:**

- wide `CodeGen` expansion before runtime/bootstrap/registration are stable
- advanced trimming sophistication before canonical manifest and diagnostics are stable
- broad interop/globalization work before the first end-to-end vertical slice is alive
- multi-platform semantics parity before the reference desktop slice exists

**Practical conclusion:**

- The first real proof point for phase 1 is not "generated a lot of C++", but:
  - one retained world
  - one metadata registration flow
  - one runtime bootstrap
  - one generated-code path
  - one sample that builds and runs correctly on the reference platform

### 2026-04-05 22:24:18 +08:00 - First vertical-slice sample recommendation

**Question:** What should the first true end-to-end vertical-slice sample be for phase 1?

**Selection criteria:**

- must exercise the whole system, not just frontend parsing
- must be small enough to debug across:
  - `Loader`
  - `Linker`
  - `Metadata Writer`
  - `CodeGen`
  - `Runtime bootstrap`
- should avoid introducing too many independent semantic variables at once
- should still require:
  - one managed entry point
  - one retained type world
  - one method body generation path
  - one metadata registration path
  - one runtime bootstrap path

**Candidate shapes considered:**

1. `HelloWorld`
   - strengths:
     - smallest possible closed loop
     - easiest to debug
   - weaknesses:
     - too weak to prove that type metadata, field access, or basic object semantics really work

2. `HelloObject`
   - proposed shape:
     - one small managed type
     - one instance field
     - one constructor
     - one instance method returning a string or integer
     - `Main` allocates the object, calls the method, prints the result
   - strengths:
     - still very small
     - proves:
       - object allocation
       - constructor call
       - instance method dispatch in the simplest form
       - field access
       - string/int output path
       - metadata registration for at least one user type
     - much more meaningful than pure `HelloWorld`
   - weaknesses:
     - slightly more moving parts than `HelloWorld`, but still manageable

3. `GenericEcho`
   - strengths:
     - immediately stresses generic instantiation
   - weaknesses:
     - too early for the first vertical slice
     - adds semantic complexity before the bootstrap loop is stable

4. `ReflectionLite` / `PInvokeLite`
   - strengths:
     - validates important later subsystems
   - weaknesses:
     - not appropriate for the first slice
     - should come only after the minimal managed-object loop is alive

**Current recommendation:**

- The first real vertical-slice sample should be `HelloObject`, not pure `HelloWorld`.
- Recommended minimal sample behavior:
  - define a small type like `Greeter`
  - constructor stores one field
  - instance method computes or returns one value
  - `Main` creates the object and prints the result

**Why this is the best first proof point:**

- It is still small enough to debug end-to-end.
- It proves substantially more architecture than `HelloWorld`.
- It avoids pulling forward generics, reflection, exceptions, or interop too early.

**Recommended immediate sequence after the first slice:**

1. `HelloObject`
2. `GenericEcho`
3. `ExceptionLite`
4. `ReflectionLite`
5. `PInvokeLite`

**Practical conclusion:**

- `HelloWorld` can still exist as a smoke micro-case.
- But the first architecture-proof vertical slice should be `HelloObject`, because it is the smallest sample that proves the system is beginning to behave like a managed runtime rather than just a code emitter.

### 2026-04-05 22:34:12 +08:00 - HelloWorld and HelloObject unified

**Decision summary:**

- User requested to unify the previous `HelloWorld` micro-case and the proposed `HelloObject` first vertical-slice sample.
- Going forward, the first vertical-slice sample is simply named `HelloWorld`.
- In meaning, this `HelloWorld` is the richer object-based version, not the older pure "print a literal only" micro-case.

### 2026-04-05 22:34:12 +08:00 - HelloWorld vertical-slice contract

**Definition summary:**

- The unified `HelloWorld` sample should be the smallest end-to-end sample that proves:
  - managed entry execution
  - managed object allocation
  - constructor invocation
  - instance field storage/load
  - instance method call
  - string/int result flow into console output
  - runtime bootstrap
  - metadata registration
  - generated-code execution

**Recommended C# shape:**

```csharp
using System;

internal sealed class Greeter
{
    private readonly string _name;

    public Greeter(string name)
    {
        _name = name;
    }

    public string BuildMessage()
    {
        return "Hello, " + _name + "!";
    }
}

internal static class Program
{
    private static int Main()
    {
        var greeter = new Greeter("World");
        Console.WriteLine(greeter.BuildMessage());
        return 0;
    }
}
```

**Expected observable result:**

- standard output:
  - `Hello, World!`
- process exit code:
  - `0`

**Why this exact shape is preferred:**

- it is still extremely small
- it exercises more real managed-runtime behavior than a literal-only print
- it avoids pulling in:
  - generics
  - exceptions
  - reflection
  - interop

**Minimum subsystem requirements implied by this sample:**

1. `Loader / Semantic World`
   - load one assembly with one user type
   - decode constructor and instance-method bodies
   - represent field access, object construction, method call, and string literal usage

2. `Linker`
   - retain:
     - `Program.Main`
     - `Greeter..ctor`
     - `Greeter.BuildMessage`
     - `Greeter._name`
     - required `System.Console` and string-related runtime/library roots for the supported subset
   - emit a stable `AOT manifest`

3. `Metadata Writer`
   - register the assembly
   - register the `Greeter` type
   - register the required method/code identities
   - emit the minimum string/literal/metadata sections needed by runtime lookup and bootstrap

4. `Runtime ABI / Runtime Core`
   - bootstrap runtime world
   - allocate one managed object
   - run constructor
   - support field store/load
   - support instance method call in the supported simplest form
   - support enough string behavior for the chosen message path

5. `GC facade`
   - allocate `Greeter`
   - manage at least the involved object/string roots for the supported subset

6. `CodeGen`
   - emit method bodies for:
     - `Program.Main`
     - `Greeter..ctor`
     - `Greeter.BuildMessage`
   - emit required wrapper/helper calls into runtime ABI/codegen bridge

**Important scoping note:**

- This `HelloWorld` sample may depend on a very small supported BCL subset, but it should not silently drag in broad `System.Console` or full string-semantic parity claims.
- If necessary, phase 1 may route console output through a tightly controlled supported path while keeping the externally observed result the same.

**Recommended immediate follow-on samples after HelloWorld:**

1. `GenericEcho`
2. `ExceptionLite`
3. `ReflectionLite`
4. `PInvokeLite`

**Practical conclusion:**

- The unified `HelloWorld` sample is now the canonical first vertical slice.
- It should be treated as the first full-system proof case, not merely as a toy print test.

### 2026-04-05 22:42:26 +08:00 - HelloWorld contract confirmed and refined into executable minimums

**Decision summary:**

- User agreed with the unified `HelloWorld` vertical-slice contract.
- The next step is to freeze the minimum executable envelope for this sample so it can guide implementation rather than remain only a conceptual example.

### 2026-04-05 22:42:26 +08:00 - HelloWorld minimum executable envelope

**Question:** What is the minimum IL/BCL/runtime surface that the unified `HelloWorld` sample should require?

**Recommended minimum IL subset:**

- object construction and arguments
  - `newobj`
  - `ldarg.*`
  - `ldloc.*`
  - `stloc.*`
- fields
  - `ldfld`
  - `stfld`
- literals and constants
  - `ldstr`
  - `ldc.i4.0`
- calls
  - `call`
  - `callvirt` only in the simplest phase-1 sense:
    - allow the common C# pattern where a non-virtual instance method is emitted as `callvirt`
    - lower it as a null-check + direct instance call for this sample
- function end
  - `ret`
- ignorable structural instructions
  - `nop` if present

**Important boundary for phase 1:**

- true general virtual dispatch should not be required by the first `HelloWorld` slice
- but the implementation must tolerate the common C# compiler pattern where non-virtual instance calls on reference types appear as `callvirt`

**Recommended minimum BCL subset:**

- `System.Object`
- `System.String`
- `System.Console.WriteLine(string)`
- one supported string-concatenation path, preferably:
  - `System.String.Concat(string, string, string)`

**Reasoning:**

- This keeps the first slice realistic while still tightly scoped.
- It avoids prematurely dragging in broad BCL coverage.
- It also lets the sample preserve its current visible behavior: `Hello, World!`

**Alternative implementation allowance:**

- If supporting `System.String.Concat(string, string, string)` immediately is inconvenient, the system may temporarily lower this specific pattern through a tightly controlled runtime helper, as long as:
  - the observable sample behavior stays the same
  - the shortcut is explicitly recorded as a phase-1 limitation rather than silently treated as general string support

**Recommended minimum runtime/helper surface:**

1. bootstrap and registration
   - runtime startup entry
   - code registration attach
   - metadata registration attach

2. object/type basics
   - allocate managed object
   - access type/class handle for the supported subset
   - run constructor
   - instance field store/load for reference fields in the supported subset

3. call helpers
   - direct instance-method invocation path
   - `callvirt`-as-null-check helper or equivalent lowering for the supported phase-1 pattern

4. string support
   - load/materialize string literals
   - support one narrow concat path needed by the sample

5. console/output support
   - support one narrow `Console.WriteLine(string)` path

6. return path
   - return managed `int` exit code to native host

**Minimum linker retention expectations for HelloWorld:**

- retain:
  - `Program.Main`
  - `Greeter..ctor`
  - `Greeter.BuildMessage`
  - `Greeter._name`
  - required `System.Console.WriteLine(string)` target
  - required `System.String.Concat(string, string, string)` target if that path is chosen
  - required runtime/bootstrap metadata roots for the supported subset

**Minimum metadata-writer expectations for HelloWorld:**

- register the assembly
- register the `Greeter` type
- register the methods used by the sample
- emit string/literal metadata required by the chosen runtime string path

**Practical conclusion:**

- The first `HelloWorld` slice should be treated as a tightly scoped executable contract.
- Its power comes from being small but complete, not from pulling in broad semantic coverage too early.

### 2026-04-05 22:48:06 +08:00 - HelloWorld implementation checkpoints confirmed

**Decision summary:**

- User agreed to refine the unified `HelloWorld` sample into concrete implementation checkpoints.
- These checkpoints are now the recommended entry path for turning the sample contract into executable engineering work.

### 2026-04-05 22:48:06 +08:00 - HelloWorld implementation checkpoints

**Checkpoint 1 - Loader / IR view**

- `Loader` and `Typed IL IR` should be able to recognize and normalize at least:
  - one managed entry point
  - one object construction path
  - one constructor body
  - one instance method body
  - one field store/load path
  - one string-literal path
  - one `Console.WriteLine(string)` call path
- The output should be inspectable enough that this sample can be snapshot-tested without depending on later runtime stages.

**Checkpoint 2 - Linker roots and closure**

- `Linker` should deterministically retain at least:
  - `Program.Main`
  - `Greeter..ctor`
  - `Greeter.BuildMessage`
  - `Greeter._name`
  - required `System.Console.WriteLine(string)` target
  - required string-concat path if the sample uses `System.String.Concat(string, string, string)`
  - required bootstrap/runtime metadata roots for the supported subset
- The resulting `AOT manifest` should be small, explicit, and snapshot-testable.

**Checkpoint 3 - Metadata Writer products**

- `Metadata Writer` should emit at least:
  - one assembly registration entry
  - one user-type registration entry for `Greeter`
  - method/code identities for the sample's methods
  - string/literal metadata required by the chosen string path
  - code and metadata registration products consumable by runtime bootstrap
- These outputs should be stable enough to diff and inspect independently of generated code bodies.

**Checkpoint 4 - Runtime minimum helpers**

- `Runtime` should provide at least:
  - runtime startup/bootstrap
  - object allocation
  - constructor execution support
  - instance field store/load support
  - direct instance call support
  - the narrow `callvirt` null-check path needed by the sample
  - string-literal materialization
  - one narrow string-concat path
  - one narrow `Console.WriteLine(string)` path
  - managed `int` exit code return path to the native host

**Checkpoint 5 - CodeGen first-emission set**

- `CodeGen` should initially emit only the smallest required body set:
  - `Program.Main`
  - `Greeter..ctor`
  - `Greeter.BuildMessage`
- The first generated output should prioritize:
  - correctness
  - readability
  - stable helper calls into `CodeGen bridge` / `Runtime ABI`
- It should not try to solve wider language/runtime coverage before this body set runs end-to-end.

**Practical conclusion:**

- `HelloWorld` is now not only a sample, but also a five-checkpoint implementation entry.
- Future implementation planning should treat these checkpoints as the first executable ladder for phase 1 rather than trying to open all subsystems uniformly at once.

### 2026-04-05 22:53:54 +08:00 - HelloWorld Checkpoint 1 refined into method-level IL expectations

**Question:** What concrete IL/IR shapes should `Loader / Typed IL IR` expect for the first `HelloWorld` slice?

**Important note:**

- Exact emitted IL can vary by compiler/version.
- The goal here is not byte-for-byte IL freezing, but freezing the semantic IL patterns that phase 1 must be able to load and normalize.

**Method 1 - `Greeter..ctor(string)`**

- expected semantic pattern:
  - load `this`
  - call base `System.Object..ctor`
  - load `this`
  - load argument `name`
  - store into field `_name`
  - return
- representative opcode family:
  - `ldarg.0`
  - `call`
  - `ldarg.0`
  - `ldarg.1`
  - `stfld`
  - `ret`
- implication for `Typed IL IR`:
  - must represent:
    - constructor call to base
    - field store on current instance

**Method 2 - `Greeter.BuildMessage()`**

- expected semantic pattern:
  - load literal `"Hello, "`
  - load `this`
  - load field `_name`
  - load literal `"!"`
  - call chosen concat path
  - return string
- representative opcode family:
  - `ldstr`
  - `ldarg.0`
  - `ldfld`
  - `ldstr`
  - `call`
  - `ret`
- implication for `Typed IL IR`:
  - must represent:
    - string literals
    - instance field load
    - static helper call returning string

**Method 3 - `Program.Main()`**

- expected semantic pattern:
  - load literal `"World"`
  - construct `Greeter`
  - store local
  - load local
  - invoke `BuildMessage`
  - pass returned string to `Console.WriteLine(string)`
  - load integer `0`
  - return
- representative opcode family:
  - `ldstr`
  - `newobj`
  - `stloc.*`
  - `ldloc.*`
  - `callvirt` or `call` for the instance method pattern depending on compiler emission
  - `call`
  - `ldc.i4.0`
  - `ret`
- implication for `Typed IL IR`:
  - must represent:
    - object construction
    - local variable store/load
    - instance method invocation
    - static console call
    - integer return

**Phase-1 normalization rule for this sample:**

- the IR layer should normalize the sample into a small set of explicit actions such as:
  - construct object
  - store/load field
  - call base constructor
  - call instance method
  - call static helper
  - return constant
- It should not require later phases to reason directly in terms of raw stack juggling just to understand this sample.

**Practical conclusion:**

- If `Loader / Typed IL IR` cannot cleanly normalize these three method shapes, the project is not yet ready to treat `HelloWorld` as a serious vertical-slice entry point.

### 2026-04-05 23:07:12 +08:00 - HelloWorld Checkpoint 2 refined into Linker roots, manifest shape, and diagnostics

**Question:** What exact root set, manifest shape, and diagnostics should `Linker` produce for the first `HelloWorld` slice?

**Recommended explicit root set:**

1. entry roots
   - `Program.Main`

2. user-type roots discovered from direct reachability
   - `Greeter..ctor(string)`
   - `Greeter.BuildMessage()`
   - `Greeter._name`

3. framework/BCL roots required by the supported path
   - `System.Object..ctor`
   - `System.Console.WriteLine(string)`
   - chosen string-concat target:
     - preferred `System.String.Concat(string, string, string)`
     - or an explicitly documented phase-1 runtime helper path if concat is temporarily lowered differently

4. runtime/bootstrap roots
   - assembly/type metadata needed for:
     - assembly registration
     - `Greeter` type lookup
     - method/code registration for the three sample methods
   - string-literal metadata roots for:
     - `"Hello, "`
     - `"World"`
     - `"!"`

**Important linker rule for HelloWorld:**

- No "smart trimming" should be attempted beyond what is required for a stable and explainable retained world.
- The first slice should prefer explicit retention and predictable diagnostics over aggressive reduction.

**Recommended minimum `AOT manifest` shape for HelloWorld:**

- `entry_points`
  - one entry: `Program.Main`
- `assemblies`
  - at least the sample assembly
  - framework assembly identities required by the supported path
- `types`
  - at least `Program`
  - `Greeter`
  - referenced framework types needed by the supported subset
- `methods`
  - `Program.Main`
  - `Greeter..ctor`
  - `Greeter.BuildMessage`
  - required framework methods on the chosen path
- `fields`
  - `Greeter._name`
- `string_literals`
  - `"Hello, "`
  - `"World"`
  - `"!"`
- `metadata_retention`
  - the subset required by runtime bootstrap and the sample's supported reflection-lite needs, if any
- `runtime_features`
  - feature flags such as:
    - `console_write_line_string`
    - `string_concat_3`
    - `object_allocation`
    - `instance_field_ref`

**Recommended diagnostics policy:**

1. supported path
   - no warning if the sample is fully within the supported phase-1 subset

2. unsupported path
   - if the sample is rewritten in a way that requires unsupported semantics, emit explicit diagnostics such as:
     - unsupported `P/Invoke`
     - unsupported reflection path
     - unsupported non-trivial virtual dispatch

3. uncertain path
   - for HelloWorld there should ideally be zero uncertain-closure diagnostics
   - if any uncertainty appears in this sample, it should be treated as a design problem in the linker subset rather than accepted as normal

**Snapshot/testing recommendation:**

- `HelloWorld` should have:
  - manifest snapshot
  - retained-member snapshot
  - no-diagnostics expectation snapshot for the happy path

**Practical conclusion:**

- For the first vertical slice, `Linker` should prove that it can produce a tiny, explicit, no-surprises retained world.
- If the `HelloWorld` manifest is hard to read or explain, the linker boundary is still too fuzzy.

### 2026-04-05 23:07:12 +08:00 - HelloWorld Checkpoint 3 refined into Metadata Writer products and registration shape

**Question:** What concrete registration and metadata products should `Metadata Writer` emit for the first `HelloWorld` slice?

**Minimum product set:**

1. code registration product
   - stable identities for:
     - `Program.Main`
     - `Greeter..ctor`
     - `Greeter.BuildMessage`
   - links from retained method identity to generated body identity

2. metadata registration product
   - retained assembly identities
   - retained type identities
   - retained method identities
   - retained field identities

3. metadata blob/table package
   - type metadata for `Greeter`
   - field metadata for `_name`
   - method metadata for the retained methods
   - string/literal sections needed by the sample

4. lookup-map support
   - token-or-identity to runtime metadata handle mapping for the supported subset
   - code-registration lookup used during runtime bootstrap or invocation wiring

**Recommended shape constraints:**

- products should be explicit and diffable
- products should not depend on generated C++ parsing in order to be understood
- products may be emitted as:
  - structured intermediate files
  - generated `.cpp/.h` registration units
  - or both
- but ownership and meaning should stay with `Metadata Writer`, not be hidden inside templates

**Minimum registration data that must be representable:**

- assembly name/identity
- type name/namespace and containing assembly
- field name/type identity for `_name`
- method name/signature identity
- generated body symbol identity
- any startup ordering/registration section needed by the runtime

**HelloWorld-specific success condition:**

- runtime startup must be able to:
  - discover the sample assembly/type/method world
  - wire generated method bodies to runtime-visible identities
  - materialize the string/literal metadata needed by the sample path

**Snapshot/testing recommendation:**

- `HelloWorld` should have:
  - code-registration snapshot
  - metadata-registration snapshot
  - minimum metadata blob/table snapshot or structured view snapshot

**Practical conclusion:**

- For the first slice, `Metadata Writer` is successful when the registration story is boring, explicit, and inspectable.
- If sample execution depends on hidden ad hoc registration logic, this checkpoint is not actually done.

### 2026-04-05 23:07:12 +08:00 - HelloWorld Checkpoint 4 refined into runtime bootstrap and minimum helper surface

**Question:** What exact runtime helper and bootstrap path should exist for `HelloWorld` to run end-to-end?

**Recommended bootstrap sequence:**

1. native host enters runtime startup
2. runtime core attaches code registration
3. runtime core attaches metadata registration
4. runtime initializes minimal metadata caches/world state
5. runtime prepares thread attachment/current-thread context
6. runtime locates `Program.Main`
7. runtime transfers control into generated code
8. generated code returns managed exit code to host

**Minimum runtime helper families required:**

1. registration/bootstrap helpers
   - runtime startup/init
   - attach code registration
   - attach metadata registration
   - lookup entry method in the supported path

2. object/type helpers
   - allocate object of retained type
   - access type handle/class info for `Greeter`
   - run class-init path if the chosen implementation requires it for the supported subset

3. method/dispatch helpers
   - direct instance call path
   - phase-1 `callvirt` null-check path for the non-virtual instance-call pattern

4. field helpers
   - store reference field `_name`
   - load reference field `_name`

5. string helpers
   - materialize string literals
   - perform one narrow concat path

6. console/output helpers
   - one narrow `Console.WriteLine(string)` support path

7. thread/GC coordination helpers
   - attach current thread
   - establish roots needed by the sample
   - interact with `GC facade` for the supported subset

8. exit/host helpers
   - convert managed `int` return value to process exit code

**Recommended phase-1 narrowness rule:**

- each helper should exist because the sample needs it, not because a broad future runtime might need it someday
- helper count should stay intentionally small and traceable back to sample requirements

**Testing recommendation:**

- `HelloWorld` should have:
  - bootstrap trace expectation
  - no-fault startup expectation
  - correct stdout expectation
  - correct exit-code expectation

**Practical conclusion:**

- The runtime checkpoint is complete when the sample can enter through bootstrap, execute generated code, and return through a controlled host path with no hidden manual patch-up logic.

### 2026-04-05 23:07:12 +08:00 - HelloWorld Checkpoint 5 refined into CodeGen first-emission strategy

**Question:** What should `CodeGen` emit first for the `HelloWorld` slice, and what should be intentionally postponed?

**First emission scope:**

- emit only the smallest required method-body set:
  - `Program.Main`
  - `Greeter..ctor`
  - `Greeter.BuildMessage`

**Recommended first-emission strategy:**

1. method-level lowering
   - `Typed IL IR` -> `Runtime-Contract IR` for the three methods only

2. emission model
   - convert `Runtime-Contract IR` into a minimal emission DTO/model consumed by `Scriban`

3. generated output set
   - one or a few generated `.cpp` translation units for the sample methods
   - minimum generated declarations/includes needed to call into `CodeGen bridge` / `Runtime ABI`

4. helper usage policy
   - prefer explicit helper calls over clever in-template logic
   - keep generated code readable enough for first-slice debugging

**What should be postponed deliberately:**

- broad template abstraction for many language/runtime features
- wide instruction coverage not needed by the sample
- aggressive codegen optimization
- multi-platform emission specialization beyond what the reference platform requires
- general-purpose interop or reflection lowering

**Recommended generated-code qualities for the first slice:**

- readable
- stable enough for diff/snapshot review
- explicit runtime-helper usage
- low surprise factor when debugging bootstrap or registration issues

**Minimum generated-code success condition:**

- generated method bodies compile on the reference platform
- generated calls line up with `Runtime ABI` / `CodeGen bridge`
- no manual handwritten per-method rescue path is needed to make the sample succeed

**Practical conclusion:**

- For the first slice, `CodeGen` should optimize for explicitness and debuggability rather than elegance or generality.
- If the first generated bodies are hard to inspect, the project is making codegen too ambitious too early.

### 2026-04-05 23:16:48 +08:00 - HelloWorld vertical-slice specification confirmed

**Decision summary:**

- User confirmed the frozen `HelloWorld` vertical-slice specification.
- This specification is now treated as the official phase-1 first vertical slice.
- The confirmed scope includes:
  - unified object-based `HelloWorld`
  - method-level IL expectations
  - linker roots and manifest shape
  - metadata-writer minimum products
  - runtime bootstrap/helper minimum
  - codegen first-emission strategy

**Next intended action:**

- Use this confirmed `HelloWorld` slice as the normalization point when syncing:
  - `design`
  - `roadmap`
  - `implementation plan`

### 2026-04-05 23:43:52 +08:00 - User confirmed current frozen package and document normalization direction

**Decision summary:**

- User confirmed the current frozen package after the `HelloWorld` slice, module-boundary, and phase-ordering convergence.
- This confirmation covers:
  - compiler skeleton and top-level module split
  - `Linker / Metadata Writer / Runtime ABI / PAL / GC facade / Interop` boundaries
  - Phase 1 runtime dependency narrowness
  - `HelloWorld` as the first formal vertical slice
  - the refined execution entrance for `roadmap / implementation plan`

**Execution note:**

- The temporary `2026-04-05 Sync Update` tail sections in:
  - `design-v1-01.md`
  - `roadmap-v1-01.md`
  - `plan-2026-03-31-23-il2cpp-like-cs-to-cpp-implementation-roadmap.md`
- have been normalized into stable正文 sections instead of continuing as long-term override patches.

**Practical conclusion:**

- The document set is no longer in a “draft sync” state.
- The next planning entry should be based on the normalized正文 and the frozen `HelloWorld` slice rather than on temporary sync appendices.

- 可以把我们当前设计重新表述为：
  - `Contracts`：schema + request objects + ABI/bridge
  - `Driver`：CLI / orchestration
  - `Loader`：SRM/PEReader 装载层
  - `Semantic World`：`Metadata Graph + Typed IL IR`
  - `Pipeline`：context / scheduler / step / collector / results
  - `Analyzer`：AOT closure / preserve / manifest
  - `CodeGen`：Runtime-Contract IR lowering + Scriban emission
  - `Metadata Writer`：registration / metadata binary / maps
  - `Runtime`：Runtime Core + GC facade + PAL
  - `Build`：native build / packaging
  - `Linker`：独立前置系统

这会让我们的设计既保留自己的显式 IR 优势，又真正吸收 Unity IL2CPP 这套成熟编译系统骨架。

### 2026-04-05 20:51:20 +08:00 - Old parent roadmap abandoned and planning entry rebooted

**Decision summary:**

- User requested that this old parent roadmap task be marked as abandoned.
- The two not-yet-started child skeletons derived from this roadmap:
  - `20260404-03-roadmap-1-assembly-and-typed-il-ir`
  - `20260404-04-roadmap-3-runtime-core-v1`
  are also retired as abandoned historical skeletons.

**Why this roadmap is being abandoned:**

- The document set had accumulated a large amount of architecture, module-boundary, dependency, and roadmap synchronization work.
- Those results remain useful as historical assets, but the parent roadmap itself is no longer a good direct execution入口.
- A fresh brainstorm is required to:
  - review the full abandoned-process end to end
  - separate “historically valuable conclusions” from “current execution commitments”
  - rebuild a new proof-first planning spine

**Follow-up task:**

- Subsequent planning work is moved to:
  - `20260405-01-il2cpp-reboot-after-abandoned-roadmap`

**Practical conclusion:**

- This finding record remains the historical trace of the abandoned roadmap.
- It should now be read as archival design context, not as the active planning spine.
