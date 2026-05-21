# AOT新Feature接入自测规范

本规范用于约束 IL2CPP AOT 主线新增 capability / syntax / lowering / emitter / runtime-support feature 的标准自测流程。

固定主线为：

`tests/unit|contracts -> owner subject managed proof -> owner subject native proof -> hotupdate proof(按需) -> benchmark(按需)`

其中：

- `benchmark` 是补充证据，不替代 correctness。
- `collector -> registry -> workspace` 属于接线闸门，必须先过，再进入 proof 执行。
- 不先写散的 demo，不靠 `Console.WriteLine` 做外置判定。

## 0. Authority 边界

- 本文档拥有 AOT capability intake、owner subject、proof / benchmark / hotupdate obligation 与 formal verification 顺序的 authority。
- [`../../docs/architecture/subject-test-framework-v1/INDEX.md`](../../docs/architecture/subject-test-framework-v1/INDEX.md) 只负责统一测试主线、collection / manifest / codegen 分层与宿主边界。
- [`../../docs/architecture/verification-v1/spec.md`](../../docs/architecture/verification-v1/spec.md) 负责 formal verification、归并归档与 projection 口径。
- [`INDEX.md`](./INDEX.md) 负责正式验证入口、对象导航与 completion 前对象优先级。
- 命中本规范的计划，必须把 obligation 显式写入 `plan-v1-01.md`；不允许留到实现时临时决定。

## 1. 适用范围

以下改动默认适用本规范：

- 新增或扩展 typed IL / semantic / lowering / codegen / runtime support 能力
- 修复 AOT planner / emitter / ABI / manifest / generated file shape 相关 bug
- 调整会影响 managed/native/hotupdate 可观察行为的 feature
- 调整 declared proof / declared benchmark 的 capability 归属与执行链路

补充约束：

- 只要改动涉及结构化文本 generated file shape，默认应遵循 `Scriban` 优先规则。
- 模板能力不足时，先扩展 `Scriban`，具体规范见 [`../04-工具与集成/scriban-usage-and-codegen-rules.md`](../04-%E5%B7%A5%E5%85%B7%E4%B8%8E%E9%9B%86%E6%88%90/scriban-usage-and-codegen-rules.md)。

## 2. 核心结论

### 2.1 验收层级

本仓库对 AOT 新 feature 的验收层级固定为：

1. `tests/unit/**` 与 `tests/contracts/**`
2. owner subject 的 managed proof
3. owner subject 的 native proof
4. owner subject 或 consumer subject 的 hotupdate proof（按规则触发）
5. benchmark（仅在 feature 具有性能意义时补充）

### 2.2 owner subject 是唯一 canonical verification home

每个 capability 只能有一个 `owner subject`，用于承载该 capability 的：

- canonical managed proof
- canonical native proof
- canonical benchmark
- 按需的 hotupdate proof

当前主归属固定为：

- `SolutionCorePack`
  负责 AOT 主线默认能力，包括 typed IL 翻译、native generated correctness、solution/project/reference 级主验证。
- `MixedExecutionFeaturePack`
  负责 mixed execution / bridge dispatch / native-interpreter 协同能力。
- `HotUpdateHostPack`
  负责 patch / host / binding / metadata visibility / hotupdate 跨边界能力。

### 2.3 非 owner subject 只允许补 consumer smoke

非 owner subject 默认不复制完整 proof / benchmark。

只有在存在独立集成差异时，才允许补一层薄的 consumer smoke。独立集成差异包括：

- 执行体不同，例如 patch/host、mixed execution bridge
- 绑定方式不同，例如 metadata-only、bridge dispatch、patch binding
- 可见性规则不同，例如跨程序集、跨边界类型或方法解析
- 调度或落体策略不同，例如 native generated 与 interpreter fallback 分流
- 工程装配方式不同且会影响行为，例如特殊 solution / reference bundle / host-patch 组合

如果只是“同一 feature 在另一个 subject 里也能跑”，不构成新增 proof 的理由。

## 3. HotUpdate Proof 触发规则

### 3.1 分级

本规范将 hotupdate 影响分为三个等级：

- `None`
  不要求 hotupdate proof。
- `Smoke`
  需要证明“可加载、可绑定、可调用”，但不一定需要展开完整语义断言。
- `Proof`
  需要使用 `Assert` 执行完整语义验证。

### 3.2 触发条件

命中以下任一条件，至少要求 `Smoke`，大多数情况应提升为 `Proof`：

- feature 改动了 metadata 可见性、token 解析、类型/方法/字段查找
- feature 会穿过 `host <-> patch` 调用边界
- feature 涉及 delegate、virtual、interface、reflection 在热更边界上的行为
- feature 涉及 generic instantiation / generic sharing / rgctx，且 patch 侧会消费
- feature 改动了静态字段、类型布局、ABI、marshal、调用约定，且 patch/host 会共享契约
- feature 涉及 interpreter/native/bridge 的路由或 fallback
- feature 改动了 assembly/reference bundle/solution 装配方式，并可能影响 patch load/bind

以下场景允许 `None`：

- 纯内部优化，没有新增外部语义
- 只影响 emitter 内部文件形状，不影响 patch 可观察行为
- managed/native proof 已完整覆盖，且明确不穿过 hotupdate 边界

### 3.3 owner subject 与 hotupdate 的关系

- 如果 capability 的 owner 本身是 `HotUpdateHostPack`，hotupdate proof 属于该 owner 的 canonical proof。
- 如果 capability 的 owner 是 `SolutionCorePack`，但命中了 hotupdate 触发条件：
  - canonical managed/native proof 仍放在 `SolutionCorePack`
  - 额外在 `HotUpdateHostPack` 补一层 hotupdate smoke 或 proof
- 如果 capability 的 owner 是 `MixedExecutionFeaturePack`，且 mixed execution 会穿过 hotupdate 边界，按同样规则在热更侧补边界验证

### 3.4 canonical generic runtime kernel 的补充 obligation

- 命中 dispatch / reflection / hotupdate generic 边界时，`formalVerificationObjects` 至少覆盖：reflection construction/invoke proof、hotupdate `host -> patch` proof、hotupdate `patch -> host` proof、advanced carrier proof、generated code review、generated cpp size/memory benchmark。
- 这一路径上的最终执行 authority 固定为 `InstantiationStubId + generation`；`subjectId`、metadata token、legacy slot string 不得重新成为 runtime final authority。
- 如 hotupdate 包因为 `PackageFormatVersion`、`KernelArtifactVersion` 或 target AOT mismatch 被拒绝，必须证明 transactional activate / rollback 不会污染 rollback history。

## 4. 标准执行步骤

### Step 0: 判定 capability 与 owner subject

在写任何 subject proof 前，先明确：

- 本次改动对应哪个 `capabilityFamily`
- 本次改动对应哪个 `capabilityItem`
- `ownerSubjectId` 是什么
- `proofRequired` 是什么
- `benchmarkRequired` 是什么
- `hotupdateImpact` 是 `None / Smoke / Proof`
- `formalVerificationObjects` 是哪些正式对象或 declared entry
- `requiredGates` 是什么；未显式改写时默认 `collector -> registry -> workspace`

约束：

- `formalVerificationObjects` 用于定义 completed 前必须执行的正式验证对象
- 如果暂时没有可复用的正式对象，先补 object 或 case，再把它写回 `formalVerificationObjects`
- 不允许先写 demo 或先随手放进某个 subject，再回头补归属与 obligation

### Step 1: 先补最小失败自动化测试

先补失败测试，再改实现。

推荐顺序：

- `tests/unit/**`
  锁 planner、loader、semantic、lowering、emitter 规则
- `tests/contracts/**`
  锁 collection schema、registry shape、generated file shape、manifest / ABI / runtime contract

补充约束：

- 如果改动涉及 async generic family / runtime-skeleton / generated wrapper carrier，必须至少补一个“结果类型与参数 carrier 脱钩”的 generated artifact 回归，例如 `Task<string>(int)`；不允许只用 `Task<int>(int)`、`Task<bool>(bool)`、`Task<string>(string)` 这类同构 case 证明泛化已完成。

没有这层失败测试，不直接修改 AOT 主线实现。

### Step 2: 在 owner subject 中补 canonical managed proof

proof 必须满足：

- 只放在 owner subject
- 通过 `Chaos.TestFramework.Sdk` attribute 声明
- 使用 `Assert` 编写断言
- 以单能力聚焦为主，不把多个 feature 混在同一个 proof 里
- 形成稳定 `stableId`

managed proof 是该 capability 的语义真源，用来先证明“语义本身是对的”。

### Step 3: 按需补 canonical benchmark

只有 feature 会影响性能或调度面时，才补 benchmark。

典型需要 benchmark 的场景：

- 分派成本
- 泛型共享
- 装箱
- GC 压力
- bridge dispatch
- ABI 包装
- patch load / bind 开销

纯 correctness feature 不强行补 benchmark。

### Step 4: 先过 `requiredGates`

这一步不是 correctness 验收，而是接线验收。默认 gate 是 `collector -> registry -> workspace`；计划改写 gate 时，必须给出明确理由。

推荐先执行：

```powershell
run generate project subject --id subject/<owner-subject> --refresh-generated
run test registry list --json
```

#### 4.1 Collector Gate

必须确认：

- 新 entry 已进入 declared collection
- `stableId` 正确
- `entryIndex` 存在且稳定
- proof 进入 `declaredUnitTests`
- benchmark 进入 `declaredBenchmarks`
- 关键元数据未丢失，例如 `assemblyName`、`declaringType`、`methodName`、`methodSignature`、`category`、`capabilityFamily`、`capabilityItem`
- benchmark 还需保留 `metrics`、`modes`、`warmupCount`、`iterationCount`、`invocationCount`
- 不存在重复 `stableId`

#### 4.2 Registry Gate

必须确认：

- 能在 `run test registry list --json` 中看到对应对象
- `declared-unit-test/<stable-id>` 或 `declared-benchmark/<stable-id>` 可解析
- `ownerSubjectId` 正确
- `defaultMatrixId` / `defaultGoalId` 正确
- `capabilityFamily` / `capabilityItem` / `supportStates` 正确
- `proofRequired` / `benchmarkRequired` 与 capability contract 一致
- registry consistency 不报错

#### 4.3 Workspace Gate

必须确认：

- `workspace.manifest.json` 存在且版本正确
- `managedTestProjects` 中已挂上对应 `proof-host`
- benchmark feature 还应挂上对应 `benchmark-host`
- 如果该 matrix 支持 native，`nativeTestProjects` 中必须挂上对应 host
- 如果该 feature 要求 hotupdate，`hotupdateTestProjects` 中必须挂上对应 host，且 `bindingManifestPath` 非空
- `matrices[*]` 已把 `managedTestProjectIds`、`nativeTestProjectIds`、`hotupdateTestProjectIds` 正确挂入
- 各 host 消费的是同一份 declared collection，而不是各自重新扫描源码
- `stableId -> entryIndex` 能从 collection 一路追到最终运行清单

### Step 5: 先跑 managed proof

managed proof 是 correctness 的第一层正式执行验收。

要求：

- 通过 declared proof 正式入口执行
- 使用与 owner subject 对应的 declared entry
- 失败时优先判断是否为 feature 语义问题，而不是直接进入 native 排查

默认命令入口为：

```powershell
run test declared-unit-test --id declared-unit-test/<stable-id>
```

### Step 6: 再跑 native proof

这是 AOT 主线的核心验收层。

要求：

- 使用与 managed proof 相同的 declared entry
- 经过 planner -> lowering -> emitter -> native host 的完整链路
- 不接受“managed 绿了就算完成”
- 不额外维护一套 native 专用手工注册测试

### Step 7: 按规则补 hotupdate proof

如果 Step 0 判定 hotupdate 等级为 `Smoke` 或 `Proof`，必须继续执行 hotupdate 验证。

要求：

- 使用 collection + binding manifest 驱动，不重新扫描源码
- `Smoke` 至少证明可加载、可绑定、可调用
- `Proof` 必须用 `Assert` 做真实语义验证

### Step 8: 按需跑 benchmark

benchmark 仅在 Step 3 判定有性能意义时执行。

默认命令入口为：

```powershell
run test declared-benchmark --id declared-benchmark/<stable-id>
```

benchmark 的职责是补充成本证据，不替代 correctness 层。

### Step 9: 回归受影响主线与 formal verification objects

最后必须跑受影响回归，而不是只看单个 entry。

至少包括：

- owner subject 的受影响 declared proof / benchmark
- 相关 consumer smoke（如果存在）
- 受影响 module / system 验证
- `formalVerificationObjects` 中声明的全部对象

## 5. 完成标准

一个 AOT 新 feature 满足以下条件，才算完成接入：

- capability 与 owner subject 已明确
- obligation 已在计划中显式冻结：`capabilityFamily`、`capabilityItem`、`ownerSubjectId`、`proofRequired`、`benchmarkRequired`、`hotupdateImpact`、`formalVerificationObjects`、`requiredGates`
- `tests/unit` 或 `tests/contracts` 已锁住最小 contract
- owner subject 中存在 canonical managed proof
- 按需存在 canonical benchmark
- `requiredGates` 已通过
- managed proof 已通过
- native proof 已通过
- 命中 hotupdate 触发条件时，对应 hotupdate smoke / proof 已通过
- `formalVerificationObjects` 已全部通过；如果原本缺失，已补齐正式对象或 case 并通过
- 受影响回归已通过
- 如果本轮同时改动 formal report / projection contract，例如 `Program / DLL / Verification Project / Artifact` 报告对象、`latest/master/reports` 字段、projection 页面字段或证据链接规则，则 formal object 与回归通过后还必须执行 `run verify verification-v1 --json` 刷新正式数据，并确认对应 `artifact/verification-catalog/projections/**` 派生产物已更新

## 6. 反模式

以下做法不符合本规范：

- 先写散 demo，再回头决定 owner subject
- 不补失败测试，直接改 AOT 实现
- 只在 managed 绿，不验证 native
- 用 `Console.WriteLine` 或 dashboard 结果替代 `Assert`
- benchmark 代替 correctness
- 在多个 subject 中复制同一 capability 的完整 proof
- managed/native/hotupdate 各自维护独立测试清单
- collection / registry / workspace 未验收就直接进入 proof 执行

## 7. 相关文档

- [`新增测试接入规范.md`](./新增测试接入规范.md)
- [`../04-工具与集成/统一测试框架.md`](../04-%E5%B7%A5%E5%85%B7%E4%B8%8E%E9%9B%86%E6%88%90/%E7%BB%9F%E4%B8%80%E6%B5%8B%E8%AF%95%E6%A1%86%E6%9E%B6.md)
- [`../../docs/architecture/subject-test-framework-v1/INDEX.md`](../../docs/architecture/subject-test-framework-v1/INDEX.md)
- [`../../docs/architecture/verification-v1/spec.md`](../../docs/architecture/verification-v1/spec.md)

## 8. Foundation DLL Capability Verification Supplement

When capability onboarding hits the foundation-dll translation-verification mainline, the following extra rules apply.

### 8.1 New DLL Onboarding

When a new DLL enters translation-verification scope, all four layers must be generated together:

- Truth Layer: DLL / family / method authority
- Evidence Layer: functional / benchmark / hotupdate case mapping and review bundle skeleton
- Execution Layer: generated `src/`, `src/patch/`, `managed_test/`, `il2cpp_dist/`, `native_test/` artifacts and solution/cmake wiring
- Projection Layer: DLL / family / method aggregate skeleton

It is not allowed to only add dashboard data or only add partial proof hosts.

### 8.2 Method-Level Full Mapping

foundation-dll `Subject Validation` does not allow weak authority.

It must establish:

- `methodSubjectId -> test case`
- `methodSubjectId -> benchmark case`
- `methodSubjectId -> hotupdate case`

### 8.3 A2 Source-Annotation-First

foundation-dll case mapping uses `A2 Source-Annotation-First`:

- generated source / metadata annotations are the case-mapping truth
- the annotation scanner may only read generated source / metadata and produce formal case indexes
- the annotation scanner may not infer truth from runtime results

Annotations should prefer enum-backed attributes. String should be kept only for `methodSubjectId` and similarly non-enumerable fields.

### 8.4 Directory Contract

Per family, fixed directories are:

- `testing/foundation-dll/<assembly>/<family>/handwritten/` — **READ ONLY** handwrite partial class `.cs` 源（管线只读不写）
- `testing/foundation-dll/<assembly>/<family>/src/` — handwrite managed source（legacy，已弃用，新 family 用 handwritten/）
- `testing/foundation-dll/<assembly>/<family>/src/patch/` — handwrite patch source
- `testing/foundation-dll/<assembly>/<family>/managed_test/tests/` — managed xunit tests
- `testing/foundation-dll/<assembly>/<family>/managed_test/benchmarks/` — managed benchmarks
- `testing/foundation-dll/<assembly>/<family>/il2cpp_dist/` — generated il2cpp output (not committed)
- `testing/foundation-dll/<assembly>/<family>/native_test/benchmark/` — native benchmark entry (committed)
- `testing/foundation-dll/<assembly>/<family>/native_test/hotupdate/` — native hotupdate skeleton (committed)
- `testing/foundation-dll/<assembly>/<family>/reports/` — test reports (not committed)

**`handwritten/` 保护规则**：
- 管线只从该目录读 `.cs` 文件，从不写入
- `il2cpp_dist/entrypoint/` 可能被 auto-generate 覆盖，但 `handwritten/` 保持不变
- Partial class 模式（仅 `.cs` 文件，无 `.csproj`）→ fall through 到 `generate_and_build()` 正常执行两阶段探测
- Legacy 全项目模式（包含 `.csproj`）→ 直接 `dotnet build`，不经过 auto-generate

Generated project/solution artifacts are centralized outside the family tree:

- `solution/` — managed .sln + .csproj (not committed)
- `solution_native/` — cmake project forwarding (not committed)

### 8.5 completed-before Gate

foundation-dll family completed-before gate is fixed to `G2 reviewable-bundle`.

Each in-scope family must satisfy:

- generated source snapshot archived
- case index generated
- latest execution results archived
- `ReviewBundle` generated
- solution executable

### 8.6 Formal Main Entry And Local Accelerators

The formal main entry is fixed to:

- `FoundationDllTranslationSolution.sln`

The system must also support local accelerated execution:

- by DLL
- by family
- by family `managed_test/`, `native_test/`, `il2cpp_dist/`

These local entries are development accelerators only and are not new formal main entries.
