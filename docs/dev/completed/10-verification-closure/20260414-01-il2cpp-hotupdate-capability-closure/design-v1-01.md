# IL2CPP 与热更全能力收口设计 v1.01

Date: 2026-04-14 01:06:01 +08:00

## 1. 设计目标

本设计面向一个多平台 IL2CPP + 热更系统，目标是把“测试能力、subject 结构、solution archetype、声明式测试 API、文档边界”一起收口。

本轮设计的核心目标：

- 定义一套可持续的能力矩阵，明确 native / hot-update / mixed execution 三类能力边界。
- 在不扩张顶层 subject 数量的前提下，扩展 retained subject 内部结构，承载完整 solution 级测试。
- 新增代表性的 dotnet 核心 DLL 引用 solution archetype，并给出全工程热更 archetype。
- 将 `Chaos.TestFramework` 固化为 subject 内唯一受支持的 C# 测试声明 API。
- 为后续 roadmap 提供清晰的阶段边界、结构改造目标和验收标准基础。

## 2. 非目标

本设计当前不追求：

- 一次性支持所有移动端真机农场与发布自动化
- 把每个语法糖都拆成独立测试项目
- 为每一个 feature 都强制新增 benchmark
- 新增更多顶层 subject

## 3. 设计原则

### 3.1 继续坚持 solution-first

subject source 的核心单位不是零散 `csproj`，而是可被完整消费的 solution/source。

### 3.2 顶层 subject 只按 executable plan 划分

顶层 subject 继续只保留：

- `SolutionCorePack`
- `HotUpdateHostPack`
- `MixedExecutionFeaturePack`

是否新增顶层 subject 的唯一理由只能是出现新的 executable plan，而不是出现新的测试点。

### 3.3 测试声明只走 `Chaos.TestFramework`

subject 内部的 proof / benchmark 一律通过：

- `[ChaosUnitTest(...)]`
- `[ChaosBenchmark(...)]`
- `Assert`

进行声明，不再回到外置 `validation/`、`scenario/`、自定义 proof family 配置的旧路线。

### 3.4 “全部 feature”按语义能力族定义，不按语法糖定义

能力矩阵必须以 runtime 语义能力族为主轴，而不是以 C# 关键字表为主轴。

### 3.5 所有公开枚举都必须轻量、带注释

- 优先 `byte` / `ushort`
- 避免随意扩大 string 协议面
- 所有公开枚举必须带 XML 注释

### 3.6 支持状态必须显式

每个能力点都应落入以下状态之一：

- native-supported
- hot-update-supported
- mixed-supported
- unsupported-with-reason

避免出现“看起来应该支持，但系统里没有明确证据”的灰区。

## 4. Canonical Subject 设计

## 4.1 `SolutionCorePack`

职责：

- 承载 IL2CPP native 翻译主线
- 承载绝大多数 C# 语义能力 proof
- 承载完整 solution archetype 验证
- 承载 native 方向主 benchmark

建议内部结构：

```text
subjects/SolutionCorePack/
  subject.manifest.json
  source/
    SolutionCorePack.sln
    Launcher/
    Archetypes/
      SingleProjectSolution/
      MultiProjectSolution/
      PackageReferenceSolution/
      ReferenceAssemblySolution/
      CoreLibReferenceSolution/
      MixedReferenceClosureSolution/
    FeatureSlices/
      PrimitivesAndOps/
      GenericsAndCollections/
      AsyncAndIterators/
      ExceptionsAndControlFlow/
      ReflectionAndMetadata/
      InteropAndMarshaling/
      ThreadingAndSynchronization/
      RuntimeServices/
      DiagnosticsAndTracing/
      LinkerAndAotClosure/
    Benchmarks/
```

这里的 `Archetypes/` 和 `FeatureSlices/` 是两个正交维度：

- `Archetypes/` 回答“工程长什么样”
- `FeatureSlices/` 回答“语义能力是什么”

这样可以同时覆盖完整 solution 验证与语言特性验证，而不会把二者混成一套目录。

## 4.2 `HotUpdateHostPack`

职责：

- 承载 host-plus-patch 主线
- 承载全工程热更 canonical 用法
- 承载 shared contracts / metadata supplement / rollback / compatibility 验证
- 承载 hot-update benchmark

建议内部结构：

```text
subjects/HotUpdateHostPack/
  subject.manifest.json
  source/
    HotUpdateHostPack.sln
    Launcher/
    Host/
    SharedContracts/
    PatchModules/
    Archetypes/
      SkeletonPatchSolution/
      FullProjectHotUpdateSolution/
      VersionCompatibilitySolution/
      ReferenceAssemblyPatchSolution/
    Benchmarks/
```

其中：

- `FullProjectHotUpdateSolution/` 是本轮新增重点
- 它应展示一个完整 solution 如何分成 host、shared contracts、patch modules、package metadata，并给出标准用法

## 4.3 `MixedExecutionFeaturePack`

职责：

- 承载 mixed native/interpreter 链路
- 承载 interpreter lowering 与 boundary proof
- 承载 mixed benchmark

建议内部结构：

```text
subjects/MixedExecutionFeaturePack/
  subject.manifest.json
  source/
    MixedExecutionFeaturePack.sln
    Launcher/
    ManagedBridge/
    Interpreter/
    Lowering/
    Archetypes/
      MixedBridgeSolution/
    Benchmarks/
```

这个 subject 不负责完整 hot-update 工作流，只负责 mixed/interpreter 语义。

## 5. 能力矩阵设计

## 5.1 语言与运行时语义矩阵

建议将“C# feature 覆盖”收口为以下能力族：

- Primitives and ops
  - 算术、位运算、比较、转换、数组、boxing/unboxing
- Type system and generics
  - 泛型实例化、约束、generic virtual/interface dispatch、generic collection 行为
- Async and iterators
  - async/await、`Task`、`ValueTask`、iterator 状态机
- Delegates and closures
  - delegate chain、lambda、capture、callback、event
- Exceptions and control flow
  - throw/catch/finally/filter、nested exception、cross-boundary exception
- Reflection and metadata
  - custom attributes、type/member lookup、metadata supplement、preserve/stripping contract
- Interop and marshaling
  - P/Invoke、delegate callback、struct layout、string/array/UTF-8 marshal
- Threading and synchronization
  - `Thread`、`Task`、`Monitor`、TLS、main-thread gate 类能力
- Runtime services
  - runtime helpers、GC-sensitive path、resource lifecycle、loader contract
- Unsafe and low-level
  - `Span<T>` / `Memory<T>`、`ref struct`、`stackalloc`、pointer / function pointer
- Diagnostics and trace
  - trace 输出、native symbol、crash/rollback evidence
- Linker and AOT closure
  - stripping、preserve、generic sharing、required instantiation closure

重要约束：

- 不要求每个能力点都独立建项目。
- 每个能力点都必须至少有 proof 证据。
- benchmark 只给高价值 runtime 路径，不给所有能力点机械补 benchmark。

## 5.2 solution archetype 矩阵

建议作为 first-class contract 的 archetype：

- `SingleProjectSolution`
  - 最小可运行 solution
- `MultiProjectSolution`
  - 多项目分层与 transitive project graph
- `PackageReferenceSolution`
  - nuget / package 闭包
- `ReferenceAssemblySolution`
  - 显式 `Reference` / `HintPath` DLL
- `CoreLibReferenceSolution`
  - 显式引用一组 dotnet 核心 DLL
- `MixedReferenceClosureSolution`
  - `ProjectReference + Reference + PackageReference` 混合闭包
- `FullProjectHotUpdateSolution`
  - host + patch + shared contracts 的完整热更 solution

## 5.3 热更能力矩阵

热更能力不能只看“能不能加载 DLL”，至少要覆盖：

- patch package load
- shared contract binding
- method replacement
- bridge dispatch
- metadata supplement
- version compatibility
- rollback
- host-to-patch callback
- patch-to-host callback
- generic / delegate / exception 跨边界
- full-project package layout and usage

## 5.4 平台与产品化能力矩阵

对于多平台 IL2CPP + 热更系统，除了功能正确性，还需要把以下能力纳入长期测试设计：

- host platform matrix
  - Windows、macOS、Linux
- target/runtime matrix
  - managed、native、interpreter、mixed
- device/package matrix
  - desktop host、mobile package、simulator/device availability
- compatibility matrix
  - AOT 主包版本 vs patch 版本
- distribution policy
  - iOS 逻辑热更边界、fallback 策略
- reliability matrix
  - rollback、soak、repeatability、artifact reproducibility
- diagnostics matrix
  - trace、symbol、benchmark、failure evidence、unsupported feature report

## 6. `Chaos.TestFramework` 扩展设计

## 6.1 路径边界

公开 managed 测试 API 继续放在：

- `src/reference/Chaos.TestFramework`

不放在：

- `src/managed`

因为 `src/managed` 是 IL2CPP 主线核心代码，而 `src/reference` 才是 C# 层工程引用代码。

## 6.2 API 继续按 C# 单元测试心智设计

对 subject source 的作者来说，目标用法仍然应该接近 `UnitTest`：

- 一个 proof 方法打 `[ChaosUnitTest]`
- 一个 benchmark 方法打 `[ChaosBenchmark]`
- 用 `Assert` 表达 correctness

不鼓励继续引入大段 JSON、字符串协议或额外配置目录来定义测试项。

## 6.3 建议新增的轻量枚举

在现有基础上，建议补充而不是推翻：

- `ChaosSolutionArchetype : byte`
  - 标记 solution 形态
- `ChaosHotUpdateCapability : ushort`
  - 标记热更能力点
- 扩展 `ChaosRuntimeFeature`
  - 补充 async、threading、unsafe、span、metadata supplement、generic virtual/interface dispatch 等高价值运行时特征

要求：

- 所有公开枚举都带 XML 注释
- 只在确实需要结构化表达时新增
- 不把“本可以用 enum 表达的字段”继续落成 string

## 7. dotnet 核心 DLL 引用方案设计

这是本轮最关键的 open question 之一，但设计层面的推荐方向已经可以先确定。

### 7.1 目标

需要一个 canonical `CoreLibReferenceSolution`，它要证明：

- solution 可以显式引用若干 dotnet 核心 DLL
- IL2CPP 可以完整 ingest 并做 native 翻译
- 这类依赖闭包可以与其他 project/package/reference 组合存在

### 7.2 方案候选

方案 A: 直接引用本机 SDK pack 路径

- 优点：最贴近真实 dotnet 引用
- 缺点：机器依赖强，目录脆弱，不利于 CI 与跨机复现

方案 B: 仓库内维护受控 reference bundle

- 优点：最稳定、最容易复现
- 缺点：维护成本高，体积增加

方案 C: 用 resolver 从本机 SDK 提取到受控缓存，再让 solution 指向受控缓存

- 优点：兼顾真实来源与可复现性
- 缺点：需要新增一层准备流程

当前推荐：

- 倾向方案 C。
- 即：不要把绝对 SDK 路径直接写进 archetype，但也不急着把一大包 Microsoft DLL 永久提交进仓库。

## 8. 文档设计

本轮能力建设完成后，至少应补齐以下文档：

- `docs/architecture/il2cpp-hotupdate-capability-matrix.md`
  - 定义能力矩阵与支持状态
- `docs/architecture/solution-archetype-matrix.md`
  - 定义完整 solution archetype contract
- `docs/architecture/full-project-hotupdate-workflow.md`
  - 定义 host + patch + contracts 的标准用法
- `docs/architecture/reference-assembly-and-corelib-policy.md`
  - 定义 reference/corelib 依赖来源策略
- `docs/architecture/unsupported-feature-policy.md`
  - 定义“不支持/部分支持”的公开表达方式

## 9. 验收目标基线

设计完成并进入实施后，最终必须满足：

- 顶层 `subjects/` 仍然只保留 3 个 retained subject
- 每个能力点都能映射到一个明确的 subject + slice/archetype 归属
- `SolutionCorePack` 拥有完整 solution archetype 矩阵
- `HotUpdateHostPack` 拥有全工程热更 canonical archetype
- `Chaos.TestFramework` 成为唯一公开的 subject C# 测试 API
- 所有能力点都能明确表达支持状态，而不是靠口头约定
- 文档层能回答“怎么用”“支持到哪”“不支持什么”“为什么”

## 10. 为什么这一定是 roadmap 任务

本设计天然需要 roadmap，因为实施上至少会拆成：

- `Chaos.TestFramework` 合同扩展
- `SolutionCorePack` archetype 与 feature slice 扩展
- `HotUpdateHostPack` 全工程热更 archetype 扩展
- `MixedExecutionFeaturePack` mixed/interpreter 能力补齐
- pipeline / discovery / gate / reporting 配套调整
- architecture 文档补齐

这些工作明显跨多阶段、多会话，不能直接降成单一 plan。
