# 完整 IL2CPP 总方案与 iOS 热更 Roadmap v1.01

Date: 2026-04-09
Status: roadmap-frozen-v1-01

## 1. 目标

本 roadmap 的目标是把已经确认的推荐架构落成一条可推进、可验证、可分阶段交付的工程路线：

- 主线 A：`IL2CPP AOT Mainline`
- 主线 B：`Hot Update Runtime Mode`
- 两条主线共享统一 `contract`

本 roadmap 重点解决两个问题：

1. 先做什么，后做什么，避免过早进入返工高风险区
2. 每一步用什么证据验证“真的通过了”，而不是只看功能 smoke

## 2. 范围边界

本 roadmap 负责：

- 冻结 `AOT mainline + interpreter hot update + metadata supplement` 的分阶段路线
- 明确每阶段的验证方式、证据类型与出关标准
- 明确主线 A、主线 B 与共享 contract 的依赖顺序
- 为后续 child task 提供拆分边界

本 roadmap 当前不负责：

- 立即实现 interpreter
- 立即实现 iOS 真机热更
- 立即给出完整 ABI 字段定义
- 立即进入具体代码实现计划

## 3. 非目标

当前不做以下事情：

- 把热更直接塞进当前 `Phase A` 的实现任务里
- 在 project ingestion、engine ABI、mobile host 未冻结前就开始冲热更实现
- 把 iOS 热更直接表述成“已可上架的远程代码更新方案”

## 4. 阶段列表

### Phase 0: Shared Contract Freeze

先冻结 AOT 与热更共享的世界模型和边界。

### Phase 1: Full Project Ingestion And Build Graph

把当前 proof-style 单程序集输入升级到可处理真实 `project/solution` 的输入层。

### Phase 2: AOT Mainline Runtime Completeness

继续补齐主线 IL2CPP 的 runtime completeness、codegen quality 与 perf governance。

### Phase 3: Engine Binding Contract

把引擎接入从“尾阶段讨论项”升级成正式 contract。

### Phase 4: Mobile Runtime And Host Matrix

把 Android/iOS 从工具链与 packaging gate 推进到 runtime/host 级验证。

### Phase 5: Hot Update Runtime Skeleton

建立热更运行模式的最小骨架，不追求完整功能，只验证主线结构正确。

### Phase 6: Metadata Supplement And Bridge Generation

建立 AOT 与热更之间的 supplemental metadata、wrapper、bridge functions 主线。

### Phase 7: Interpreter Core And Mixed Execution

建立解释执行核心，并验证 `AOT -> HotUpdate -> AOT` 的混合调用链。

### Phase 8: Productization Gates

补齐性能、兼容性、回滚、包格式、iOS 发行边界等产品级 gate。

## 5. 每阶段完成定义

### Phase 0

- goal:
  - 冻结共享 contract，避免后续 AOT 与热更各长一套 runtime 模型
- exit_criteria:
  - 已冻结统一的 type/method/field identity 方案
  - 已冻结统一的 object/handle model
  - 已冻结 delegate/callback ABI、exception boundary 基线
  - **已冻结 ABI calling convention 与 handle primitive types（不含 engine lifecycle 语义，engine lifecycle 归 Phase 3）**
  - 已冻结 hot update package manifest v0
  - **已冻结 contract schema formatVersion 策略与 struct_size 向前兼容规则**
  - **已完成 interpreter IR vs raw IL 的架构决策（影响 Phase 5 package format 和 Phase 6 metadata supplement 设计）**
  - 关键 contract 都有 schema / header / snapshot
- deliverables:
  - `shared-contract-v0.md`
  - ABI/header/schema 样例
  - manifest snapshot tests
  - **version compatibility policy v0**
  - **interpreter execution model decision record**
- dependencies:
  - 当前分析设计结论
- open_questions:
  - object handle 是否直接复用现有 native handle 模型
  - metadata token 与跨版本 stable id 的映射策略
- verification:
  - contract snapshot test
  - schema round-trip
  - AOT 与热更 mock 双边样例可共用同一组 identity/ABI 定义

### Phase 1

- goal:
  - 从 proof input 升级到完整 project/solution ingestion
- exit_criteria:
  - 支持 `ProjectReference` 闭包
  - 支持 `PackageReference` 基础闭包
  - 支持 project graph、entry discovery、resource/config manifest 生成
  - 至少 3 个真实 multi-project golden projects 能稳定进入 managed closure 阶段
- deliverables:
  - project graph loader
  - build manifest v0
  - golden project fixtures
- dependencies:
  - Phase 0
- baseline:
  - **已有 `chaos-il2cpp convert` CLI 入口，manifest 驱动三种 input.kind，可在此基础上扩展 project graph 支持**
  - **已有 subject 测试框架（10 个 subjects、validation profiles、environment matrices），golden projects 应复用此基础设施**
- open_questions:
  - 与 MSBuild 的接线深度是轻量 graph 读取还是更深的 design-time build
- verification:
  - golden project ingestion tests
  - graph snapshot / diff
  - change detection / incremental correctness checks

### Phase 2

- goal:
  - 把 AOT 主线补到可承担主包代码的程度
- exit_criteria:
  - generics/rgctx、EH、delegates、reflection/interop、threading、GC integration 到达指定主线子集
  - generated native perf 有稳定 baseline
  - 关键 golden projects 的 native output 可运行且与 reference 行为一致
  - **Linker reachability analysis baseline 已建立（dead code elimination、metadata stripping）**
  - **managed stack trace recovery + native crash → managed source mapping baseline 已建立**
- deliverables:
  - runtime-core 扩展
  - codegen arbitration/perf evidence
  - native regression dashboards/baselines
  - **linker policy: reachability analysis + metadata stripping baseline**
  - **debug/diagnostics: stack trace recovery + crash mapping + minimal tracing hooks**
- dependencies:
  - Phase 1
- open_questions:
  - generic sharing 与 specialization 的默认裁决顺序
- verification:
  - unit + contract + end-to-end subject tests
  - differential execution against reference runtime
  - native perf/profile gates
  - generated C++ inspection snapshots

### Phase 3

- goal:
  - 冻结 engine-facing contract，避免后期 bridge/ownership 返工
- exit_criteria:
  - lifecycle、ownership、callback、thread model、exception boundary 已文档化并有最小宿主验证
  - 引擎 mock host 能稳定完成 `init -> callback -> shutdown`
- deliverables:
  - engine ABI v0
  - host embedding sample
  - binding generation baseline
- dependencies:
  - Phase 0
  - 与 Phase 2 可部分并行，但不能晚于 Phase 5
- open_questions:
  - engine object ownership 与 GC root 报告边界
- verification:
  - host embedding smoke
  - callback round-trip tests
  - lifecycle stress tests

### Phase 4

- goal:
  - 把移动端从“只会编译/打包”推进到“有真实运行时证据”
- exit_criteria:
  - Android 至少达到 startup/runtime subset gate
  - iOS 至少达到 simulator/device host startup + runtime subset gate
  - crash/symbol/package baseline 可采集
- deliverables:
  - mobile host harness
  - startup/runtime subset tests
  - symbol/crash collection flow
- dependencies:
  - Phase 2
  - Phase 3
- open_questions:
  - iOS 设备与模拟器的最小统一 host 组织方式
- verification:
  - simulator/device smoke
  - runtime subset tests on mobile
  - package/sign/symbol verification

### Phase 5

- goal:
  - 建立热更 runtime mode 的最小骨架，不先追求完整语义
- exit_criteria:
  - hot update package 能被装载
  - runtime mode switch 存在且不污染 AOT 主线
  - 能跑一个解释执行的最小热更方法
- deliverables:
  - package loader
  - runtime mode switch
  - hot update manifest v0
- dependencies:
  - Phase 0
  - Phase 3
  - **注意：不依赖 Phase 4 (mobile)。热更骨架先在 desktop 上验证，mobile 验证推迟到 Phase 7-8。**
- open_questions:
  - package signing/校验与版本协商的最小策略
- verification:
  - package load/unload smoke
  - corrupted package rejection tests
  - AOT-only mode 无回归验证

### Phase 6

- goal:
  - 建立 supplemental metadata 与桥接代码主线
- exit_criteria:
  - AOT 类型/方法可被热更侧稳定解析
  - AOT 调热更、热更调 AOT、热更调引擎三类桥接已跑通
  - 泛型/反射相关的 supplemental metadata 最小闭环已建立
- deliverables:
  - metadata supplement writer/loader
  - bridge generation
  - wrapper stubs
- dependencies:
  - Phase 0
  - Phase 2
  - Phase 3
  - Phase 5
- open_questions:
  - 哪些 metadata 必须补，哪些可以懒加载
- verification:
  - bridge round-trip tests
  - delegate/callback cross-mode tests
  - generic/reflection supplement tests

### Phase 7

- goal:
  - 建立解释器核心与混合执行主线
- exit_criteria:
  - 最小 IL/IR 子集可解释执行
  - `AOT -> HotUpdate -> AOT` 调用链稳定
  - exception、delegate、reflection-lite、engine callback 在 mixed mode 下可工作
- deliverables:
  - interpreter core
  - IL/IR lowering to interpreter IR
  - mixed-mode runtime tests
- dependencies:
  - Phase 5
  - Phase 6
- open_questions:
  - 解释器直接执行 IL 还是先转内部 IR
- verification:
  - mixed-mode integration tests
  - exception propagation tests
  - performance sanity check for hot path escape ratio

### Phase 8

- goal:
  - 把整条路线收口到产品化 gate，而不只是技术样机
- exit_criteria:
  - 兼容性报告、回滚策略、版本矩阵、包策略齐备
  - iOS 发行边界有明确产品策略
  - 主线 perf 回归和热更稳定性回归进入持续验证
- deliverables:
  - compatibility report
  - rollback/versioning strategy
  - iOS distribution policy note
- dependencies:
  - 前述全部阶段
- open_questions:
  - 远程热更在 iOS 上的上架策略到底做不做、做多大
- verification:
  - upgrade/downgrade tests
  - rollback drills
  - long-run soak tests
  - release checklist reviews

## 6. 子任务映射

| task_id | phase | status | owner | purpose | depends_on |
| --- | --- | --- | --- | --- | --- |
| `shared-contract-freeze` | `Phase 0` | `ready` | `codex` | 冻结 AOT 与热更共享 contract、ABI 与 package manifest v0 | `-` |
| `full-project-ingestion-and-build-graph` | `Phase 1` | `planned` | `codex` | 把输入层升级到真实 multi-project graph | `shared-contract-freeze` |
| `aot-mainline-runtime-completeness` | `Phase 2` | `planned` | `codex` | 补齐主线 runtime completeness 与 perf governance | `full-project-ingestion-and-build-graph` |
| `engine-binding-contract` | `Phase 3` | `planned` | `codex` | 冻结 engine-facing ABI、ownership、callback 与 host embedding | `shared-contract-freeze` |
| `mobile-runtime-and-host-matrix` | `Phase 4` | `planned` | `codex` | 建立 Android/iOS runtime/host 验证矩阵 | `aot-mainline-runtime-completeness, engine-binding-contract` |
| `hot-update-runtime-skeleton` | `Phase 5` | `planned` | `codex` | 建立 runtime mode switch、package loader 与最小热更骨架 | `shared-contract-freeze, engine-binding-contract` |
| `metadata-supplement-and-bridge-generation` | `Phase 6` | `planned` | `codex` | 建立 supplemental metadata、wrapper、bridge 主线 | `hot-update-runtime-skeleton, aot-mainline-runtime-completeness` |
| `interpreter-core-and-mixed-execution` | `Phase 7` | `planned` | `codex` | 建立解释器与 mixed mode | `metadata-supplement-and-bridge-generation` |
| `productization-gates` | `Phase 8` | `planned` | `codex` | 补齐回滚、兼容性、发行、长期回归 gate | `interpreter-core-and-mixed-execution, mobile-runtime-and-host-matrix` |

## 7. 依赖

固定依赖顺序建议如下：

1. 先冻结共享 contract
2. 再补 project ingestion
3. 并行推进 AOT completeness 与 engine binding
4. 再推进 mobile runtime/host
5. 再建立 hot update skeleton
6. 再补 metadata supplement 与 bridge generation
7. 再进入 interpreter core 与 mixed execution
8. 最后进入产品化与发行 gate

## 8. 风险

### 风险 1：过早实现 interpreter

如果在共享 contract 和 engine ABI 未冻结前就做解释器，后续会在 metadata、bridge、callback ABI 上连续返工。

### 风险 2：AOT 与热更各自长出对象模型

这是最大的架构风险，会直接把系统撕成两套 runtime。

### 风险 3：移动端验证长期停留在 packaging

如果 iOS 一直只停留在 compile/link/packaging，热更方案最后会缺失最关键的运行态证据。

### 风险 4：没有产品化回归 gate

如果只有技术样机，没有升级、回滚、稳定性、兼容性 gate，后面无法进入真实业务环境。

## 9. 备选路径

### 备选路径 A：先做 interpreter，再补 contract

不推荐。短期快，长期几乎必返工。

### 备选路径 B：先把 AOT 做到底，热更完全后置

可以做，但会错过最关键的 contract 冻结时机，后续热更接入成本更高。

### 备选路径 C：保持当前推荐路线

先 contract，再 ingestion/completeness，再 mobile/engine，再 hot update runtime。

## 10. 当前建议推进顺序

我推荐你下一步立即做的是两个 child task：

1. `shared-contract-freeze`
   - 这是热更与 AOT 不返工的前提
   - 同时也是后面所有验证方案的基准面
2. `full-project-ingestion-and-build-graph`
   - 这是从 proof subject 走向真实工程输入的硬门槛

这两个任务完成后，后续每一步的验证都会更稳定，因为：

- 验证对象不再是单个 demo assembly
- AOT 与热更看到的是同一套 identity/ABI/manifest

## 11. 审核修订记录 (2026-04-11)

基于代码库变更和架构审核，对 v1.01 做了以下修订：

### 基线更新

- design-v1-01.md: Driver 已重构为子命令 CLI，manifest 已支持三种 input.kind，基线判断已更新

### Phase 0 调整

- engine API ABI scope 收窄为 **ABI calling convention + handle primitive types**，不含 engine lifecycle 语义（归 Phase 3）
- 新增 **version compatibility policy v0**（formatVersion 策略、struct_size 向前兼容）
- 新增 **interpreter IR vs raw IL 架构决策**（影响 Phase 5 package format 和 Phase 6 metadata supplement）

### Phase 1 调整

- 标注 CLI convert 基础设施和 subject 测试框架已可复用，起点前移

### Phase 2 调整

- 新增 **Linker reachability analysis + metadata stripping baseline**
- 新增 **managed stack trace recovery + crash mapping baseline**（调试能力不能留到产品化阶段）

### Phase 5 调整

- **移除对 Phase 4 (mobile) 的依赖**。热更骨架先在 desktop 验证，mobile 验证推迟到 Phase 7-8
- 这解除了热更开发被 mobile runtime 阻塞的问题

### 新增遗漏识别

- design 新增 2.4 节（Linker tree-shaking）和 2.5 节（调试与诊断支撑层）
- 原 2.4-2.7 重新编号为 2.6-2.9

