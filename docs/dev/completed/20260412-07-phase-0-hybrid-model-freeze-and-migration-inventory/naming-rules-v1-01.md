# Naming Rules v1.01

**日期：** 2026-04-12 17:55 +08:00

## 1. 顶层 subject 命名

### 1.1 archetype subject

工程 archetype 一律使用：

- `Solution*`

首批冻结如下：

- `SolutionSimpleLib`
- `SolutionMultiProject`
- `SolutionPackageReference`

### 1.2 materially distinct root

若一个 root 代表真正独立的 canonical solution source bundle，可保留语义名，例如：

- `MainlineFeaturePack`
- `EngineHostPack`（候选）
- `HotUpdateHostPack`（候选）
- `MobileHostPack`（候选）

### 1.3 禁止作为默认新增模式的命名

以下命名不再作为默认新增方式：

- `*Proof`
- `Bench*`

如果仍新增这类命名，必须证明：

- 它真的是 materially distinct solution root
- 而不是 correctness / benchmark 切片

## 2. 源码内声明对象命名

### 2.1 proof 命名

源码内 correctness 入口统一使用：

- `ChaosUnitTest`

### 2.2 benchmark 命名

源码内 runtime benchmark 入口统一使用：

- `ChaosBenchmark`

### 2.3 测试 identity

测试主 identity 默认从方法符号自动生成。

可选短名：

- `Alias`

约束：

- `Alias` 仅为 selector/report 短名
- 不作为全局主键

## 3. 目录命名

### 3.1 subject 目录

目标结构只保留：

- `subject.manifest.json`
- `source/`
- `assets/`（按需）

### 3.2 不再默认保留的目录名

以下目录不再作为目标结构中的默认一等目录：

- `scenarios/`
- `workloads/`
- `fixtures/`
- `expected/`
- `baselines/`
- `validation/`

这些语义默认收敛为 C# API 或 shared orchestration profile。

### 3.3 assets 命名边界

`assets/` 只保留：

- 二进制样本
- 静态模板
- 无法 API 化的大体积文件型输入
- 需要按文件保真的外部资产

## 4. tests 命名

### 4.1 文件命名

测试文件不再使用：

- `test_phase*`
- `test_stage*`

应使用功能命名，例如：

- `test_registry_scan.py`
- `test_execution_plan.py`
- `test_reporting_layout.py`

### 4.2 目录命名

`tests/` 的长期分类轴应是职责，而不是阶段。

冻结的目标轴：

- `registry`
- `selection`
- `planning`
- `execution`
- `reporting`
- `performance`
- `compatibility`
- `cli`
- `pipelines`
- `platform-hosts`
- `device-runners`

## 5. Phase 0 结论

Phase 0 的命名冻结结论：

- 顶层 source root 使用 `Solution*` 或真正语义化 canonical 名称
- proof / benchmark 不再决定顶层 root 名
- 目录语义优先 API 化，不再持续增生 subject 子目录
- `tests/` 不再用阶段命名组织长期结构
