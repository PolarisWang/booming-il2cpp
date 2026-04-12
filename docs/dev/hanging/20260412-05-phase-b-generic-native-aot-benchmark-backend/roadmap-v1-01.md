# Phase B Generic Native AOT Benchmark Backend Roadmap v1-01

Date: 2026-04-12 11:42:02 +08:00
Status: in_progress

## 1. 目标

用正确的方式打通 native benchmark 路线：

- 把 benchmark native 从 `generated-native-proof` 中拆出，建立独立的 `generated-native-aot` 路线。
- 冻结 proof backend 与 generic native AOT backend 的 artifact/contract 边界。
- 在 benchmark manifest 中引入 `workloadEntry`，让 perf harness 调 workload，而不是以 benchmark source 的 `Main` 直接承担计时逻辑。
- 保持性能采样与计时在 generated code 之外。
- 先让 `BenchArithmetic` 在 native 模式下真实产出 record 和 dashboard 数据，再扩展到其他 `Bench*`。

## 2. 范围边界

本 roadmap 负责：

- 定义并落地 `generated-native-proof` 与 `generated-native-aot` 的职责拆分。
- 定义 benchmark native 所需的 artifact contract、entry contract、worker 边界。
- 明确 `BenchArithmetic` 的 generic native AOT MVP 路线与验收命令。
- 为后续 `BenchAllocation` / `BenchDispatch` / `BenchGeneric` 等 subject 的迁移建立可复用路线。

本 roadmap 不直接承担：

- 把所有 `Bench*` 一次性全部迁移完成。
- 为 proof backend 继续补 benchmark family 特判。
- 把性能分析代码、计时代码或 profiler 注入 generated code。
- 重新打开 Phase D 真机资源问题。

## 3. 非目标

- 不把 benchmark native 重新包装成 proof subject。
- 不在 C# benchmark source 内保留新的 perf scaffolding。
- 不在本轮里把 generic native AOT 扩展成“支持所有 IL/C# 语义”的完全体 backend。
- 不用 dashboard “显示 no data” 去掩盖 native pipeline 实际没有跑通的事实。

## 4. 阶段列表

### Stage 1：Backend 拆线与 Contract Freeze

- 目标是把 proof backend 与 generic native AOT backend 的输入、输出、选路规则明确拆开。

### Stage 2：Benchmark Entry Boundary 与 External Harness

- 目标是把 benchmark source、workload entry、外部 perf harness 三者的边界冻住。

### Stage 3：BenchArithmetic Generic Native AOT MVP

- 目标是基于新的 backend 和 entry contract，让 `BenchArithmetic` 在 native 模式端到端成功。

### Stage 4：多 Subject Rollout 与 Dashboard 回补

- 目标是让后续 `Bench*` 可以接入同一条 generic native AOT 路线，而不是重复开 proof family 例外。

## 5. 每阶段完成定义

### Stage 1

- `goal`:
  - 明确 `generated-native-proof` 只服务 proof/capability 验证。
  - 明确 `generated-native-aot` 作为 benchmark / 通用 AOT backend 的入口。
- `exit_criteria`:
  - benchmark native 不再经过 `NativeReferenceLoweringPlanner.SelectLoweringFamily` 的 proof family 分类路径。
  - 新旧两条 backend 的 artifact contract、stage kind、责任边界有书面定义。
  - 既有 proof subject 回归路径不被 `generated-native-aot` 误影响。
- `deliverables`:
  - backend 拆线说明文档。
  - `subject_workers.py` / codegen stage 所需的选路改造计划。
  - generic native AOT artifact contract 草案。
- `dependencies`:
  - 现有 frontend 输出、codegen stage、proof emitter 结构。
- `open_questions`:
  - `generated-native-aot` 是全新 stage kind，还是现有 codegen stage 下的新 backend mode。
  - proof 与 generic AOT 是否共享部分 metadata/code-registration 结构。

### Stage 2

- `goal`:
  - 明确 benchmark source 只提供 workload，perf timing 由外部 harness 负责。
  - 用 manifest contract 描述 workload entry，而不是默认绑死 `Main()`。
- `exit_criteria`:
  - benchmark manifest 支持 `workloadEntry` 或等价字段，并能被三种模式统一消费。
  - `BenchArithmetic` source 保持纯 workload，不再承载 timing/profiler 逻辑。
  - native perf worker 能基于新的 entry contract 调用 generated workload。
- `deliverables`:
  - benchmark manifest contract 更新方案。
  - external native perf harness 调用约定。
  - `BenchArithmetic` source/manifest 边界说明。
- `dependencies`:
  - Stage 1 的 backend contract。
  - 现有 `run_native_runtime_perf` / `benchmark.py` 记录链路。
- `open_questions`:
  - `workloadEntry` 是方法引用、符号名，还是 frontend 解析后的稳定 method id。
  - managed / interpreter / native 三模式是否统一暴露同一种 workload 标识。

### Stage 3

- `goal`:
  - 基于 Stage 1/2 的 contract，让 `BenchArithmetic` 的 native benchmark 真正跑通。
- `exit_criteria`:
  - `python build/toolchains/run/run.py benchmark --subject BenchArithmetic --mode native --record` 成功。
  - `subjects/BenchArithmetic/benchmark-records/records.jsonl` 出现 native 记录。
  - `docs/benchmark/overview.json` 和 `docs/benchmark/subjects/BenchArithmetic.json` 出现 native 最新数据。
  - 既有 managed / interpreter benchmark 路线无回归。
- `deliverables`:
  - `BenchArithmetic` generic native AOT MVP 实现。
  - 覆盖新 contract 的单元测试 / 端到端验证命令。
  - benchmark dashboard 回补 native 数据的证据。
- `dependencies`:
  - Stage 1、Stage 2。
  - 当前已补齐的 `Ldlen` / `Blt_s` 等 interpreter/frontend 基础支持。
- `open_questions`:
  - MVP 需要支持的最小 opcode / lowering 集合是否只覆盖 `BenchArithmetic`。
  - native workload ABI 是否需要专门的 bootstrap wrapper。

### Stage 4

- `goal`:
  - 把 `BenchArithmetic` 的通路推广成 benchmark native 的标准做法。
- `exit_criteria`:
  - 后续 `Bench*` 新增 native 支持时不再要求新增 proof family。
  - dashboard 在 rollout 期间对缺失 native 数据保持稳定，对已迁移 subject 正常显示最新 native 记录。
  - 多 subject 迁移顺序、优先级和退出条件明确。
- `deliverables`:
  - subject rollout 清单。
  - dashboard / comparison 层兼容策略说明。
  - 后续执行子任务切分建议。
- `dependencies`:
  - Stage 3 的 BenchArithmetic MVP。
- `open_questions`:
  - 第二批 subject 优先选 `BenchAllocation` 还是 `BenchDispatch`。
  - generic native AOT 与后续 hot update / mobile perf 的衔接点如何冻结。

## 6. 子任务映射

| task_id | phase | status | owner | purpose | depends_on |
| --- | --- | --- | --- | --- | --- |
| `pending-phase-b-aot-contract-split` | `Stage 1` | `ready` | `codex` | 冻结 `generated-native-proof` 与 `generated-native-aot` 的职责拆分、stage kind 和 artifact contract | `20260412-05-phase-b-generic-native-aot-benchmark-backend` |
| `pending-phase-b-workload-entry-boundary` | `Stage 2` | `planned` | `codex` | 定义 benchmark `workloadEntry` 与 external native perf harness 的稳定边界，确保 perf timing 不进入 generated code | `pending-phase-b-aot-contract-split` |
| `pending-phase-b-bencharithmetic-generic-native-aot-mvp` | `Stage 3` | `planned` | `codex` | 基于新 backend 和新 entry contract 打通 `BenchArithmetic` native benchmark，并产出 record/dashboard 数据 | `pending-phase-b-aot-contract-split, pending-phase-b-workload-entry-boundary` |
| `pending-phase-b-native-rollout` | `Stage 4` | `planned` | `codex` | 将 BenchArithmetic 的 generic native AOT 路线推广到更多 benchmark subject，并稳定 dashboard 行为 | `pending-phase-b-bencharithmetic-generic-native-aot-mvp` |

说明：

- 当前唯一应立即派生的执行任务是 `pending-phase-b-aot-contract-split`。
- 在 Stage 1 和 Stage 2 边界未冻结之前，不应直接跳去实现 BenchArithmetic MVP。

## 7. 依赖

全局依赖关系如下：

1. benchmark 记录层、dashboard 生成层、managed/interpreter perf 路线已经可用，但 native 记录仍缺失。
2. `BenchArithmetic` 的 source 已经去掉 perf scaffolding，这为 `workloadEntry` 合同提供了干净输入。
3. 当前 frontend / interpreter 侧缺的 `Ldlen`、`Blt/Bgt/Ble/Bge` 等基础支持已补齐，可以避免把根因误判为 opcode 缺失。
4. 真正的 blocker 已收敛到 `generated-native-proof` 误承载 benchmark native，这要求先做 backend 拆线，再做实现。

## 8. 风险

- 如果 Stage 1 拆线不彻底，proof artifact 与 generic AOT artifact 仍会互相污染，后续 worker 很难稳定消费。
- 如果 `workloadEntry` 设计得不稳定，managed / interpreter / native 三模式会再次出现不同入口、不同计时方式的分叉。
- 如果为了尽快看到 native 数据而重新给 planner 加 benchmark family 特判，会把错误结构重新固化。
- `BenchArithmetic` 打通后仍可能暴露 generic native AOT 在其他 subject 上的新缺口，因此必须把 MVP 与 rollout 分阶段推进。

## 9. 备选路径

### 方案 A：继续补 proof family

- 优点：短期看起来最快。
- 缺点：本质上继续走错方向，直接否决。

### 方案 B：proof backend 上加 generic fallback

- 优点：比纯特判略通用。
- 缺点：backend 边界依旧混乱，不能作为 benchmark/AOT 正式路线。

### 方案 C：proof 与 generic AOT 拆线，perf harness 外置

- 优点：结构正确，能支持 MVP 后的复制扩展。
- 缺点：需要先做 contract freeze，再做实现。

当前采用：

- **方案 C**

## 10. 当前建议推进顺序

1. 先派生 Stage 1 执行子任务，冻结 `generated-native-proof` / `generated-native-aot` 拆线方案。
2. 紧接着派生 Stage 2 执行子任务，冻结 `workloadEntry` 和 external native perf harness 边界。
3. 在 contract 稳定后实现 Stage 3，让 `BenchArithmetic` 真实产出 native record。
4. 只有当以下命令成功后，才进入 Stage 4 rollout：
   - `python build/toolchains/run/run.py benchmark --subject BenchArithmetic --mode native --record`
   - `python build/toolchains/run/run.py benchmark status --subject BenchArithmetic`
5. native 数据回到 dashboard 后，再按 `BenchAllocation -> BenchDispatch -> BenchGeneric` 的顺序评估第二批迁移。
