# HybridCLR 对齐的 AOT 与热更开发计划 Roadmap v1.01

Date: 2026-04-14 23:34:39 +08:00
Status: in-progress

## 1. 目标

建立一条以 `typed-il` 为共享契约、以 HybridCLR 对齐为热更方向、以 solution owner pack 为 subject 载体的长期开发路线，最终让：

- C# feature 扩展能够稳定转入 IL2CPP AOT 与热更体系。
- 全部目标 AOT translation surface 都能被显式映射到 supported / partial / unsupported 边界，而不是只记录成功路径。
- `subjects/` 既是 capability owner，也是工程级 solution gate。
- proof、benchmark、full-solution、mixed execution 和 hot update 能共享统一证据链。

## 2. 范围边界

本 roadmap 负责：

- 冻结 `typed-il` / capability taxonomy / body availability / reference bundle 等基础契约。
- 冻结 AOT translation surface ledger 与 unsupported surface ledger。
- 把历史语法点机械拆成单文件 capability slice，并按领域归并到少量 owner project。
- 把 `ChaosUnitTest` / `ChaosBenchmark` 固化为 proof / benchmark 声明入口。
- 逐步建立 HybridCLR 对齐的 dispatch、supplemental metadata、bridge/thunk 和 AOT Core IR。
- 建立从 capability proof 到工程级 solution gate 的统一验证体系。

本 roadmap 不负责：

- 在 Phase 0 之前直接跳进大规模功能实现。
- 继续扩大 string 协议驱动的 subject 内部调度面。
- 把所有 benchmark 都做成强制项。
- 无上限增加顶层 `subject` 数量。

## 3. 非目标

- 不把当前计划定义成“所有 C# feature 一次性补齐”。
- 不把“未入账的能力空白”误判成“默认已支持”。
- 不重新引入复杂的 subject 目录 DSL。
- 不为了兼容历史入口长期保留多套 pipeline。
- 不把移动平台、发布产品化和 desktop 开发主线混成一个阶段推进。

## 4. 阶段列表

### Phase 0: Contract Freeze And Capability Ledger

- 冻结 repository-owned reference bundle 边界。
- 冻结两级 capability taxonomy。
- 冻结 stable identity、body availability、proof/benchmark 声明模型。
- 冻结 `AOT translation surface ledger` 与 `unsupported surface ledger` 的最小字段和维护方式。
- 形成 capability ledger 和 Phase 后续子任务的唯一输入基线。

### Phase 1: Subject Source Reshape And Test Metadata Cutover

- 把历史语法点机械拆成单文件 capability slice。
- 在少量 owner solution pack 内重组 project 边界。
- 用 `ChaosUnitTest` / `ChaosBenchmark` 替代旧的 subject 内目录协议。
- 清理残余 `Console.WriteLine` 判定，统一 Assert 套件。

### Phase 2: Typed-IL Shared Contract And Hybrid Dispatch Foundation

- 把 `typed-il` 变成 AOT、解释执行、bridge/thunk 的共享输入。
- 引入 `BodyAvailabilityResolver` 和 `HybridDispatchResolver`。
- 建立从 method identity 到执行方式的统一决策面。

### Phase 3: HotUpdate Runtime And Supplemental Metadata Foundation

- 建立 `HotUpdateAssemblyLoader`。
- 建立 `SupplementalMetadataRegistry`。
- 打通宿主程序集、热更程序集和稳定 identity 的对齐。
- 形成最小 mixed execution 闭环。

### Phase 4: AOT Core IR And Native Codegen Strengthening

- 从 `typed-il` 降到 `AotCoreIr`。
- 在 `AotCoreIr` 上扩展对象模型、调用模型、泛型、异常、元数据闭包。
- 逐步替代当前 ad-hoc `NativeAotEmitter` 直出路径。

### Phase 5: Feature Coverage Expansion On Owner Subjects

- 以 capability family/item 和 translation surface ledger 为单位补齐全部目标 AOT 翻译面。
- 每个能力点都落到 owner subject、proof、可选 benchmark 和 archetype gate。
- 保持“单文件 capability slice + 少量 owner project”的结构纪律。

### Phase 6: Engineering Archetype, Benchmark And Release Gates

- 打通完整 solution ingestion、reference bundle、full-solution translation 与 native run。
- 收口 benchmark dashboard、support-state 和 reason code。
- 形成 mixed execution / hot update / productization 的统一发布级证据链。

## 5. 每阶段完成定义

### Phase 0

- `goal`
  - 冻结所有后续实现会反复依赖的基础契约。
- `exit_criteria`
  - Q0 `reference bundle` 的仓库边界、刷新策略、使用 archetype 和校验方式明确。
  - Q1 `capability taxonomy` 固定为两级结构。
  - `body availability` 枚举与 stable identity 契约明确。
  - `ChaosUnitTest` / `ChaosBenchmark` 的最小公共 contract 明确。
  - 全部目标 AOT translation surface 都进入 `aot-translation-surface-ledger`。
  - 明确不支持或本轮暂缓的翻译面都进入 `aot-unsupported-surface-ledger`，并附 reason code。
- `deliverables`
  - capability ledger
  - aot translation surface ledger
  - aot unsupported surface ledger
  - reference bundle policy
  - stable identity policy
  - body availability enum draft
- `dependencies`
  - 无
- `open_questions`
  - 无。默认冻结如下：
  - `reference bundle` 采用“repository-owned canonical base bundle + 受控 supplement”模型。
  - 两级 `capability taxonomy` 按语义/运行时域划分 family。
  - `public stable id` 采用 `family enum + item enum` 复合稳定 numeric id。

### Phase 1

- `goal`
  - 完成 subject 源码和测试元数据的结构化重组。
- `exit_criteria`
  - 历史语法点完成单文件切片。
  - owner subject pack 和 project 归并边界稳定。
  - 旧的 subject 内字符串目录协议不再扩大。
  - proof 全部使用 Assert 套件表达结果。
- `deliverables`
  - subject migration matrix
  - owner project layout note
  - `Chaos.TestFramework` metadata contract expansion
- `dependencies`
  - Phase 0
- `open_questions`
  - 无。默认冻结如下：
  - 路径或命名空间体现 family / domain，文件名体现 capability item。

### Phase 2

- `goal`
  - 建立 typed-il 共享契约和统一 dispatch 基础层。
- `exit_criteria`
  - AOT、解释执行、bridge/thunk 至少共享同一套 method/type identity。
  - `BodyAvailabilityResolver` 能给出稳定结果。
  - mixed execution 不再依赖临时分支判断。
- `deliverables`
  - typed-il contract freeze note
  - hybrid dispatch design patch set
  - body availability integration note
- `dependencies`
  - Phase 0
  - Phase 1
- `open_questions`
  - 无。默认冻结如下：
  - `typed-il` 先降到薄层 `interpreter IR`，而不是直接被解释执行层消费，也不单独演化成第二套总模型。

### Phase 3

- `goal`
  - 建立 HybridCLR 对齐的热更最小运行时主线。
- `exit_criteria`
  - supplemental metadata 可注册、可查询、可与程序集 identity 对齐。
  - 热更入口可通过统一 dispatch 进入 mixed execution。
  - 至少一条 host -> hot update -> fallback 的闭环跑通。
- `deliverables`
  - hot update runtime foundation
  - supplemental metadata registry
  - bridge/thunk minimum contract
- `dependencies`
  - Phase 2
- `open_questions`
  - 无。默认冻结如下：
  - bridge/thunk 采用编译期主导、运行时有限补充的策略。

### Phase 4

- `goal`
  - 强化 AOT codegen，使其建立在稳定中层之上。
- `exit_criteria`
  - `AotCoreIr` 可承接关键对象/调用/泛型/异常/元数据闭包能力。
  - 当前 `NativeAotEmitter` 的核心路径开始迁移到 `AotCoreIr`。
  - AOT 与热更共享 identity，不再形成平行世界。
- `deliverables`
  - `AotCoreIr` contract
  - native codegen migration slices
  - codegen capability gap ledger
- `dependencies`
  - Phase 2
  - Phase 3
- `open_questions`
  - 无。默认冻结如下：
  - 先冻结 generic context / sharing contract，再优先补 codegen closure 与显式实例化，同时给 runtime sharing 预留钩子。

### Phase 5

- `goal`
  - 以 capability ledger 与 translation surface ledger 为主线扩展完整 AOT 翻译覆盖。
- `exit_criteria`
  - 每个新增 capability item 都能映射到 owner subject、proof、可选 benchmark、support-state。
  - 每个目标 translation surface 条目都能映射到 owner subject、proof、可选 benchmark 与当前状态。
  - 明确 deferred / unsupported 的翻译面都保留稳定记录，而不是从计划中消失。
  - 不再新增无 owner 的实验性 subject。
  - 历史语法点迁移收口完成。
- `deliverables`
  - capability slice batches
  - translation surface coverage refresh
  - unsupported surface report refresh
  - proof coverage expansion
  - selective benchmark expansion
- `dependencies`
  - Phase 1
  - Phase 4
- `open_questions`
  - 无。默认冻结如下：
  - benchmark 采用分层策略：高价值 runtime path 必带 benchmark，其余能力点保留 correctness proof 即可。

### Phase 6

- `goal`
  - 把能力覆盖从单点 proof 提升到工程级和发布级 gate。
- `exit_criteria`
  - full-solution、reference bundle、benchmark dashboard、mixed execution、hot update 形成统一证据链。
  - native generate 流程在 retained owner subjects 上稳定跑通。
  - dashboard 能以 managed baseline 为中心展示 solution 内更细粒度条目。
  - unsupported / partial translation surface 能稳定进入外层报告、dashboard 或 release 文档。
- `deliverables`
  - engineering archetype gate matrix
  - dashboard/reason-code closure
  - unsupported feature report
  - release-gate checklist
- `dependencies`
  - Phase 3
  - Phase 4
  - Phase 5
- `open_questions`
  - 无。默认冻结如下：
  - 主线先纳入 mobile-ready contract 与最小 smoke gate，完整 device/package/perf gate 作为后置平台专项。

## 6. 子任务映射

| task_id | phase | status | owner | purpose | depends_on |
| --- | --- | --- | --- | --- | --- |
| `phase-0-contract-freeze-and-capability-ledger` | `Phase 0` | `ready` | `codex` | 冻结 reference bundle、capability taxonomy、stable identity、body availability 与 AOT translation surface 双台账 | `-` |
| `phase-1-subject-source-reshape-and-test-metadata-cutover` | `Phase 1` | `planned` | `codex` | 完成 subject 单文件切片、owner project 归并和测试 attribute 收口 | `phase-0-contract-freeze-and-capability-ledger` |
| `phase-2-typed-il-shared-contract-and-hybrid-dispatch-foundation` | `Phase 2` | `planned` | `codex` | 建立 typed-il 共享契约和统一 dispatch 基础层 | `phase-0-contract-freeze-and-capability-ledger, phase-1-subject-source-reshape-and-test-metadata-cutover` |
| `phase-3-hotupdate-runtime-and-supplemental-metadata-foundation` | `Phase 3` | `planned` | `codex` | 建立热更运行时、supplemental metadata 和 mixed execution 最小闭环 | `phase-2-typed-il-shared-contract-and-hybrid-dispatch-foundation` |
| `phase-4-aot-core-ir-and-native-codegen-strengthening` | `Phase 4` | `planned` | `codex` | 引入 AOT Core IR 并逐步替代 ad-hoc native emitter 路线 | `phase-2-typed-il-shared-contract-and-hybrid-dispatch-foundation, phase-3-hotupdate-runtime-and-supplemental-metadata-foundation` |
| `phase-5-feature-coverage-expansion-on-owner-subjects` | `Phase 5` | `planned` | `codex` | 以 capability ledger 与 translation surface ledger 扩展目标 AOT 翻译面和 proof/benchmark | `phase-1-subject-source-reshape-and-test-metadata-cutover, phase-4-aot-core-ir-and-native-codegen-strengthening` |
| `phase-6-engineering-archetype-benchmark-and-release-gates` | `Phase 6` | `planned` | `codex` | 建立工程级 gate、dashboard、unsupported report 与发布级证据链 | `phase-3-hotupdate-runtime-and-supplemental-metadata-foundation, phase-4-aot-core-ir-and-native-codegen-strengthening, phase-5-feature-coverage-expansion-on-owner-subjects` |

## 7. 依赖

- Phase 0 是整个路线的冻结入口，没有这一步，后续所有实现都会继续边做边改契约。
- Phase 1 负责 subject 和 proof/benchmark 承载面的稳定化，是 capability 扩展的输入清洗阶段。
- Phase 2 和 Phase 3 决定后续系统是否真的 HybridCLR 对齐，而不是“热更补丁附着在 AOT 后面”。
- Phase 4 必须在 Phase 2/3 之后推进，否则 AOT core IR 仍会缺失 mixed execution 和 metadata 语义。
- Phase 5 和 Phase 6 是 capability 扩展与工程级收口阶段，依赖前面契约已经稳定。

## 8. 风险

### 风险 1: roadmap 再次退化为“列语法点打表”

- 缓解
  - 先冻结两级 capability taxonomy。
  - 每个历史语法点先映射 capability item，再决定 owner pack 与实现顺序。

### 风险 2: subject 单文件化演变成 project 爆炸

- 缓解
  - 单文件化只发生在 capability slice 层。
  - project 仍按领域聚合，不强制一能力一工程。

### 风险 3: 热更与 AOT 各自维护 identity 和 metadata

- 缓解
  - Phase 0 明确 stable identity、bundle policy 和 body availability。
  - Phase 2/3 统一 dispatch 与 supplemental metadata。

### 风险 4: dashboard 与真实能力状态继续漂移

- 缓解
  - dashboard 统一以 managed baseline 为基线。
  - 缺失数据必须归入明确的 `Unsupported` 或 reason code，而不是静默丢失。

## 9. 备选路径

### 备选路径 A: 先只做 AOT codegen 增强，热更后置

- 优点
  - 短期更像直接“补功能”。
- 缺点
  - 大概率在 bridge、metadata、dispatch 上返工。

### 备选路径 B: 先只做 subject/test 清理，运行时设计后置

- 优点
  - 文档和目录短期变干净。
- 缺点
  - 如果没有统一 runtime 契约，subject 清理很快会再次漂移。

### 当前判断

- 推荐保持当前主路线：
  - 先冻结契约
  - 再收口 subject 与 metadata
  - 再做 hybrid dispatch / hot update foundation
  - 再强化 AOT core IR
  - 最后扩大能力覆盖和工程级 gate

## 10. 已冻结决策

- `Q0`
  - `reference bundle` 采用“单一 canonical base bundle + 受控 supplement”。
- `Q1`
  - `capability taxonomy` 固定为两级，并按语义/运行时域划分 family。
- `Q2`
  - `public stable id` 采用 `family enum + item enum` 复合稳定 numeric id。
- `Q3`
  - capability slice 命名采用“family/domain 路径 + item 文件名”。
- `Q4`
  - `typed-il` 降到薄层 `interpreter IR`，不直接裸供解释器，也不分叉成第二套总模型。
- `Q5`
  - bridge/thunk 采用编译期主导、运行时有限补充。
- `Q6`
  - 泛型策略先冻结 generic/sharing contract，再优先完成 codegen closure，同时预留 runtime sharing 钩子。
- `Q7`
  - benchmark 采用分层策略，只强制覆盖高价值 runtime path。
- `Q8`
  - 移动平台先进入 contract + smoke gate，不立即拉入完整主线发布 gate。

## 11. 当前建议推进顺序

1. 先进入 `Phase 0`，逐条确认 Q0、Q1 以及剩余 open questions 的默认解。
2. Phase 0 完成后，立即切 `Phase 1`，把 subject 源码、能力切片和测试 metadata 先收口。
3. 然后进入 `Phase 2` 和 `Phase 3`，建立 HybridCLR 对齐的 shared contract 与 hot update foundation。
4. 再执行 `Phase 4`，把 AOT codegen 真正抬到稳定中层上。
5. 最后执行 `Phase 5` 和 `Phase 6`，完成 capability 扩张、benchmark/dashboard 收口和工程级 gate。
