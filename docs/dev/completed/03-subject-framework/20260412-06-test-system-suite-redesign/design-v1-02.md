# 理想测试体系设计 v1.02

**日期：** 2026-04-12 16:10 +08:00

## 1. 本版变更说明

本版设计替换了 v1.01 中“顶层以 `ProofSuite` / `BenchmarkSuite` 为 canonical source root”的假设。

用户进一步明确后的长期模型是：

- 顶层 `subject` 自身就应承载 **solution-first canonical source**
- `scenario` 与 `workload` 都挂在这个 solution subject 之下
- proof / benchmark 不再是顶层 source 划分轴，而是验证切片类型
- `reference DLL`、`project reference`、`package reference`、`host + patch` 等差异，本质上是 **solution 依赖/组织/可执行方案** 的一部分，而不是“案例的一种”

因此，本版设计的核心结论是：

- `subject = canonical solution source bundle`
- `scenario = correctness / trace / recovery / packaging 验证切片`
- `workload = convert / codegen / startup / runtime / hot-update perf 验证切片`

## 2. 设计目标

本设计面向一个多平台 IL2CPP + 热更系统，目标是建立一套以 solution 为真源的测试体系：

- `subjects/` 中的每个顶层 `subject` 都代表一个真实的 canonical solution source bundle。
- `tests/` 只保留 generic mechanism 测试，不再承载具体 subject 的 canonical source。
- proof、benchmark、hot-update、mobile、release gate 都挂在 solution subject 的 `scenario/workload` 上，而不是继续平铺新的顶层 subject。
- 相同 solution 上的 correctness、performance、rollback、packaging、device evidence 能共用一套 source 真源。
- 未来新增测试点默认不再新增顶层 subject，而是优先新增该 solution subject 下的 `scenario/workload`。

## 3. 核心原则

### 3.1 顶层 `subject` 的 canonical 含义

顶层 `subject` 只在以下条件明显成立时才允许新增：

- 存在一套 materially distinct 的 solution source bundle
- project graph / dependency model / packaging model / runtime host model 明显不同
- 需要独立维护 source 真源、validation asset 与 artifact lineage

如果只是同一 solution 上多一个验证点，不新增顶层 subject。

### 3.2 `scenario` 与 `workload` 的长期职责

- `scenario` 负责表达 correctness / trace / rollback / compatibility / packaging 等“证据型”验证。
- `workload` 负责表达 convert / codegen / startup / throughput / latency / memory / hot-update perf 等“指标型”验证。

二者都依附于 solution subject，而不是独立 source root。

### 3.3 `tests/` 只验证机制，不拥有业务真源

`tests/` 的职责是验证：

- schema
- selector
- planner
- registry
- execution pipeline
- report/event
- CLI/TUI/tooling contract
- perf evaluator / budget policy / baseline comparator

凡是具体 solution source、package fixture、expected、baseline 的 canonical 真源，都应位于对应 `subject` 下。

### 3.4 顶层 subject 命名反映 source identity，而不是验证类型

canonical subject id 应优先表达 solution identity，而不是 proof/benchmark 标签。

例如：

- `SolutionSimpleLib`
- `SolutionMultiProject`
- `SolutionPackageReference`
- `MainlineFeaturePack`
- `HotUpdateHostPack`

proof/benchmark 是 `scenario/workload` 或 capability 维度，不是顶层 source 维度。

## 4. 对象模型

### 4.1 顶层对象

| 对象 | 作用 | 是否 source 真源 |
| --- | --- | --- |
| `subject` | canonical solution source bundle | 是 |
| `scenario` | correctness/trace/recovery 等验证切片 | 否 |
| `workload` | convert/startup/runtime/hot-update perf 切片 | 否 |
| `validationProfile` | 一组验证方式的组合入口 | 否 |
| `matrix` | host/target/toolchain/runtime 组合 | 否 |
| `capability` | 覆盖维度标签 | 否 |
| `pipeline` | dev/merge/nightly/release 编排入口 | 否 |

### 4.2 `subject` 模型

`subject` 最小结构建议为：

```text
subjectId
displayName
sourceModel
dependencyModel
executablePlan
validationProfiles
validation
scenarios/
workloads/
expected/
baselines/
fixtures/
```

### 4.3 `sourceModel` / `dependencyModel` / `executablePlan`

为了表达用户强调的“solution 中是用 reference DLL 还是其他方案”，顶层 subject 需要显式建模 source 组织方式：

- `sourceModel`
  - `dotnet-solution`
  - `dotnet-project-set`
  - `host-plus-patch`
  - `mixed-solution`

- `dependencyModel`
  - `project-reference`
  - `package-reference`
  - `binary-reference`
  - `mixed`

- `executablePlan`
  - `managed-host`
  - `generated-native`
  - `external-native-host`
  - `device-package`
  - `host-plus-hot-update-patch`

这些字段决定如何 ingest、如何闭包、如何生成 native executable，不应由测试文件名隐式表达。

### 4.4 `scenario` 模型

`scenario` 表达 solution 上的证据型验证：

```text
scenarioId
displayName
capabilities[]
matrices[]
availability{ platform -> state }
expected/
baselines/
validationProfiles[]
```

例子：

- `project-graph`
- `metadata-registration`
- `runtime-output`
- `native-buildability`
- `trace-compare`
- `rollback`
- `package-integrity`

### 4.5 `workload` 模型

`workload` 表达 solution 上的指标型验证：

```text
workloadId
displayName
capabilities[]
matrices[]
availability{ platform -> state }
metricPolicy
budgetPolicy
warmupPolicy
measurementPolicy
```

例子：

- `convert`
- `codegen`
- `startup`
- `runtime-dispatch`
- `hot-update-roundtrip`
- `package-load`

## 5. 目标目录终态

```text
subjects/
  SolutionSimpleLib/
    subject.manifest.json
    source/
    validation/
    fixtures/
    scenarios/
      runtime-output/
        scenario.manifest.json
        expected/
        baselines/
      native-buildability/
        scenario.manifest.json
        expected/
    workloads/
      convert/
        workload.manifest.json
        baselines/
        budgets/

  SolutionMultiProject/
    subject.manifest.json
    source/
    validation/
    fixtures/
    scenarios/
      project-graph/
      metadata-registration/
      generated-executable/
    workloads/
      convert/
      codegen/

  SolutionPackageReference/
    subject.manifest.json
    source/
    validation/
    fixtures/
    scenarios/
      package-resolution/
      runtime-output/
    workloads/
      convert/

  MainlineFeaturePack/
    subject.manifest.json
    source/
    validation/
    fixtures/
    scenarios/
      dispatch/
      generic-layout/
      delegate/
      exception/
      reflection-interop/
    workloads/
      startup/
      runtime-dispatch/

tests/
  unit/
    run/
      schema/
      selection/
      planning/
      execution/
      reporting/
      compatibility/
      capabilities/
  integration/
    run/
      cli/
      registry/
      pipelines/
      platform-hosts/
      release-gates/
  tooling/
    run/
      tui/
      wrappers/
      bootstrap/
      commands/
```

## 6. `golden` 与 `convert-golden-multi-project` 在新模型中的解释

### 6.1 `golden`

`golden` 不再被视为“普通 fixture 集合”或“某类 scenario”。

它代表的是第一批核心 **solution archetype subjects** 的历史来源：

- `GoldenSimpleLib` -> `SolutionSimpleLib`
- `GoldenMultiProject` -> `SolutionMultiProject`
- `GoldenWithPackage` -> `SolutionPackageReference`

这些 subject 用来验证：

- project graph ingestion
- package / reference resolution
- full native generated executable feasibility
- metadata / code registration 完整性

### 6.2 `convert-golden-multi-project`

它不再被解释为 `tests/perf` 下的一份长期业务型 perf fixture。

在新模型里，它应该是：

- `SolutionMultiProject` subject 下的 `workload: convert`

也就是说：

- correctness 场景、性能 workload 共用同一套 canonical solution source
- `tests/perf` 只保留 generic perf evaluator / perf policy / baseline compare 机制

## 7. 多平台 IL2CPP + 热更的能力模型

以下 capability 仍然保留为横切维度，但现在挂在 solution subject 的 `scenario/workload` 上：

- semantic correctness
- metadata/artifact correctness
- execution-model consistency
- platform host/packaging
- stability/recovery
- performance
- observability
- compliance/security

这些能力不再决定顶层 subject；顶层 subject 由 canonical solution source 决定。

## 8. 迁移原则

### 8.1 应做

- 把现有 `golden`、proof-like、benchmark-like subject 重新归类为 solution-first canonical subjects。
- 把原先“单个 proof / 单个 benchmark = 一个顶层 subject”的模型，改成“一个 solution = 一个顶层 subject，多个 scenario/workload 挂在其下”。
- 把具体 perf 检查从 `tests/perf` 挪回对应 solution subject 的 `workloads/`.
- 把 `reference DLL / package / project graph / host+patch` 建成 subject source 属性，而不是目录特例。

### 8.2 不应做

- 不再把 `proof` / `benchmark` 当成顶层 source 维度。
- 不再让 `tests/` 保有具体 subject 的 canonical source。
- 不再让 `golden` 以历史桶的方式长期留在 `subjects/golden/*`.
- 不再让 `convert-golden-multi-project` 这类 solution-specific workload 长期停留在 `tests/perf/`.

## 9. 结论

v1.02 的最终判断是：

- 顶层 `subject` 的长期语义是 **canonical solution source bundle**
- `scenario` 与 `workload` 都服务于这个 solution
- `golden` 会被重解释为第一批 solution archetype subjects
- `convert-golden-multi-project` 会被重解释为对应 solution subject 的 workload

这条基线一旦冻结，后续 roadmap 就不再围绕“ProofSuite/BenchmarkSuite 两大 source 根”展开，而是围绕“如何把现有 subject 迁到 solution-first canonical model”展开。
