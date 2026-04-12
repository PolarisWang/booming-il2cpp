# 测试体系 Solution-First + Attribute-Declared Hybrid 设计 v1.03

**日期：** 2026-04-12 17:15 +08:00

## 1. 本版变更说明

v1.03 替换 v1.02 中“`scenario/`、`workload/` 作为 subject 下默认物理目录实体”的做法，改为以下新基线：

- 顶层 `subject` 仍然表示 **canonical solution source bundle**。
- proof 与 runtime benchmark 的入口，优先在 solution 内通过 C# attribute 声明。
- `subject.manifest.json` 退化为 **薄 orchestration manifest**，只保留 source 描述、工程档位、矩阵/流水线/预算/兼容性等外部编排信息。
- `scenario` 与 `workload` 不再要求长期落成 subject 目录树；它们保留为 **逻辑分类**：
  - `scenario-like` = 工程级验证 + `ChaosUnitTest`
  - `workload-like` = 工程级 workload + `ChaosBenchmark`
- “工程级验证”与“源码内声明测试”被正式分层：
  - 工程级验证是核心、默认、始终存在
  - attribute 声明测试是可选增强层
- 新 attribute 命名采用用户确认的命名：
  - `ChaosUnitTest`
  - `ChaosBenchmark`
- 元数据设计要求减少自由字符串，优先使用紧凑枚举和位标志，字符串只保留在不可避免的 identity/path/alias 边界。

## 2. 问题定义

当前仓库的主要结构问题已经比较清晰：

- `subjects/` 仍然混杂了多种含义：
  - 有些顶层目录代表真正的 solution source
  - 有些顶层目录只是某个 proof
  - 有些顶层目录只是 benchmark 切片
- `tests/` 中仍然存在：
  - `phase*` / `stage*` 命名
  - subject-specific source/fixture 逻辑
  - `tests/perf/convert-golden-multi-project` 这类本应回归 subject 的工程 workload
- `subject.manifest.json` 目前过重，重复承载大量 `validationProfiles`、`executionPipelines`、`environmentMatrices`，导致每个 subject 目录噪音过大。
- solution 已经是 subject 的真实源后，再继续用 `validation/`、`scenario/`、`workload/` 目录定义 proof/benchmark，会让“测试声明离源码过远”。
- 但如果把所有信息都塞回 attribute，又会丢失跨平台编排、流水线、预算、设备可用性等 orchestration 能力。

因此，本版设计要同时解决两个目标：

- 把 `subjects/` 整理成真正干净的 solution-first 结构。
- 又不牺牲多平台 IL2CPP + 热更系统所必需的工程级测试能力与编排能力。

## 3. 设计目标

本版设计的目标如下：

- 新增一个测试样例时，默认动作是新增一个 `subject` solution。
- 如果该 subject 需要 proof 或 runtime benchmark，直接在 solution 源码中声明 `ChaosUnitTest` / `ChaosBenchmark`。
- 如果该 subject 不引用测试框架 DLL，也仍然能作为工程级验证 subject 参与 convert/codegen/native executable/package/hot-update 等核心路径验证。
- subject 根目录尽量精简，不再默认展开 `scenarios/`、`workloads/`、大量 per-subject pipeline 定义。
- 原先按需散落在 subject 根目录中的 `fixtures/`、`expected/`、`baselines/`、`validation/` 语义，默认收敛为 C# API，而不是继续扩张目录结构。
- `tests/` 只保留 generic mechanism 测试与 generic fixture，不再承载 canonical subject source。
- 所有低基数字段尽量采用枚举和紧凑类型，减少自由字符串的扩散。
- 兼容现有仓库迁移现实：允许阶段性 alias、compatibility shell、过渡层。

## 4. 非目标

本版设计不追求一次性解决以下问题：

- 一次性完成所有历史 subject 的物理迁移与代码改造
- 一次性去掉全部兼容入口
- 一次性完成所有真机与设备农场自动化
- 为每个历史 benchmark/proof 立刻给出最终合并目标名
- 把所有 orchestrator 元数据都压缩成纯二进制表示

## 5. 核心决策

### 5.1 `subject` 的唯一含义

顶层 `subject` 只表示：

- 一套 canonical solution source bundle
- 一套稳定的 project graph / dependency model / executable plan
- 一套可被 IL2CPP 处理成工程产物的真实输入

以下对象不再单独构成顶层 `subject` 的默认理由：

- 某一个 proof 点
- 某一个 benchmark 点
- 某一个 validation profile
- 某一个 pipeline
- 某一个 matrix

### 5.2 物理目录最小化

subject 目录不再默认要求存在：

- `scenarios/`
- `workloads/`

这两个概念保留为逻辑分类，用于 selector、report、artifact 归属和 pipeline 组织，但不作为长期默认物理目录层级。

### 5.3 两层测试模型

系统中的测试对象被明确拆成两层：

| 层级 | 是否必需 | 谁拥有真源 | 典型内容 |
| --- | --- | --- | --- |
| 工程级验证 | 必需 | system + subject manifest | project graph、dependency resolution、convert、codegen、native link、native executable、package、hot-update patch |
| 源码内声明测试 | 可选 | subject solution source | `ChaosUnitTest`、`ChaosBenchmark` |

核心原则：

- 工程级验证是主验证，不依赖 `ChaosUnitTest` / `ChaosBenchmark` 才成立。
- attribute 声明层负责表达 solution 内部的 proof 函数与 runtime benchmark 函数。
- 这两层在 selector、report 和 artifact 上统一汇总，但在设计职责上必须分离。

### 5.4 Attribute 负责“局部声明”，manifest 负责“外部编排”

放进 attribute 的内容：

- 单个测试入口的方法身份
- 测试类别
- 指标类型
- 默认 warmup / iteration / invocation
- 运行时功能依赖
- 证据类型提示

保留在 manifest / orchestration 的内容：

- host / target platform 矩阵
- pipeline 归属
- availability
- budget / baseline profile
- device runner 选择
- artifact/evidence 编排
- alias / redirect / deprecation

### 5.5 尽量少字符串，优先紧凑枚举

本版设计明确限制字符串使用边界：

- 必需字符串：
  - `subjectId`
  - 路径
  - 可读 display name
  - 兼容 alias
  - 少数可选的 test alias
- 尽量不用字符串数组表达：
  - tags
  - metrics
  - capabilities
  - requirements
  - categories

这些应优先改成：

- `byte` 枚举
- `ushort` / `uint` 位标志
- 由发现阶段自动生成的稳定 identity

### 5.6 无测试 DLL 引用时的语义

若 subject solution 未引用 IL2CPP 测试框架 DLL，则该 subject 被视为：

- `runtime-only subject`

它仍然是一等公民，参与工程级验证。

更完整的判定规则如下：

- 未引用测试框架 DLL：`runtime-only`
- 已引用测试框架 DLL，但未发现任何 `ChaosUnitTest` / `ChaosBenchmark`：仍按 `runtime-only` 处理，并发出配置警告
- 已发现声明测试：`declared-test subject`

## 6. 对象模型

### 6.1 主对象

| 对象 | 真源 | 是否物理目录实体 | 说明 |
| --- | --- | --- | --- |
| `subject` | `subjects/<id>/` | 是 | canonical solution source bundle |
| `engineering validation` | manifest + planner | 否 | 系统拥有的工程级正确性验证 |
| `engineering workload` | manifest + planner | 否 | 系统拥有的工程级 workload，例如 convert/codegen |
| `declared test` | source attribute | 否 | 由 `ChaosUnitTest` / `ChaosBenchmark` 声明 |
| `orchestration profile` | shared registry / config | 否 | matrix、pipeline、budget、baseline 等共享定义 |
| `compiled catalog` | discovery result | 否 | 把 subject、工程项、声明项合并后的运行态目录 |

### 6.2 逻辑分类保留

尽管 `scenario/` 与 `workload/` 不再默认落成目录，本版仍保留逻辑分类：

- `scenario-like`
  - 工程级 correctness validation
  - `ChaosUnitTest`
- `workload-like`
  - 工程级 performance / throughput workload
  - `ChaosBenchmark`

这两个分类继续服务于：

- selector
- report
- artifact path
- pipeline membership

## 7. 工程架构设计

### 7.1 总览

```text
subject.manifest.json + source/
        |
        v
Project Graph / Build / Host Input Resolve
        |
        +--> Engineering Plan Compiler
        |        |
        |        +--> engineering validations
        |        +--> engineering workloads
        |
        +--> Declaration Discovery
                 |
                 +--> ChaosUnitTest
                 +--> ChaosBenchmark
        |
        v
Compiled Subject Catalog
        |
        v
Selector / Planner / Pipeline / Artifact / Report
```

### 7.2 组件职责

| 组件 | 责任 |
| --- | --- |
| `SubjectManifestLoader` | 读取薄 manifest，解析 source model、engineering profile、orchestration references |
| `ProjectGraphResolver` | 构建 solution/project graph，确定入口工程、依赖形态、输出程序集 |
| `EngineeringPlanCompiler` | 基于 `engineeringProfile` 生成系统拥有的 validation/workload 条目 |
| `DeclarationDiscovery` | 扫描程序集中的 `ChaosUnitTest` / `ChaosBenchmark` |
| `CatalogMerger` | 合并工程级条目、声明条目与 orchestration profile |
| `Selector` | 允许按 subject / engineering / unit / benchmark / pipeline 选择 |
| `ExecutionPlanner` | 把 catalog 项与 matrix/pipeline/profile 对齐后展开执行 |
| `EvidenceEvaluator` | 汇总 expected、baseline、budget、artifact evidence |

### 7.3 新增 reference 组件建议

本版建议新增一个轻量 C# 引用组件：

- `src/reference/Chaos.TestFramework/`

路径边界必须明确：

- `src/managed/` 承载 IL2CPP 阶段核心代码，是 loader、pipeline、driver、codegen 等核心执行链的一部分。
- `src/reference/` 承载被 subject solution 直接引用的 C# 层框架代码。
- 因此测试声明框架不应落在 `src/managed/`，而应落在 `src/reference/Chaos.TestFramework/`。

用途：

- 只提供 attribute 和枚举定义
- 提供少量与声明测试同层的 helper API
- 不承担 orchestration
- 不承担 device / pipeline / budget 策略
- 尽量保持极薄，避免把 infra 语义灌回业务 solution

不建议一开始就再拆更多公共 DLL。优先策略是：

- `Chaos.TestFramework` 提供声明契约与辅助测试 API
- discovery 与 catalog 合并逻辑继续落在现有 loader / pipeline / driver 侧

## 8. subject 根目录设计

### 8.1 目标结构

```text
subjects/
  <subject-id>/
    subject.manifest.json
    source/
    assets/        # optional, 仅保留无法通过 C# API 表达的文件型资产
```

明确约束：

- `source/` 是唯一必需目录
- `assets/` 只用于文件型输入、模板、二进制样本或无法 API 化的静态数据
- 不再默认要求 `scenarios/` / `workloads/`
- 不允许把大量共享 pipeline/matrix/stage 定义重复灌进每个 subject 目录
- `fixtures/`、`expected/`、`baselines/`、`validation/` 不再作为目标结构中的一等目录
- 这些语义默认改为 `Chaos.TestFramework` 提供的 C# API，使用方式与 `ChaosUnitTest` / `ChaosBenchmark` 同层

### 8.2 顶层命名规则

推荐命名分两类：

- archetype solution subjects 使用 `Solution*`
  - `SolutionSimpleLib`
  - `SolutionMultiProject`
  - `SolutionPackageReference`
- 真实语义上 materially distinct 的大 solution 可保留语义名
  - `MainlineFeaturePack`
  - `EngineHostPack`
  - `HotUpdateHostPack`

默认不再新建：

- `*Proof`
- `Bench*`

除非它本身真的是独立 solution source bundle，而不是某个验证切片。

## 9. 薄 manifest 设计

### 9.1 目标

`subject.manifest.json` 只保留 subject 自己无法从源码自动推导，且又必须由外部编排控制的信息。

### 9.2 建议字段

| 字段 | 类型 | 说明 |
| --- | --- | --- |
| `subjectId` | `string` | 稳定 ID |
| `displayName` | `string?` | 可读名称 |
| `source` | object | 路径、入口工程、入口程序集线索 |
| `sourceModel` | enum | `dotnet-solution` / `dotnet-project-set` / `host-plus-patch` / `mixed-solution` |
| `dependencyModel` | enum | `project-reference` / `package-reference` / `binary-reference` / `mixed` |
| `executablePlan` | enum | `managed-host` / `generated-native` / `device-package` / `host-plus-hot-update-patch` |
| `engineeringProfile` | enum | 工程级验证档位 |
| `testDeclarationMode` | enum | `auto` / `none` / `require` |
| `orchestration` | object | 共享 matrix/pipeline/budget/baseline profile 引用 |
| `availability` | map | 各平台 readiness |
| `compatibility` | object | legacy alias / redirect / deprecation |

### 9.3 共享 profile，不再 per-subject 重复展开

v1.03 明确要求：

- `executionPipelines`
- `environmentMatrices`
- 大段 stage DAG

不再作为 per-subject 默认内联定义长期存在。

subject manifest 只引用共享 profile，例如：

- `matrixProfile`
- `pipelineProfile`
- `budgetProfile`
- `baselineProfile`

这样每个 subject 只声明“我属于哪个外部编排集合”，而不是复制整套编排细节。

### 9.4 authoring 与 runtime 的类型策略

为了同时兼顾可读性与紧凑性，采用双层策略：

- authoring 层：
  - `subject.manifest.json` 可继续使用少量可读的 symbolic enum name
- runtime/catalog 层：
  - 编译为紧凑枚举值与 profile id

也就是说：

- 人写的配置保持可审查
- 执行时的 catalog 保持 typed、紧凑、少字符串

## 10. `Chaos.IL2CPP.Testing` 设计

### 10.1 目标

`Chaos.IL2CPP.Testing` 是一个极薄的声明层 DLL，只做三件事：

- 定义 `ChaosUnitTest`
- 定义 `ChaosBenchmark`
- 定义配套紧凑枚举和位标志

不做：

- pipeline orchestration
- device selection
- budget evaluation
- baseline storage
- matrix resolution

### 10.2 建议公开面

```csharp
namespace Chaos.IL2CPP.Testing;

public enum ChaosUnitCategory : byte
{
    RuntimeContract = 1,
    MetadataContract = 2,
    InteropContract = 3,
    HostContract = 4,
    HotUpdateContract = 5,
}

public enum ChaosBenchmarkCategory : byte
{
    RuntimeDispatch = 1,
    Startup = 2,
    Allocation = 3,
    HotUpdate = 4,
}

[Flags]
public enum ChaosMetric : ushort
{
    None = 0,
    WallClockUs = 1 << 0,
    ManagedAllocBytes = 1 << 1,
    NativeAllocBytes = 1 << 2,
    PeakWorkingSet = 1 << 3,
}

[Flags]
public enum ChaosRuntimeFeature : uint
{
    None = 0,
    GenericSharing = 1 << 0,
    Reflection = 1 << 1,
    Delegate = 1 << 2,
    ExceptionFlow = 1 << 3,
    NativeInterop = 1 << 4,
    HotUpdate = 1 << 5,
}

[Flags]
public enum ChaosEvidenceKind : ushort
{
    None = 0,
    Stdout = 1 << 0,
    FileArtifact = 1 << 1,
    Metadata = 1 << 2,
    NativeSymbol = 1 << 3,
}

[AttributeUsage(AttributeTargets.Method, AllowMultiple = false)]
public sealed class ChaosUnitTestAttribute : Attribute
{
    public ChaosUnitTestAttribute(ChaosUnitCategory category) { }

    public string? Alias { get; init; }
    public ChaosRuntimeFeature Requires { get; init; }
    public ChaosEvidenceKind Evidence { get; init; }
    public byte Priority { get; init; }
}

[AttributeUsage(AttributeTargets.Method, AllowMultiple = false)]
public sealed class ChaosBenchmarkAttribute : Attribute
{
    public ChaosBenchmarkAttribute(ChaosBenchmarkCategory category, ChaosMetric metrics) { }

    public string? Alias { get; init; }
    public ChaosRuntimeFeature Requires { get; init; }
    public byte WarmupCount { get; init; }
    public ushort IterationCount { get; init; }
    public ushort InvocationCount { get; init; }
}
```

### 10.3 为什么不把 identity 也做成枚举

这是本版一个重要判断。

如果把每个测试点 identity 也做成公共枚举，会带来两个坏处：

- 每新增一个业务测试点都要更新公共 DLL
- 测试框架 DLL 会反向支配 subject 的增长

因此：

- 低基数、跨 subject 共享的维度用枚举
- 单个测试点 identity 默认从方法符号自动生成
- 只有在需要兼容短名或人类可读短名时，才使用可选 `Alias` 字符串

### 10.4 类型节省规则

本版要求如下：

- 分类字段优先 `byte`
- 小型 flags 优先 `ushort`
- 中型 feature flags 优先 `uint`
- 计数值优先 `byte` / `ushort`
- 不引入 `string[] tags`
- 不引入 `Dictionary<string, string>` 这类松散 metadata 容器

## 11. 声明发现与 compiled catalog

### 11.1 发现流程

1. 根据 `subject.manifest.json` 解析 source model。
2. 构建 subject 的 project graph 与 host input。
3. 计算 `engineeringProfile` 对应的系统级验证项与 workload。
4. 按 `testDeclarationMode` 判断是否执行 attribute discovery。
5. 若执行 discovery，则扫描编译产物中的：
   - `ChaosUnitTestAttribute`
   - `ChaosBenchmarkAttribute`
6. 合并为统一的 `CompiledSubjectCatalog`。

### 11.2 发现来源

声明发现应优先使用 metadata 读取，而不是运行时反射执行程序集。理由是：

- 现有仓库已经具备 metadata/attribute 读取能力
- 不需要实际加载业务程序集执行代码
- 更适合与 IL2CPP 的 loader / analysis 流程对齐

### 11.3 稳定 identity 规则

每个声明测试默认生成稳定 ID：

```text
<subject-id>::<assembly>::<declaring-type>::<method-signature>
```

若提供 `Alias`，则：

- `Alias` 只作为 selector/report 的短名别名
- 稳定主键仍然以自动生成的 canonical identity 为准

### 11.4 catalog 中的条目类型

compiled catalog 至少包含：

- `engineeringValidations[]`
- `engineeringWorkloads[]`
- `declaredUnitTests[]`
- `declaredBenchmarks[]`

这样能明确回答两个问题：

- 这个 subject 在工程层能验证什么
- 这个 subject 在源码层声明了哪些 proof/benchmark

## 12. 工程级验证模型

### 12.1 为什么工程级验证是核心

用户已经明确指出：

- solution 可能包含多个 C# project
- IL2CPP 必须验证“整套 solution 可被 ingest，并生成完整 native executable”

因此，本版把工程级验证定义为主验证层，而不是附属层。

### 12.2 工程级验证不走 `ChaosBenchmark`

以下对象不属于 `ChaosBenchmark`：

- `convert`
- `codegen`
- `native link`
- `generated native executable`
- `device package`
- `patch generation`

这些是 **system-owned engineering workload**。

它们的定义由 `engineeringProfile + orchestration profile` 决定，而不是由业务 solution 里的某个方法 attribute 决定。

### 12.3 建议的 `engineeringProfile`

| profile | 核心验证内容 |
| --- | --- |
| `managed-output` | project graph、managed build、managed runtime output |
| `native-executable` | project graph、convert、codegen、native link、native executable smoke |
| `device-package` | `native-executable` 全部内容 + package integrity + device deploy smoke |
| `hot-update-host` | host build、patch generation、patch load、roundtrip、rollback |

### 12.4 `golden` 的重新解释

`golden` 不再只是 fixture 桶，而是第一批工程 archetype subject 的历史来源：

- `GoldenSimpleLib` -> `SolutionSimpleLib`
- `GoldenMultiProject` -> `SolutionMultiProject`
- `GoldenWithPackage` -> `SolutionPackageReference`

`convert-golden-multi-project` 的长期归属应改为：

- `SolutionMultiProject` 的 engineering workload：`convert`

而不是继续长期留在 `tests/perf/`

## 13. 执行、选择与产物归属

### 13.1 统一选择模型

selector 层建议统一支持以下对象：

- `subject`
- `engineering-validation`
- `engineering-workload`
- `unit-test`
- `benchmark`
- `pipeline`

### 13.2 artifact 路径建议

```text
artifacts/
  subjects/
    <subject-id>/
      runs/
        <run-id>/
          engineering/
            validations/<kind>/
            workloads/<kind>/
          declared/
            unit/<stable-id>/
            benchmark/<stable-id>/
```

这样有几个好处：

- 工程级与声明级条目分层清晰
- 仍然能回溯到同一个 subject
- report/ledger 不需要再通过目录命名猜测语义

## 14. `tests/` 目标结构

### 14.1 基本原则

- `tests/` 只测试 generic mechanism
- `tests/fixtures/` 承载 generic fixture
- `tests/` 不再拥有 canonical solution subject source
- 不再使用 `phase*` / `stage*` 作为长期命名维度

### 14.2 建议结构

```text
tests/
  fixtures/
    contracts/
    registry/
    integration/

  contracts/
    schema/
    discovery/
    reporting/
    artifacts/

  unit/
    registry/
    selection/
    planning/
    execution/
    reporting/
    performance/
    compatibility/

  integration/
    cli/
    registry/
    pipelines/
    platform-hosts/
    device-runners/

  tooling/
    bootstrap/
    wrappers/
    tui/
    commands/
```

### 14.3 现有目录的归并方向

现有目录建议按以下方向收敛：

- `tests/proof/` -> 删除或并入 `tests/unit/` / `tests/integration/`
- `tests/perf/` -> 仅保留 generic perf mechanism；subject-specific engineering workload 迁回 subject
- `tests/platform/` -> 并入 `tests/integration/platform-hosts/`
- `tests/gate/` -> 并入 `tests/integration/pipelines/`
- `tests/registry/` 中的 fixture 化内容 -> `tests/fixtures/registry/`
- `tests/integration/run/fixtures/` -> `tests/fixtures/integration/`

## 15. 历史 subject 的迁移规则

### 15.1 archetype subjects

直接升级为 solution archetype：

- `GoldenSimpleLib`
- `GoldenMultiProject`
- `GoldenWithPackage`

### 15.2 proof-like roots

原则上应并入更大的 canonical solution subject，并把 proof 改写为：

- 工程级 validation
- 或 `ChaosUnitTest`

### 15.3 benchmark-like roots

原则上应并入对应 solution subject，并把 benchmark 改写为：

- 工程级 workload
- 或 `ChaosBenchmark`

### 15.4 允许保留独立 subject 的条件

只有在以下条件明显成立时，才允许保留独立顶层 subject：

- source bundle 物理上独立
- project graph 明显不同
- dependency model 明显不同
- executable plan 明显不同
- 长期 owner 与 artifact lineage 独立

## 16. 全局验收标准

本版架构完成后，应满足以下验收标准：

- 新增一个纯工程样例时，只需新增一个 solution subject，无需再造 proof/benchmark 顶层根。
- 新增一个 runtime proof 或 runtime benchmark 时，只需在 subject solution 内新增方法并标注 `ChaosUnitTest` / `ChaosBenchmark`。
- 一个未引用测试框架 DLL 的 subject 仍能完整参与工程级验证。
- `subject.manifest.json` 不再内联大段 `executionPipelines` / `environmentMatrices`。
- `subjects/` 默认不再出现 `scenarios/` / `workloads/` 物理目录要求。
- `tests/` 不再保留 `phase*` / `stage*` 文件命名作为长期结构。
- `tests/perf/` 不再承载 subject-specific canonical workload。
- 顶层新增 subject 时，必须证明它是新的 solution source bundle，而不是新增一个 proof/benchmark 切片。
- 低基数字段的 authoring / runtime 表达均具备 typed 边界，不依赖松散字符串协议。

## 17. 风险与缓解

### 17.1 风险：把所有测试都误塞进 attribute

缓解：

- 明确工程级 validation/workload 不走 attribute
- 只有源码内部可声明的入口才用 `ChaosUnitTest` / `ChaosBenchmark`

### 17.2 风险：枚举过度中心化

缓解：

- 只有低基数共享维度使用枚举
- 单个测试 identity 默认由方法符号自动派生
- `Alias` 字符串只作可选短名

### 17.3 风险：subject manifest 虽然变薄，但 shared profile 外部化后变得难找

缓解：

- 对 shared profile 建立清晰 registry 入口
- 在 subject report 中回显实际使用的 profile 展开结果

### 17.4 风险：工程级验证与声明测试报告混淆

缓解：

- artifact 与 report 路径明确分成 `engineering/` 与 `declared/`
- selector 层也明确区分 entry family

## 18. 本版结论

v1.03 的最终结论是：

- `subject = canonical solution source bundle`
- 工程级验证是核心测试层
- 源码内 proof / runtime benchmark 用 `ChaosUnitTest` / `ChaosBenchmark`
- `subject.manifest.json` 只保留薄 orchestrator 信息
- `scenario/workload` 保留为逻辑分类，不再要求长期物理目录化
- 少字符串、强类型、紧凑枚举是本版元数据设计的硬约束
