# Phase 3 Capability-Driven Lowering And Arbitration Brainstorm v1.01

Date: 2026-04-09
Status: plan-ready

## 1. 目标

本轮不再讨论 `Phase 2` 是否成立；它已经成立。  
当前要解决的是：`CodeGen` 还没有成为 lowering 决策层，`NativeReferenceProofEmitter` 仍然同时承担 family 识别、plan 组装与模板渲染。

## 2. 备选路径

### 方案 A：继续让 emitter 做 planner，只补文档

- 优点：改动最小。
- 缺点：`proof-shape -> lowering family` 的反向推理继续存在，`CodeGen` 仍然不是正式 lowering 边界。

### 方案 B：`CodeGen` 产出 managed lowering plan，emitter 只消费 plan

- 优点：最符合已冻结讨论边界。
- 优点：可以把 lowering family 真源前移到 `managed closure` 侧，同时保留 `native-proof.plan.json` 作为 native 输出镜像。
- 缺点：需要同步改 `CodeGen`、`Driver`、`Emitter` 与 contract tests。

### 方案 C：本轮直接把 lowering plan 改成完整 role-binding 新 contract

- 优点：最终形态更干净。
- 缺点：当前改动面过大，会把 `Phase 3` 和后续 role contract 重构绑死在一轮里。

## 3. 选择

本轮采用方案 B。

原因：

- 用户要求持续推进主任务，当前最值钱的改动是先切掉 emitter 的 lowering discovery 职责。
- 这一步已经足够把主决策面改成 `semantic subject structure + capability bundle`。
- role-binding contract 可以在 `Phase 3` 后续 chunk 继续收紧，不需要在本批次一次性做完。

## 4. 本轮边界

本轮要做：

- 冻结 canonical lowering family 名称。
- 让 `SemanticWorld` 提供足够区分当前 4 类 family 的 method structure signals。
- 让 `CodeGen` 基于 semantic structure + capability bundle 选择 family，并产出 `native-reference.lowering-plan.json`。
- 让 `DriverEntry.Run(ManagedClosureRequest)` 把 managed lowering plan 写盘。
- 让 `NativeReferenceProofEmitter` 只读取 plan 并渲染，同时在 native proof 输出目录镜像 `native-proof.plan.json`。

本轮不做：

- `contract` / `subject` 边界清理。
- 完整 role-binding contract 重构。
- 完整 performance arbitration scoring system。
- `MainlineFeaturePack` onboarding。
