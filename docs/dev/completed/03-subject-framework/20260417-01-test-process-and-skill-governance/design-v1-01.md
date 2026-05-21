# 测试流程与 Skill 规范化 Design v1.03

Date: 2026-04-17
Status: design-frozen

## 1. 目标

把本仓库的测试与验证主线统一为：

`managed solution -> dotnet 8 collection analysis -> collection files -> managed test project -> native project -> native test project -> hotupdate patch project + hotupdate test host project`

并冻结以下长期规则：

- `Chaos.TestFramework.Sdk` 是 subject / managed project 唯一直接引用的测试 SDK
- `Chaos.TestFramework.Runtime` 是执行宿主侧运行环境
- `.NET 8` collector 是唯一 collection 产出入口
- managed/native/hotupdate 执行侧统一消费 collection file
- 迁移完成后删除旧命名、旧入口、旧双轨逻辑，不保留长期 compatibility alias

## 2. 设计原则

### 2.1 managed project 只引用 `Chaos.TestFramework.Sdk`

- `subjects/` 中的 managed solution 只直接引用 `Chaos.TestFramework.Sdk`
- 测试方法中的声明与断言都必须来自 `Sdk`
- subject 内不再引用 `Runtime`

### 2.2 `Sdk` 是 subject authoring surface

`Chaos.TestFramework.Sdk` 负责：

- attributes
- enums
- stable contracts
- `Assert`
- `ChaosAssertionException`
- 极少量 AOT-friendly assertion state / test authoring helper

约束：

- 必须轻量
- 必须可被 IL2CPP / native generate 正常翻译
- 不得引入执行宿主、报告聚合或 collection loader 逻辑

### 2.3 `Runtime` 是 execution host surface

`Chaos.TestFramework.Runtime` 负责：

- collection loader
- managed test host execution
- benchmark harness
- result aggregation / reporting
- runner orchestration
- hotupdate test host execution

约束：

- `Runtime` 可依赖 `Sdk`
- `Sdk` 绝不能反向依赖 `Runtime`

### 2.4 collection file 是唯一跨执行形态契约

- collection file 统一由 `.NET 8` collector 产出
- collector 分析 managed project 中对 `Chaos.TestFramework.Sdk` 的使用结果
- collector 输出 UnitTest / Benchmark / HotUpdate collection file
- managed test project、native test project、hotupdate test host project 都只消费 collection file

### 2.5 collection 与 execution binding 分层

collection file 只描述稳定测试声明，不承载执行形态特有绑定。

执行侧额外引入：

- native dispatch manifest
- hotupdate binding manifest

这样可以避免把 native 符号名、patch 绑定细节污染进 collection schema。

### 2.6 native test project 采用薄宿主

推荐处理：

- subject managed code只引用 `Sdk`
- IL2CPP 翻译后的 native 代码中保留 `Sdk` 断言语义
- native test project 作为薄宿主，输入：
  - collection file
  - native dispatch manifest
- native test project 不直接依赖 `Chaos.TestFramework.Runtime` DLL
- 但它必须遵循 `Runtime` 定义的执行协议和结果协议

### 2.7 hotupdate 采用 patch + host 分离

推荐处理：

- hotupdate patch project 只引用 `Sdk`
- hotupdate test host project 引用 `Sdk + Runtime`
- hotupdate test host project 输入：
  - collection file
  - hotupdate binding manifest
- host 负责加载 patch、按 collection 选择入口、执行业务验证与结果汇总

### 2.8 Python 测试优先模板化

- Python 负责拉起最小模板
- C# fixture 尽量收敛到少量 canonical template
- bugfix 证据优先沉淀为模板化自动测试，而不是手工复跑

### 2.9 文件级生成优先 Scriban

以下默认使用 Scriban：

- managed test project 生成代码
- native test project 生成代码
- hotupdate test host project 生成代码
- native dispatch manifest 相关生成代码
- hotupdate binding manifest 相关生成代码
- IL2CPP file-level emitter

### 2.10 清理旧写法是强制工作，不是附带工作

迁移完成后必须删除：

- `Chaos.TestFramework.Annotation` 命名
- 旧的 `Assert` 所在位置
- 旧 runner 手工注册逻辑
- 旧 annotation/runtime 双轨说明
- 临时 compatibility alias
- 已被新模板替代的旧 fixture / 旧生成入口

## 3. 目标结构

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

collector 约束：

- 运行时固定为 `.NET 8`
- 输入是 managed project 或其 build output
- 分析对象是 `Chaos.TestFramework.Sdk` 的声明结果
- 输出是 versioned collection file

### 3.3 artifacts

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

### 3.4 Python templates

```text
tests/
  templates/
    managed-solution/
    managed-test-project/
    native-test-project/
    hotupdate-patch-project/
    hotupdate-test-host-project/
```

## 4. 统一流程

### 4.1 managed solution

- subject source 只维护业务/验证源码
- UnitTest / Benchmark / HotUpdate 入口通过 `Sdk` attribute 标注
- subject 中的断言通过 `Sdk.Assert` 完成

### 4.2 dotnet 8 collection analysis

- 使用 `.NET 8` collector 分析 managed project
- collector 解析 `Chaos.TestFramework.Sdk` 的声明结果
- collector 输出 UnitTest / Benchmark / HotUpdate collection file
- collection file 是后续所有 test host 的唯一输入清单

### 4.3 managed test project

- 由生成链路产出
- 引用 `Sdk + Runtime`
- 加载 collection file 并在 managed 环境执行 UnitTest / Benchmark

### 4.4 native project + native test project

- IL2CPP 根据 managed solution 生成 native project
- native test project 作为薄宿主，加载 collection file 与 native dispatch manifest
- 它不重新扫描 `Sdk`，也不维护手工测试列表

### 4.5 hotupdate patch project + hotupdate test host project

- patch project 只引用 `Sdk`
- hotupdate test host project 引用 `Sdk + Runtime`
- host 加载 collection file 与 hotupdate binding manifest，负责 patch 装载、入口绑定、验证与回归

## 5. 清理原则

### 5.1 命名切换原则

- 统一切换到 `Chaos.TestFramework.Sdk`
- 不保留长期 `Annotation` alias
- 迁移完成后删除旧项目名、旧 namespace、旧文档名中的旧说法

### 5.2 逻辑切换原则

- 迁移到 collection 驱动后，立即删除旧注册表逻辑
- 迁移到 manifest 绑定后，立即删除旧分散入口
- 迁移到模板化测试后，立即删除一次性旧 fixture

## 6. 验收标准

文档层验收：

- architecture、wiki、skill、roadmap 四处口径一致
- 新规范能明确回答：
  - subject 为什么只引用 `Sdk`
  - `Assert` 为什么在 `Sdk`
  - `.NET 8` collector 如何产生 collection file
  - native test project 和 hotupdate test host project 怎么消费 collection
  - 为什么 collection 与 dispatch/binding manifest 分层
  - 哪些旧写法会被强制删除

后续代码实现验收：

- `Chaos.TestFramework.Sdk` 替代旧 `Annotation`
- subject / managed project 只引用 `Sdk`
- managed test host 引用 `Sdk + Runtime`
- native test project 实现薄宿主 + native dispatch manifest
- hotupdate patch project / test host project 完成拆分
- collection file 与 manifest 双层契约稳定
- 不保留长期 compatibility alias
