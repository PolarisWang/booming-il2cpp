# Engineering vs Declared Boundary v1.01

**日期：** 2026-04-12 17:55 +08:00

## 1. 目标

冻结三条关键边界：

- engineering validation / workload 的边界
- `ChaosUnitTest` / `ChaosBenchmark` 的边界
- 文件型资产目录与 C# helper API 的边界

## 2. engineering layer

以下对象属于 engineering layer，不通过 `ChaosUnitTest` / `ChaosBenchmark` 定义：

### 2.1 engineering validations

- project graph ingestion
- dependency resolution
- source resolve
- host-input buildability
- native buildability
- package integrity
- hot-update package integrity
- compatibility / matrix readiness

### 2.2 engineering workloads

- `convert`
- `codegen`
- `native link`
- `generated native executable`
- `device package`
- `patch generation`
- `patch load` 的系统级工作流
- `rollback` 的系统级工作流

冻结结论：

- `convert/codegen/native executable` 不属于 `ChaosBenchmark`
- 它们是 system-owned engineering workload

## 3. declared layer

### 3.1 `ChaosUnitTest`

适用对象：

- runtime observable output
- metadata-visible behavior
- host-facing correctness probe
- interop correctness probe
- hot-update correctness probe

不适用对象：

- project graph
- native link
- device package
- cross-platform matrix orchestration

### 3.2 `ChaosBenchmark`

适用对象：

- runtime dispatch benchmark
- startup benchmark
- allocation / GC pressure benchmark
- hot-update runtime path benchmark

不适用对象：

- convert
- codegen
- native build pipeline
- package pipeline

## 4. API 与目录边界

### 4.1 默认 API 化的语义

以下语义默认不再作为 subject 根目录下的长期目录约定：

- fixture
- expected
- baseline
- validation helper

它们应优先被 `Chaos.TestFramework` 中的 helper API 承接，例如：

- `ChaosFixture`
- `ChaosExpect`
- `ChaosBaseline`
- `ChaosAssert`

### 4.2 保留在 `assets/` 的内容

只有以下内容继续保留为文件资产：

- 二进制样本
- 大体积基线文件
- 模板文件
- 需要按文件保真的外部输入

## 5. 冻结矩阵

| 对象 | 所属层 | 归属方式 |
| --- | --- | --- |
| `project-graph` | engineering validation | manifest + planner |
| `convert` | engineering workload | manifest + planner |
| `codegen` | engineering workload | manifest + planner |
| `native executable smoke` | engineering workload | manifest + planner |
| runtime correctness method | declared unit | `ChaosUnitTest` |
| runtime benchmark method | declared benchmark | `ChaosBenchmark` |
| expected/baseline helper | declared support API | `Chaos.TestFramework` helper |
| binary fixture file | file asset | `assets/` |

## 6. Phase 0 结论

Phase 0 对边界的冻结结论：

- engineering layer 优先，且是核心验证层
- declared layer 只负责源码可声明入口
- `assets/` 仅保留不可 API 化的文件资产
