# IL2CPP Mainline 性能优先核心与完整 Subject 设计 v1.01

Date: 2026-04-09
Status: design-frozen-for-roadmap

## 1. 设计目标

本设计要把 `il2cpp-mainline` 讨论里已经稳定的几条主张，落成下一阶段可执行的目标状态：

- generated native 工程性能优先于“边界看起来更优雅”的次级诉求。
- `SemanticWorld` 继续承担 canonical semantic facts，而不是背负 optimization-specific payload。
- `OptimizationFacts` 成为 `LinkedWorld -> CodeGen` 的 first-class contract。
- 现有最小 subjects 继续保留，另增一个完整 mainline subject 作为能力补全和 native perf 回归宿主。

## 2. 需要继承的 governing decisions

### 2.1 `SemanticWorld` 六块 canonical output 保持不变

下一阶段仍沿用已经讨论冻结的六块：

- `CanonicalSubjects`
- `SemanticShapes`
- `MethodBodies`
- `CapabilityBundles`
- `LiteralPool`
- `Diagnostics`

这意味着：

- semantic layer 继续是 definition-oriented、canonical、可审计的。
- performance-specific facts 不回流进 `SemanticWorld`。

### 2.2 `OptimizationFacts` 作为新的正式边界引入

下一阶段的关键不是继续让 emitter 猜样例形状，而是让 `CodeGen` 能正式拿到 performance-oriented closed-world facts。

推荐最小落点：

- `ClosedWorldSpecializationFacts`
- `OptimizationShapeFacts`

它们至少要覆盖以下能力：

- closed generic instantiation facts
- devirtualization-relevant facts
- layout/storage-relevant facts
- const-foldability / concat-shape / allocation-elision-relevant facts

### 2.3 `LoweringFamily` 选择改为“合法性过滤 + 性能裁决”

下一阶段不再把“谁排在前面”当成主逻辑，而是：

1. 先根据 `SemanticShapes + CapabilityBundles + OptimizationFacts` 过滤所有合法 family。
2. 再根据 target-aware performance policy 选择 native outcome 最优的 family。
3. 只有在性能裁决无法区分时，才回退到 deterministic tie-breaker。

### 2.4 emitter 必须从 sample-driven 退化逻辑中退出

`NativeReferenceProofEmitter` 当前的形状判断只能继续服务过渡期，不能再作为 mainline 长期核心。

目标状态是：

- emitter 消费显式 lowering plan。
- lowering family 与 plan 由上游 contract 决定。
- emitter 只负责 template/render/backend glue，不再负责“猜当前样例属于哪一类”。

## 3. 目标架构分层

### 3.1 `Loader`

职责保持为：

- assembly / metadata / IL 结构解码
- definition-level facts 装载

不承担：

- semantic normalization
- retained-world policy
- optimization policy

### 3.2 `SemanticWorld`

职责保持为：

- 生成 canonical semantic subjects / shapes / bodies / capability summaries / literal pool / diagnostics

不承担：

- closed-world specialization facts
- target-aware performance decisions
- emitter shape 特化

### 3.3 `Linker`

职责升级为：

- 基于 semantic facts 形成 retained world
- 冻结 dependency obligations
- 汇总 `OptimizationFacts` 所需的闭包级事实

### 3.4 `CodeGen`

职责升级为：

- 基于 `LinkedWorld + OptimizationFacts` 做 lowering family legality filter
- 用 performance arbitration 决定具体 lowering
- 产出对 backend/emitter 透明的 lowering plan

### 3.5 `Emitter / Native Backend`

职责收缩为：

- 消费 lowering plan
- 生成 target-native 工程与 glue
- 输出 runtime/perf/report 所需工件

## 4. 完整 subject 设计

### 4.1 推荐新增独立宿主：`MainlineFeaturePack`

推荐新增 subject id：

- `MainlineFeaturePack`

理由：

- 它能和 `HelloWorldObject` 的最小 correctness/reference 角色分离。
- 它能和 `GenericEcho` 的 managed perf 角色分离。
- 名称天然表达“按能力批次逐步补全”的主线定位。

### 4.2 目标目录结构

```text
subjects/MainlineFeaturePack/
  subject.manifest.json
  source/
    MainlineFeaturePack.csproj
    Features/
      Dispatch/
      GenericLayout/
      ArrayBoxing/
      DelegateCallback/
      Exception/
  validation/
    unit/
      MainlineFeaturePack.Subject.UnitTests/
    perf/
      MainlineFeaturePack.Subject.PerfHarness/
    proof/
      native-reference/
  expected/
    analysis/
    codegen/
    runtime/
  baselines/
    perf/
```

设计约束：

- 仍然遵守当前 `subject + matrix + artifact-chain` 模型。
- unit、proof、perf 都归 subject 自有目录，不回流到 `tests/`。
- feature 以切片目录保留，不允许做成大而全不可拆的 smoke 源码。

### 4.3 推荐 validation profiles 与矩阵

推荐第一版保留三个主 profile：

- `proof-dev`
  - 默认验证：`proof + unit`
  - 默认 variant：`CHECK`
  - 默认矩阵：`windows-native-check`
- `trace-platform`
  - 默认验证：`proof`
  - 默认 variant：`CHECK`
  - 默认矩阵：`windows-reference-trace`
- `perf-profile`
  - 默认验证：`proof + perf`
  - 默认 variant：`PROFILE`
  - 默认矩阵：`windows-native-profile`

这样设计的原因是：

- correctness、trace、native perf 分别可观察。
- 仍保持统一 subject 入口。
- 不要求一开始就把 `SHIP` 也塞进闭环。

### 4.4 推荐新增 `native-runtime-perf` pipeline

完整 subject 需要的不再只是 output/trace，还必须有 native perf 主线。

推荐第一版 pipeline：

```text
source-resolve
host-input-build
analysis-frontend
generated-native-proof
build-target
runtime-perf-collect
report-assemble
```

其中：

- canonical bucket 仍然只落在既有 `source / host-input / analysis / generated / build / runtime / report`
- 不新增平行 artifact bucket
- perf 样本与 baseline compare 作为 `report` 附件，而不是新的顶层模型

## 5. capability 补全策略

完整 subject 不应变成“所有 feature 一次性塞进去”，而应按性能导向批次推进。

### 5.1 推荐首批 capability

推荐第一批优先覆盖：

- `virtual/interface dispatch`
- `closed generic specialization + layout`
- `arrays + boxing/unboxing`

推荐原因：

- 它们对 generated native code quality 影响更直接。
- 它们天然要求 `OptimizationFacts` 尽早成为正式 contract。
- 它们能逼出 `devirtualization`、layout、storage、specialization 等性能相关事实。

### 5.2 推荐第二批 capability

推荐第二批再进入：

- `delegate/callback`
- `exceptions`
- 需要补齐的反射或 interop 边角

原因：

- 这批更容易把调度、宿主 glue、控制流复杂度一起带进来。
- 如果第一批基础 contract 还没稳住，太早引入会放大噪声。

补充确认：

- `exceptions` 已被用户明确要求纳入本 roadmap 的刚性交付项。
- 因此这里的“第二批”只表示实现排序，不表示可以省略或推迟到 roadmap 之外。

## 6. 现有最小 subject 的新定位

下一阶段不应删除它们，而是重新定位：

- `HelloWorldObject`
  - 继续做最小 correctness/reference subject
- `GenericEcho`
  - 继续做 managed perf baseline subject
- `ReflectionLite` / `PInvokeLite`
  - 继续做 feature probe，而不是 mainline full subject
- `MainlineFeaturePack`
  - 新增的 integration + native perf 主宿主

## 7. Engine Binding 排序

`engine binding` 不再被排除在整个 roadmap 之外，但用户已经明确要求它放到路线最后。

本设计采用的排序是：

- 前六阶段先把 IL2CPP core performance-first 主线、完整 subject、native perf 回归和 capability 补全做稳。
- 最后一阶段再把现有 `docs/discuss/il2cpp-mainline/20260407-10` 到 `20260407-15` 的 engine binding 讨论收口成正式 child task。

这样排序的原因是：

- engine binding 会同时引入 service call、object handle、callback/thread lane 三类额外维度。
- 如果太早卷入，会污染 `Phase A` 里 core contract 的收口节奏。
- 把它放在最后，反而更容易复用已经稳定的 `subject + matrix + artifact-chain` 主线。

## 8. 非目标

本设计当前不包含：

- 在同一阶段同时推进 `.NET 10`、`macOS` parity、Android、iOS
- 把 engine binding 提前卷入前六阶段
- 用一个完整 subject 取代所有最小 probe
- 新造一套平行于 `subject + matrix + artifact-chain` 的工件模型

## 9. 设计结论

本轮设计冻结为以下选择：

1. 继续沿用 `SemanticWorld` 六块 canonical output。
2. 新增 first-class `OptimizationFacts`，作为 performance-first `CodeGen` 的正式输入。
3. `LoweringFamily` 选择改为“合法性过滤 + 性能裁决”。
4. `NativeReferenceProofEmitter` 的长期方向是 capability-driven lowering backend，而不是样例形状识别器。
5. 保留现有最小 probe subjects，新增独立 full subject `MainlineFeaturePack`。
6. 完整 subject 的第一版同时承接 native output、trace、perf 三条主线。
7. 首批 capability 推荐先做 `dispatch + closed generics/layout + arrays/boxing`，同时保证 `exceptions` 作为本 roadmap 的刚性交付项保留在后续阶段。
8. `engine binding` 保留到整个 roadmap 最后阶段再收口，不允许提前打乱前六阶段节奏。
