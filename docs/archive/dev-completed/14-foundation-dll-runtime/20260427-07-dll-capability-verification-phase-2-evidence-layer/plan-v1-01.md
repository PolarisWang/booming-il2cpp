# DLL Capability Verification Phase 2 Evidence Layer Plan

> **面向执行 Agent：** 必须使用 `dev:executing-plans` 执行本计划。步骤使用复选框语法跟踪。本任务是 `20260427-04` 的 roadmap child，完成时必须先归档当前子任务目录，再回写父任务。

**目标：** 落地 A2 Source-Annotation-First 所需的 annotation、scanner、formal case index 与 `ReviewBundle` skeleton，使 Evidence Layer 成为正式证据层。

**架构：** 本阶段只负责证据对象和 case mapping，不进入统一 `.sln`/项目接线的大规模生成；Execution Layer 的完整工程与入口接线在 Phase 3 收口。

**技术栈：** Python scanner / generated C# attributes / JSON formal indexes / review bundle schema

**架构审核模式：** critical

**结构告警重点：** annotation 与 truth 字段漂移、scanner authority 边界、formal index 与 runtime result 混层、review bundle 退化成链接集合

**权责图审核主题：** Evidence Layer formal case mapping and review bundle authority

**AOT/IL2CPP/Test Governance Intake：**
- capabilityFamily: `foundation-dll-verification-closure`
- capabilityItem: `evidence-layer-formal-case-mapping`
- ownerSubjectId: `SolutionCorePack`
- proofRequired: false
- benchmarkRequired: false
- hotupdateImpact: None
- formalVerificationObjects:
  - `wiki/03-功能模块/06-il2cpp核心架构/05-架构参考/foundation-dll-capability-verification-closure-v1.md`
  - `verification/foundation-dll/**/method-*-case-index.json`
  - `verification/foundation-dll/**/review/bundle.json`
- requiredGates:
  - `collector`
  - `registry`
  - `workspace`

**设计文档：** n/a

**问题清零来源：** `docs/dev/completed/20260427-03-dll-capability-verification-closure-brainstorm/brainstorm-v1-01.md`

**计划来源：** `roadmap-child-auto`

**预期知识沉淀：**
- `wiki/03-功能模块/06-il2cpp核心架构/05-架构参考/foundation-dll-capability-verification-closure-v1.md`
- `wiki/03-功能模块/06-il2cpp核心架构/05-架构参考/verification-v1/spec.md`

**收尾约束：** 完成后必须进入“结构告警与架构审视 -> 测试通过 -> 归档 completed -> 回写父 roadmap”固定链路。

## 文件与职责

- `build/toolchains/run/testing/foundation_dll/**`
  - annotation / scanner / case index / review bundle generators
- `verification/foundation-dll/**`
  - formal case indexes and review bundle output root
- `tests/verification/unit/projections/foundation_dll/**`
  - unit tests for annotation/scanner/index/review bundle

## 执行步骤

- [ ] 设计并落地 `[MethodSubjectId]` annotation contract
- [ ] 设计并落地 `[BenchmarkSubjectId]` annotation contract
- [ ] 设计并落地 `[HotUpdateSubjectId]` annotation contract
- [ ] 尽量将 annotation 所需字段枚举化
- [ ] 实现 annotation scanner，只读取 generated source / metadata
- [ ] 产出 `method-test-case-index.json`
- [ ] 产出 `method-benchmark-case-index.json`
- [ ] 产出 `method-hotupdate-case-index.json`
- [ ] 让 formal case index 可被 method page / tooltip 消费
- [ ] 设计并落地 `ReviewBundle` formal shape
- [ ] 让新增 DLL / family 生成时同步创建 review bundle skeleton
- [ ] 识别并列出需要在后续阶段清理的旧 evidence / old mapping logic
- [ ] 更新当前子任务 `STATUS.md` 与父任务 `STATUS.md`

## 本阶段验收标准

### 自动化/结构验收

- [ ] 三类 annotation contract 明确存在
- [ ] scanner authority 边界明确且被测试锁住
- [ ] 三份 formal case index 可生成
- [ ] `ReviewBundle` skeleton 可生成
- [ ] case index 不从运行结果反推 truth

### 用户验收

- [ ] 用户可以直接看懂 method 的 test/benchmark/hotupdate case 从哪里来
- [ ] 用户可以直接看懂 tooltip 的 method 测试详情会消费哪些 formal index
- [ ] 用户可以直接看懂 review bundle 至少包含哪些内容

## 验证命令

- `python -m pytest tests/verification/unit/projections/foundation_dll/test_truth_contracts.py tests/verification/unit/projections/foundation_dll/test_truth_skeleton_generator.py tests/verification/unit/projections/foundation_dll/test_foundation_dll_gap_analyzer.py tests/verification/unit/projections/foundation_dll/test_foundation_dll_audit_generator.py -q`
- `python build/toolchains/run/run.py verify verification-v1 --json`

## 归档条件

- [ ] 本计划所有复选框完成
- [ ] 结构审视无未解决 blocker
- [ ] 用户验收标准全部满足
- [ ] 当前子任务目录归档时，父 roadmap 已同步回写 phase-2 完成状态与下一 child
