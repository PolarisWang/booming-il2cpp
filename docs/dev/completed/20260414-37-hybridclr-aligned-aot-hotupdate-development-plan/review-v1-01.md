# HybridCLR 对齐的 AOT 与热更开发计划审计 v1.01

Date: 2026-04-15 00:08:00 +08:00
Status: reviewed-after-q-freeze

## 1. 结论

当前计划在架构层面已经满足继续推进的条件：

- Q0-Q8 已全部冻结为默认决策。
- AOT translation surface 与 unsupported surface 已被纳入 Phase 0 强制交付物。
- 计划已经覆盖 AOT、热更、mixed execution、subject 收口、工程级 gate、benchmark 与外层报告。

当前剩余的不确定性不再是路线级不确定性，而主要是实现级和拆批顺序级细节。

## 2. 已冻结决策

- `reference bundle`
  - 采用 `repository-owned canonical base bundle + controlled supplement`。
- `capability taxonomy`
  - 固定为两级，family 按语义/运行时域划分。
- `public stable id`
  - 采用 `family enum + item enum` 的复合稳定 numeric id。
- `capability slice naming`
  - 采用 `family/domain path + item file name`。
- `typed-il -> interpreter`
  - 采用薄层 `interpreter IR`。
- `bridge/thunk policy`
  - 编译期主导，运行时有限补充。
- `generic strategy`
  - 先冻结 generic/sharing contract，再先做 codegen closure，同时保留 runtime sharing 钩子。
- `benchmark policy`
  - 分层覆盖，只强制高价值 runtime path。
- `mobile gate policy`
  - 主线先纳入 mobile-ready contract 和 smoke gate，完整真机 gate 后置。

## 3. 任务目标覆盖审核

### 目标 1: 完善 IL2CPP 核心与 AOT 翻译能力

- 覆盖情况
  - 已覆盖。
- 对应阶段
  - `Phase 0`
  - `Phase 2`
  - `Phase 4`
  - `Phase 5`
- 说明
  - `Phase 0` 冻结目标翻译面台账。
  - `Phase 2` 冻结 shared contract。
  - `Phase 4` 建立 `AotCoreIr` 与 codegen 主线。
  - `Phase 5` 扩张实际能力覆盖。

### 目标 2: 热更方案贴近 HybridCLR

- 覆盖情况
  - 已覆盖。
- 对应阶段
  - `Phase 2`
  - `Phase 3`
  - `Phase 6`
- 说明
  - shared dispatch、supplemental metadata、bridge/thunk、mixed execution 都已纳入主线，而不是旁挂。

### 目标 3: subject 继续 solution 化并保持整洁

- 覆盖情况
  - 已覆盖。
- 对应阶段
  - `Phase 1`
  - `Phase 5`
- 说明
  - 顶层 owner pack 稳定，内部按 capability slice 单文件化，但 project 仍按领域归并。

### 目标 4: 不再继续扩大 string 协议面

- 覆盖情况
  - 已覆盖。
- 对应阶段
  - `Phase 0`
  - `Phase 1`
  - `Phase 2`
- 说明
  - stable numeric id、枚举、body availability 与 generated metadata 已覆盖这条约束。

### 目标 5: 完整 solution 验证与工程级 gate

- 覆盖情况
  - 已覆盖。
- 对应阶段
  - `Phase 1`
  - `Phase 6`
- 说明
  - subject 保持 solution owner pack，最终由 engineering archetype / full-solution gate 收口。

### 目标 6: proof / benchmark / report 共用统一证据链

- 覆盖情况
  - 已覆盖。
- 对应阶段
  - `Phase 1`
  - `Phase 5`
  - `Phase 6`

## 4. 分阶段任务清单、注意细节与剩余不确定性

### Phase 0: Contract Freeze And Capability Ledger

- 任务清单
  - 冻结 `reference bundle` 分层模型。
  - 冻结两级 `capability taxonomy`。
  - 冻结 `public stable id` 复合 numeric id 规则。
  - 冻结 `body availability` 枚举。
  - 建立 `aot-translation-surface-ledger`。
  - 建立 `aot-unsupported-surface-ledger`。
  - 冻结 `ChaosUnitTest` / `ChaosBenchmark` 的最小公共 contract。
- 注意细节
  - translation surface 必须覆盖 language、compiler-lowered、runtime service、metadata/closure、interop/ABI 五类面。
  - `unsupported` / `partial` 项必须带稳定 reason code。
  - `reference bundle` supplement 只能是受控补层，不能演变成平行 bundle。
- 剩余不确定性
  - 无路线级不确定性。
  - 仅剩实现时如何组织 ledger 文件格式和枚举编码的细节。
- 阶段验证
  - 所有 translation surface 都能归档到 `supported/partial/unsupported/not-applicable`。
  - 每个 capability item 都能映射到 owner subject、owner archetype、proof kind、可选 benchmark。
  - `unsupported surface ledger` 能直接进入外层报告。

### Phase 1: Subject Source Reshape And Test Metadata Cutover

- 任务清单
  - 将历史语法点机械拆成单文件 capability slice。
  - 在 owner pack 内按领域归并 project。
  - 将 proof 全部转为 `ChaosUnitTest`。
  - 将 benchmark 全部转为 `ChaosBenchmark`。
  - 移除 `Console.WriteLine` 判定，改为 Assert。
  - 清理旧的 subject 目录协议和残余手工注册面。
- 注意细节
  - 单文件化只发生在 feature slice 层，不允许项目粒度继续爆炸。
  - 路径体现 family，文件名体现 item。
  - 热更和 mixed execution 的 owner 边界不能因为目录清理而被揉平。
- 剩余不确定性
  - 无路线级不确定性。
  - 仅剩具体哪些 slice 合并到哪个 project 的拆批顺序问题。
- 阶段验证
  - subject source 仍能以 solution 模式完整构建。
  - discovery 结果与 ledger 对齐。
  - 不再需要额外目录 DSL 才能定义 proof / benchmark。

### Phase 2: Typed-IL Shared Contract And Hybrid Dispatch Foundation

- 任务清单
  - 冻结 `typed-il` 共享契约。
  - 建立薄层 `interpreter IR`。
  - 建立 `BodyAvailabilityResolver`。
  - 建立 `HybridDispatchResolver`。
  - 对齐 AOT / interpreter / bridge 的 method/type identity。
- 注意细节
  - `interpreter IR` 必须是 typed-il 的薄层正规化，而不是第二套世界。
  - body availability 要覆盖 native、interpreted、bridge、metadata-only、unsupported。
  - 这里不能提前把运行时 fallback 写成 ad-hoc 分支。
- 剩余不确定性
  - 无架构级不确定性。
  - 实现级仍需要决定 typed-il 与 IR 的具体字段边界。
- 阶段验证
  - 同一 method identity 在三条执行路径下可追踪。
  - mixed execution 的 dispatch 决策稳定且可解释。
  - `typed-il`、`interpreter IR`、`AotCoreIr` 的关系文档齐全。

### Phase 3: HotUpdate Runtime And Supplemental Metadata Foundation

- 任务清单
  - 建立 `HotUpdateAssemblyLoader`。
  - 建立 `SupplementalMetadataRegistry`。
  - 建立最小 `BridgeThunkGenerator` contract。
  - 打通 host/hot-update identity 对齐。
  - 跑通最小 host -> patch -> fallback 链路。
- 注意细节
  - bridge/thunk 默认前置生成，运行时补充只能覆盖晚绑定例外。
  - supplemental metadata 必须可查询、可诊断、可进入 report。
  - hot update 不应拥有独立的 identity 体系。
- 剩余不确定性
  - 无路线级不确定性。
  - 仅剩 bridge 补充生成的最小边界如何定义。
- 阶段验证
  - 至少一条 mixed/hot-update 验证链跑通。
  - metadata supplement 有稳定证据与 diagnostics。
  - host/patch 边界的 callback、异常、共享契约有 proof。

### Phase 4: AOT Core IR And Native Codegen Strengthening

- 任务清单
  - 建立 `AotCoreIr`。
  - 将 `typed-il` 降到 `AotCoreIr`。
  - 把对象模型、调用模型、泛型、异常、metadata closure 接入 `AotCoreIr`。
  - 逐步替代 ad-hoc `NativeAotEmitter` 路径。
- 注意细节
  - 不能再直接依赖临时 loader model 发射 C++。
  - 泛型先冻结 sharing contract，再推进 codegen closure。
  - AOT 与 hot update 仍要共用 identity 和 body availability。
- 剩余不确定性
  - 无路线级不确定性。
  - 实现级仍需要决定先迁哪一批 emitter path。
- 阶段验证
  - `AotCoreIr` 能承接已定义的 translation surface 子集。
  - 当前关键 codegen 路径已有迁移切片。
  - generic closure、exception、metadata closure 有明确证据。

### Phase 5: Feature Coverage Expansion On Owner Subjects

- 任务清单
  - 按 ledger 补齐 target translation surface。
  - 每个 surface 条目映射到 owner subject、proof、可选 benchmark、support-state。
  - 刷新 `unsupported surface report`。
  - 保持 owner pack 不膨胀。
- 注意细节
  - 不允许新增“没有 owner 的实验 subject”。
  - benchmark 只给高价值 runtime path。
  - deferred / unsupported 项也必须留在账上。
- 剩余不确定性
  - 无架构级不确定性。
  - 仅剩分批实现顺序和 priority 取舍。
- 阶段验证
  - translation surface ledger 无空白项。
  - proof 与 benchmark 的覆盖符合分层策略。
  - unsupported 项能稳定出现在报告中。

### Phase 6: Engineering Archetype, Benchmark And Release Gates

- 任务清单
  - 打通 engineering archetype gate。
  - 打通 full-solution ingestion -> translate -> native generate -> run。
  - 收口 benchmark dashboard。
  - 收口 unsupported / partial report。
  - 建立 release-gate checklist。
  - 纳入 mobile-ready contract 与 smoke gate。
- 注意细节
  - dashboard 必须以 managed baseline 为核心。
  - 缺数据要明确区分 `missing evidence` 与 `unsupported`。
  - 移动平台只先纳入 smoke / contract，不立刻拉 full perf gate。
- 剩余不确定性
  - 无路线级不确定性。
  - 平台专项的排期仍可后置。
- 阶段验证
  - retained owner subjects 的 native generate 全部稳定。
  - engineering archetype / benchmark / unsupported report 三条线能互相对照。
  - release checklist 可直接用于外层 gate。

## 5. 测试工程需要新增的功能

测试工程后续至少需要新增或补强以下能力：

### 5.1 `src/reference/Chaos.TestFramework` 侧

- `ChaosUnitTest` / `ChaosBenchmark` attribute 的枚举化元数据
  - capability family
  - capability item
  - archetype
  - requirement flags
  - execution mode flags
- Assert 套件增强
  - 值相等
  - 异常断言
  - 序列断言
  - 引用/类型断言
  - 异步结果断言
  - 可选性能门槛断言
- 统一的 stable entry metadata
  - 不继续依赖 string 协议做条目路由。

### 5.2 discovery / generation 侧

- 基于 attribute 的 solution 级 discovery。
- managed test host 自动生成。
- native test host 自动生成。
- benchmark host 自动生成。
- host 产物和 source slice 的 stable mapping。

### 5.3 coverage / ledger 对账能力

- `translation surface -> discovered test entries` 的对账器。
- `unsupported surface ledger` 校验器。
- `capability item -> owner subject/archetype` 的完整性校验器。

### 5.4 benchmark / reporting 侧

- 以 managed baseline 为中心的比较模型。
- `unsupported` / `partial` / `missing evidence` 区分显示。
- entry 级 benchmark identity 稳定化。
- benchmark 与 proof 结果交叉链接。

### 5.5 engineering gate 侧

- solution archetype gate runner。
- full-solution native generate gate。
- mixed execution gate。
- hot update gate。
- mobile smoke gate。

## 6. 当前是否仍有未决空间

如果以“路线级、架构级、契约级”标准审视，当前没有必须先讨论才能推进的未决空间。

仍然存在的空间主要是：

- 拆批顺序
- 文件命名和目录落点的细部选择
- 某些 feature batch 的优先级
- 某些平台专项进入主线的具体时点

这些不再阻塞 roadmap 本身。

## 7. 各阶段验证总表

- `Phase 0`
  - 文档与 ledger 验证。
- `Phase 1`
  - solution source build + attribute discovery + Assert proof 迁移验证。
- `Phase 2`
  - shared identity / dispatch / IR contract 验证。
- `Phase 3`
  - host/patch/fallback mixed execution 验证。
- `Phase 4`
  - `typed-il -> AotCoreIr -> native` 关键路径验证。
- `Phase 5`
  - translation surface 覆盖率与 unsupported report 验证。
- `Phase 6`
  - engineering archetype / benchmark dashboard / release gate 验证。

## 8. 最终判断

这份计划现在已经满足当前提出的任务目标：

- 有完整主线。
- 有冻结后的架构决策。
- 有完整 AOT translation surface 的纳管要求。
- 有每阶段可执行清单。
- 有每阶段验证出口。
- 有测试工程需要新增的能力清单。

因此，roadmap 可以视为通过本轮规划审计，下一步可以直接进入 `Phase 0` 执行。
