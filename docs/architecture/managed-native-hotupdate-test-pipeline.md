# Managed Native HotUpdate 测试主线规范

## 1. 目标

本规范定义本仓库后续唯一正式测试主线：

`managed solution -> dotnet 8 collection analysis -> collection files -> managed test project -> native project -> native test project -> hotupdate patch project + hotupdate test host project`

它同时约束：

- subject 测试声明方式
- collection 产物契约
- managed/native/hotupdate 三条执行链路
- Python 自动测试策略
- IL2CPP codegen 与文件结构规则
- 旧写法清理规则

## 1.1 Authority 边界

- 本文档拥有统一测试主线、分层边界、collection / manifest / codegen contract 的 authority。
- [`wiki/06-测试验证/INDEX.md`](../../wiki/06-测试验证/INDEX.md) 拥有正式验证入口、对象导航与 completion 前的对象选择顺序。
- [`wiki/06-测试验证/AOT新Feature接入自测规范.md`](../../wiki/06-测试验证/AOT新Feature接入自测规范.md) 拥有 AOT 新 capability 的 intake、owner subject、proof / benchmark / hotupdate obligation 与 formal verification 顺序。
- 计划和执行阶段命中 AOT / IL2CPP / test governance 任务时，必须把 authority 文档中的 obligation 显式写入计划，而不是留给实现时临时判断。

## 2. 强制规则

### 2.1 managed solution 只直接引用 `Chaos.TestFramework.Sdk`

- subject 目录只保留 solution 级 source
- UnitTest / Benchmark / HotUpdate 项全部通过 `Chaos.TestFramework.Sdk` 声明
- 断言通过 `Chaos.TestFramework.Sdk.Assert` 完成
- subject / managed project 不再直接引用 `Runtime`

### 2.2 `Chaos.TestFramework.Sdk` 与 `Chaos.TestFramework.Runtime` 分层

`Chaos.TestFramework.Sdk` 负责：

- attribute
- enums
- 稳定 metadata contract
- `Assert`
- `ChaosAssertionException`
- 最小 assertion state / subject authoring helper

`Chaos.TestFramework.Runtime` 负责：

- collection file loader
- 运行时调度
- benchmark harness
- result/report sink
- managed test host
- hotupdate test host

`.NET 8` collector 负责分析 `Sdk` 声明并生成 collection file，不属于以上两个程序集本体。

### 2.3 collection file 是唯一跨执行形态契约

managed、native、hotupdate 必须消费同一组 versioned collection schema。

schema 规则：

- 优先使用枚举值，不使用自由字符串表示类别、模式、指标、能力等分类字段
- 只在不可避免的标识符和路径上保留字符串
- collection file 默认放在 `artifacts/subjects/<subject-id>/collections/`
- collection file 必须由 `.NET 8` collector 分析 managed project 中的 `Chaos.TestFramework.Sdk` 声明后统一产出
- managed test project、native test project、hotupdate test host project 只消费 collection file，不各自重新扫描 `Sdk`

### 2.4 execution binding 必须从 collection 分层

执行形态特有绑定信息不写入 collection file。

推荐：

- native：使用 native dispatch manifest
- hotupdate：使用 hotupdate binding manifest

### 2.5 native test project 采用薄宿主

- native test project 负责加载 collection file 与 native dispatch manifest
- native test project 不直接依赖 `Chaos.TestFramework.Runtime` DLL
- 但必须遵循 `Runtime` 定义的执行协议与结果协议

### 2.6 hotupdate 采用 patch/host 分离

- hotupdate patch project 只引用 `Sdk`
- hotupdate test host project 引用 `Sdk + Runtime`
- host 负责 collection 消费、patch 装载、绑定、执行与结果汇总

### 2.7 Python 测试必须模板化

- Python 单测优先通过模板拉起最小 managed/native/hotupdate 样例
- 尽量减少长期保留的手写 C# fixture
- benchmark 手工复跑、控制台日志或 dashboard 结果，不能替代正式自动测试
- 测试阶段如果 `dotnet build` / `dotnet test` / `msbuild` 编译崩溃，必须先检查崩溃原因并修复，再继续后续验证

### 2.8 file-level codegen 默认 Scriban

以下场景默认使用 Scriban：

- managed test project 生成
- native test project 生成
- hotupdate test host project 生成
- manifest 生成相关整文件
- IL2CPP 生成完整 C# / C++ 文件
- `NativeAotEmitter` 的文件级输出

### 2.9 迁移完成后删除旧写法

禁止长期保留：

- `Chaos.TestFramework.Annotation`
- compatibility alias
- 旧 runner / registry / discovery 双轨逻辑
- 已被模板替代的旧 fixture
- 已被 manifest 机制替代的旧绑定路径
- canonical subject 的 formal proof / benchmark / host correctness 资产中出现 `Console.WriteLine` 或 `ChaosEvidenceKind.Stdout`

### 2.10 owner subject 与 obligation authority

- `subject.features.json` 是 completed feature 的 owner subject 与 proof / benchmark obligation authority
- 当前 canonical owner subjects 固定为 `SolutionCorePack`、`MixedExecutionFeaturePack`、`HotUpdateHostPack`
- `proofRequired = true` 的 feature 必须在 owner subject 中存在正式 proof 资产
- `benchmarkRequired = true` 的 feature 必须在 owner subject 中存在正式 benchmark 资产
- engineering scenario 可以保留 `Program.cs` 用于全工程样例或启动，但 canonical proof / benchmark correctness 不允许依赖 stdout 判定
- 命中 AOT onboarding 的计划必须显式冻结：
  - `capabilityFamily`
  - `capabilityItem`
  - `ownerSubjectId`
  - `proofRequired`
  - `benchmarkRequired`
  - `hotupdateImpact`
  - `formalVerificationObjects`
  - `requiredGates`
- 这些字段的深层判定与执行顺序由 `wiki/06-测试验证/AOT新Feature接入自测规范.md` 继续定义

## 3. 推荐结构

### 3.1 TestFramework

```text
src/reference/
  Chaos.TestFramework.Sdk/
    Attributes/
    Assertions/
    Enums/
    Contracts/
    State/
  Chaos.TestFramework.Runtime/
    Collection/
    Execution/
    Benchmarking/
    Reporting/
    Hosts/
```

### 3.2 collector

```text
src/tools/
  Chaos.TestFramework.CollectionGen/
```

### 3.3 collection 与执行产物

```text
artifacts/subjects/<subject-id>/
  collections/
    unit-tests.v1.json
    benchmarks.v1.json
    hotupdates.v1.json
  managed-test/
  native/
    dispatch-manifest.v1.json
  native-test/
  hotupdate/
    binding-manifest.v1.json
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

- subject solution 中的测试入口由 `Chaos.TestFramework.Sdk` 标注
- 断言、计量与最小 authoring helper 由 `Sdk` 提供

### 4.2 collection phase

- `.NET 8` collector 分析 managed project 中的 `Chaos.TestFramework.Sdk` 声明
- collector 输出 UnitTest / Benchmark / HotUpdate collection file
- collection file 成为 managed/native/hotupdate test host 的唯一输入清单

### 4.3 managed test project

- 由生成链路产出
- 引用 `Sdk + Runtime`
- 加载 collection file，执行 managed UnitTest / Benchmark

### 4.4 native test project

- IL2CPP 先生成 native project
- native test project 通过 collection file 与 native dispatch manifest 选择并执行 native UnitTest / Benchmark
- 不允许 native 侧手工补充额外测试注册

### 4.5 hotupdate patch / host

- patch project 只引用 `Sdk`
- host project 引用 `Sdk + Runtime`
- host 通过 collection file 与 hotupdate binding manifest 驱动需要验证的 hotupdate 项

## 5. 自动测试规范

### 5.1 bugfix 先补失败自动测试

以下改动必须先有失败自动测试：

- IL2CPP compiler / linker / loader / semantic world bugfix
- `Sdk` / `Runtime` 行为改动
- `.NET 8` collector 改动
- collection file schema 改动
- manifest 生成逻辑改动
- runner / host 生成逻辑改动
- benchmark 页面与数据生成逻辑改动

### 5.2 自动测试优先级

1. `tests/unit/**` 的 Python 模板化单测
2. `tests/contracts/**` 的 schema / snapshot / collection / manifest contract 测试
3. `tests/integration/**` 的端到端流水线测试
4. subject 级 managed/native/hotupdate 实跑

### 5.3 测试阶段 dotnet 编译崩溃闸门

在任何测试或验证阶段，只要 `dotnet build` / `dotnet test` / `msbuild` 发生编译崩溃：

- 当前验证立即视为失败
- 必须保留并检查失败的 project / target / task、退出码、stderr、binlog 与崩溃堆栈或 dump 信息（如果可用）
- 必须先查明根因并修复，再重新运行受影响的验证对象
- 不允许通过重试、跳过当前测试或把崩溃降级成“环境偶发”继续推进

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

- Planner：lowering、closure、dispatch、ABI planning
- Emitter：plan 到文件的渲染
- RuntimeSupport：helper / prelude / support contract
- Templates：Scriban 模板

## 7. 验收标准

满足以下条件，视为流程规范落地：

- 新增一个测试项时，只需要在 subject source 中新增带 `Sdk` attribute 的入口
- subject / managed project 只引用 `Sdk`
- 存在稳定的 `.NET 8` collector 从 managed project 生成 collection file
- managed test project、native test project、hotupdate test host project 都通过 collection file 驱动
- native dispatch manifest 与 hotupdate binding manifest 边界清晰
- `Sdk` 与 `Runtime` 边界清晰
- Python 测试优先模板化
- file-level codegen 默认 Scriban
- 不保留长期 compatibility alias

## 8. AOT新Feature接入验证

AOT 主线新增 feature 的执行顺序以 [`wiki/06-测试验证/AOT新Feature接入自测规范.md`](../../wiki/06-%E6%B5%8B%E8%AF%95%E9%AA%8C%E8%AF%81/AOT%E6%96%B0Feature%E6%8E%A5%E5%85%A5%E8%87%AA%E6%B5%8B%E8%A7%84%E8%8C%83.md) 为准。

本架构文档只定义统一测试主线与分层边界；新增 capability 的正式自测流程固定为：

`tests/unit|contracts -> owner subject managed proof -> owner subject native proof -> hotupdate proof(按需) -> benchmark(按需)`

其中：

- 先判定 capability 的 `owner subject`
- 先过 `collector -> registry -> workspace` 三层接线闸门
- 计划中应显式写出 `formalVerificationObjects` 与 `requiredGates`
- `completed` 前必须先跑这些正式对象，不能用“项目测试套件大致通过”替代
- benchmark 只作为补充证据，不替代 correctness 验收

## 9. Legacy Cutover Contract

- product pipeline 不得回退到 `ChaosSourceEntryArguments`、`DeclaredProofEntriesBySourceEntry` 或旧 subject 命名协议
- canonical subject 不得重新引入 `validation`、`Archetypes`、`FeatureSlices`、`PatchModules`、`ManagedBridge`、`Lowering`、`Launcher` 等旧目录语义
