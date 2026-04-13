# 理想测试体系设计 v1.01

**日期：** 2026-04-12 15:22 +08:00

## 1. 设计目标

本设计面向一个多平台 IL2CPP + 热更系统，目标不是只把当前 proof/benchmark 文件搬家，而是定义一套长期稳定的测试终态：

- `tests/` 只承载通用机制验证，不再承载 subject-specific 配置。
- `subjects/` 只承载 suite subject 及其内部 scenario/workload、baseline、fixture、validation 资产。
- 新增测试点的标准动作从“新增顶层 subject”改成“新增 `scenario` 或 `workload`”。
- proof、benchmark、hot-update、mobile、release gate 都能落入同一套对象模型，而不是继续平铺新目录。
- 平台成熟度、能力覆盖度与执行入口都具有稳定语义，可直接进入后续 roadmap。

## 2. 非目标

本设计当前不直接解决以下事情：

- 不直接实施代码迁移。
- 不强行承诺永远只会有两个 suite subject；但新增顶层 suite 必须有更高门槛。
- 不把 capability 维度升级成新的顶层 subject。
- 不在本轮直接冻结全部 CLI 兼容细节，只冻结终态对象模型与边界。

## 3. 核心原则

### 3.1 `subject` 是 suite root，不是单个测试点

顶层 `subject` 只在以下条件同时成立时才允许新增：

- source root 明显不同
- validation 资产明显不同
- artifact 生命周期明显不同
- owner / 维护边界明显不同

如果只是“多一个 proof 点”或“多一个 benchmark 项”，不满足新增顶层 suite 的条件。

### 3.2 新增测试点的标准扩展单位

- correctness 类测试点：新增 `scenario`
- performance 类测试点：新增 `workload`

这也是对“未来新增测试项只要新增 subject 就行”的正式修正。理想终态里，默认不再新增顶层 subject，而是新增 suite 内部单元。

### 3.3 capability / matrix / availability / pipeline 是正交维度

一个测试点应该同时具备多个横切属性：

- 它属于哪个 capability
- 它在哪些 matrix 上可运行
- 它在哪些平台上是 `ready / planned / blocked / unsupported`
- 它会被哪些 pipeline/gate 消费

这些都不应该通过目录膨胀表达。

### 3.4 `tests/` 只验证框架机制

`tests/` 的职责是验证：

- schema
- selector
- planner
- registry
- execution pipeline
- report/event
- CLI/TUI/tooling contract

凡是与某个具体 proof/workload 的 source、expected、baseline 强绑定的内容，都应回到 `subjects/`.

## 4. 对象模型

### 4.1 顶层对象

| 对象 | 作用 | 扩展频率 | 是否顶层目录 |
| --- | --- | --- | --- |
| `suite subject` | 稳定 source/validation/artifact root | 低 | 是 |
| `scenario` | correctness/trace/recovery 场景 | 高 | 否 |
| `workload` | perf/stress/soak 工作负载 | 高 | 否 |
| `validationProfile` | 一组验证方式的组合入口 | 中 | 否 |
| `matrix` | host/target/toolchain/runtime 组合 | 中 | 否 |
| `capability` | 覆盖维度标签 | 中 | 否 |
| `pipeline` | dev/merge/nightly/release 编排入口 | 中 | 否 |
| `baseline` | expected/perf budget/trace snapshot | 高 | 否 |

### 4.2 推荐顶层 suite subject

第一阶段终态推荐只有两个主 suite：

- `ProofSuite`
- `BenchmarkSuite`

说明：

- `ProofSuite` 承载 correctness、trace、compatibility、recovery 等“证据型”验证。
- `BenchmarkSuite` 承载 latency、throughput、memory、startup、size 等“指标型”验证。
- release gate 不是新的 subject，而是 `pipeline`。
- 如果未来出现真正独立的 source graph，例如独立设备农场宿主或单独的引擎兼容性工程，才考虑新增第三个 suite。

### 4.3 `scenario` 模型

`scenario` 是 `ProofSuite` 的内部对象，最小结构建议为：

```text
scenarioId
displayName
capabilities[]
sourceOverrides?
validationProfiles[]
matrices[]
availability{ platform -> state }
expected/
baselines/
evidenceContract
```

具体例子：

- `dispatch`
- `generic-layout`
- `array-boxing`
- `delegate`
- `exception`
- `reflection-interop-closure`
- `hot-update-rollback`
- `mobile-host-startup`
- `metadata-version-compat`
- `mixed-mode-boundary-call`

### 4.4 `workload` 模型

`workload` 是 `BenchmarkSuite` 的内部对象，最小结构建议为：

```text
workloadId
displayName
capabilities[]
workloadEntry
metricPolicy
warmupPolicy
measurementPolicy
validationProfiles[]
matrices[]
availability{ platform -> state }
baselines/
budgets/
```

具体例子：

- `arithmetic`
- `allocation`
- `dispatch`
- `generic`
- `exception`
- `game-loop`
- `hot-update-load`
- `hot-update-dispatch`
- `hot-update-roundtrip`
- `mixed`

### 4.5 `availability` 平台可用性模型

每个 `scenario` / `workload` 对每个平台都应显式声明状态：

| 状态 | 语义 | 是否进入默认 gate |
| --- | --- | --- |
| `ready` | 已实现且可稳定执行 | 是 |
| `planned` | 设计已定，但尚未实现 | 否 |
| `blocked` | 理论适用，但受外部依赖阻塞 | 否 |
| `unsupported` | 明确不适用 | 否 |

重要约束：

- “移动端还没做”不应该新开一个 subject，而应该标成 `planned` 或 `blocked`。
- 平台差异属于 `matrix + availability`，不是顶层目录差异。

## 5. 理想目录终态

```text
subjects/
  ProofSuite/
    subject.manifest.json
    source/
    validation/
      unit/
      platform-hosts/
      package-probes/
    scenarios/
      dispatch/
        scenario.manifest.json
        source-overrides/
        expected/
        baselines/
        assets/
      generic-layout/
        scenario.manifest.json
        expected/
        baselines/
      hot-update-rollback/
        scenario.manifest.json
        expected/
        baselines/
        package-fixtures/
      mobile-host-startup/
        scenario.manifest.json
        package-fixtures/
  BenchmarkSuite/
    subject.manifest.json
    source/
    validation/
      perf-harness/
      device-runners/
    workloads/
      arithmetic/
        workload.manifest.json
        baselines/
        budgets/
      hot-update-roundtrip/
        workload.manifest.json
        baselines/
        budgets/
        package-fixtures/
      game-loop/
        workload.manifest.json
        baselines/
        budgets/

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

artifacts/
  subjects/
    ProofSuite/
      runs/<run-id>/
        scenarios/<scenario-id>/
          analysis/
          generated/
          build/
          runtime/
          report/
    BenchmarkSuite/
      runs/<run-id>/
        workloads/<workload-id>/
          analysis/
          generated/
          build/
          runtime/
          report/
```

### 目录边界解释

- `tests/` 只留 generic mechanism 测试与 tooling 契约测试。
- `subjects/ProofSuite/scenarios/*` 与 `subjects/BenchmarkSuite/workloads/*` 才是测试点的正式落位处。
- 所有 expected、baseline、fixture、package probe、device runner 配置都优先放到对应 subject 内。

## 6. 命名体系重构

### 6.1 旧问题

当前 `test_phase7_*`、`test_phase8_*`、`test_phase9_*` 这类命名表达的是历史推进顺序，而不是测试职责。随着系统持续演进，阶段编号只会越来越失真。

### 6.2 新命名原则

文件名按“对象 + 职责/行为”命名，不再按历史阶段命名。推荐模式：

```text
test_<domain>_<behavior>.py
```

或：

```text
test_<object>_<contract>.py
```

### 6.3 推荐分类轴

| 旧轴 | 新轴 |
| --- | --- |
| `phase7/phase8/phase9` | `schema/selection/planning/execution/reporting/compatibility/capabilities` |
| `proof/benchmark` 混在 generic tests | 只在 subject 内使用 proof/workload 术语 |
| subject 名写进 generic tests | 只在 fixture 或 manifest sample 中出现 |

### 6.4 示例

| 旧命名味道 | 新命名方向 |
| --- | --- |
| `test_phase7_subject_registry.py` | `test_registry_subject_loading.py` |
| `test_phase8_matrix_resolution.py` | `test_execution_matrix_planning.py` |
| `test_phase9_runtime_summary.py` | `test_report_runtime_summary.py` |
| `test_phase8_proof_selector.py` | `test_validation_profile_selection.py` |

## 7. 多平台 IL2CPP + 热更系统的能力覆盖模型

proof 与 benchmark 不足以描述整个系统。理想测试体系至少要覆盖以下 8 类 capability。

### 7.1 semantic correctness

关注：

- dispatch / virtual / interface
- generics / layout / boxing
- delegate / closure / exception
- reflection / metadata supplement
- engine binding / native interop
- hot-update 调用结果正确性

### 7.2 metadata/artifact correctness

关注：

- metadata manifest
- generated C++ / symbol / trace artifact
- patch manifest / package content
- assembly 版本与引用关系
- baseline snapshot 完整性

### 7.3 execution-model consistency

关注：

- managed / AOT / interpreter / mixed 模式是否一致
- AOT 与热更边界调用是否一致
- 同一 scenario/workload 在 host/target 差异下结果是否一致
- fallback 到 baked AOT 实现时语义是否稳定

### 7.4 platform host/packaging

关注：

- Windows / macOS host
- Android / iOS 打包、安装、启动
- package probe、resource layout、runtime bootstrap
- 真机部署、签名、包结构、host 生命周期

### 7.5 stability/recovery

关注：

- repeated load/unload
- rollback / fallback
- crash 后恢复
- network interruption / package corruption
- long-run soak
- low-memory / low-storage / app lifecycle 切换

### 7.6 performance

关注：

- latency / throughput
- cold start / hot start
- hot-update 包加载时间
- mixed-mode boundary 开销
- memory / binary size
- mobile battery / thermal / frame pacing

### 7.7 observability

关注：

- event schema 完整性
- report 聚合正确性
- artifact lineage 可追踪
- failure hint / parser hint 可定位
- trace 与日志能否回溯到 scenario/workload/matrix

### 7.8 compliance/security

关注：

- patch 包完整性与签名
- ABI/API compatibility
- sandbox / entitlement / permission 边界
- version compatibility matrix
- update provenance 与防回滚策略

## 8. 额外需要的测试能力

如果站在“多平台 IL2CPP + 热更系统”的更大维度审视，除了 proof 与 benchmark，本系统还应尽早具备以下能力：

1. **版本兼容矩阵能力**
   - 宿主版本、metadata 版本、热更包版本、引擎绑定版本之间的兼容测试。
2. **回滚与降级能力**
   - 热更包加载失败时，能否回滚到旧包或 baked AOT 行为。
3. **状态迁移能力**
   - 热更版本切换前后的状态、存档、静态字段、单例状态是否可控迁移。
4. **真实设备证据能力**
   - Android/iOS 不是只验证可编译，还要验证可安装、可启动、可执行、可取证。
5. **弱环境仿真能力**
   - 低内存、断网、包损坏、磁盘不足、后台恢复等异常条件下的稳定性。
6. **可重复构建与符号化能力**
   - 同一输入是否能得到稳定产物；崩溃/trace 是否可符号化回源。
7. **引擎/原生插件边界能力**
   - engine binding、native plugin、P/Invoke、ABI 升级等边界验证。
8. **发布门禁能力**
   - perf budget、package 完整性、回滚能力、关键 correctness scenario 组合成 release gate。

## 9. `scenario` 与 `workload` 的具体例子

### 9.1 `scenario` 例子：`generic-layout`

目标：

- 验证 value type / reference type / generic nesting 在 IL2CPP codegen 与 runtime 上的布局和输出一致。

证据：

- expected output
- optional trace snapshot
- generated artifact diff

平台语义：

- `windows-x64`: `ready`
- `android-arm64`: `planned`
- `ios-arm64`: `planned`

### 9.2 `scenario` 例子：`hot-update-rollback`

目标：

- 宿主先加载 `patch-v1`，再升级到 `patch-v2`，制造失败后回滚到 `patch-v1`，验证行为与状态恢复。

证据：

- runtime report
- rollback event trace
- package integrity log

平台语义：

- `windows-x64`: `ready`
- `android-arm64`: `blocked`
  原因：设备侧宿主与包分发链路未完成

### 9.3 `workload` 例子：`arithmetic`

目标：

- 测量固定 `workloadEntry` 在 managed / interpreter / native 上的耗时、吞吐、checksum 与迭代对齐。

指标：

- `elapsedMilliseconds`
- `opsPerSecond`
- `checksum`
- `iterations`

### 9.4 `workload` 例子：`hot-update-roundtrip`

目标：

- 测量热更包加载、方法分发、卸载、重载的完整成本。

指标：

- package load latency
- first invoke latency
- steady-state throughput
- unload/reload latency
- memory delta

## 10. Pipeline / Gate 模型

理想终态里，release 与持续集成应该通过 `pipeline` 管理，而不是再开新 subject。

推荐至少有以下几类 pipeline：

- `dev-smoke`
  - 当前主机可运行的核心 `scenario`
  - schema / selector / planner / report 通用机制
- `merge-gate`
  - 核心 correctness scenario
  - 核心 benchmark smoke
  - contract / registry / artifact consistency
- `nightly-cross-platform`
  - 多 host / 多 target
  - real device / packaging / rollback / soak
- `release-gate`
  - correctness 核心链路
  - hot-update compatibility
  - perf budget
  - package integrity / signing / compliance

## 11. 迁移原则

### 11.1 应做

- 先把现有 proof 类 subject 收敛进 `ProofSuite/scenarios/*`
- 再把现有 benchmark 类 subject 收敛进 `BenchmarkSuite/workloads/*`
- generic tests 按职责重命名，并把 fixture 改成基于 sample manifest / registry 数据驱动
- 把 subject-specific baseline、package fixture、validation 资产迁回对应 suite
- 在 manifest 中引入 `availability` 与 `capabilities`

### 11.2 不应做

- 不继续新增“单个 proof = 一个 subject”
- 不把 `phase` 继续写进测试文件名
- 不把 capability 升成新的顶层 subject
- 不把“移动端暂未实现”编码成新目录或新 subject
- 不把 release gate 做成新的 benchmark/proof 变种 subject

## 12. 对当前方案的补充建议

1. 你原先提出的“proof 合并成一个 subject、benchmark 合并成一个 subject”方向是对的，但要把“新增测试项新增 subject”修正为“新增 `scenario/workload`”。
2. 不要只看 proof/benchmark 两个口径。对这个系统来说，hot-update compatibility、rollback、device evidence、package integrity、mixed-mode consistency 同样需要进入正式测试模型。
3. 如果要控制复杂度，第一阶段可以只先落：
   - `ProofSuite`
   - `BenchmarkSuite`
   - `availability`
   - `capabilities`
   - `dev-smoke / merge-gate / nightly-cross-platform`
4. release gate、security/compliance、弱环境仿真可以在后续 roadmap 里后置，但对象模型现在就要预留。

## 13. 结论

理想终态不是“更多 subject”，而是：

- 更少的顶层 suite subject
- 更清晰的 `scenario/workload` 扩展单位
- 更严格的 `tests/` 与 `subjects/` 边界
- 更完整的 capability / matrix / availability / pipeline 横切模型

只要这几个边界冻结下来，后续无论是 AOT、热更、移动端还是 release gate，都能在同一套测试体系里持续演进，而不会再回到 `phase` 命名和顶层 subject 膨胀。
