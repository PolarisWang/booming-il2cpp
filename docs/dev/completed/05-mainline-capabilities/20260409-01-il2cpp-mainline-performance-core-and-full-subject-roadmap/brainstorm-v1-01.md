# IL2CPP Mainline 下阶段 Brainstorm v1.01

Date: 2026-04-09
Status: design-approved-enter-roadmap

## 1. 目标

本轮 brainstorm 不直接进入实现，而是把用户提出的三条要求先收敛成可执行的阶段路线：

- 加强 IL2CPP 核心功能。
- 一切核心决策以 generated native 工程性能为第一目标。
- 配套一个功能完整的 subject 工程，持续验证后续能力补全。

本轮输出要求是：

- 先把 `il2cpp-mainline` 讨论过程读完并总结成当前共识。
- 再对照仓库现实判断下一阶段该走哪条路线。
- 最后进入新的父级 `roadmap`，而不是直接写单个实现 `plan`。

## 2. `il2cpp-mainline` 讨论过程的收敛结果

### 2.1 `2026-04-07` 先搭了主线骨架

`20260407-01-il2cpp-mainline-roadmap-skeleton-v1-01.md` 已经把大方向定清楚了：

- 先做 `Phase A` 核心层稳定化。
- 再做 `Phase B` 的 `.NET 8` 核心语义扩展。
- 再谈 `.NET 10`、`macOS` parity、Android、iOS。

也就是说，当前主线不是去抢平台数量，也不是先扩 TFM，而是先把核心层从 proof-first sample spine 收口成长期主线。

### 2.2 `2026-04-07` 到 `2026-04-08` 连续冻结了 `Phase A` 的边界

这一轮讨论把以下边界逐步拆开了：

- `Loader / SemanticWorld / Linker / CodeGen / Emitter` 的职责分层。
- `SemanticWorld` 输出只保留六块 canonical output。
- `Linker` 只消费 semantic facts，不回读 raw loader facts 或 debug views。
- `CodeGen` 不应再依赖样例特化的输入形状。
- `LoweringFamily` 应成为正式 contract，而不是 emitter 内部私有判断。

### 2.3 `2026-04-08` 最新结论把“性能优先”上升成 governing rule

`20260408-26-phase-a-generated-native-performance-first-governing-decisions-v1-01.md` 把下一阶段的最高优先级直接改成：

- 在 correctness 成立、边界仍可维护的前提下，一切核心层架构决策优先服务于 generated native 工程性能最优。

这份讨论同时冻结了三条关键导向：

- `SemanticWorld` 六块 canonical output 保留，不粗暴把性能信息塞回 semantic layer。
- 新增 first-class `OptimizationFacts` 作为 `LinkedWorld -> CodeGen` 的正式输入。
- `LoweringFamily` 选择从“静态优先级排序”修正为“合法性过滤 + 性能裁决”。

## 3. 当前仓库现实

对照现有实现，讨论共识还没有变成代码现实：

- `src/managed/Chaos.IL2CPP.SemanticWorld/SemanticWorldStage.cs` 目前仍接近 Loader 结果透传，离真正 canonical semantic layer 还有明显距离。
- `src/managed/Chaos.IL2CPP.Linker/LinkerStage.cs` 仍有显式的 dependency reason 硬编码与特例归一化。
- `src/managed/Chaos.IL2CPP.CodeGen/CodeGenStage.cs` 仍主要产出 typed IL / registration，缺少 performance-first 的下游 contract。
- `src/managed/Chaos.IL2CPP.CodeGen/NativeReferenceProofEmitter.cs` 仍通过固定 entry-point shape 猜测 lowering plan，明显是 sample-driven emitter。

subject 层也存在同样的结构断层：

- `subjects/HelloWorldObject` 是 correctness/reference proof subject。
- `subjects/GenericEcho` 是 managed perf benchmark subject。
- 仓库里还没有一个完整 subject 同时承担 native output、trace、perf 三类主线验证。

## 4. 可选路线

### 方案 A：继续沿用多个最小 subject，核心按 feature 零散补

做法：

- 继续保持 `HelloWorldObject`、`ReflectionLite`、`PInvokeLite` 等最小 probe 为主。
- 新能力来了就按 feature 点修 `SemanticWorld / Linker / CodeGen / Emitter`。
- perf 继续主要放在 `GenericEcho` 的 managed benchmark。

优点：

- 启动成本最低。
- 每次失败点容易定位到单个最小样例。

缺点：

- 无法验证完整 native 主线是否真的朝“性能优先”在收口。
- 会继续容忍 sample-driven emitter 与 ad-hoc lowering 逻辑长期存在。
- 后续缺少一个统一的 full subject 承接能力补全与 perf 回归。

判断：

- 不推荐作为下阶段主线。

### 方案 B：保留最小 probe subjects，再新增一个完整 mainline subject

做法：

- 现有最小 subjects 继续保留，承担分层回归与失败定位。
- 新增一个完整 subject，承接 native output、trace、perf 三类验证。
- 同时把核心层按 performance-first contract 重构，优先落地 `OptimizationFacts` 与 lowering arbitration。

优点：

- 兼顾最小 probe 的定位价值与 full subject 的集成价值。
- 更符合 `subject + matrix + artifact-chain` 既有主线。
- 能把“generated native 性能优先”变成长期可观察事实，而不是讨论口号。

缺点：

- 比方案 A 多一层完整 subject 设计与 pipeline 扩展成本。
- 需要更早收敛 full subject 的命名、validation profile 与 perf 报告边界。

判断：

- 推荐。

### 方案 C：直接把 `HelloWorldObject` 扩成完整 mainline subject

做法：

- 不新建 full subject。
- 直接把 `HelloWorldObject` 扩展为后续所有能力与 perf 的主宿主。

优点：

- 看起来对象更少，路径更短。
- 可以复用现有 native proof host。

缺点：

- 会把 correctness/reference 的最小 probe 与完整 integration/perf subject 混在一起。
- 失败面会显著放大，未来很难再把它当作“最小 canonical reference subject”。
- 也会让 `HelloWorldObject` 背负太多历史兼容职责。

判断：

- 可行，但不如方案 B 稳妥。

## 5. 推荐结论

推荐采用方案 B：

- 保留现有最小 probe subjects。
- 新增一个完整 mainline subject。
- 围绕 `OptimizationFacts`、performance arbitration、capability-driven lowering 做下一阶段的父级 roadmap。

这样处理的原因很直接：

1. 它最符合 `2026-04-08` 最新讨论已经冻结的 performance-first governing rule。
2. 它不需要牺牲现有最小 subject 的定位能力。
3. 它能把“能力补全”和“generated native 性能回归”挂到同一个完整宿主上。
4. 它更适合拆分为多个 child task，跨多次会话推进。

## 6. 为什么必须进入 `roadmap`

本任务已经明显超出单一 plan 的边界：

- 同时涉及 `src/managed`、`src/native`、`subjects/`、pipeline、report、perf baseline。
- 至少会拆成 4 个以上相对独立的 child task。
- 需要跨多次会话推进，不可能一轮写完并稳定。
- 期间必然会出现需要和用户确认的设计分歧。

因此，当前自然分流不是 `writing-plans`，而是新的父级 `roadmap`。

## 7. 已确认决策

用户已在 `2026-04-09` 明确确认以下三点：

1. 完整 subject 接受新增独立宿主。
   - 正式采用：`MainlineFeaturePack`
2. 首批性能导向 capability 要做，同时 `exceptions` 也必须做。
   - 采用解释：首批主顺序仍先围绕 `dispatch + arrays/boxing + closed generics/layout` 展开，但 `exceptions` 不再是可做可不做的尾部议题，而是本 roadmap 的刚性交付项。
3. `engine binding` 不提前卷入前面阶段，而是放到整个 roadmap 最后。

因此，本轮 brainstorm 已不再保留策略层 open questions，可以直接进入父级 roadmap 的 child plan 派生。
