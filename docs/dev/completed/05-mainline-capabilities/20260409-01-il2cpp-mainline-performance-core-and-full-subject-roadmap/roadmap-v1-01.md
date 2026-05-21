# IL2CPP Mainline 性能优先核心与完整 Subject Roadmap v1.01

Date: 2026-04-09
Status: active-roadmap

## 1. 目标

把当前仓库从“proof-first、sample-driven、subject 分裂为 correctness 与 managed perf” 的状态，推进成一条新的 mainline：

- 核心层一切决策以 generated native 工程性能最优为第一目标。
- `SemanticWorld -> Linker -> CodeGen -> Emitter` 的分层边界正式收口。
- 现有最小 probe subjects 保留。
- 新增一个完整 subject，统一承接后续 capability 补全、native correctness、trace 与 perf 回归。

这个父 roadmap 不直接承担实现，而是负责冻结阶段边界、child task 映射、依赖关系和 open questions。

## 2. 范围边界

本 roadmap 负责：

- `src/managed/Chaos.IL2CPP.*` 的主线 contract 与阶段切分
- `src/native/**` 中与 generated native 主线直接相关的 runtime/bootstrap/support 边界
- `OptimizationFacts`、`performance arbitration`、`native perf evidence/report` 的正式落点
- `subjects/HelloWorldObject`、`subjects/GenericEcho` 的角色重定位
- 新增 `subjects/MainlineFeaturePack`
- subject pipelines、validation profiles、perf baseline 与 report 主线
- engine binding 的最终收口阶段

本 roadmap 不负责：

- `.NET 10` 输入兼容
- `macOS` parity
- Android / iOS runtime subset
- 删除现有最小 probe subjects

## 3. 非目标

当前不在这条父 roadmap 中一次性解决：

- 完整异常模型与所有高阶 BCL 语义的全量兼容
- 多平台矩阵同步推进
- 新造平行于 `subject + matrix + artifact-chain` 的工件模型
- 把一个完整 subject 伪装成覆盖所有失败面的“大一统 smoke”

## 4. 阶段列表

### Phase 1: `OptimizationFacts` And Native Perf Contracts

- 先冻结 performance-first 核心 contract。
- 明确完整 subject 的第一版 validation profile、matrix、perf 证据与报告边界。

### Phase 2: Core Contract Cutover

- 把 `SemanticWorld -> Linker -> CodeGen` 的 sample-driven 暂存逻辑替换为正式 contract 落点。
- 在代码层引入 `OptimizationFacts` 的最小闭环。

### Phase 3: Capability-Driven Lowering And Arbitration

- 把 lowering family 从 emitter 内部样例判断迁出。
- 建立“合法性过滤 + 性能裁决”的 codegen 选择主线。

### Phase 4: `MainlineFeaturePack` Onboarding

- 新增完整 subject。
- 补齐 native output / trace / perf 三条验证主线。

### Phase 5: Capability Batch A

- 先落首批性能敏感能力：
  - dispatch
  - closed generics/layout
  - arrays/boxing

### Phase 6: Capability Batch B And Phase A Closure

- 再落第二批能力：
  - delegates/callbacks
  - exceptions
  - 必要的反射/interop 补位
- 最后总结 Phase A 剩余缺口并决定是否进入更大的 Phase B follow-up。

### Phase 7: Engine Binding Tail Roll-In

- 在 core performance-first 主线与完整 subject 稳定后，再把 engine binding 收口进正式 roadmap。
- 复用既有 `subject + matrix + artifact-chain` 主线，不允许为 engine binding 新造平行系统。

## 5. 每阶段完成定义

### Phase 1

- `goal`: 冻结 `OptimizationFacts`、native perf evidence/report、完整 subject 第一版 contract，避免后续实现继续在 sample-specific 逻辑上加层。
- `exit_criteria`:
  - 已明确 `OptimizationFacts` 最小结构与归属边界
  - 已明确 `native-runtime-perf` pipeline 与 report 附件边界
  - 已明确 `MainlineFeaturePack` 的 manifest/profile/matrix 第一版设计
- `deliverables`:
  - `OptimizationFacts` 设计文档
  - native perf report/evidence 设计文档
  - `MainlineFeaturePack` subject contract 设计文档
- `dependencies`: 无
- `open_questions`:
  - 完整 subject 是否接受独立命名为 `MainlineFeaturePack`
  - `perf-profile` 是否在第一版就强制同时跑 proof + perf

### Phase 2

- `goal`: 在代码层为新的 performance-first contract 建骨架，并让现有 subjects 仍然可回归。
- `exit_criteria`:
  - `OptimizationFacts` 已有正式类型与工件落点
  - `Linker` 可以输出最小 optimization-oriented facts
  - `CodeGen` 可以消费 `OptimizationFacts`，即使暂时只支持有限 family
- `deliverables`:
  - 新增或更新的 contracts/schema/snapshot
  - `SemanticWorld`、`Linker`、`CodeGen` 的切换骨架
  - focused regression tests
- `dependencies`:
  - Phase 1
- `open_questions`:
  - `OptimizationFacts` 首版是先落 JSON artifact，还是先只落内存 contract 再补持久化

### Phase 3

- `goal`: 把 lowering 选择逻辑从 sample-shape 分支迁移到正式 capability/fact 驱动。
- `exit_criteria`:
  - `LoweringFamily` legality filter 生效
  - performance arbitration 已替代静态优先级成为主裁决器
  - `NativeReferenceProofEmitter` 不再承担长期 sample-driven family 识别职责
- `deliverables`:
  - lowering family contract
  - performance arbitration policy
  - emitter/template/backend 切分方案与回归用例
- `dependencies`:
  - Phase 2
- `open_questions`:
  - 首版 arbitration 是否只做 deterministic policy，不引入 target-specific cost tuning 表

### Phase 4

- `goal`: 把完整 subject `MainlineFeaturePack` 正式接入仓库主线。
- `exit_criteria`:
  - `subjects/MainlineFeaturePack` 目录、manifest、validation 结构稳定
  - 存在 `windows-native-check`、`windows-reference-trace`、`windows-native-profile` 三类主矩阵
  - 可以通过统一入口运行 output、trace、perf 三类主验证
- `deliverables`:
  - 完整 subject source/unit/proof/perf 目录
  - `native-runtime-perf` pipeline
  - perf baseline 与 report 附件主线
- `dependencies`:
  - Phase 1
  - Phase 3
- `open_questions`:
  - 第一版 perf baseline 是否只固定 Windows host
  - 是否允许完整 subject 在早期只覆盖 `CHECK` 与 `PROFILE`

### Phase 5

- `goal`: 让第一批性能敏感能力在 core contract 与完整 subject 上同时落地。
- `exit_criteria`:
  - dispatch、closed generics/layout、arrays/boxing 均有明确 subject 切片与 perf 回归
  - 对应能力不再依赖样例特化 emitter 分支
  - 性能回归已能通过 baseline compare 被观察
- `deliverables`:
  - Batch A capability matrix
  - 对应 core/codegen/runtime 变更
  - 更新后的 full subject 切片源码与 perf baseline
- `dependencies`:
  - Phase 4
- `open_questions`:
  - Batch A 内部优先顺序是否先做 dispatch，再做 generics/layout，再做 array/boxing

### Phase 6

- `goal`: 完成第二批能力补位，并把 `Phase A` 剩余缺口写清楚。
- `exit_criteria`:
  - delegates/callbacks、exceptions、必要的反射/interop 补位已完成或明确降级
  - 已形成 `Phase A` 残余缺口列表
  - 已明确下一步是继续 core semantics，还是转入更大的 mainline follow-up
- `deliverables`:
  - Batch B capability matrix
  - `Phase A` closure note
  - 后续 follow-up 建议
- `dependencies`:
  - Phase 5
- `open_questions`:
  - exceptions 是否一定要求进入这一轮 closure，还是允许转成下一条 follow-up

### Phase 7

- `goal`: 按用户要求，把 engine binding 放在整个 roadmap 最后收口，并复用已经稳定的 core/full-subject 主线。
- `exit_criteria`:
  - engine binding 不再只停留在 discuss 文档，而是有正式 child task 与实现入口
  - service call / object handle / callback-thread-lane 至少形成第一版正式 subject/matrix 方案
  - engine proof 仍复用 `subject + matrix + artifact-chain`，不新造平行 artifact bucket
- `deliverables`:
  - engine binding tail-phase design/plan
  - 首批 engine binding child task
  - 对 `docs/discuss/il2cpp-mainline/20260407-10` 到 `20260407-15` 的收口映射
- `dependencies`:
  - Phase 6
- `open_questions`:
  - engine binding 首轮是新增独立 engine probe subjects，还是扩展 `MainlineFeaturePack` 的 engine slice

## 6. 子任务映射

| task_id | phase | status | owner | purpose | depends_on |
| --- | --- | --- | --- | --- | --- |
| `20260409-02-phase-1-optimization-facts-and-native-perf-contracts` | `Phase 1` | `in-progress` | `codex` | 冻结 `OptimizationFacts`、native perf 证据与完整 subject 第一版 contract | `-` |
| `phase-2-core-contract-cutover` | `Phase 2` | `planned` | `codex` | 把 performance-first contract 从讨论层落到 `SemanticWorld -> Linker -> CodeGen` 代码骨架 | `phase-1-optimization-facts-and-native-perf-contracts` |
| `phase-3-capability-driven-lowering-and-arbitration` | `Phase 3` | `planned` | `codex` | 迁出样例驱动的 lowering 判断，建立正式 family + arbitration 主线 | `phase-2-core-contract-cutover` |
| `phase-4-mainline-feature-pack-onboarding` | `Phase 4` | `planned` | `codex` | 新增完整 subject 并接入 output/trace/perf 三条统一主线 | `phase-3-capability-driven-lowering-and-arbitration` |
| `phase-5-capability-batch-a-dispatch-generic-layout-and-array-boxing` | `Phase 5` | `planned` | `codex` | 落首批性能敏感能力并建立 baseline compare | `phase-4-mainline-feature-pack-onboarding` |
| `phase-6-capability-batch-b-delegate-exception-and-phase-a-closure` | `Phase 6` | `planned` | `codex` | 落第二批能力、收口剩余缺口并决定下一条 follow-up | `phase-5-capability-batch-a-dispatch-generic-layout-and-array-boxing` |
| `phase-7-engine-binding-tail-roll-in` | `Phase 7` | `planned` | `codex` | 把 engine binding 作为路线尾阶段收口，复用成熟的主线模型 | `phase-6-capability-batch-b-delegate-exception-and-phase-a-closure` |

## 7. 依赖

全局依赖顺序固定为：

1. 先冻结 `OptimizationFacts` 与 native perf contract。
2. 再把新 contract 切进 core stages。
3. 再把 lowering arbitration 从 sample-driven 迁出。
4. 再接入完整 subject。
5. 再按批次补 capabilities。
6. 再做 `Phase A` closure。
7. 最后收口 engine binding。

不能倒置的原因是：

- contract 不稳时，完整 subject 只能再次沦为大样例。
- lowering arbitration 不稳时，perf baseline 没有真正意义。
- 完整 subject 不稳时，capability 补全无法形成长期 native perf 回归。
- 如果忘记把 engine binding 作为尾阶段正式收口，前面 discuss 结论又会重新悬空。

## 8. 风险

### 风险 1：`OptimizationFacts` 继续停留在隐式逻辑

如果 `OptimizationFacts` 不 first-class 化，所有性能相关事实都会继续散落在 query、payload、emitter 分支和 ad-hoc metadata fallback 中，最后无法支撑真正的 native performance-first codegen。

### 风险 2：完整 subject 退化成巨型 smoke

如果 `MainlineFeaturePack` 只追求“一个工程全都能跑”，而没有 feature slice 与批次规划，它会掩盖问题，而不是帮助定位问题。

### 风险 3：perf 继续绕开 generated native 主线

如果 perf 仍以 managed benchmark 为主，生成 native 性能就无法成为正式治理目标。

### 风险 4：最小 probe subject 被误删或被过度扩张

如果把 `HelloWorldObject`、`ReflectionLite`、`PInvokeLite` 等最小 probe 删除或过度膨胀，整个主线会失去分层回归点。

## 9. 备选路径

### 方案 A：继续只靠最小 probe subjects 推进

- 优点：实施最轻。
- 缺点：无法形成完整 native perf 主线。

### 方案 B：保留 probe subjects + 新增 full subject

- 优点：兼顾定位能力和集成能力。
- 缺点：需要更早冻结 full subject contract。

当前采用：

- **方案 B**

### 方案 C：直接扩张 `HelloWorldObject`

- 优点：短期对象更少。
- 缺点：最小 correctness/reference subject 会失去边界。

## 10. 当前建议推进顺序

1. 先派生 `Phase 1` child plan，冻结 `OptimizationFacts`、native perf report 与完整 subject contract。
2. 再派生 `Phase 2`，让新 contract 进入 core stages。
3. 再派生 `Phase 3`，把 lowering arbitration 主线改成性能导向。
4. `Phase 3` 稳定后，再接入 `MainlineFeaturePack`。
5. 完整 subject 稳定后，再按 Batch A / Batch B 分批补能力。
6. 再以 `Phase 6` 收口 `Phase A`，整理剩余缺口。
7. 最后执行 `Phase 7`，正式收口 engine binding。

## 11. 结论

当前已经具备稳定父级 roadmap 的条件，而且用户已经确认：

- 新增独立完整 subject `MainlineFeaturePack`
- 首批 capability 先按性能导向排序推进，同时 `exceptions` 必须纳入本 roadmap
- `engine binding` 放到整个 roadmap 最后收口

因此下一步直接派生 `Phase 1` child plan。
