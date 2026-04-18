# Core BCL Layer 01 Priority Batch Execution 设计 v1.01

## 1. 背景

`20260418-06` 已完成 dependency-layer authority 冻结，并确认 `core-bcl-layer-01` 在 `.NET 8` / `.NET 10` 下都存在 `29` 个 ready DLL。下一步不应直接把整层 `29` 个程序集一次性拉进执行闭环，而应先选一组覆盖面高、风险可控、能代表 collections / memory / threading / console 四类能力的 representative batch，先把真实 native proof / benchmark / engineering gate 跑通。

## 2. 目标

- 冻结 `core-bcl-layer-01` priority batch 的程序集边界、proof / benchmark authority 与 required gates。
- 围绕 `System.Collections`、`System.Memory`、`System.Threading`、`System.Console` 四个代表方向建立可执行批次。
- 为后续扩展到剩余 layer-01 DLL 提供一条可复用的 execution 模板，而不是再次回到“先试跑几个 proof 再说”的临时路径。

## 3. 非目标

- 本任务不尝试一次性认证 `core-bcl-layer-01` 的全部 `29` 个 ready DLL。
- 本任务不进入 `core-bcl-layer-02` 或更高 layer。
- 本任务不新增新的 `SolutionCorePack` proof / benchmark 资产；优先消费当前 registry 已声明的 canonical commands。
- 本任务不调整 `subject.features.json`、registry schema 或 dependency-layer builder 的长期 authority。

## 4. 设计原则

- 代表性优先：先覆盖 collections / memory / threading / console 四个方向的 representative assemblies，而不是按字母顺序推进。
- authority 先于执行：先冻结 machine-readable batch scope 与 formal verification objects，再进入真实 proof / benchmark。
- owner subject 复用：继续使用 `SolutionCorePack` 作为 proof / benchmark / engineering workload authority，不引入新的 owner subject。
- 双 TFM 同构：`net8.0` 与 `net10.0` 使用同一批程序集、同一套 gates、同一组 formal verification objects。

## 5. Priority Batch 范围

当前 priority batch 冻结为 `8` 个程序集：

- collections:
  - `System.Collections`
  - `System.Collections.Concurrent`
  - `System.Collections.NonGeneric`
- memory:
  - `System.Memory`
- threading:
  - `System.Threading`
  - `System.Threading.Channels`
  - `System.Threading.Tasks.Dataflow`
- console:
  - `System.Console`

选择理由：

- `System.Collections` / `System.Collections.Concurrent` / `System.Collections.NonGeneric` 代表 layer-01 中最直接的 collections-and-generics 负载。
- `System.Memory` 覆盖 span / memory / low-level buffer contract，是后续更高层 BCL 的关键基础。
- `System.Threading` / `System.Threading.Channels` / `System.Threading.Tasks.Dataflow` 共同代表 threading-and-async 方向。
- `System.Console` 是受控官方 DLL 落点与 archetype / host-input 消费链路的直接代表。

## 6. 产物

- `priority-batch-scope-v1-01.json`
- `native-proof-summary-v1-01.json`
- `nativeization-throughput-benchmark-v1-01.json`
- `STATUS.md`
- `plan-v1-01.md`
- `notes/progress-v1-01.md`

## 7. 执行思路

1. 读取 `20260418-06` 生成的 `layer-plan-v1-01.json`、`proof-benchmark-lane-v1-01.json` 与 per-assembly plans。
2. 冻结 representative batch 的 machine-readable scope，明确：
   - 选中的程序集
   - required gates
   - monitor signals
   - formal proof / benchmark / engineering validation objects
3. 以 registry canonical commands 为 authority，先执行 representative unit/native proof，再执行 engineering validation 与 benchmark。
4. 汇总 per-assembly / per-batch 的通过情况、阻塞点与下一批扩展建议。

## 8. 完成判断

- `priority-batch-scope-v1-01.json` 能完整描述本轮 batch 的程序集、authority 与 verification objects。
- collections / memory / threading / console 四个方向均至少有正式 proof 对象进入执行闭环。
- benchmark 与 engineering validation 结果可回溯到 registry canonical commands。
- child task 的 `STATUS.md`、父 roadmap `STATUS.md`、`docs/dev/ACTIVE.md` 与索引文件保持同步。
