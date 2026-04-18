# Managed Native HotUpdate 测试主线规范

## 1. 目标

本规范定义仓库后续唯一正式测试主线：

`managed solution -> dotnet 8 collection analysis -> collection files -> managed test project -> native project -> native test project -> hotupdate patch project + hotupdate test host project`

它同时约束：

- subject 的测试声明方式
- collection / manifest / codegen 的分层边界
- managed / native / hotupdate 三条执行链路
- Python 自动化测试与验证顺序
- IL2CPP 代码生成与文件结构约定

## 1.1 Authority 边界

- 本文档拥有统一测试主线、分层边界、`collection / manifest / codegen` contract 的 authority。
- `wiki/06-测试验证/INDEX.md` 拥有正式验证入口、对象导航与 completion 前的对象选择顺序。
- `wiki/06-测试验证/AOT新Feature接入自测规范.md` 拥有 AOT capability intake、owner subject、proof / benchmark / hotupdate obligation 与 formal verification 顺序。
- 任何命中 AOT / IL2CPP / test governance 的规划和实现，都必须把 obligation 显式写进计划，而不是留到实现时临时判断。

## 2. 强制规则

### 2.1 managed solution 只直接引用 `Chaos.TestFramework.Sdk`

- subject 目录只保留 solution 级 source。
- UnitTest / Benchmark / HotUpdate 入口全部通过 `Chaos.TestFramework.Sdk` 声明。
- subject / managed project 不再直接引用 `Chaos.TestFramework.Runtime`。

### 2.2 `Sdk` 与 `Runtime` 分层固定

`Chaos.TestFramework.Sdk` 负责：

- attribute
- enum
- 稳定 metadata contract
- `Assert`
- `ChaosAssertionException`
- 最小 authoring helper

`Chaos.TestFramework.Runtime` 负责：

- collection file loader
- 运行时调度
- benchmark harness
- result / report sink
- managed test host
- hotupdate test host

`.NET 8` collector 只分析 `Sdk` 声明并产出 collection file，不属于以上两个程序集本体。

### 2.3 collection file 是唯一跨执行形态契约

- managed / native / hotupdate 必须消费同一份 versioned collection schema。
- collection file 默认放在 `artifacts/subjects/<subject-id>/collections/`。
- collection file 必须由 `.NET 8` collector 统一产出。
- managed test project、native test project、hotupdate test host project 只消费 collection file，不再各自重新扫描 `Sdk`。

### 2.4 execution binding 必须从 collection 分层

- 执行形态特有绑定信息不写进 collection file。
- native 使用 `native dispatch manifest`。
- hotupdate 使用 `hotupdate binding manifest`。

### 2.5 native test project 只消费契约化输入

- native test project 负责加载 collection file 和 native dispatch manifest。
- native test project 不直接依赖 `Chaos.TestFramework.Runtime` DLL。
- 但必须遵守 `Runtime` 定义的执行协议与结果协议。

### 2.6 hotupdate 采用 patch / host 分离

- hotupdate patch project 只引用 `Sdk`。
- hotupdate test host project 引用 `Sdk + Runtime`。
- host 负责 collection 消费、patch 装载、绑定、执行与结果汇总。

### 2.7 Python 测试必须模板化

- Python 单测优先通过模板拉起最小 managed / native / hotupdate 样例。
- 尽量减少长期保留的手写 C# fixture。
- benchmark 手工复跑、控制台日志或 dashboard 结果不能替代正式自动测试。
- 任何测试阶段一旦出现 `dotnet build` / `dotnet test` / `msbuild` 编译崩溃，当前验证立即视为失败，必须先定位并修复根因。

### 2.8 file-level codegen 默认 Scriban

以下场景默认使用 Scriban：

- managed test project 生成
- native test project 生成
- hotupdate test host project 生成
- manifest 相关整文件生成
- IL2CPP 整体 C# / C++ 发射
- `NativeAotEmitter` 文件级输出

### 2.9 cutover 后删除旧写法

禁止长期保留：

- `Chaos.TestFramework.Annotation`
- compatibility alias
- 旧 runner / registry / discovery 双轨逻辑
- 已被模板替代的旧 fixture
- 已被 manifest 替代的旧绑定路径
- canonical proof / benchmark correctness 依赖 `Console.WriteLine` 或 `ChaosEvidenceKind.Stdout`

### 2.10 owner subject 是 obligation authority

- `subject.features.json` 是 completed feature 的 owner subject 与 proof / benchmark obligation authority。
- 当前 canonical owner subjects 固定为 `SolutionCorePack`、`MixedExecutionFeaturePack`、`HotUpdateHostPack`。
- `proofRequired = true` 的 feature 必须在 owner subject 中存在正式 proof 资产。
- `benchmarkRequired = true` 的 feature 必须在 owner subject 中存在正式 benchmark 资产。
- 命中 AOT onboarding 的计划必须显式冻结：
  - `capabilityFamily`
  - `capabilityItem`
  - `ownerSubjectId`
  - `proofRequired`
  - `benchmarkRequired`
  - `hotupdateImpact`
  - `formalVerificationObjects`
  - `requiredGates`

## 3. 推荐结构

### 3.1 TestFramework

```text
src/reference/
  Chaos.TestFramework.Sdk/
  Chaos.TestFramework.Runtime/
```

### 3.2 collector

```text
src/tools/
  Chaos.IL2CPP.Tools.TestFramework.CollectionGen/
  Chaos.IL2CPP.Tools.Benchmark.WorkloadEntry.PerfHarness/
  Chaos.IL2CPP.Tools.FoundationPack.Analysis/
```

### 3.3 collection 与执行产物

```text
artifacts/subjects/<subject-id>/
  collections/
  managed-test/
  native/
  native-test/
  hotupdate/
  hotupdate-test/
  reports/
```

### 3.4 Python 模板

```text
tests/
  templates/
    managed-solution/
    managed-test-project/
    native-test-project/
    hotupdate-patch-project/
    hotupdate-test-host-project/
```

## 4. 流程定义

### 4.1 managed solution

- subject solution 中的测试入口使用 `Chaos.TestFramework.Sdk` 标注。
- 断言、计量与最小 authoring helper 由 `Sdk` 提供。

### 4.2 collection phase

- `.NET 8` collector 分析 managed project 中的 `Sdk` 声明。
- collector 输出 UnitTest / Benchmark / HotUpdate collection file。
- collection file 成为 managed / native / hotupdate 三条执行链路的统一输入清单。

### 4.3 managed test project

- 由生成链路产出。
- 引用 `Sdk + Runtime`。
- 加载 collection file 并执行 managed UnitTest / Benchmark。

### 4.4 native test project

- IL2CPP 先生成 native project。
- native test project 通过 collection file 和 native dispatch manifest 选择并执行 native UnitTest / Benchmark。
- native 侧不允许手工补充额外测试注册。

### 4.5 hotupdate patch / host

- patch project 只引用 `Sdk`。
- host project 引用 `Sdk + Runtime`。
- host 通过 collection file 与 hotupdate binding manifest 驱动需要验证的 hotupdate 项。

## 5. 自动测试规范

### 5.1 bugfix 先补失败自动测试

以下改动必须先有失败自动测试：

- IL2CPP compiler / linker / loader / semantic world bugfix
- `Sdk` / `Runtime` 行为改动
- `.NET 8` collector 改动
- collection schema 改动
- manifest 生成逻辑改动
- runner / host 生成逻辑改动
- benchmark 数据生成逻辑改动

### 5.2 自动测试优先级

1. `tests/unit/**`
2. `tests/contracts/**`
3. `tests/integration/**`
4. subject 级 managed / native / hotupdate 实跑

### 5.3 `dotnet` 编译崩溃闸门

- 只要 `dotnet build` / `dotnet test` / `msbuild` 崩溃，当前验证立即视为失败。
- 必须保留并检查 `stderr`、`binlog`、栈信息或 dump。
- 根因修复之前，不得声称测试通过或任务完成。

## 6. IL2CPP 代码生成规范

推荐结构：

```text
src/managed/Chaos.IL2CPP.CodeGen/
  Planner/
  Emitter/
  RuntimeSupport/
  Templates/
```

职责边界：

- `Planner`: lowering、closure、dispatch、ABI planning
- `Emitter`: plan 到文件的渲染
- `RuntimeSupport`: helper / prelude / support contract
- `Templates`: Scriban 模板

## 7. 验收标准

满足以下条件，视为流程规范落地：

- 新增测试项时，只需在 subject source 中新增带 `Sdk` attribute 的入口。
- subject / managed project 只引用 `Sdk`。
- 存在稳定的 `.NET 8` collector 从 managed project 生成 collection file。
- managed / native / hotupdate 执行链都由 collection file 驱动。
- native dispatch manifest 与 hotupdate binding manifest 边界清晰。
- `Sdk` 与 `Runtime` 边界清晰。
- Python 测试优先模板化。
- file-level codegen 默认 Scriban。
- 不保留长期 compatibility alias。

## 8. AOT 新 feature 接入验证

AOT 主线新增 feature 的正式顺序以 `wiki/06-测试验证/AOT新Feature接入自测规范.md` 为准。本文只定义统一主线与分层边界；正式自测顺序固定为：

`tests/unit|contracts -> owner subject managed proof -> owner subject native proof -> hotupdate proof(按需) -> benchmark(按需)`

其中：

- 先确定 capability 的 `owner subject`
- 先过 `collector -> registry -> workspace` 三层闸门
- 计划中必须写出 `formalVerificationObjects` 与 `requiredGates`
- `completed` 前必须跑正式对象，不能用“项目大测大致通过”替代
- benchmark 只作为补充证据，不替代 correctness

## 9. Legacy Cutover Contract

- product pipeline 不得回退到 `ChaosSourceEntryArguments`、`DeclaredProofEntriesBySourceEntry` 或旧 subject 命名协议
- canonical subject 不得重新引入 `validation`、`Archetypes`、`FeatureSlices`、`PatchModules`、`ManagedBridge`、`Lowering`、`Launcher` 等旧目录语义

## 10. `System.Private.CoreLib.dll` 到 `SolutionCorePack` proof 的实际文件流向

`System.Private.CoreLib.dll` 在这条主线里不是“直接塞进 proof 项目里执行”的角色，而是 foundation nativeization 的 runtime authority 输入；`SolutionCorePack` 是消费这些 authority 的 proof subject。两条文件流会在同一次 subject run workspace 里汇合。

### 10.1 runtime nativeization 主链

```text
src/dll/dotnet-foundation/net8.0/runtime/System.Private.CoreLib.dll
src/dll/dotnet-foundation/net10.0/runtime/System.Private.CoreLib.dll
  -> docs/dev/completed/20260418-02-phase-0-framework-pack-catalog-and-asset-policy/framework-catalog-v1-01.json
  -> src/tools/Chaos.IL2CPP.Tools.FoundationPack.Analysis/Program.cs
  -> docs/dev/completed/20260418-04-phase-2-system-private-corelib-certification-substrate/assembly-semantic-ir-system-private-corelib-v1-01.json
  -> docs/dev/completed/20260418-04-phase-2-system-private-corelib-certification-substrate/runtime-helper-contracts-v1-01.json
  -> docs/dev/completed/20260418-05-phase-3-system-private-corelib-semantic-family-nativeization/semantic-family-execution-entry-v1-01.json
  -> artifacts/tests/registry/current/index.json
  -> run test declared-unit-test --id declared-unit-test/SolutionCorePack::CoreRuntimeFeatures::CoreRuntimeFeatures.ArithmeticOpsProofEntry::Run()
```

含义：

- `System.Private.CoreLib.dll` 先被 phase2 / phase3 产物语义化。
- phase3 把“当前 corelib family 用哪些 proof / benchmark / engineering gate 验证”写成 execution-entry。
- registry 根据 execution-entry 暴露 canonical command。

### 10.2 官方 ref DLL 进入 subject host-input 的分支

```text
src/dll/dotnet-foundation/net8.0/ref/mscorlib.dll
src/dll/dotnet-foundation/net8.0/ref/netstandard.dll
src/dll/dotnet-foundation/net8.0/ref/System.Runtime.dll
src/dll/dotnet-foundation/net8.0/ref/System.Runtime.Extensions.dll
src/dll/dotnet-foundation/net8.0/ref/System.Console.dll
src/dll/dotnet-foundation/net8.0/ref/System.Collections.dll
src/dll/dotnet-foundation/net8.0/ref/System.Linq.dll
  -> subjects/SolutionCorePack/source/EngineeringScenarios/CoreLibReferenceSolution/App/GoldenCoreLibReference.App.csproj
  -> subjects/SolutionCorePack/source/SolutionCorePack.sln
  -> artifacts/subjects/SolutionCorePack/runs/<run-id>/analysis/host-input/
```

含义：

- 官方 ref DLL 现在通过 `src/dll/` 进入 `CoreLibReferenceSolution`。
- proof 执行时，solution 中的工程产物会一起进入同一次 run 的 `analysis/host-input/`。

### 10.3 一个实际 proof run 的文件落盘形态

以 `ArithmeticOpsProofEntry::Run()` 为例，实际消费链会落到以下路径形态：

```text
subjects/SolutionCorePack/source/Proofs/CoreRuntimeFeatures/PrimitivesAndOps/ArithmeticOpsProof.cs
  -> artifacts/subjects/SolutionCorePack/runs/<run-id>/analysis/source/source.manifest.json
  -> artifacts/subjects/SolutionCorePack/runs/<run-id>/analysis/host-input/host-input.manifest.json
  -> artifacts/subjects/SolutionCorePack/runs/<run-id>/analysis/analysis/aot-core-ir.json
  -> artifacts/subjects/SolutionCorePack/runs/<run-id>/analysis/generated/native-aot.plan.json
  -> artifacts/subjects/SolutionCorePack/runs/<run-id>/analysis/generated/generated/native-aot.generated.cpp
  -> artifacts/subjects/SolutionCorePack/runs/<run-id>/matrices/windows-native-check/build/build.manifest.json
  -> artifacts/subjects/SolutionCorePack/runs/<run-id>/matrices/windows-native-check/build/out/chaos_subject_native_aot.exe
  -> artifacts/subjects/SolutionCorePack/runs/<run-id>/matrices/windows-native-check/runtime/runtime.manifest.json
  -> artifacts/subjects/SolutionCorePack/runs/<run-id>/matrices/windows-native-check/runtime/stdout.log
  -> artifacts/subjects/SolutionCorePack/runs/<run-id>/run-report/summary.json
```

可以把整条链理解成三层：

- foundation 层：`src/dll/.../runtime/System.Private.CoreLib.dll` 提供 single-DLL semantic authority。
- subject 层：`SolutionCorePack` 提供可执行 proof entry、engineering validation 和 benchmark consumer。
- run 层：每次 `run test ...` 都在 `artifacts/subjects/SolutionCorePack/runs/<run-id>/` 下沉淀 `source -> host-input -> analysis -> generated -> build -> runtime -> report` 的完整闭环。
