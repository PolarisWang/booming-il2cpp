# Phase 4 MainlineFeaturePack Onboarding Brainstorm v1.02

Date: 2026-04-09
Status: needs-review

## 1. 新发现的结构冲突

`Phase 4` 原设计同时要求：

- `MainlineFeaturePack` 的真实托管入口支持 `--trace-platform <platform>` / `--trace-output <path>`
- `MainlineFeaturePack` 继续走当前 `proof -> generated native -> build-target -> native-runtime-perf` 主线

但当前主线的 managed closure / lowering 仍然默认把 **程序集真实 entrypoint** 当作 native proof entry 来分析，而当前 `LoaderStage` + `NativeReferenceLoweringPlanner` 只接受非常收敛的 entry 形状：

- 单 basic block
- 无分支
- `Program::Main` 必须匹配当前已冻结的几类 minimal lowering 入口

这与 trace CLI 需要的真实参数解析天然冲突。只要把 trace 解析逻辑放进真实 `Main`，native proof 侧就会掉出当前 lowering 家族边界。

## 2. 备选方案

### 方案 A：引入 proof entry override，把“程序集真实入口”和“native proof 入口”拆开

做法：

- 保留程序集真实 `Program::Main(System.String[])`，用于 managed runtime output / trace export
- 让 subject manifest 的 `source.entry` 真正成为 **proof entry subject id**
- `subject_workers.py -> DriverEntry -> ManagedClosureRequest -> LoaderStage` 链路支持传入 `EntryPointSubjectIdOverride`
- native proof 继续只分析一个极简 proof entry；managed trace 继续执行程序集真实 `Main`

优点：

- 不需要在 `Phase 4` 扩大 IL / lowering 能力面
- 完整保住 `performance-first` 决策，`perf.profile` 继续观测 generated native executable
- `MainlineFeaturePack` 可以同时拥有真实 trace CLI 和稳定 native proof
- `source.entry` 不再只是规划元数据，而成为真实的 proof 绑定点

缺点：

- 会新增一条 core pipeline contract surface
- 需要补一轮 driver / loader / worker / unit tests

### 方案 B：扩当前 lowering / loader，让真实 `Main` 直接支持 trace CLI 形状

做法：

- 扩 loader opcode 支持、分支支持，或新增 trace-aware lowering family

优点：

- 不需要拆 entry 语义

缺点：

- 明显越过 `Phase 4` onboarding 边界，提前侵入 `Phase 5/6`
- 风险大，验证面扩大，且很难证明只是 onboarding 改动

结论：

- 当前不采用

### 方案 C：把 `trace-platform` 降级为 managed-only trace pipeline

做法：

- `MainlineFeaturePack` 的 trace 路线改成类似 `HostEmbeddingLite` 的 managed-only pipeline

优点：

- 代码改动最小

缺点：

- 破坏 `Phase 4` 已冻结的 proof-oriented 路线
- 削弱 `MainlineFeaturePack` 作为完整主宿主 subject 的价值
- 不符合 performance-first 主线

结论：

- 当前不采用

## 3. 推荐结论

推荐采用 **方案 A：proof entry override**。

这不是 capability 扩张，而是 `MainlineFeaturePack` onboarding 的必要基础设施：

- managed execution 使用真实 `Main`
- native proof 使用 subject 显式声明的 proof entry

这样 `Phase 4` 仍然是 “把 planning-only subject 接成真实完整 subject”，而不是提前做语义能力扩张。

## 4. 对当前计划的影响

如果采用方案 A，则 `Chunk 1` 的 RED 需要先补一条新边界：

- `source.entry` 必须被 worker / driver / loader 真正消费为 proof entry override

然后再继续：

- `MainlineFeaturePack` 真实 `source/validation/baselines`
- subject-scoped proof build
- focused verification
