# Phase 4 MainlineFeaturePack Onboarding Design v1.02

Date: 2026-04-09
Status: needs-review

## 1. 设计调整目标

在不扩大 `Phase 4` 能力边界的前提下，同时满足：

- `MainlineFeaturePack` 的真实托管入口支持 `--trace-platform` / `--trace-output`
- native proof / perf 继续走当前 generated native 主线
- 一切仍以 generated native 工程性能优先为准

## 2. 核心设计

### 2.1 双入口语义

同一个 subject 内区分两类入口：

- **runtime entry**
  - 程序集真实 entrypoint
  - 由 `dotnet <assembly>.dll ...` 直接执行
  - 负责 managed output 与 trace export CLI
- **proof entry**
  - 由 subject manifest `source.entry` 声明
  - 由 managed closure / CodeGen / native proof 链路消费
  - 必须继续满足当前 minimal lowering 边界

### 2.2 pipeline contract 调整

新增 `ManagedClosureRequest.EntryPointSubjectIdOverride`，默认可空：

- 为空：沿用程序集真实 entrypoint
- 非空：`LoaderStage` 使用 override 作为 `LoadedAssemblyModel.EntryPointSubjectId`

这样其他已有 subject 不变，`MainlineFeaturePack` 可以显式声明自己的 proof entry。

### 2.3 worker 接线

`build/toolchains/run/testing/subject_workers.py`

- `run_frontend_pipeline_worker(...)` 调用 driver 时，追加 `selection.source.entry`
- driver 把该值传入 `ManagedClosureRequest`

### 2.4 MainlineFeaturePack source 结构

`MainlineFeaturePack` 第一版 source 采用：

- `Program::Main(System.String[])`
  - 真实 runtime entry
  - 负责 trace CLI
- `ProofEntry::Run()`
  - 简单 proof entry
  - 保持当前 lowering family 可接受的极简形状

subject manifest 的 `source.entry` 改为 `MainlineFeaturePack/ProofEntry::Run()`

## 3. 为什么这仍然属于 Phase 4

这次调整解决的是 **onboarding 基础设施冲突**，不是新增 capability：

- 没有扩 loader 的 IL opcode 面
- 没有扩 CodeGen lowering family 族谱
- 没有提前做 `exceptions`、delegates、arrays、dispatch 等后续能力

它只是把 “真实 managed 入口” 和 “当前 native proof 入口” 做了职责拆分，让 subject 可以同时承接：

- managed trace 验证
- native proof / native perf 验证

## 4. 执行顺序调整

在原 `Chunk 1-4` 前面插入一个新的前置步：

1. 先补 RED，锁定 proof entry override contract
2. 再落地 `MainlineFeaturePack` 的真实 source / unit / proof / baseline
3. 再落地 `CHAOS_SUBJECT_PROOF_ROOT`
4. 最后做 focused verification

## 5. 风险

- 需要改动 managed contract surface，必须补 unit tests 锁死默认兼容行为
- `source.entry` 的语义从“规划元数据”升级为“proof entry 绑定点”，需要在任务文档里明确

## 6. 推荐结论

采用本设计继续推进 `Phase 4`。
