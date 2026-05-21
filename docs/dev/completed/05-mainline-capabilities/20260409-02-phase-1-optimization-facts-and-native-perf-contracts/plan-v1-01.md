# Phase 1 OptimizationFacts 与 Native Perf Contracts 实现计划

> **面向执行 Agent：** 必须使用 `dev:executing-plans` 执行本计划，并按 TDD 先补失败测试，再推进实现。
> **目标：** 冻结 `OptimizationFacts`、native perf evidence/report，以及 `MainlineFeaturePack` 的第一版 planning surface，为后续 core cutover、lowering arbitration 和 full subject onboarding 提供稳定入口。
> **架构：** 先用 RED 锁定 contract 与 planning/reporting surface，再补 managed contract 类型与分析层 artifact，随后补 subject planner/reporting 的 native perf 结构，最后跑聚焦回归并回写父 roadmap 状态。
> **技术栈：** C#、Python、JSON manifest、subject pipeline/reporting、unittest
> **设计文档：** `docs/dev/completed/20260409-02-phase-1-optimization-facts-and-native-perf-contracts/design-v1-01.md`
> **预期知识沉淀：** 暂不写 wiki；待 `Phase 2/3/4` 真正落地后，再统一回写 `wiki/06-测试验证/subject-mainline-traceability-and-variants.md` 与后续新的 core architecture 页面。

## 目标文件与职责

- `src/managed/Chaos.IL2CPP.Contracts/ManagedClosureContracts.cs`
  - 新增或扩展 `OptimizationFacts`、native perf evidence/report contract
- `src/managed/Chaos.IL2CPP.Linker/LinkerStage.cs`
  - 输出第一版 `OptimizationFacts`
- `src/managed/Chaos.IL2CPP.CodeGen/CodeGenStage.cs`
  - 接入 `OptimizationFacts` 输入边界，哪怕首版只做透传/占位消费
- `build/toolchains/run/runtime_manifest.json`
  - 冻结 `native-runtime-perf` stage/pipeline 所需运行时 surface
- `build/toolchains/run/manifest.py`
  - 扩展 subject manifest/planning surface
- `build/toolchains/run/operation_reporting.py`
  - 扩展 native perf report 附件与 summary 字段
- `build/toolchains/run/commands/test.py`
  - 确保统一入口能消费新的 planning surface
- `tests/unit/run/test_subject_manifest_schema.py`
  - 锁定 `MainlineFeaturePack` 第一版 profile/matrix/pipeline schema
- `tests/unit/run/test_subject_planner.py`
  - 锁定 `native-runtime-perf` planning 行为
- `tests/unit/run/test_subject_reporting.py`
  - 锁定 native perf report/attachment 摘要
- `tests/unit/run/test_subject_workers.py`
  - 锁定 native perf worker surface 或至少 contract 入口
- `tests/tooling/run/test_subject_command.py`
  - 锁定统一入口对新 profile/matrix 的解析

## Chunk 1：任务初始化与 RED

- [x] 更新 `docs/dev/ACTIVE.md`、本任务 `STATUS.md` 与 `notes/progress-v1-01.md`
- [x] 先补失败测试，锁定 `OptimizationFacts` contract 与 native perf report 结构
- [x] 先补 `MainlineFeaturePack` 第一版 planning surface 的 manifest/planner 断言

## Chunk 2：managed contract 与分析层 artifact

- [x] 在 `ManagedClosureContracts.cs` 增加 `OptimizationFacts` 相关类型
- [x] 在 `ManagedClosureContracts.cs` 或关联 contract 中补上 native perf evidence/report 类型
- [x] 在线路上冻结 `analysis/analysis/optimization-facts.json` 的 artifact 名称与 manifest 引用

## Chunk 3：Linker / CodeGen contract threading

- [x] 让 `LinkerStage` 产出第一版 `OptimizationFacts`
- [x] 让 `CodeGenStage` 接受 `OptimizationFacts` 输入
- [x] 保持现有最小 subjects 回归不被打断

## Chunk 4：subject planning / reporting surface

- [x] 扩展 planner 支持 `native-runtime-perf`
- [x] 扩展 reporting 支持 `report.json + report/` perf 附件摘要
- [x] 为 `MainlineFeaturePack` 冻结 profile/matrix/pipeline 结构，但不提前接入真实 source tree

## Chunk 5：回归与收尾

- [x] 运行聚焦 unittest 回归
- [x] 更新 child `STATUS.md`、`notes/progress-*.md`
- [x] 回写父 roadmap `STATUS.md` 与 child 映射状态

## 验证命令

- [x] `python -m unittest tests.unit.run.test_subject_manifest_schema tests.unit.run.test_subject_planner tests.unit.run.test_subject_reporting tests.unit.run.test_subject_workers`
- [x] `python -m unittest tests.unit.run.test_subject_workers_perf`
- [x] `python -m unittest tests.tooling.run.test_subject_command`
- [x] `python -m unittest tests.unit.run.test_managed_closure_contract_bundle`

## 完成标准

- [x] `OptimizationFacts` 已成为独立正式 contract，而不是 sample-driven 私有逻辑
- [x] native perf evidence/report 已有稳定 artifact/report 落点
- [x] `MainlineFeaturePack` 的第一版 planning surface 已被冻结
- [x] 不提前引入 engine binding
- [x] 聚焦回归通过

## 收尾说明

- 本计划已完成并归档到 `docs/dev/completed/20260409-02-phase-1-optimization-facts-and-native-perf-contracts/`。
- 当前结果仍然只是 `Phase A` 的 performance-first core mainline 前两阶段，不是完整 `C# -> C++` total solution 终局。
